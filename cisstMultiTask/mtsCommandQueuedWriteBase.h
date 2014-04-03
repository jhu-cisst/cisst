/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2005-2014 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstMultiTask/mtsCommandWriteBase.h>
#include <cisstMultiTask/mtsMailBox.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class mtsCommandWriteBase;

class CISST_EXPORT mtsCommandQueuedWriteBase: public mtsCommandWriteBase {
protected:
    typedef mtsCommandWriteBase BaseType;
    mtsMailBox * MailBox;
    mtsCommandWriteBase * ActualCommand;

    /*! Queue of flags to indicate if the command is blocking or
      not */
    mtsQueue<mtsBlockingType> BlockingFlagQueue;

    /*! Queue for return events (to send result to caller).
        If non-zero, this indicates that a blocking call was made
        (previously, this was a BlockingFlagQueue). */
    mtsQueue<mtsCommandWriteBase *> FinishedEventQueue;

    inline mtsCommandQueuedWriteBase(void):
        BaseType("??"),
        MailBox(0),
        ActualCommand(0),
        BlockingFlagQueue(0, MTS_NOT_BLOCKING)
    {
        mtsCommandWriteBase *cmd = 0;
        FinishedEventQueue.SetSize(0, cmd);
    }

public:
    inline mtsCommandQueuedWriteBase(mtsMailBox * mailBox, mtsCommandWriteBase * actualCommand, size_t size):
        BaseType(actualCommand->GetName()),
        MailBox(mailBox),
        ActualCommand(actualCommand),
        BlockingFlagQueue(size, MTS_NOT_BLOCKING)
    {
        mtsCommandWriteBase *cmd = 0;
        FinishedEventQueue.SetSize(size, cmd);
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


    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsBlockingType blocking);


    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsBlockingType blocking,
                               mtsCommandWriteBase *finishedEventHandler) = 0;


    virtual const mtsGenericObject * ArgumentPeek(void) const = 0;


    virtual mtsGenericObject * ArgumentGet(void) = 0;

    mtsBlockingType BlockingFlagGet(void);

    mtsCommandWriteBase *FinishedEventGet(void);

    inline virtual const std::string GetMailBoxName(void) const {
        return this->MailBox ? this->MailBox->GetName() : "NULL";
    }
};

#endif // _mtsCommandQueuedWrite_h
