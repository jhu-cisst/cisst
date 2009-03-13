/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Define an internal command for cisstMultiTask
*/


#ifndef _mtsCommandQueuedWrite_h
#define _mtsCommandQueuedWrite_h

#include <cisstMultiTask/mtsCommandQueuedWriteBase.h>


/*!
  \ingroup cisstMultiTask

 */
template <class _argumentType>
class mtsCommandQueuedWrite: public mtsCommandQueuedWriteBase
{
public:
    typedef mtsCommandQueuedWriteBase BaseType;
    typedef _argumentType ArgumentType;

    /*! This type. */
    typedef mtsCommandQueuedWrite<ArgumentType> ThisType;

protected:
    mtsQueue<ArgumentType> ArgumentsQueue;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandQueuedWrite(const ThisType & CMN_UNUSED(other));

public:

    inline mtsCommandQueuedWrite(void):
        BaseType(),
        ArgumentsQueue(0, ArgumentType())
    {}


    inline mtsCommandQueuedWrite(mtsCommandWriteBase * actualCommand):
        BaseType(0, actualCommand),
        ArgumentsQueue(0, ArgumentType())
    {}

    
    inline mtsCommandQueuedWrite(mtsMailBox * mailBox, mtsCommandWriteBase * actualCommand, unsigned int size):
        BaseType(mailBox, actualCommand),
        ArgumentsQueue(0, ArgumentType())
    {
        const ArgumentType * argumentPrototype = dynamic_cast<const ArgumentType *>(this->GetArgumentPrototype());
        if (argumentPrototype) {
            ArgumentsQueue.SetSize(size, *argumentPrototype);
        } else {
            CMN_LOG(1) << "Class mtsCommandQueuedWrite: constructor: Can't find argument prototype from actual command."
                       << std::endl;
        }
    }

    
    // ArgumentsQueue destructor should get called
    inline virtual ~mtsCommandQueuedWrite() {}


    inline virtual mtsCommandQueuedWriteBase * Clone(mtsMailBox * mailBox, unsigned int size) const {
        return new mtsCommandQueuedWrite(mailBox, this->ActualCommand, size);
    }

    
    // Allocate should be called when a task calls GetMethodXXX().
    inline virtual void Allocate(unsigned int size) {
        if (ArgumentsQueue.GetSize() != size) {
            if (ArgumentsQueue.GetSize() > 0) {
                // Probably should never happen
                CMN_LOG(3) << "Class mtsCommandQueuedWrite: Allocate(): Changing ArgumentsQueue size from " << ArgumentsQueue.GetSize()
                           << " to " << size << std::endl;
            }
            const ArgumentType * argumentPrototype = dynamic_cast<const ArgumentType *>(this->GetArgumentPrototype());
            if (argumentPrototype) {
                ArgumentsQueue.SetSize(size, *argumentPrototype);
            } else {
                CMN_LOG(1) << "Class mtsCommandQueuedWrite: constructor: Can't find argument prototype from actual command."
                           << std::endl;
            }
        }
    }
    
    
    virtual mtsCommandBase::ReturnType Execute(const cmnGenericObject & argument) {
        if (this->IsEnabled()) {
            const ArgumentType * argumentTyped = dynamic_cast<const ArgumentType*>(&argument);
            if (!argumentTyped) {
                return mtsCommandBase::BAD_INPUT;
            }
            // Now, copy the argument to the local storage.
            if (ArgumentsQueue.Put(*argumentTyped)) {
                if (MailBox->Write(this)) {
                    return mtsCommandBase::DEV_OK;
                } else {
                    CMN_LOG(5) << "Class mtsCommandQueuedWrite: Execute(): Mailbox full for \"" 
                               << this->Name << "\"" << std::endl;
                    ArgumentsQueue.Get();  // Pop argument from local storage
                }
            } else {
                CMN_LOG(5) << "Class mtsCommandQueuedWrite: Execute(): ArgumentsQueue full for \""
                           << this->Name << "\"" << std::endl;
            }
            return mtsCommandBase::MAILBOX_FULL;
        }
        return mtsCommandBase::DISABLED;
    }

    /* commented in base class */
    const cmnGenericObject * GetArgumentPrototype(void) const {
        return this->ActualCommand->GetArgumentPrototype();
    }    

    inline virtual const cmnGenericObject * ArgumentPeek(void) const {
        return ArgumentsQueue.Peek();
    }
    

    inline virtual cmnGenericObject * ArgumentGet(void) {
        return ArgumentsQueue.Get();
    }
};

#endif // _mtsCommandQueuedWrite_h

