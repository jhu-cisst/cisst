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
    for adding event handlers, and could perhaps be replaced by mtsCommandQueuedWriteGeneric. TODO BB: Can/should we replace? */
template <class _argumentType>
class mtsCommandQueuedWrite: public mtsCommandQueuedWriteBase
{
public:
    // TODO BB: evaluate how this interacts with mtsQueue<ArgumentQueueType>
    typedef mtsCommandQueuedWriteBase BaseType;
    typedef _argumentType ArgumentType;
    typedef typename mtsGenericTypes<ArgumentType>::FinalType ArgumentQueueType; // TODO BB: why not ArgumentType? Is goal to propagate IsValid/Timestamp?
    typedef typename mtsGenericTypesUnwrap<ArgumentQueueType>::BaseType ArgumentQueueBaseType;

    /*! This type. */
    typedef mtsCommandQueuedWrite<ArgumentType> ThisType;

protected:
    /*! Queue to store arguments */
    mtsQueue<ArgumentQueueType> ArgumentsQueue;

private:
    /*! Private copy constructor to prevent copies */
    mtsCommandQueuedWrite(const ThisType & CMN_UNUSED(other)) = delete;

public:
    mtsCommandQueuedWrite(void):
        BaseType(),
        ArgumentsQueue(0, ArgumentQueueType())
    {}


    mtsCommandQueuedWrite(mtsCommandWriteBase * actualCommand):
        BaseType(0, actualCommand, 0),
        ArgumentsQueue(0, ArgumentQueueType())
    {}


    mtsCommandQueuedWrite(mtsMailBox * mailBox, mtsCommandWriteBase * actualCommand, size_t size):
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


    virtual ~mtsCommandQueuedWrite() = default;


    virtual mtsCommandQueuedWriteBase * Clone(mtsMailBox * mailBox, size_t size) const {
        return new mtsCommandQueuedWrite(mailBox, this->ActualCommand, size);
    }


    // Allocate should be called when a task calls GetMethodXXX().
    virtual void Allocate(size_t size) {
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
        auto argument_reservation = ArgumentsQueue.Reserve(*argumentTyped);
        if (!argument_reservation) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: ArgumentsQueue.Reserve failed for \""
                              << this->Name << "\"" << std::endl;
            cmnThrow("mtsCommandQueuedWrite: Execute: ArgumentsQueue.Reserve failed");
            return mtsExecutionResult::UNDEFINED;
        }
        // copy the blocking flag to the local storage.
        auto block_reservation = BlockingFlagQueue.Reserve(blocking);
        if (!block_reservation) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: BlockingFlagQueue.Reserve failed for \""
                              << this->Name << "\"" << std::endl;
            cmnThrow("mtsCommandQueuedWrite: Execute: BlockingFlagQueue.Reserve failed");
            return mtsExecutionResult::UNDEFINED;
        }
        // copy the finished event handler to the local storage.
        auto finished_reservation = FinishedEventQueue.Reserve(finishedEventHandler);
        if (!finished_reservation) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: FinishedEventQueue.Reserve failed for \""
                              << this->Name << "\"" << std::endl;
            cmnThrow("mtsCommandQueuedWrite: Execute: FinishedEventQueue.Reserve failed");
            return mtsExecutionResult::UNDEFINED;
        }
        // finally try to queue to mailbox
        auto mailbox_reservation = MailBox->Write(this);
        if (!mailbox_reservation) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWrite: Execute: MailBox.Write failed for \""
                              << this->Name << "\"" << std::endl;
            cmnThrow("mtsCommandQueuedWrite: Execute: MailBox.Write failed");
            return mtsExecutionResult::UNDEFINED;
        }

        argument_reservation.commit();
        block_reservation.commit();
        finished_reservation.commit();
        mailbox_reservation.commit();

        return mtsExecutionResult::COMMAND_QUEUED;
    }

    /* commented in base class */
    const mtsGenericObject * GetArgumentPrototype(void) const {
        return this->ActualCommand->GetArgumentPrototype();
    }

    virtual const mtsGenericObject * ArgumentPeek(void) const {
        return ArgumentsQueue.Peek();
    }

    virtual void ArgumentPop(void) {
        ArgumentsQueue.Pop();
    }
};


/*! Write queued command using mtsGenericObject parameter. This is used for all
    queued write commands.  It is also used to create a generic event
    observer (combined with mtsMulticastCommandWriteBase) that can
    accept any payload (derived from mtsGenericObject). TODO BB: This that true? Class services / zero-allocation queue doesn't seem to work that way */
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
    mtsCommandQueuedWriteGeneric() = default;

    /*! Private copy constructor to prevent copies */
    mtsCommandQueuedWriteGeneric(const ThisType & CMN_UNUSED(other)) = delete;

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
    virtual ~mtsCommandQueuedWriteGeneric() {}


    virtual void ToStream(std::ostream & outputStream) const;

    // TODO BB: is this dangerous?
    virtual mtsCommandQueuedWriteGeneric * Clone(mtsMailBox * mailBox, size_t size) const {
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


    virtual const mtsGenericObject * ArgumentPeek(void) const {
        return ArgumentsQueue.Peek();
    }


    virtual void ArgumentPop(void) {
        ArgumentsQueue.Pop();
    }
};

#endif // _mtsCommandQueuedWrite_h

