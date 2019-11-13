/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2010-09-24

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsEventReceiver.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

//************************************* mtsEventReceiverBase ***************************************************

mtsEventReceiverBase::mtsEventReceiverBase() : Name("UnknownEventReceiver"), Required(0), EventSignal(0), OwnEventSignal(false), WaitState(EVENT_RECEIVER_IDLE)
{}

mtsEventReceiverBase::~mtsEventReceiverBase()
{
    if (OwnEventSignal) {
        delete EventSignal;
    }
}

void mtsEventReceiverBase::SetRequired(const std::string & name, mtsInterfaceRequired * interfaceRequired)
{
    SetName(name);
    Required = interfaceRequired;
    EventSignal = 0;
    if (Required && (Required->MailBox)) {
        SetThreadSignal(Required->GetThreadSignal());
    }
}

void mtsEventReceiverBase::SetThreadSignal(osaThreadSignal *signal)
{
    EventSignal = signal;
}

bool mtsEventReceiverBase::CheckRequired() const
{
    if (Required == 0) {
        CMN_LOG_INIT_WARNING << "mtsEventReceiverBase: required interface not initialized. "
                             << "Please add event receiver to required interface before calling SetHandler." << std::endl;
    }
    return (Required != 0);
}

void mtsEventReceiverBase::CheckEventSignal()
{
    if (!EventSignal) {
        CMN_LOG_RUN_WARNING << "mtsEventReceiverBase: Creating local thread signal for event " << Name << std::endl;
        EventSignal = new osaThreadSignal;
        OwnEventSignal = true;
    }
}

bool mtsEventReceiverBase::WaitCommon()
{
    CheckEventSignal();
    if (WaitState == EVENT_RECEIVER_WAITING) {
        // Cannot call from multiple threads (not thread-safe).
        CMN_LOG_RUN_WARNING << "mtsEventReceiverBase: already waiting on event " << Name << std::endl;
        return false;
    }
    return true;
}

void mtsEventReceiverBase::ClearWait()
{
    WaitState = EVENT_RECEIVER_IDLE;
}

bool mtsEventReceiverBase::PrepareToWait()
{
    CheckEventSignal();
    // Not locking mutex because other thread should not yet be involved
    if (WaitState == EVENT_RECEIVER_IDLE)
        WaitState = EVENT_RECEIVER_PREPARING;
    else {
        // Cannot call from multiple threads (not thread-safe). Of course, we do not know if
        // this has been called from multiple threads (programmer could have accidentally called
        // PrepareToWait twice from same thread), but anyway that should be discouraged.
        CMN_LOG_RUN_WARNING << "mtsEventReceiverBase: already waiting on event " << Name
                            << ", state = " << WaitState << std::endl;
        return false;
    }
    return true;
}

bool mtsEventReceiverBase::Wait()
{
    bool ret = WaitCommon();
    if (ret) {
        WaitMutex.Lock();
        // WaitCommon already checked if WaitState is equal to EVENT_RECEIVER_WAITING, so following is
        // equivalent to checking whether WaitState is EVENT_RECEIVER_IDLE or EVENT_RECEIVER_PREPARING
        if (WaitState != EVENT_RECEIVER_SIGNALED)
            WaitState = EVENT_RECEIVER_WAITING;
        WaitMutex.Unlock();
        if (WaitState == EVENT_RECEIVER_WAITING)
            EventSignal->Wait();
        WaitState = EVENT_RECEIVER_IDLE;
    }
    return ret;
}

bool mtsEventReceiverBase::WaitWithTimeout(double timeoutInSec)
{
    bool ret = WaitCommon();
    if (ret) {
        WaitMutex.Lock();
        // WaitCommon already checked if WaitState is equal to EVENT_RECEIVER_WAITING, so following is
        // equivalent to checking whether WaitState is EVENT_RECEIVER_IDLE or EVENT_RECEIVER_PREPARING
        if (WaitState != EVENT_RECEIVER_SIGNALED)
            WaitState = EVENT_RECEIVER_WAITING;
        WaitMutex.Unlock();
        if (WaitState == EVENT_RECEIVER_WAITING) {
            ret = EventSignal->Wait(timeoutInSec);
            // If timed out, can check if event may have been raised (and missed)
            // either just before or just after the call to Wait.
            if (!ret && (WaitState == EVENT_RECEIVER_SIGNALED))
                ret = true;
        }
        WaitState = EVENT_RECEIVER_IDLE;
    }
    return ret;
}

void mtsEventReceiverBase::Detach()
{
    // PK: to be implemented
}

//************************************* mtsEventReceiverVoid ***************************************************

mtsEventReceiverVoid::mtsEventReceiverVoid() : mtsEventReceiverBase(), Command(0), UserHandler(0)
{}

mtsEventReceiverVoid::~mtsEventReceiverVoid()
{
    delete Command;
}

