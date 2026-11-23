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
#include <limits>
#include <stdexcept>
#include <vector>

/*!
  \ingroup cisstMultiTask

  Lock-free SPSC queue for one producer thread and one consumer thread, holding up
  to GetSize() elements.  The producer calls Put; the consumer calls Peek and
  then Pop (or Get).  The status methods are snapshots and are safe from either
  thread.  SetSize and destruction require exclusive use.

  The pointer returned by Peek remains valid until the matching Pop or Get.
*/
template<typename value_type>
class mtsQueue
{
    // Work around for annoying std::vector<bool> issue
    class BoolWrapper {
    public:
        bool value;
        BoolWrapper(bool v) : value(v) {}

        operator bool() const noexcept { return value; }
        bool* operator&() noexcept { return &value; }
        const bool* operator&() const noexcept { return &value; }
    };

    // Map bool -> BoolWrapper, keep all other T as-is
    template <typename T>
    using StorageType = std::conditional_t<std::is_same_v<T, bool>, BoolWrapper, T>;

protected:
    size_t Capacity;
    std::vector<StorageType<value_type>> Data; // Capacity + 1 slots, the spare one tells full from empty
    std::atomic<size_t> Head;     // Next slot to write, only modified by the producer
    std::atomic<size_t> Tail;     // Next slot to read, only modified by the consumer

    size_t Advance(size_t index) const
    {
        ++index;
        return index > Capacity ? 0 : index;
    }

    // Strong guarantee: on exception the queue is unchanged.
    void Allocate(size_t capacity, const value_type & value)
    {
        std::vector<StorageType<value_type>> data;
        if (capacity >= data.max_size()) {
            throw std::length_error("mtsQueue: capacity too large");
        }
        if (capacity > 0) {
            data.assign(capacity + 1, value);
        }
        Data.swap(data);
        Capacity = capacity;
        Head.store(0, std::memory_order_relaxed);
        Tail.store(0, std::memory_order_relaxed);
    }

public:
    mtsQueue():
        Capacity(0),
        Head(0),
        Tail(0)
    {}

    mtsQueue(size_t capacity, const value_type& value):
        Capacity(0),
        Head(0),
        Tail(0)
    {
        Allocate(capacity, value);
    }

    mtsQueue(const mtsQueue&) = delete;
    mtsQueue& operator=(const mtsQueue&) = delete;

    /*! Destructive resize, queued elements are discarded. */
    void SetSize(size_t capacity, const value_type& value)
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
    const value_type* Put(const typename mtsGenericTypesUnwrap<value_type>::BaseType& newObject)
    {
        const size_t head = Head.load(std::memory_order_relaxed);
        const size_t next = Advance(head);
        if (next == Tail.load(std::memory_order_acquire)) {
            return nullptr;
        }
        Data[head] = newObject;
        Head.store(next, std::memory_order_release);
        return &Data[head];
    }

    /*! Consumer only. Returns nullptr if the queue is empty. */
    value_type* Peek()
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return nullptr;
        }
        return &Data[tail];
    }

    /*! Consumer only. Returns nullptr if the queue is empty. */
    const value_type* Peek() const
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return nullptr;
        }
        return &Data[tail];
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

    /*! Consumer only. Copies the oldest element to \c out and releases it. If the copy throws, the element stays queued. */
    bool Get(value_type& out)
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return false;
        }
        out = Data[tail];
        Tail.store(Advance(tail), std::memory_order_release);
        return true;
    }
};


/*!
  \ingroup cisstMultiTask

  Same contract as mtsQueue for polymorphic elements.  All slots are
  preallocated copies of the prototype given to the constructor or SetSize, so
  every element queued must have the prototype's class.
*/
class mtsQueueGeneric
{
public:
    typedef mtsGenericObject value_type;

protected:
    const cmnClassServicesBase* ClassServices;
    size_t Capacity;
    mtsGenericObject** Data; // Capacity + 1 slots, the spare one tells full from empty
    std::atomic<size_t> Head; // Next slot to write, only modified by the producer
    std::atomic<size_t> Tail; // Next slot to read, only modified by the consumer

    size_t Advance(size_t index) const
    {
        ++index;
        return index > Capacity ? 0 : index;
    }

