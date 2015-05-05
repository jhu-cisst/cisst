/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2010-09-24

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsEventReceiver.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

//************************************* mtsEventReceiverBase ***************************************************

mtsEventReceiverBase::mtsEventReceiverBase() : Name("UnknownEventReceiver"), Required(0), EventSignal(0), Waiting(false), OwnEventSignal(false)
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
    if (Required == 0)
        CMN_LOG_INIT_WARNING << "mtsEventReceiverBase: required interface not initialized. "
                             << "Please add event receiver to required interface before calling SetHandler." << std::endl;
    return (Required != 0);
}

bool mtsEventReceiverBase::WaitCommon()
{
    if (!EventSignal) {
        CMN_LOG_RUN_WARNING << "mtsEventReceiverBase: Creating local thread signal for event " << Name << std::endl;
        EventSignal = new osaThreadSignal;
        OwnEventSignal = true;
    }
    if (Waiting) {
         // This can only happen if we are called from multiple threads, which is a problem (not thread-safe).
        CMN_LOG_RUN_WARNING << "mtsEventReceiverBase: already waiting on event " << Name << std::endl;
        return false;
    }
    return true;
}

bool mtsEventReceiverBase::Wait()
{
    bool ret = WaitCommon();
    if (ret) {
        Waiting = true;
        EventSignal->Wait();
        Waiting = false;
        ret = true;
    }
    return ret;
}

bool mtsEventReceiverBase::WaitWithTimeout(double timeoutInSec)
{
    bool ret = WaitCommon();
    if (ret) {
        Waiting = true;
        ret = EventSignal->Wait(timeoutInSec);
        Waiting = false;
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

#if !CISST_HAS_SAFECASS_EXT
mtsCommandVoid * mtsEventReceiverVoid::GetCommand(void)
#else
mtsCommandVoid * mtsEventReceiverVoid::GetCommand(const std::string & componentName, const std::string & interfaceName)
#endif
{
    if (!Command) {
#if !CISST_HAS_SAFECASS_EXT
        Command = new mtsCommandVoid(new mtsCallableVoidMethod<mtsEventReceiverVoid>(&mtsEventReceiverVoid::EventHandler, this), Name);
#else
        Command = new mtsCommandVoid(new mtsCallableVoidMethod<mtsEventReceiverVoid>(&mtsEventReceiverVoid::EventHandler, this), Name,
                                     componentName, interfaceName);
#endif
    }
    return Command;
}

void mtsEventReceiverVoid::EventHandler(void)
{
    if (Waiting) {
        EventSignal->Raise();
    }
    if (UserHandler) {
        UserHandler->Execute(MTS_NOT_BLOCKING);
    }
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

#if !CISST_HAS_SAFECASS_EXT
mtsCommandWriteBase *mtsEventReceiverWrite::GetCommand()
#else
mtsCommandWriteBase *mtsEventReceiverWrite::GetCommand(const std::string & componentName, const std::string & interfaceName)
#endif
{
    if (!Command)
#if !CISST_HAS_SAFECASS_EXT
        Command = new mtsCommandWriteGeneric<mtsEventReceiverWrite>(&mtsEventReceiverWrite::EventHandler, this, Name, 0);
#else
        Command = new mtsCommandWriteGeneric<mtsEventReceiverWrite>(&mtsEventReceiverWrite::EventHandler, this, Name, 0,
                                                                    componentName, interfaceName);
#endif
    return Command;
}

void mtsEventReceiverWrite::EventHandler(const mtsGenericObject &arg)
{
    if (Waiting)
        EventSignal->Raise();
    if (ArgPtr && !ArgPtr->Services()->Create(ArgPtr, arg)) {
        CMN_LOG_RUN_ERROR << "mtsEventReceiverWrite: could not copy from " << arg.Services()->GetName()
                          << " to " << ArgPtr->Services()->GetName() << std::endl;
        ArgPtr = 0; // Set this to signal an error
    }
    if (UserHandler)
        UserHandler->Execute(arg, MTS_NOT_BLOCKING);
}

void mtsEventReceiverWrite::SetHandlerCommand(mtsCommandWriteBase *cmdHandler)
{
    if (cmdHandler != UserHandler) {
        if ((UserHandler != 0) && (cmdHandler != 0))
            CMN_LOG_INIT_WARNING << "SetHandlerCommand: changing event handler for write event " << GetName() << std::endl;
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
