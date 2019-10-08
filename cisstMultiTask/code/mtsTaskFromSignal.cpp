/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2009-12-10

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>


mtsTaskFromSignal::mtsTaskFromSignal(const std::string & name,
                                     unsigned int sizeStateTable):
    mtsTaskContinuous(name, sizeStateTable),
    PostCommandQueuedCallable(0)
{
    this->Init();
}


mtsTaskFromSignal::mtsTaskFromSignal(const mtsTaskConstructorArg & arg):
    mtsTaskContinuous(arg.Name, arg.StateTableSize),
    PostCommandQueuedCallable(0)
{
    this->Init();
}


void mtsTaskFromSignal::Init(void)
{
    this->PostCommandQueuedCallable = new mtsCallableVoidMethod<mtsTaskFromSignal>(&mtsTaskFromSignal::PostCommandQueuedMethod,
                                                                                   this);
    if (!this->PostCommandQueuedCallable) {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: can't create post command queued callable based on method." << std::endl;
    }
}


void mtsTaskFromSignal::PostCommandQueuedMethod(void) {
    this->Thread.Wakeup();
}


void * mtsTaskFromSignal::RunInternal(void * CMN_UNUSED(data)) {

    if (ExecIn && ExecIn->GetConnectedInterface()) {
        CMN_LOG_CLASS_RUN_ERROR << "RunInternal for " << this->GetName()
                                << " called, even though task receives thread from "
                                << ExecIn->GetConnectedInterface()->GetComponent()->GetName() << std::endl;
        return 0;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: begin task " << this->GetName() << std::endl;
    if (this->State == mtsComponentState::INITIALIZING) {
        this->StartupInternal();
    }

    // Use a local variable, currentState, because otherwise we can have a problem when this->State is
    // changed by another thread. Specifically, if the state is changed from READY to ACTIVE in between
    // these conditions, then both will evaluate to false.
    mtsComponentState currentState = this->State;
    while ((currentState == mtsComponentState::ACTIVE) || (currentState == mtsComponentState::READY)) {
        while (this->State == mtsComponentState::READY) {
            // Suspend the task until there is a call to Start().
            CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: wait to start task \"" << this->GetName() << "\"" << std::endl;
            WaitForWakeup();
        }
        if (this->State == mtsComponentState::ACTIVE) {
            DoRunInternal();
            // put the task to sleep until next signal
            Thread.WaitForWakeup();
        }
        currentState = this->State;
    }

    if (this->State == mtsComponentState::FINISHING) {
        CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: end of task \"" << this->GetName() << "\"" << std::endl;
        this->CleanupInternal();
    }
    void * returnValue = this->ReturnValue;
    return returnValue;
}


void mtsTaskFromSignal::Kill(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Kill: task \"" << this->GetName() << "\", current state \"" << this->State << "\"" << std::endl;
    mtsTask::Kill();
    // only difference is that we need to wake up the thread to make sure it processes the request
    this->Thread.Wakeup();
}


mtsInterfaceRequired * mtsTaskFromSignal::AddInterfaceRequiredWithoutSystemEventHandlers(const std::string & interfaceRequiredName,
                                                                                         mtsRequiredType required)
{
    // create a mailbox with post command queued command
    mtsMailBox * mailBox = new mtsMailBox(interfaceRequiredName + "Events",
                                          mtsInterfaceRequired::DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE,
                                          this->PostCommandQueuedCallable);
    mtsInterfaceRequired * result;
    if (mailBox) {
        // try to create and add interface
        result = this->AddInterfaceRequiredUsingMailbox(interfaceRequiredName, mailBox, required);
        if (!result) {
            delete mailBox;
        }
        return result;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceRequired: unable to create mailbox for \""
                             << interfaceRequiredName << "\"" << std::endl;
    delete mailBox;
    return 0;
}


mtsInterfaceProvided * mtsTaskFromSignal::AddInterfaceProvidedWithoutSystemEvents(const std::string & interfaceProvidedName,
                                                                                  mtsInterfaceQueueingPolicy queueingPolicy,
                                                                                  bool isProxy)
{
    mtsInterfaceProvided * interfaceProvided;
    if ((queueingPolicy == MTS_COMPONENT_POLICY)
        || (queueingPolicy == MTS_COMMANDS_SHOULD_BE_QUEUED)) {
        mtsCallableVoidBase * postCommandQueuedCallable = this->PostCommandQueuedCallable;
        // If the internal provided interface (i.e., for the Manager Component Client), then the "post command queued"
        // callable object should be the one defined in mtsTask, which processes the mailbox if the task is not active.
        // Note that if the task is active, we always wait for the task's DoRunInternal method to process this mailbox.
        if (interfaceProvidedName == mtsManagerComponentBase::GetNameOfInterfaceInternalProvided())
            postCommandQueuedCallable = InterfaceProvidedToManagerCallable;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_BE_QUEUED, postCommandQueuedCallable, isProxy);
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "AddInterfaceProvided: adding provided interface \"" << interfaceProvidedName
                                   << "\" with policy MTS_COMMANDS_SHOULD_NOT_BE_QUEUED to task \""
                                   << this->GetName() << "\". This bypasses built-in thread safety mechanisms, make sure your commands are thread safe.  "
                                   << "Furthermore, the thread will not wake up since the post queued command will not be executed. "
                                   << std::endl;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED, 0, isProxy);
    }
    if (interfaceProvided) {
        if (InterfacesProvided.AddItem(interfaceProvidedName, interfaceProvided)) {
            return interfaceProvided;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: task \"" << this->GetName() << "\" unable to add interface \""
                                 << interfaceProvidedName << "\"" << std::endl;
        delete interfaceProvided;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: task \"" << this->GetName() << "\" unable to create interface \""
                             << interfaceProvidedName << "\"" << std::endl;
    return 0;
}
