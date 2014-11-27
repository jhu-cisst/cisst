/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2005-2014 Johns Hopkins University (JHU), All Rights
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

/*! Write queued command using templated _argumentType parameter. Currently, this is only used
    for adding event handlers, and could perhaps be replaced by mtsCommandQueuedWriteGeneric. */
template <class _argumentType>
class mtsCommandQueuedWrite: public mtsCommandQueuedWriteBase
{
public:
    typedef mtsCommandQueuedWriteBase BaseType;
    typedef _argumentType ArgumentType;
    typedef typename mtsGenericTypes<ArgumentType>::FinalType ArgumentQueueType;
    typedef typename mtsGenericTypesUnwrap<ArgumentQueueType>::BaseType ArgumentQueueBaseType;

    /*! This type. */
    typedef mtsCommandQueuedWrite<ArgumentType> ThisType;

protected:
    /*! Queue to store arguments */
    mtsQueue<ArgumentQueueType> ArgumentsQueue;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandQueuedWrite(const ThisType & CMN_UNUSED(other));

public:

    inline mtsCommandQueuedWrite(void):
        BaseType(),
        ArgumentsQueue(0, ArgumentQueueType())
    {}


    inline mtsCommandQueuedWrite(mtsCommandWriteBase * actualCommand):
        BaseType(0, actualCommand, 0),
        ArgumentsQueue(0, ArgumentQueueType())
    {}


    inline mtsCommandQueuedWrite(mtsMailBox * mailBox, mtsCommandWriteBase * actualCommand, size_t size):
        BaseType(mailBox, actualCommand, size),
        ArgumentsQueue(0, ArgumentQueueType())
    {
        const ArgumentQueueType * argumentPrototype = dynamic_cast<const ArgumentQueueType *>(this->GetArgumentPrototype());
        if (argumentPrototype) {
            ArgumentsQueue.SetSize(size, *argumentPrototype);
            BlockingFlagQueue.SetSize(size, MTS_NOT_BLOCKING);
            mtsCommandWriteBase *cmd = 0;
            FinishedEventQueue.SetSize(size, cmd);
        } else {
            CMN_LOG_INIT_ERROR << "Class mtsCommandQueuedWrite: constructor: can't find argument prototype from actual command."
                               << std::endl;
        }
    }


    // ArgumentsQueue destructor should get called
    inline virtual ~mtsCommandQueuedWrite() {}


    inline virtual mtsCommandQueuedWriteBase * Clone(mtsMailBox * mailBox, size_t size) const {
        return new mtsCommandQueuedWrite(mailBox, this->ActualCommand, size);
    }