    static void DestroySlots(mtsGenericObject** data, size_t count)
    {
        for (size_t index = 0; index < count; ++index) {
            delete data[index];
        }
        delete[] data;
    }

    static mtsGenericObject** CreateSlots(
        const cmnClassServicesBase* services,
        size_t count,
        const mtsGenericObject& value
    )
    {
        mtsGenericObject** data = new mtsGenericObject * [count];
        size_t created = 0;
        try {
            for (; created < count; ++created) {
                cmnGenericObject* generic = services->Create(value);
                if (!generic) {
                    throw std::runtime_error("mtsQueueGeneric: unable to create an element of class "
                                             + services->GetName());
                }
                mtsGenericObject* typed = dynamic_cast<mtsGenericObject *>(generic);
                if (!typed) {
                    delete generic;
                    throw std::runtime_error("mtsQueueGeneric: class " + services->GetName()
                                             + " is not derived from mtsGenericObject");
                }
                data[created] = typed;
            }
        } catch (...) {
            DestroySlots(data, created);
            throw;
        }
        return data;
    }

    // Strong guarantee: on exception the queue is unchanged.
    void Allocate(size_t capacity, const mtsGenericObject& value)
    {
        const cmnClassServicesBase* services = value.Services();
        if (!services) {
            throw std::runtime_error("mtsQueueGeneric: element has no class services");
        }
        if (capacity >= std::numeric_limits<size_t>::max() / sizeof(mtsGenericObject *)) {
            throw std::length_error("mtsQueueGeneric: capacity too large");
        }
        mtsGenericObject** data = capacity > 0 ? CreateSlots(services, capacity + 1, value) : nullptr;

        // nothing below can throw
        DestroySlots(Data, Data ? Capacity + 1 : 0);
        Data = data;
        Capacity = capacity;
        ClassServices = services;
        Head.store(0, std::memory_order_relaxed);
        Tail.store(0, std::memory_order_relaxed);
    }

public:
    mtsQueueGeneric():
        ClassServices(nullptr),
        Capacity(0),
        Data(nullptr),
        Head(0),
        Tail(0)
    {}

    mtsQueueGeneric(size_t capacity, const mtsGenericObject& value):
        ClassServices(nullptr),
        Capacity(0),
        Data(nullptr),
        Head(0),
        Tail(0)
    {
        Allocate(capacity, value);
    }

    mtsQueueGeneric(const mtsQueueGeneric&) = delete;
    mtsQueueGeneric& operator=(const mtsQueueGeneric&) = delete;

    ~mtsQueueGeneric()
    {
        DestroySlots(Data, Data ? Capacity + 1 : 0);
    }

    /*! Destructive resize, queued elements are discarded. */
    void SetSize(size_t capacity, const mtsGenericObject& value)
    {
        if (ClassServices && value.Services() != ClassServices) {
            CMN_LOG_INIT_WARNING << "mtsQueueGeneric::SetSize: type of elements expected \""
                                 << ClassServices->GetName() << "\", now using \""
                                 << value.Services()->GetName() << "\"" << std::endl;
        }
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

    /*! Producer only.  Returns 0 if the queue is full or the class differs from the prototype's. */
    const mtsGenericObject* Put(const mtsGenericObject& newObject)
    {
        const size_t head = Head.load(std::memory_order_relaxed);
        const size_t next = Advance(head);
        if (next == Tail.load(std::memory_order_acquire)) {
            return nullptr;
        }
        // Create destroys the slot before copy-constructing into it, a throwing copy constructor leaves it dead.
        if (!ClassServices->Create(Data[head], newObject)) {
            CMN_LOG_RUN_ERROR << "mtsQueueGeneric::Put failed for "
                              << newObject.Services()->GetName() << std::endl;
            return nullptr;
        }
        Head.store(next, std::memory_order_release);
        return Data[head];
    }

    /*! Consumer only.  Returns 0 if the queue is empty. */
    mtsGenericObject* Peek() const
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return nullptr;
        }
        return Data[tail];
    }

    /*! Consumer only.  Releases the element returned by Peek. */
    bool Pop()
    {
        const size_t tail = Tail.load(std::memory_order_relaxed);
        if (tail == Head.load(std::memory_order_acquire)) {
            return false;
        }
        Tail.store(Advance(tail), std::memory_order_release);
        return true;
    }
};

#endif // _mtsQueue_h
