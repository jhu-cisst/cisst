/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */ /* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstCommon/cmnExport.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaGetTime.h>

#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>
#if CISST_HAS_SAFETY_PLUGINS
#include <cisstMultiTask/mtsMonitorFilterBase.h>
#endif

#include <iostream>

#if CISST_HAS_SAFETY_PLUGINS
#include "dict.h"
#endif

/********************* Methods that call user methods *****************/

void mtsTask::DoRunInternal(void)
{
    StateTables.ForEachVoid(&mtsStateTable::StartIfAutomatic);
    // Make sure following is called
    if (InterfaceProvidedToManager)
        InterfaceProvidedToManager->ProcessMailBoxes();
    this->Run();
    // advance all state tables (if automatic)
    StateTables.ForEachVoid(&mtsStateTable::AdvanceIfAutomatic);
}

void mtsTask::StartupInternal(void) {
    CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: started for task \"" << this->GetName() << "\"" << std::endl;

    // Loop through the required interfaces and make sure they are all connected. This extra check is probably not needed.
    bool success = true;
    InterfacesRequiredOrInputMapType::const_iterator requiredIterator = InterfacesRequiredOrInput.begin();
    const mtsInterfaceProvidedOrOutput * connectedInterface;
    for (;
         requiredIterator != InterfacesRequiredOrInput.end();
         requiredIterator++) {
        connectedInterface = requiredIterator->second->GetConnectedInterface();
        if (!connectedInterface) {
            if (requiredIterator->second->IsRequired() == MTS_REQUIRED) {
                CMN_LOG_CLASS_INIT_ERROR << "StartupInternal: void pointer to required/input interface \""
                                         << this->GetName() << ":" << requiredIterator->first
                                         << "\" (required/input not connected to provided/output)" << std::endl;
                success = false;
            } else {
                CMN_LOG_CLASS_INIT_WARNING << "StartupInternal: void pointer to optional required/input interface \""
                                           << this->GetName() << ":" << requiredIterator->first
                                           << "\" (required/input not connected to provided/output)" << std::endl;
            }
        }
    }
    if (success) {
        // Call user-supplied startup function
        this->Startup();
        ChangeState(mtsComponentState::READY);
    }
    else {
        CMN_LOG_CLASS_INIT_ERROR << "StartupInternal: task \"" << this->GetName() << "\" cannot be started." << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: ended for task \"" << this->GetName() << "\"" << std::endl;
}


void mtsTask::CleanupInternal() {
    // Call user-supplied cleanup function
    this->Cleanup();

    // Kill each state table
    StateTables.ForEachVoid(&mtsStateTable::Cleanup);

    // Perform Cleanup on all interfaces provided
    InterfacesProvidedOrOutput.ForEachVoid(&mtsInterfaceProvidedOrOutput::Cleanup);

    if (InterfaceProvidedToManagerCallable) {
        delete InterfaceProvidedToManagerCallable;
        InterfaceProvidedToManagerCallable = 0;
    }

    ChangeState(mtsComponentState::FINISHED);
    CMN_LOG_CLASS_INIT_VERBOSE << "CleanupInternal: ended for task \"" << this->GetName() << "\"" << std::endl;

    InterfaceProvidedToManager = 0;
}


/**************** Methods for managing task timing ********************/

void mtsTask::Sleep(double timeInSeconds)
{
    if (Thread.IsValid()) {
        Thread.Sleep(timeInSeconds);
    } else {
        osaSleep(timeInSeconds);
    }
}


mtsStateIndex::TimeTicksType mtsTask::GetTick(void) const {
    return StateTable.GetIndexWriter().Ticks();
}


void mtsTask::SaveThreadStartData(void * data) {
    ThreadStartData = data;
}


void mtsTask::SetThreadReturnValue(void * returnValue) {
    ReturnValue = returnValue;
}

void mtsTask::ChangeState(mtsComponentState::Enum newState)
{
    StateChange.Lock();
    this->State = newState;
    StateChange.Unlock();
    StateChangeSignal.Raise();

    // Inform the manager component client of the state change
    if (InterfaceProvidedToManager) {
        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        EventGeneratorChangeState(mtsComponentStateChange(LCM->GetProcessName(), this->GetName(), this->State));
    }
}

bool mtsTask::WaitForState(mtsComponentState desiredState, double timeout)
{
    if (this->State == desiredState) {
        return true;
    }
    if (osaGetCurrentThreadId() == Thread.GetId()) {
        // This shouldn't happen
        CMN_LOG_CLASS_INIT_WARNING << "WaitForState(" << desiredState << "): called from self for task \""
                                   << this->GetName() << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: waiting for task \"" << this->GetName() << "\" to enter state \""
                                   << desiredState << "\"" << std::endl;
        double curTime = osaGetTime();
        double startTime = curTime;
        double endTime = startTime + timeout;
        while (timeout > 0) {
#if (CISST_OS == CISST_LINUX_XENOMAI)
            osaSleep(1 * cmn_ms);
#endif
            StateChangeSignal.Wait(timeout);
            curTime = osaGetTime();
            if (this->State == desiredState) {
                break;
            }
            timeout = endTime - curTime;
        }

        mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
        if (this->State == desiredState) {
            if (LCM->IsLogAllowed())
                CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: waited for " << curTime-startTime
                                        << " seconds." << std::endl;
        } else {
            if (LCM->IsLogAllowed())
                CMN_LOG_CLASS_INIT_ERROR << "WaitForState: task \"" << this->GetName()
                                        << "\" did not reach state \"" << desiredState
                                        << "\", current state is \"" << this->State << "\"" << std::endl;
        }
    }
    return (this->State == desiredState);
}

/********************* Task constructor and destructor *****************/

mtsTask::mtsTask(const std::string & name,
                 unsigned int sizeStateTable) :
    mtsComponent(name),
    Thread(),
    InitializationDelay(3.0 * cmn_s), // if this value is modified, update documentation in header file
    StateChange(),
    StateChangeSignal(),
    StateTable(sizeStateTable, "Default"),
#if CISST_HAS_SAFETY_PLUGINS
    StateTableMonitor(sizeStateTable, "Monitor"),
#endif
    OverranPeriod(false),
    ThreadStartData(0),
    ReturnValue(0)
{
    this->AddStateTable(&this->StateTable);
#if CISST_HAS_SAFETY_PLUGINS
    this->AddStateTable(&this->StateTableMonitor);

    mtsInterfaceProvided * provided = GetInterfaceProvided(
        mtsStateTable::GetNameOfStateTableInterface(StateTableMonitor.GetName()));
    CMN_ASSERT(provided);
    // Make Period available through the command pattern
    provided->AddCommandReadState(this->StateTableMonitor, this->StateTableMonitor.Period, "GetPeriod");
    // Add fault notification event
    provided->AddEventWrite(this->GenerateFaultEvent, SF::Dict::FaultNames::FaultEvent, std::string());
    // [SFUPDATE]
#endif

    this->InterfaceProvidedToManagerCallable = new mtsCallableVoidMethod<mtsTask>(&mtsTask::ProcessManagerCommandsIfNotActive, this);
}


mtsTask::~mtsTask()
{
    CMN_LOG_CLASS_INIT_VERBOSE << "destructor: deleting task \"" << this->GetName() << "\"" << std::endl;
    if (!IsTerminated()) {
        //It is safe to call CleanupInternal() more than once.
        //Should we call the user-supplied Cleanup()?
        CleanupInternal();
    }
}


/********************* Methods to change task state ******************/

void mtsTask::Kill(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Kill: task \"" << this->GetName() << "\", current state \"" << this->State << "\"" << std::endl;

    // If the task has only been constructed (i.e., no thread created), then we just enter the FINISHED state directly.
    // Otherwise, we set the state to FINISHING and let the thread (RunInternal) set it to FINISHED.
    if (this->State == mtsComponentState::CONSTRUCTED) {
        ChangeState(mtsComponentState::FINISHED);
    } else {
        ChangeState(mtsComponentState::FINISHING);
    }
}



/********************* Methods to manage interfaces *******************/

mtsInterfaceRequired * mtsTask::AddInterfaceRequiredWithoutSystemEventHandlers(const std::string & interfaceRequiredName,
                                                                               mtsRequiredType required)
{
    mtsMailBox * mailBox = new mtsMailBox(interfaceRequiredName + "Events",
                                          mtsInterfaceRequired::DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE);
    mtsInterfaceRequired * result;
    // try to create and add interface
    result = this->AddInterfaceRequiredUsingMailbox(interfaceRequiredName, mailBox, required);
    if (!result)
        delete mailBox;
    return result;
}


mtsInterfaceProvided * mtsTask::AddInterfaceProvidedWithoutSystemEvents(const std::string & interfaceProvidedName,
                                                                        mtsInterfaceQueueingPolicy queueingPolicy,
                                                                        bool isProxy)
{
    mtsInterfaceProvided * interfaceProvided;
    if ((queueingPolicy == MTS_COMPONENT_POLICY)
        || (queueingPolicy == MTS_COMMANDS_SHOULD_BE_QUEUED)) {
        mtsCallableVoidBase * postCommandQueuedCallable = 0;
        if (interfaceProvidedName == mtsManagerComponentBase::GetNameOfInterfaceInternalProvided())
            postCommandQueuedCallable = InterfaceProvidedToManagerCallable;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_BE_QUEUED, postCommandQueuedCallable, isProxy);
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "AddInterfaceProvided: adding provided interface \"" << interfaceProvidedName
                                   << "\" with policy MTS_COMMANDS_SHOULD_NOT_BE_QUEUED to task \""
                                   << this->GetName() << "\". This bypasses built-in thread safety mechanisms, make sure your commands are thread safe."
                                   << std::endl;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED, 0, isProxy);
    }
    if (interfaceProvided) {
        if (InterfacesProvidedOrOutput.AddItem(interfaceProvidedName, interfaceProvided)) {
            InterfacesProvided.push_back(interfaceProvided);
            return interfaceProvided;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: task " << this->GetName() << " unable to add interface \""
                                 << interfaceProvidedName << "\"" << std::endl;
        delete interfaceProvided;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceProvided: task " << this->GetName() << " unable to create interface \""
                             << interfaceProvidedName << "\"" << std::endl;
    return 0;
}


/********************* Methods for task synchronization ***************/

bool mtsTask::WaitToStart(double timeout)
{
    if (this->State == mtsComponentState::INITIALIZING) {
        WaitForState(mtsComponentState::READY, timeout);
    }
    return (this->State >= mtsComponentState::READY);
}


bool mtsTask::WaitToTerminate(double timeout)
{
    bool ret = true;
    if (this->State < mtsComponentState::FINISHING) {
        CMN_LOG_CLASS_INIT_WARNING << "WaitToTerminate: not finishing task \"" << this->GetName() << "\"" << std::endl;
        ret = false;
    }
    else if (this->State == mtsComponentState::FINISHING) {
        ret = WaitForState(mtsComponentState::FINISHED, timeout);
    }

    // If task state is finished, we wait for the thread to be destroyed
    if ((this->State == mtsComponentState::FINISHED) && Thread.IsValid()) {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitToTerminate: waiting for task \"" << this->GetName()
                                   << "\" thread to exit." << std::endl;
        Thread.Wait();
    }
    return ret;
}


void mtsTask::ProcessManagerCommandsIfNotActive()
{
    if (InterfaceProvidedToManager) {
        // Lock the StateChange mutex so that the task state does not change
        StateChange.Lock();
        if (!IsRunning()) {
            CMN_LOG_CLASS_INIT_VERBOSE << "Task " << this->GetName() << " not active, processing internal mailbox" << std::endl;
            InterfaceProvidedToManager->ProcessMailBoxes();
        }
        else { // Wake up the thread just in case (e.g., for mtsTaskFromSignal)
            CMN_LOG_CLASS_INIT_VERBOSE << "Task " << this->GetName() << " active, not processing internal mailbox" << std::endl;
            this->Thread.Wakeup();
        }
        StateChange.Unlock();
    }
}

bool mtsTask::CheckForOwnThread(void) const
{
    return (osaGetCurrentThreadId() == Thread.GetId());
}

void mtsTask::ToStream(std::ostream & outputStream) const
{
    outputStream << "Task name: " << Name << std::endl;
    StateTable.ToStream(outputStream);
    InterfacesProvidedOrOutput.ToStream(outputStream);
    InterfacesRequiredOrInput.ToStream(outputStream);
}


void mtsTask::SetInitializationDelay(double delay)
{
    this->InitializationDelay = delay;
}

#if CISST_HAS_SAFETY_PLUGINS
bool mtsTask::AddFilter(mtsMonitorFilterBase * filter)
{
    return this->StateTableMonitor.AddFilter(filter);
}

void mtsTask::SetOverranPeriod(bool overran)
{
    this->OverranPeriod = overran;

    // Generate event to inform the safety supervisor of the thread overrun fault
    // of this component.
    if (!this->GenerateFaultEvent.IsValid()) {
        CMN_LOG_CLASS_RUN_WARNING << "Fault event cannot be generated: task \"" << this->GetName() << "\"" << std::endl;
        return;
    }

    // smmy
    std::string jsonFDDResult;
    // MJ: This FDI result should be generated through monitor automatically.
    //GenerateFaultEvent(
    
    // MJ TODO: How/when to reset overrun flag??
}

#endif