    // Allocate should be called when a task calls GetMethodXXX().
    inline virtual void Allocate(size_t size) {
        if (ArgumentsQueue.GetSize() != size) {
            if (ArgumentsQueue.GetSize() > 0) {
                // Probably should never happen
                CMN_LOG_INIT_WARNING << "Class mtsCommandQueuedWrite: Allocate(): changing ArgumentsQueue size from " << ArgumentsQueue.GetSize()
                                     << " to " << size << std::endl;
            }
            const ArgumentQueueType * argumentPrototype = dynamic_cast<const ArgumentQueueType *>(this->GetArgumentPrototype());
            if (argumentPrototype) {
                ArgumentsQueue.SetSize(size, *argumentPrototype);
                BlockingFlagQueue.SetSize(size, MTS_NOT_BLOCKING);
                mtsCommandWriteBase *cmd = 0;
                FinishedEventQueue.SetSize(size, cmd);
            } else {
                CMN_LOG_INIT_ERROR << "Class mtsCommandQueuedWrite: constructor: can't find argument prototype from actual command."
                                   << std::endl;
            }
        }
    }

    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsBlockingType blocking,
                               mtsCommandWriteBase * finishedEventHandler) {
        // check if this command is enabled
        if (!this->IsEnabled()) {
            return mtsExecutionResult::COMMAND_DISABLED;
        }
        // check if there is a mailbox (i.e. if the command is associated to an interface
        if (!MailBox) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: no mailbox for \""
                              << this->Name << "\"" << std::endl;
            return mtsExecutionResult::COMMAND_HAS_NO_MAILBOX;
        }
        // try to cast the input type
        const ArgumentQueueBaseType * argumentTyped = dynamic_cast<const ArgumentQueueBaseType*>(&argument);
        if (!argumentTyped) {
            return mtsExecutionResult::INVALID_INPUT_TYPE;
        }
        // check if all queues have some space
        if (ArgumentsQueue.IsFull() || BlockingFlagQueue.IsFull() || FinishedEventQueue.IsFull() || MailBox->IsFull()) {
            CMN_LOG_RUN_WARNING << "Class mtsCommandQueuedWrite: Execute: Queue full for \""
                                << this->Name << "\" ["
                                << ArgumentsQueue.IsFull() << "|"
                                << BlockingFlagQueue.IsFull() << "|"
                                << FinishedEventQueue.IsFull() << "|"
                                << MailBox->IsFull() << "]"
                                << std::endl;
            return mtsExecutionResult::COMMAND_ARGUMENT_QUEUE_FULL;
        }
        // copy the argument to the local storage.
        if (!ArgumentsQueue.Put(*argumentTyped)) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: ArgumentsQueue full for \""
                              << this->Name << "\"" << std::endl;
            cmnThrow("mtsCommandQueuedWrite: Execute: ArgumentsQueue.Put failed");
            return mtsExecutionResult::UNDEFINED;
        }
        // copy the blocking flag to the local storage.
        if (!BlockingFlagQueue.Put(blocking)) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: BlockingFlagQueue full for \""
                              << this->Name << "\"" << std::endl;
            ArgumentsQueue.Get(); // pop argument
            cmnThrow("mtsCommandQueuedWrite: Execute: BlockingFlagQueue.Put failed");
            return mtsExecutionResult::UNDEFINED;
        }
        // copy the finished event handler to the local storage.
        if (!FinishedEventQueue.Put(finishedEventHandler)) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: FinishedEventQueue.Put failed for \""
                              << this->Name << "\"" << std::endl;
            ArgumentsQueue.Get();       // Remove the argument that was already queued
            BlockingFlagQueue.Get();    // Remove the blocking flag that was already queued
            cmnThrow("mtsCommandQueuedWrite: Execute: FinishedEventQueue.Put failed");
            return mtsExecutionResult::UNDEFINED;
        }
        // finally try to queue to mailbox
        if (!MailBox->Write(this)) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: mailbox full for \""
                              << this->Name << "\"" << std::endl;
            ArgumentsQueue.Get();  // pop argument, blocking flag, and finished event from local storage
            BlockingFlagQueue.Get();
            FinishedEventQueue.Get();
            cmnThrow("mtsCommandQueuedWrite: Execute: MailBox.Write failed");
            return mtsExecutionResult::UNDEFINED;
        }
        return mtsExecutionResult::COMMAND_QUEUED;
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
};


/*! Write queued command using mtsGenericObject parameter. This is used for all
    queued write commands.  It is also used to create a generic event
    observer (combined with mtsMulticastCommandWriteBase) that can
    accept any payload (derived from mtsGenericObject). */
// PK: methods are defined in mtsCommandQueuedWriteBase.cpp
class CISST_EXPORT mtsCommandQueuedWriteGeneric: public mtsCommandQueuedWriteBase
{
 protected:
    typedef mtsCommandQueuedWriteBase BaseType;
    typedef mtsCommandQueuedWriteGeneric ThisType;

    size_t ArgumentQueueSize; // size used for queue
    /*! Queue to store arguments */
    mtsQueueGeneric ArgumentsQueue;

private:
    /*! Private default constructor to prevent use. */
    inline mtsCommandQueuedWriteGeneric(void);

    /*! Private copy constructor to prevent copies */
    inline mtsCommandQueuedWriteGeneric(const ThisType & CMN_UNUSED(other));

public:
    /*! Constructor, requires a mailbox to queue commands, a pointer
      on actual command and size used to create the argument queue.
      If the actual command doesn't provide an argument prototype, the
      argument queue is not allocated.  Queue allocation will
      potentially occur later, i.e. when SetArgumentPrototype is
      used.  This is useful when the queued command is added to a
      multicast command. */
    mtsCommandQueuedWriteGeneric(mtsMailBox * mailBox, mtsCommandWriteBase * actualCommand, size_t size);


    /*! Destructor */
    inline virtual ~mtsCommandQueuedWriteGeneric() {}


    virtual void ToStream(std::ostream & outputStream) const;

    inline virtual mtsCommandQueuedWriteGeneric * Clone(mtsMailBox * mailBox, size_t size) const {
        return new mtsCommandQueuedWriteGeneric(mailBox, this->ActualCommand, size);
    }


    // Allocate should be called when a task calls GetMethodXXX().
    virtual void Allocate(size_t size);


    inline virtual void SetArgumentPrototype(const mtsGenericObject * argumentPrototype) {
        this->ActualCommand->SetArgumentPrototype(argumentPrototype);
        this->Allocate(this->ArgumentQueueSize);
    }


    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsBlockingType blocking,
                               mtsCommandWriteBase *finishedEventHandler);


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
};

#endif // _mtsCommandQueuedWrite_h