mtsCommandVoid * mtsEventReceiverVoid::GetCommand(void)
{
    if (!Command) {
        Command = new mtsCommandVoid(new mtsCallableVoidMethod<mtsEventReceiverVoid>(&mtsEventReceiverVoid::EventHandler, this), Name);
    }
    return Command;
}

void mtsEventReceiverVoid::EventHandler(void)
{
    if (UserHandler) {
        UserHandler->Execute(MTS_NOT_BLOCKING);
    }
    WaitMutex.Lock();
    WaitStates oldWaitState = WaitState;
    if ((WaitState == EVENT_RECEIVER_PREPARING) || (WaitState == EVENT_RECEIVER_WAITING))
        WaitState = EVENT_RECEIVER_SIGNALED;
    WaitMutex.Unlock();
    if (oldWaitState == EVENT_RECEIVER_WAITING)
        EventSignal->Raise();
}

void mtsEventReceiverVoid::SetHandlerCommand(mtsCommandVoid * commandHandler)
{
    if (commandHandler != UserHandler) {
        if ((UserHandler != 0) && (commandHandler != 0)) {
            CMN_LOG_INIT_WARNING << "SetHandlerCommand: changing event handler for void event \"" << GetName() << "\"" << std::endl;
        }
        UserHandler = commandHandler;
    }
}

bool mtsEventReceiverVoid::RemoveHandler(void)
{
    return CheckRequired() ? (Required->RemoveEventHandlerVoid(this->GetName())) : false;
}

void mtsEventReceiverVoid::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsEventReceiverVoid for " << this->Name;
    if (UserHandler)
        outputStream << ", handler = " << UserHandler->GetName();
}

//************************************* mtsEventReceiverWrite ***************************************************

mtsEventReceiverWrite::mtsEventReceiverWrite() : mtsEventReceiverBase(), Command(0), UserHandler(0), ArgPtr(0)
{}

mtsEventReceiverWrite::~mtsEventReceiverWrite()
{
    delete Command;
}

mtsCommandWriteBase *mtsEventReceiverWrite::GetCommand()
{
    if (!Command)
        Command = new mtsCommandWriteGeneric<mtsEventReceiverWrite>(&mtsEventReceiverWrite::EventHandler, this, Name, 0);
    return Command;
}

void mtsEventReceiverWrite::EventHandler(const mtsGenericObject &arg)
{
    // Copy from arg to *ArgPtr. In many cases (e.g., for VoidReturn and WriteReturn commands)
    // &arg == ArgPtr (point to same memory), but this is handled in cmnClassServices::Create.
    if (ArgPtr && !ArgPtr->Services()->Create(ArgPtr, arg)) {
        CMN_LOG_RUN_ERROR << "mtsEventReceiverWrite: could not copy from " << arg.Services()->GetName()
                          << " to " << ArgPtr->Services()->GetName() << std::endl;
        ArgPtr = 0; // Set this to signal an error
    }
    if (UserHandler)
        UserHandler->Execute(arg, MTS_NOT_BLOCKING);
    WaitMutex.Lock();
    WaitStates oldWaitState = WaitState;
    if ((WaitState == EVENT_RECEIVER_PREPARING) || (WaitState == EVENT_RECEIVER_WAITING))
        WaitState = EVENT_RECEIVER_SIGNALED;
    WaitMutex.Unlock();
    if (oldWaitState == EVENT_RECEIVER_WAITING)
        EventSignal->Raise();
}

void mtsEventReceiverWrite::SetHandlerCommand(mtsCommandWriteBase *cmdHandler)
{
    if (cmdHandler != UserHandler) {
        if ((UserHandler != 0) && (cmdHandler != 0)) {
            CMN_LOG_INIT_WARNING << "SetHandlerCommand: changing event handler for write event " << GetName() << std::endl;
        }
        UserHandler = cmdHandler;
    }
}

// Here, a false return value could mean that the wait failed, or that the wait succeeded but the return value (obj)
// is invalid.
bool mtsEventReceiverWrite::Wait(mtsGenericObject &obj)
{
    ArgPtr = &obj;
    bool ret = mtsEventReceiverBase::Wait();
    if (ArgPtr == 0) ret = false;
    ArgPtr = 0;
    return ret;
}

// Here, a false return value could mean that the wait failed, or that the wait succeeded but the return value (obj)
// is invalid.
bool mtsEventReceiverWrite::WaitWithTimeout(double timeoutInSec, mtsGenericObject &obj)
{
    ArgPtr = &obj;
    bool ret = mtsEventReceiverBase::WaitWithTimeout(timeoutInSec);
    if (ArgPtr == 0) ret = false;
    ArgPtr = 0;
    return ret;
}

bool mtsEventReceiverWrite::RemoveHandler(void)
{
    return CheckRequired() ? (Required->RemoveEventHandlerWrite(this->GetName())) : false;
}

void mtsEventReceiverWrite::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsEventReceiverWrite for " << this->Name;
    if (UserHandler)
        outputStream << ", handler = " << UserHandler->GetName();
}
