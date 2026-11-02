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

mtsEventReceiverBase::mtsEventReceiverBase()
    : Name("UnknownEventReceiver"),
      Required(nullptr),
      EventSignal(nullptr),
      OwnEventSignal(false),
      WaitState(EVENT_RECEIVER_IDLE)
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
    EventSignal = nullptr;
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
    if (Required == nullptr) {
        CMN_LOG_INIT_WARNING << "mtsEventReceiverBase: required interface not initialized. "
                             << "Please add event receiver to required interface before calling SetHandler." << std::endl;
    }
    return Required != nullptr;
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
    if (!WaitCommon()) { return false; }

    {
        std::scoped_lock lock(WaitMutex);

        // WaitCommon already checked if WaitState is equal to EVENT_RECEIVER_WAITING, so following is
        // equivalent to checking whether WaitState is EVENT_RECEIVER_IDLE or EVENT_RECEIVER_PREPARING
        if (WaitState != EVENT_RECEIVER_SIGNALED) {
            WaitState = EVENT_RECEIVER_WAITING;
        }
    }

    // TODO - seems like there's a race condition where event is signaled after above check but before Wait()
    // Using std::condition_variable instead of EventSignal should fix this
    if (WaitState == EVENT_RECEIVER_WAITING) {
        EventSignal->Wait();
    }

    WaitState = EVENT_RECEIVER_IDLE;
    return true;
}

bool mtsEventReceiverBase::WaitWithTimeout(double timeoutInSec)
{
    if (!WaitCommon()) { return false; }

    {
        std::scoped_lock lock(WaitMutex);
        // WaitCommon already checked if WaitState is equal to EVENT_RECEIVER_WAITING, so following is
        // equivalent to checking whether WaitState is EVENT_RECEIVER_IDLE or EVENT_RECEIVER_PREPARING
        if (WaitState != EVENT_RECEIVER_SIGNALED) {
            WaitState = EVENT_RECEIVER_WAITING;
        }
    }

    bool success = true;
    if (WaitState == EVENT_RECEIVER_WAITING) {
        success = EventSignal->Wait(timeoutInSec);
        // If timed out, can check if event may have been raised (and missed)
        // either just before or just after the call to Wait.
        if (!success && (WaitState == EVENT_RECEIVER_SIGNALED)) {
            success = true;
        }
    }

    WaitState = EVENT_RECEIVER_IDLE;
    return success;
}

void mtsEventReceiverBase::Detach()
{
    // PK: to be implemented
    // TODO
}

//************************************* mtsEventReceiverVoid ***************************************************

mtsEventReceiverVoid::mtsEventReceiverVoid() : mtsEventReceiverBase(), Command(nullptr), UserHandler(nullptr)
{}

mtsCommandVoid* mtsEventReceiverVoid::GetCommand(void)
{
    if (!Command) {
        Command = std::make_unique<mtsCommandVoid>(
            new mtsCallableVoidMethod<mtsEventReceiverVoid>(&mtsEventReceiverVoid::EventHandler, this),
            Name
        );
    }

    return Command.get();
}

void mtsEventReceiverVoid::EventHandler(void)
{
    if (UserHandler) {
        UserHandler->Execute(MTS_NOT_BLOCKING);
    }

    WaitStates previous_state;
    {
        std::scoped_lock lock(WaitMutex);
        previous_state = WaitState;
        if ((WaitState == EVENT_RECEIVER_PREPARING) || (WaitState == EVENT_RECEIVER_WAITING)) {
            WaitState = EVENT_RECEIVER_SIGNALED;
        }
    }

    if (previous_state == EVENT_RECEIVER_WAITING) {
        EventSignal->Raise();
    }
}

void mtsEventReceiverVoid::SetHandlerCommand(mtsCommandVoid * commandHandler)
{
    if (commandHandler != UserHandler) {
        if ((UserHandler != nullptr) && (commandHandler != nullptr)) {
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

mtsEventReceiverWrite::mtsEventReceiverWrite() : mtsEventReceiverBase(), Command(nullptr), UserHandler(nullptr), ArgPtr(nullptr)
{}

mtsCommandWriteBase* mtsEventReceiverWrite::GetCommand()
{
    if (!Command) {
        Command = std::make_unique<mtsCommandWriteGeneric<mtsEventReceiverWrite>>(&mtsEventReceiverWrite::EventHandler, this, Name, nullptr);
    }

    return Command.get();
}

void mtsEventReceiverWrite::EventHandler(const mtsGenericObject &arg)
{
    // Copy from arg to *ArgPtr. In many cases (e.g., for VoidReturn and WriteReturn commands)
    // &arg == ArgPtr (point to same memory), but this is handled in cmnClassServices::Create.
    if (ArgPtr && !ArgPtr->Services()->Create(ArgPtr, arg)) {
        CMN_LOG_RUN_ERROR << "mtsEventReceiverWrite: could not copy from " << arg.Services()->GetName()
                          << " to " << ArgPtr->Services()->GetName() << std::endl;
        ArgPtr = nullptr; // Set this to signal an error
    }
    if (UserHandler) {
        UserHandler->Execute(arg, MTS_NOT_BLOCKING);
    }

    WaitStates previous_state;
    {
        std::scoped_lock lock(WaitMutex);
        previous_state = WaitState;
        if ((WaitState == EVENT_RECEIVER_PREPARING) || (WaitState == EVENT_RECEIVER_WAITING)) {
            WaitState = EVENT_RECEIVER_SIGNALED;
        }
    }

    if (previous_state == EVENT_RECEIVER_WAITING) {
        EventSignal->Raise();
    }
}

void mtsEventReceiverWrite::SetHandlerCommand(mtsCommandWriteBase *cmdHandler)
{
    if (cmdHandler != UserHandler) {
        if ((UserHandler != 0) && (cmdHandler != nullptr)) {
            CMN_LOG_INIT_WARNING << "SetHandlerCommand: changing event handler for write event " << GetName() << std::endl;
        }
        UserHandler = cmdHandler;
    }
}

bool mtsEventReceiverWrite::PrepareToWait(mtsGenericObject &obj)
{
    bool ret = mtsEventReceiverBase::PrepareToWait();
    if (ret) {
        ArgPtr = &obj;
    }
    return ret;
}

// Here, a false return value could mean that the wait failed, or that the wait succeeded but the return object
// is invalid. Note that the return object was specified in PrepareToWait.
bool mtsEventReceiverWrite::Wait()
{
    if (ArgPtr == nullptr) {
        CMN_LOG_RUN_WARNING << "mtsEventReceverWrite::Wait -- null ArgPtr (should first call PrepareToWait)" << std::endl;
        // Wait anyway, but will return false
    }
    bool ret = mtsEventReceiverBase::Wait();
    if (ArgPtr == nullptr) ret = false;
    return ret;
}

// Here, a false return value could mean that the wait failed, or that the wait succeeded but the return object
// is invalid. Note that the return object was specified in PrepareToWait.
bool mtsEventReceiverWrite::WaitWithTimeout(double timeoutInSec)
{
    if (ArgPtr == nullptr) {
        CMN_LOG_RUN_WARNING << "mtsEventReceverWrite::WaitWithTimeout -- null ArgPtr (should first call PrepareToWait)" << std::endl;
        // Wait anyway, but will return false
    }
    bool ret = mtsEventReceiverBase::WaitWithTimeout(timeoutInSec);
    if (ArgPtr == nullptr) ret = false;
    return ret;
}

void mtsEventReceiverWrite::ClearWait()
{
    mtsEventReceiverBase::ClearWait();
    ArgPtr = nullptr;
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
