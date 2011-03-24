/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id $

  Author(s): Martin Kelly, Anton Deguet
  Created on: 2011-03-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstConfig.h> // to define CISST_OS and CISST_COMPILER
#include <cisstOSAbstraction/osaConfig.h> // to know which atomic operations are available

#include <cisstOSAbstraction/osaMutex.h>

template <class _elementType>
class osaTripleBuffer
{
    friend class osaTripleBufferTest;

    typedef _elementType value_type;

    typedef osaTripleBuffer<value_type> ThisType;

    typedef value_type * pointer;
    typedef const value_type * const_pointer;
    typedef value_type & reference;
    typedef const value_type & const_reference;

    // did the buffer allocate memory or used existing pointers
    bool OwnMemory;
    pointer Memory;
    pointer Pointers[3];

    // circular buffer node
    struct Node {
        pointer Pointer;
        Node * Next;
    };

    // last write node (behaves like a head in our lifo circular buffer
    Node * LastWriteNode;
    Node * WriteNode;
    Node * ReadNode;


    osaMutex Mutex;

public:
    /*! Constructor that allocates memory for the triple buffer using
      the default constructor for each element. */
    inline osaTripleBuffer(void):
        OwnMemory(true)
    {
        this->Memory = new value_type[3];
        this->Pointers[0] = this->Memory;
        this->Pointers[1] = this->Memory + 1;
        this->Pointers[2] = this->Memory + 2;
        SetupNodes();
    }

    /*! Constructor that allocates memory for the triple buffer using
      the copy constructor for each element.  User has to provide an
      value which will be used to initialize the buffer elements. */
    inline osaTripleBuffer(const_reference initialValue):
        OwnMemory(true),
        Memory(0)
    {
        this->Pointers[0] = new value_type(initialValue);
        this->Pointers[1] = new value_type(initialValue);
        this->Pointers[2] = new value_type(initialValue);
        SetupNodes();
    }

    /*! Constructor that doesn't allocate any memory, user has to
      provide 3 valid pointers on 3 different pre-allocated objects.
      This can be used in combination with libraries such as VTK which
      have private constructors. */
    inline osaTripleBuffer(pointer pointer1, pointer pointer2, pointer pointer3):
        OwnMemory(false),
        Memory(0)
    {
        this->Pointers[0] = pointer1;
        this->Pointers[1] = pointer2;
        this->Pointers[2] = pointer3;
        SetupNodes();
    }


    inline void SetupNodes(void) {
        this->LastWriteNode = new Node();
        this->LastWriteNode->Pointer = this->Pointers[0];
        this->LastWriteNode->Next = new Node;
        this->LastWriteNode->Next->Pointer = this->Pointers[1];
        this->LastWriteNode->Next->Next = new Node;
        this->LastWriteNode->Next->Next->Pointer = this->Pointers[2];
        this->LastWriteNode->Next->Next->Next = this->LastWriteNode;
    }

    inline ~osaTripleBuffer() {
        if (this->OwnMemory) {
            if (this->Memory) {
                delete[] this->Memory;
            } else {
                delete this->Pointers[0];
                delete this->Pointers[1];
                delete this->Pointers[2];
            }
        }
        delete this->LastWriteNode->Next->Next;
        delete this->LastWriteNode->Next;
        delete this->LastWriteNode;
    }


    inline void Read(reference placeHolder) {
        this->BeginRead();
        placeHolder = *(ReadNode->Pointer);
        this->EndRead();
    }


    inline void Write(const_reference newValue) {
        this->BeginWrite();
        *(WriteNode->Pointer) = newValue;
        this->EndWrite();
    }


    inline const_pointer GetReadPointer(void) const {
        return this->ReadNode->Pointer;
    }


    inline pointer GetWritePointer(void) const {
        return this->WriteNode->Pointer;
    }


    inline void BeginRead(void) {
        Mutex.Lock(); {
            this->ReadNode = this->LastWriteNode;
        } Mutex.Unlock();
    }


    inline void EndRead(void) {
        Mutex.Lock(); {
            this->ReadNode = 0;
        } Mutex.Unlock();
    }


    inline void BeginWrite(void) {
        Mutex.Lock(); {
            this->WriteNode = this->LastWriteNode->Next;
            while (this->WriteNode == this->ReadNode) {
                this->WriteNode = this->WriteNode->Next;
            }
        } Mutex.Unlock();
    }


    inline void EndWrite(void) {
        Mutex.Lock(); {
            this->LastWriteNode = this->WriteNode;
        } Mutex.Unlock();
    }
};
