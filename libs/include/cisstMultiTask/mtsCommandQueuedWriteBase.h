/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines base class for a queued write command.
*/

#ifndef _mtsCommandQueuedWriteBase_h
#define _mtsCommandQueuedWriteBase_h

#include <cisstMultiTask/mtsCommandReadOrWriteBase.h>
#include <cisstMultiTask/mtsMailBox.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsCommandQueuedWriteBase: public mtsCommandWriteBase {
protected:
    typedef mtsCommandWriteBase BaseType;
    mtsMailBox * MailBox;
    mtsCommandWriteBase * ActualCommand;

private:
    inline mtsCommandQueuedWriteBase(void):
        BaseType("??"),
        MailBox(0),
        ActualCommand(0)
    {}

public:
    inline mtsCommandQueuedWriteBase(mtsMailBox * mailBox, mtsCommandWriteBase * actualCommand):
        BaseType(actualCommand->GetName()),
        MailBox(mailBox),
        ActualCommand(actualCommand)
    {
        this->SetArgumentPrototype(ActualCommand->GetArgumentPrototype());
    }


    inline virtual ~mtsCommandQueuedWriteBase() {}


    inline virtual mtsCommandWriteBase * GetActualCommand(void) {
        return ActualCommand;
    }


    virtual void ToStream(std::ostream & outputStream) const;


    virtual mtsCommandQueuedWriteBase * Clone(mtsMailBox* mailBox, size_t size) const = 0;


    // Allocate should be called when a task calls GetMethodXXX().
    virtual void Allocate(size_t size) = 0;


    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument) = 0;


    virtual const mtsGenericObject * ArgumentPeek(void) const = 0;


    virtual mtsGenericObject * ArgumentGet(void) = 0;


    inline virtual const std::string & GetMailBoxName(void) const {
        return MailBox->GetName();
    }
};



/*! Write queued command using mtsGenericObject pointer.  Since
  mtsCommandQueuedWriteGeneric is not templated, there is no need to
  have distinct classes therefore the base class contains all
  required methods and doesn't need to be derived.
  mtsCommandQueuedWriteGeneric is defined as a typedef of
  mtsCommandQueuedWriteGenericBase.

  mtsCommandQueuedWriteGeneric is used to create a generic event
  observer (combined with mtsMulticastCommandWriteBase) that can
  accept any payload (derived from mtsGenericObject. */
class CISST_EXPORT mtsCommandQueuedWriteGenericBase: public mtsCommandWriteGenericBase 
{
 protected:
    typedef mtsCommandWriteGenericBase BaseType;
    typedef mtsCommandQueuedWriteGenericBase ThisType;

    mtsMailBox * MailBox;
    mtsCommandWriteGenericBase * ActualCommand;

    size_t ArgumentQueueSize; // size used for queue
    mtsQueueGeneric ArgumentsQueue;

private:

private:
    /*! Private default construtor to prevent use. */
    inline mtsCommandQueuedWriteGenericBase(void);

    /*! Private copy constructor to prevent copies */
    inline mtsCommandQueuedWriteGenericBase(const ThisType & CMN_UNUSED(other));

public:
    /*! Constructor, requires a mailbox to queue commands, a pointer
      on actual command and size used to create the argument queue.
      If the actual command doesn't provide an argument prototype, the
      argument queue is not allocated.  Queue allocation will
      protentially occur later, i.e. when SetArgumentPrototype is
      used.  This is useful when the queue command is added to a
      multicast command. */
    inline mtsCommandQueuedWriteGenericBase(mtsMailBox * mailBox, mtsCommandWriteGenericBase * actualCommand, size_t size):
        BaseType(actualCommand->GetName()),
        MailBox(mailBox),
        ActualCommand(actualCommand),
        ArgumentQueueSize(size),
        ArgumentsQueue()
    {
        if (this->ActualCommand) {
            this->SetArgumentPrototype(ActualCommand->GetArgumentPrototype());
        }
        const mtsGenericObject * argumentPrototype = dynamic_cast<const mtsGenericObject *>(this->GetArgumentPrototype());
        if (argumentPrototype) {
            ArgumentsQueue.SetSize(size, *argumentPrototype);
        } else {
            CMN_LOG_INIT_DEBUG << "Class mtsCommandQueuedWriteGeneric: constructor: can't find argument prototype from actual command \""
                               << this->GetName() << "\"" << std::endl;
        }
    }

