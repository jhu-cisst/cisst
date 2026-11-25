/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2007-09-05

  (C) Copyright 2007-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines mtsQueue
*/


#ifndef _mtsQueue_h
#define _mtsQueue_h

#include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <atomic>
#include <stdexcept>
#include <new>

// Should remove this eventually, but for now, we need to support older compilers
// that don't have std::hardware_interference_size, e.g. Clang v18 on Ubuntu 24
#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
    // 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
    constexpr std::size_t hardware_constructive_interference_size = 64;
    constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

template <typename T, typename = void>
struct mtsQueueUnwrapPointer {
    using type = T;
    static type* get(T& val) noexcept { return &val; }
};

// Specialization for any type with an `element_type` and `.get()` (unique_ptr, shared_ptr, etc.)
template <typename T>
struct mtsQueueUnwrapPointer<T, std::void_t<typename T::element_type, decltype(std::declval<T>().get())>> {
    using type = typename T::element_type;
    static type* get(const T& ptr) noexcept { return ptr.get(); }
};

// Default cloning is just copy construction, cannot fail
template <typename T>
struct mtsQueueDefaultClone {
    // Allows us to accept mtsGenericObjectProxyRef for mtsGenericObejctProxy
    using BaseType = typename mtsGenericTypesUnwrap<T>::BaseType;

    static bool clone(T* place, const BaseType& value) {
        *place = value;
        return true;
    }

    static bool clone(T& place, const BaseType& value) {
        place = value;
        return true;
    }
};

// mtsGenericObject cloning is ClassServices::Create. Note that this requires place and value to have same concrete type, which is not checked at compile time nor at runtime.
template <>
struct mtsQueueDefaultClone<std::unique_ptr<mtsGenericObject>> {
    static bool clone(std::unique_ptr<mtsGenericObject>& place, const mtsGenericObject& value) {
        const auto services = value.Services();

        // delete without freeing memory, so we don't have to re-allocate memory
        mtsGenericObject* raw = place.release();
        if (raw) {
            // Create takes care of deleting the existing object
            const bool ok = services->Create(raw, value);
            // If Create succeeds raw is new object, and if Create fails, raw is untouched, so either way we keep it
            place.reset(std::launder(raw));
            return ok;
        } else {
            mtsGenericObject* const ptr = dynamic_cast<mtsGenericObject*>(services->Create(value));
            place.reset(ptr);
            return ptr != nullptr;
        }

        return place.get();
    }

    static bool clone(mtsGenericObject& place, const mtsGenericObject& value) {
        const auto services = value.Services();
        return services->Create(&place, value);
    }
};

/*!
  \ingroup cisstMultiTask

  Lock-free SPSC queue for one producer thread and one consumer thread, holding up
  to GetSize() elements.  The producer calls Put; the consumer calls Peek and
  then Pop (or Get).  The status methods are snapshots and are safe from either
  thread.  SetSize and destruction require exclusive use.

  The pointer returned by Peek remains valid until the matching Pop or Get.
*/
template<typename T, typename InPlaceClone = mtsQueueDefaultClone<T>>
class mtsQueue final
{
public:
    // TODO BB: why accept proxy-ref? versus making user dereference type...
    using element_type = typename mtsQueueUnwrapPointer<T>::type;
    using base_type = typename mtsGenericTypesUnwrap<element_type>::BaseType;

protected:
    size_t Capacity = 0;
    std::unique_ptr<T[]> Data;                                                 // Capacity + 1 slots, the spare one tells full from empty
    alignas(hardware_destructive_interference_size) std::atomic<size_t> Head{0};     // Next slot to write, only modified by the producer
    alignas(hardware_destructive_interference_size) std::atomic<size_t> Tail{0};     // Next slot to read, only modified by the consumer

    size_t Advance(size_t index) const
    {
        ++index;
        return index > Capacity ? 0 : index;
    }

    // Strong guarantee: on exception the queue is unchanged.
    void Allocate(size_t capacity, const base_type& value)
    {
        std::unique_ptr<T[]> data = nullptr;
        if (capacity > 0) {
            data = std::make_unique<T[]>(capacity + 1);
            for (size_t idx = 0; idx < capacity + 1; ++idx) {
                const bool ok = InPlaceClone::clone(data[idx], value);
                if (!ok) {
                    throw std::runtime_error("mtsQueue: unable to clone an element of type " + std::string(typeid(T).name()));
                }
            }
        }

        Data.swap(data);
        Capacity = capacity;
        Head.store(0, std::memory_order_relaxed);
        Tail.store(0, std::memory_order_relaxed);
    }

public:
    mtsQueue() {}

    mtsQueue(size_t capacity, const base_type& value)
    {
        Allocate(capacity, value);
    }

    mtsQueue(const mtsQueue&) = delete;
    mtsQueue& operator=(const mtsQueue&) = delete;

    /*! Destructive resize, queued elements are discarded. */
    void SetSize(size_t capacity, const base_type& value)
    {
        Allocate(capacity, value);
    }

    size_t GetSize() const { return Capacity; }

    size_t GetAvailable() const
    {
        const size_t tail = Tail.load(std::memory_order_acquire);
        const size_t head = Head.load(std::memory_order_acquire);
        return head >= tail ? head - tail : head + (Capacity + 1) - tail;
    }

    bool IsFull() const
    {
        const size_t head = Head.load(std::memory_order_acquire);
        return Advance(head) == Tail.load(std::memory_order_acquire);
    }

    bool IsEmpty() const
    {
        const size_t tail = Tail.load(std::memory_order_acquire);
        return tail == Head.load(std::memory_order_acquire);
    }

    /*! Producer only. Returns nullptr if the queue is full. */
    const element_type* Put(const base_type& newObject)
    {
        const size_t head = Head.load(std::memory_order_relaxed);
        const size_t next = Advance(head);
        if (next == Tail.load(std::memory_order_acquire)) {
            return nullptr;
        }

        if (!InPlaceClone::clone(Data[head], newObject)) {
            return nullptr;
        }

        Head.store(next, std::memory_order_release);
        return mtsQueueUnwrapPointer<T>::get(Data[head]);
    }

    /*! Consumer only. Returns nullptr if the queue is empty. */
    element_type* Peek()
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return nullptr;
        }
        return mtsQueueUnwrapPointer<T>::get(Data[tail]);
    }

    /*! Consumer only. Returns nullptr if the queue is empty. */
    const element_type* Peek() const
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return nullptr;
        }
        return mtsQueueUnwrapPointer<T>::get(Data[tail]);
    }

    /*! Consumer only. Releases the element returned by Peek. */
    bool Pop()
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return false;
        }
        Tail.store(Advance(tail), std::memory_order_release);
        return true;
    }

    /*! Consumer only. Copies the oldest element to \c out and releases it. If the copy throws, the element stays queued. If T is a smart pointer, we require caller to have allocated memory so we can get via copy */
    bool Get(element_type& out)
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return false;
        }
        if (!InPlaceClone::clone(&out, Data[tail])) {
            return false;
        }

        Tail.store(Advance(tail), std::memory_order_release);
        return true;
    }
};

using mtsQueueGeneric = mtsQueue<std::unique_ptr<mtsGenericObject>>;

#endif // _mtsQueue_h
