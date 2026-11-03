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
      OwnEventSignal(false)
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

void mtsEventReceiverBase::SetThreadSignal(osaThreadSignal* signal)
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
    if (is_waiting) {
        // Cannot call from multiple threads (not thread-safe).
        CMN_LOG_RUN_WARNING << "mtsEventReceiverBase: already waiting on event " << Name << std::endl;
        return false;
    }
    return true;
}

void mtsEventReceiverBase::ClearWait()
{
    is_waiting = false;
    EventSignal->Clear();
}

bool mtsEventReceiverBase::Wait()
{
    if (!WaitCommon()) { return false; }

    EventSignal->Wait();
    return EventSignal->raised();
}

bool mtsEventReceiverBase::WaitWithTimeout(double timeoutInSec)
{
    if (!WaitCommon()) { return false; }

    EventSignal->Wait(timeoutInSec);
    return EventSignal->raised();
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

    EventSignal->Raise();
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
        Command = std::make_unique<mtsCommandWriteGeneric<mtsEventReceiverWrite>>(
            &mtsEventReceiverWrite::EventHandler,
            this,
            Name,
            nullptr
        );
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

    EventSignal->Raise();
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

// Here, a false return value could mean that the wait failed, or that the wait succeeded but the return object
// is invalid.
bool mtsEventReceiverWrite::Wait(mtsGenericObject& arg)
{
    ArgPtr = &arg;
    bool ret = mtsEventReceiverBase::Wait();
    if (ArgPtr == nullptr) ret = false;
    return ret;
}

// Here, a false return value could mean that the wait failed, or that the wait succeeded but the return object
// is invalid.
bool mtsEventReceiverWrite::WaitWithTimeout(mtsGenericObject& arg, double timeoutInSec)
{
    ArgPtr = &arg;
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
