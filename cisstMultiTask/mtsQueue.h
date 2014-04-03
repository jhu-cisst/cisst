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

/*!
  \ingroup cisstMultiTask

  Defines a queue that can be accessed in a thread-safe manner,
  assuming that there is only one reader and one writer and that
  pointer updates are atomic.
*/
template<class _elementType>
class mtsQueue
{
public:
    typedef _elementType value_type;
    typedef value_type * pointer;
    typedef const value_type * const_pointer;
    typedef value_type & reference;
    typedef const value_type & const_reference;
    typedef size_t size_type;
    typedef size_t index_type;

protected:
    pointer Data;
    pointer Head;
    pointer Tail;
    pointer Sentinel;  // end marker
    size_type Size;

    // private method, can only be used once by constructor.  Doesn't support resize!
    void Allocate(size_type size, const_reference value) {
        this->Size = size;
        if (this->Size > 0) {
            this->Data = new value_type[this->Size];
            index_type index;
            for (index = 0; index < this->Size; index++) {
                new(&this->Data[index]) value_type(value);
            }
        } else {
            this->Data = 0;
        }
        // head == tail implies empty queue
        this->Head = this->Data;
        this->Tail = this->Data;
        this->Sentinel = this->Data + this->Size;
    }

public:

    inline mtsQueue(void):
        Data(0),
        Head(0),
        Tail(0),
        Sentinel(0),
        Size(0)
    {}


    inline mtsQueue(size_type size, const_reference value) {
        Allocate(size, value);
    }


    inline ~mtsQueue() {
        delete [] Data;
    }


    /*! Sets the size of the queue (destructive, i.e. won't preserve
      previously queued elements). */
    inline void SetSize(size_type size, const_reference value) {
        delete [] Data;
        this->Allocate(size, value);
    }


    /*! Returns size of queue. */
    inline size_type GetSize(void) const {
        return Size;
    }


    /*! Returns number of elements available in queue, i.e. the number
      of slots used. */
    inline size_type GetAvailable(void) const
    {
        ptrdiff_t available = Head - Tail;
        if (available < 0) {
            available += Size;
        }
        return available;
    }


    /*! Returns true if queue is full. */
    inline bool IsFull(void) const {
        pointer oneAfterHead = this->Head + 1;
        if (oneAfterHead >= this->Sentinel) {
            oneAfterHead = this->Data;
        }
        return oneAfterHead == this->Tail;
    }


    /*! Returns true if queue is empty. */
    inline bool IsEmpty(void) const {
        return Head == Tail;
    }


    /*! Copy an object to the queue.
      \param in reference to the object to be copied
      \result Pointer to element in queue (use iterator instead?)
    */
    //inline const_pointer Put(const_reference newObject)
    //Following signature is equivalent for types that are not Proxy types. If a Proxy type,
    //then we use the ProxyBase instead, so that we can also accept ProxyRef objects.
    inline const_pointer Put(const typename mtsGenericTypesUnwrap<value_type>::BaseType &newObject)
    {
        pointer newHead = this->Head + 1;
        // test if end of buffer (same as IsFull method)
        if (newHead >= this->Sentinel) {
            newHead = this->Data;
        }
        // test if full
        if (newHead == this->Tail) {
            return 0;    // queue full
        }
        // queue new object and move head
        *(this->Head) = newObject;
        this->Head = newHead;
        return this->Head;
    }


    /*! Get a pointer to the next object to be read, but do not
        remove the item from the queue.
        \result Pointer to top element in queue (use iterator instead?)
     */
    inline pointer Peek(void) const {
        if (this->IsEmpty()) {
            return 0;
        }
        return this->Tail;
    }


    /*! Pop the next object to be read from the queue.
        \result Pointer to element just popped (use iterator instead?)
     */
    inline pointer Get(void) {
        if (this->IsEmpty()) {
            return 0;
        }
        pointer result = this->Tail;
        this->Tail++;
        if (this->Tail >= this->Sentinel) {
            this->Tail = this->Data;
        }
        return result;
    }

};




class mtsQueueGeneric
{
public:
    typedef mtsGenericObject value_type;
    typedef value_type * pointer;
    typedef const value_type * const_pointer;
    typedef value_type & reference;
    typedef const value_type & const_reference;
    typedef size_t size_type;
    typedef size_t index_type;

protected:
    const cmnClassServicesBase * ClassServices;
    pointer * Data;
    pointer * Head;
    pointer * Tail;
    pointer * Sentinel;  // end marker
    size_type Size;