    /*! Destructor */
    inline virtual ~mtsCommandQueuedWriteGenericBase() {
    }


    inline virtual mtsCommandWriteGenericBase * GetActualCommand(void) {
        return ActualCommand;
    }


    virtual void ToStream(std::ostream & outputStream) const;


    inline virtual mtsCommandQueuedWriteGenericBase * Clone(mtsMailBox * mailBox, size_t size) const {
        return new mtsCommandQueuedWriteGenericBase(mailBox, this->ActualCommand, size);
    }


    // Allocate should be called when a task calls GetMethodXXX().
    inline virtual void Allocate(size_t size)
    {
        if (ArgumentsQueue.GetSize() != size) {
            if (ArgumentsQueue.GetSize() > 0) {
                // Probably should never happen
                CMN_LOG_INIT_WARNING << "Class mtsCommandQueuedWriteGeneric: Allocate: changing ArgumentsQueue size from " << ArgumentsQueue.GetSize()
                                     << " to " << size << std::endl;
            }
            const mtsGenericObject * argumentPrototype = dynamic_cast<const mtsGenericObject *>(this->GetArgumentPrototype());
            if (argumentPrototype) {
                CMN_LOG_INIT_DEBUG << "Class mtsCommandQueuedWriteGeneric: Allocate: resizing argument queue to " << size
                                   << " with \"" << argumentPrototype->Services()->GetName() << "\"" << std::endl; 
                ArgumentsQueue.SetSize(size, *argumentPrototype);
            } else {
                CMN_LOG_INIT_ERROR << "Class mtsCommandQueuedWriteGeneric: Allocate: can't find argument prototype from actual command \""
                                   << this->GetName() << "\"" << std::endl;
            }
        }
    }

    
    inline virtual void SetArgumentPrototype(const mtsGenericObject * argumentPrototype) {
        this->ActualCommand->SetArgumentPrototype(argumentPrototype);
        this->Allocate(this->ArgumentQueueSize);
    }


    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject * argument) {
        if (this->IsEnabled()) {
            const mtsGenericObject * argumentTyped = dynamic_cast<const mtsGenericObject *>(argument);
            if (!argumentTyped) {
                return mtsCommandBase::BAD_INPUT;
            }
            // Now, copy the argument to the local storage.
            if (ArgumentsQueue.Put(*argumentTyped)) {
                if (MailBox->Write(this)) {
                    return mtsCommandBase::DEV_OK;
                } else {
                    CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteGeneric: Execute: mailbox full for \""
                                      << this->Name << "\"" << std::endl;
                    ArgumentsQueue.Get();  // Pop argument from local storage
                }
            } else {
                CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteGeneric: Execute: ArgumentsQueue full for \""
                                  << this->Name << "\"" << std::endl;
            }
            return mtsCommandBase::MAILBOX_FULL;
        }
        return mtsCommandBase::DISABLED;
    }


    /* commented in base class */
    const mtsGenericObject * GetArgumentPrototype(void) const {
        return this->ActualCommand->GetArgumentPrototype();
    }


    inline virtual const mtsGenericObject * ArgumentPeek(void) const {
        return ArgumentsQueue.Peek();
    }


    inline virtual mtsGenericObject * ArgumentGet(void) {
        return ArgumentsQueue.Get();
    }


    inline virtual const std::string & GetMailBoxName(void) const {
        return MailBox->GetName();
    }
};


#endif // _mtsCommandQueuedWrite_h

