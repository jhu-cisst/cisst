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

#include <cisstOSAbstraction/osaConfig.h>

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

    bool NewData;
    bool Reading;
    pointer ReadPointer, WritePointer, FreePointer;

public:
    osaTripleBuffer(void):
        OwnMemory(true),
        NewData(false),
        Reading(false)
    {
        this->Memory = new value_type[3];
        this->ReadPointer = this->Memory;
        this->WritePointer = this->Memory + 1;
        this->FreePointer = this->Memory + 2;
    }

    osaTripleBuffer(const_reference initialValue):
        OwnMemory(true),
        Memory(0),
        NewData(false),
        Reading(false)
    {
        this->ReadPointer = new value_type(initialValue);
        this->WritePointer = new value_type(initialValue);
        this->FreePointer = new value_type(initialValue);
    }

    osaTripleBuffer(pointer pointer1, pointer pointer2, pointer pointer3):
        OwnMemory(false),
        Memory(0),
        NewData(false),
        Reading(false)
    {
        this->ReadPointer = pointer1;
        this->WritePointer = pointer2;
        this->FreePointer = pointer3;
    }

    ~osaTripleBuffer() {
        if (this->OwnMemory) {
            if (this->Memory) {
                delete[] this->Memory;
            } else {
                delete this->ReadPointer;
                delete this->WritePointer;
                delete this->FreePointer;
            }
        }
    }

    void Read(reference placeHolder) {
        this->BeginRead();
        placeHolder = *ReadPointer;
        this->EndRead();
    }

    void Write(const_reference newValue) {
        this->BeginWrite();
        *WritePointer = newValue;
        this->EndWrite();
    }

    pointer GetWritePointer(void) const {
        return this->WritePointer;
    }

    void BeginRead(void) {
        Reading = true;
        NewData = false;
    }

    const_pointer GetReadPointer(void) const {
        return this->ReadPointer;
    }

    void EndRead(void) {
        if (NewData) {
#if CISST_OSA_HAS_sync_val_compare_and_swap
            ReadPointer = __sync_val_compare_and_swap(&FreePointer, FreePointer, ReadPointer);
#endif
        }
        Reading = false;
    }

    void BeginWrite(void) {
    }


    void EndWrite(void) {
        if (!Reading) {
#if CISST_OSA_HAS_sync_val_compare_and_swap
            WritePointer = __sync_val_compare_and_swap(&ReadPointer, ReadPointer, WritePointer);
#endif
        }
#if CISST_OSA_HAS_sync_val_compare_and_swap
        WritePointer = __sync_val_compare_and_swap(&FreePointer, FreePointer, WritePointer);
#endif
        NewData = true;
    }
};