    // private method, can only be used once by constructor.  Doesn't support resize!
    void Allocate(size_type size, const_reference value) {
        CMN_ASSERT(this->ClassServices);
        this->Size = size;
        if (this->Size > 0) {
            this->Data = new pointer[size];
            size_t index;
            for (index = 0; index < size; index++) {
                cmnGenericObject * genericPointer = this->ClassServices->Create(value);
                if (!genericPointer) {
                    CMN_LOG_RUN_ERROR << "class mtsQueueGeneric: Allocate failed to create generic object for "
                                      << this->ClassServices->GetName() << std::endl;
                }
                pointer typedPointer = dynamic_cast<pointer>(genericPointer);
                CMN_ASSERT(typedPointer);
                this->Data[index] = typedPointer;
            }
        } else {
            this->Data = 0;
        }
        // head == tail implies empty queue
        this->Head = this->Data;
        this->Tail = this->Data;
        this->Sentinel = this->Data + this->Size;
    }

    void Free(void) {
        size_t index;
        for (index = 0; index < this->Size; index++) {
            delete this->Data[index];
        }
        delete [] this->Data;
        this->Data = 0;
    }

public:

    inline mtsQueueGeneric(void):
        ClassServices(0),
        Data(0),
        Head(0),
        Tail(0),
        Sentinel(0),
        Size(1)
    {}


    inline mtsQueueGeneric(size_type size, const_reference value) {
        this->ClassServices = value.Services();
        CMN_ASSERT(this->ClassServices);
        CMN_LOG_INIT_DEBUG << "class mtsQueueGeneric: constructor, created queue of "
                           << this->ClassServices->GetName() << std::endl;
        Allocate(size + 1, value);
    }


    inline ~mtsQueueGeneric() {
        this->Free();
    }


    /*! Sets the size of the queue (destructive, i.e. won't preserve
        previously queued elements). */
    inline void SetSize(size_type size, const_reference value) {
        if (this->ClassServices != 0) { // if non zero, it has already been set
            if (this->ClassServices != value.Services()) {
                CMN_LOG_INIT_WARNING << "mtsQueueGeneric::SetSize: type of elements expected \""
                                     << this->ClassServices->GetName() << "\", now using \""
                                     << value.Services()->GetName() << "\"" << std::endl;
            }
            this->Free();
            if (this->Data != 0) {
                CMN_LOG_INIT_WARNING << "mtsQueueGeneric::SetSize: failed to delete data array for \""
                                     << this->ClassServices->GetName() << "\"" << std::endl;
            }
        }
        this->ClassServices = value.Services();
        this->Allocate(size + 1, value);
    }


    /*! Returns size of queue. */
    inline size_type GetSize(void) const {
        return (Size - 1);
    }


    /*! Returns number of elements available in queue, i.e. the number
      of slots used. */
    inline size_type GetAvailable(void) const
    {
        ptrdiff_t available = Head - Tail;
        if (available < 0) {
            available += Size;
        }
        return available;
    }


    /*! Returns true if queue is full. */
    inline bool IsFull(void) const {
        pointer * oneAfterHead = this->Head + 1;
        if (oneAfterHead >= this->Sentinel) {
            oneAfterHead = this->Data;
        }
        return oneAfterHead == this->Tail;
    }


    /*! Returns true if queue is empty. */
    inline bool IsEmpty(void) const {
        return Head == Tail;
    }


    /*! Copy an object to the queue.
      \param in reference to the object to be copied
      \result Pointer to element in queue (use iterator instead?)
    */
    inline const_pointer Put(const_reference newObject) {
        pointer * newHead = this->Head + 1;
        // test if end of buffer (same as IsFull method)
        if (newHead >= this->Sentinel) {
            newHead = this->Data;
        }
        // test if full
        if (newHead == this->Tail) {
            return 0;    // queue full
        }
        // queue new object and move head
        // using in place new to make sure copy constructor is used
        if (!this->ClassServices->Create(*(this->Head), newObject)) {
            // if Create fails, it does not modify the input parameter (this->Head)
            CMN_LOG_RUN_ERROR << "mtsQueueGeneric::Put failed for " << newObject.Services()->GetName() << std::endl;
            return 0;
        }
        this->Head = newHead;
        return *(this->Head);
    }


    /*! Get a pointer to the next object to be read, but do not
        remove the item from the queue.
        \result Pointer to top element in queue (use iterator instead?)
     */
    inline pointer Peek(void) const {
        if (this->IsEmpty()) {
            return 0;
        }
        return *(this->Tail);
    }


    /*! Pop the next object to be read from the queue.
        \result Pointer to element just popped (use iterator instead?)
     */
    inline pointer Get(void) {
        if (this->IsEmpty()) {
            return 0;
        }
        pointer result = *(this->Tail);
        this->Tail++;
        if (this->Tail >= this->Sentinel) {
            this->Tail = this->Data;
        }
        return result;
    }

};


#endif // _mtsQueue_h

