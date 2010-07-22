/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
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

#include <iostream>


/********************* Methods that call user methods *****************/

void mtsTask::DoRunInternal(void)
{
    StateTables.ForEachVoid(&mtsStateTable::StartIfAutomatic);
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
            CMN_LOG_CLASS_INIT_WARNING << "StartupInternal: void pointer to required/input interface \""
                                       << this->GetName() << ":" << requiredIterator->first
                                       << "\" (required/input not connected to provided/output)" << std::endl;
            success = false;
        }
    }
    if (success) {
        // Call user-supplied startup function
        this->Startup();
        ChangeState(READY);
    }
    else {
        CMN_LOG_CLASS_INIT_ERROR << "StartupInternal: task \"" << this->GetName() << "\" cannot be started." << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: ended for task \"" << this->GetName() << "\"" << std::endl;
}


void mtsTask::CleanupInternal() {
    // Call user-supplied cleanup function
    this->Cleanup();
    // Perform Cleanup on all interfaces provided
    InterfacesProvidedOrOutput.ForEachVoid(&mtsInterfaceProvidedOrOutput::Cleanup);
    ChangeState(FINISHED);
    CMN_LOG_CLASS_INIT_VERBOSE << "CleanupInternal: ended for task \"" << this->GetName() << "\"" << std::endl;
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

void mtsTask::ChangeState(TaskStateType newState)
{
    StateChange.Lock();
    TaskState = newState;
    StateChange.Unlock();
    StateChangeSignal.Raise();
}

bool mtsTask::WaitForState(TaskStateType desiredState, double timeout)
{
    if (TaskState == desiredState)
        return true;
    if (osaGetCurrentThreadId() == Thread.GetId()) {
        // This shouldn't happen
        CMN_LOG_CLASS_INIT_WARNING << "WaitForState(" << TaskStateName(desiredState) << "): called from self for task \""
                                   << this->GetName() << "\"" << std::endl;
    }
    else {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: waiting for task \"" << this->GetName() << "\" to enter state \""
                                   << TaskStateName(desiredState) << "\"" << std::endl;
        double curTime = osaGetTime();
        double startTime = curTime;
        double endTime = startTime + timeout;
        while (timeout > 0) {
            StateChangeSignal.Wait(timeout);
            curTime = osaGetTime();
            if (TaskState == desiredState)
                break;
            timeout = endTime - curTime;
        }
        if (TaskState == desiredState) {
            CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: waited for " << curTime-startTime
                                       << " seconds." << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "WaitForState: task \"" << this->GetName()
                                     << "\" did not reach state \"" << TaskStateName(desiredState)
                                     << "\", current state is \"" << GetTaskStateName() << "\"" << std::endl;
        }
    }
    return (TaskState == desiredState);
}

/********************* Task constructor and destructor *****************/

mtsTask::mtsTask(const std::string & name,
                 unsigned int sizeStateTable) :
    mtsComponent(name),
    Thread(),
    TaskState(CONSTRUCTED),
    StateChange(),
    StateChangeSignal(),
    StateTable(sizeStateTable, "StateTable"),
    StateTables("StateTables"),
    OverranPeriod(false),
    ThreadStartData(0),
    ReturnValue(0)
{
    this->StateTables.SetOwner(*this);
    this->AddStateTable(&this->StateTable);
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
    CMN_LOG_CLASS_INIT_VERBOSE << "Kill: task \"" << this->GetName() << "\", current state \"" << GetTaskStateName() << "\"" << std::endl;

    // Kill each state table
    StateTables.ForEachVoid(&mtsStateTable::Kill);

    // If the task has only been constructed (i.e., no thread created), then we just enter the FINISHED state directly.
    // Otherwise, we set the state to FINISHING and let the thread (RunInternal) set it to FINISHED.
    if (TaskState == CONSTRUCTED) {
        ChangeState(FINISHED);
    } else {
        ChangeState(FINISHING);
    }
}


/********************* Methods to query the task state ****************/

const char * mtsTask::TaskStateName(TaskStateType state) const
{
    static const char * const taskStateNames[] = { "constructed", "initializing", "ready", "active", "finishing", "finished" };
    if ((state < CONSTRUCTED) || (state > FINISHED)) {
        return "unknown";
    } else {
        return taskStateNames[state];
    }
}


bool mtsTask::AddStateTable(mtsStateTable * existingStateTable, bool addInterfaceProvided) {
    const std::string tableName = existingStateTable->GetName();
    const std::string interfaceName = "StateTable" + tableName;
    if (!this->StateTables.AddItem(tableName,
                                   existingStateTable,
                                   CMN_LOG_LOD_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddStateTable: can't add state table \"" << tableName
                                 << "\" to task \"" << this->GetName() << "\"" << std::endl;
        return false;
    }
    if (addInterfaceProvided) {
        mtsInterfaceProvided * providedInterface = this->AddInterfaceProvided(interfaceName);
        if (!providedInterface) {
            CMN_LOG_CLASS_INIT_ERROR << "AddStateTable: can't add provided interface \"" << interfaceName
                                     << "\" to task \"" << this->GetName() << "\"" << std::endl;
            return false;
        }
        providedInterface->AddCommandWrite(&mtsStateTable::DataCollectionStart,
                                           existingStateTable,
                                           "StartCollection");
        providedInterface->AddCommandWrite(&mtsStateTable::DataCollectionStop,
                                           existingStateTable,
                                           "StopCollection");
        providedInterface->AddEventWrite(existingStateTable->DataCollection.BatchReady,
                                         "BatchReady", mtsStateTable::IndexRange());
        providedInterface->AddEventVoid(existingStateTable->DataCollection.CollectionStarted,
                                        "CollectionStarted");
        providedInterface->AddEventWrite(existingStateTable->DataCollection.CollectionStopped,
                                         "CollectionStopped", mtsUInt());
        providedInterface->AddEventWrite(existingStateTable->DataCollection.Progress,
                                         "Progress", mtsUInt());
    }
    CMN_LOG_CLASS_INIT_DEBUG << "AddStateTable: added state table \"" << tableName
                             << "\" and corresponding interface \"" << interfaceName
                             << "\" to task \"" << this->GetName() << "\"" << std::endl;
    return true;
}


/********************* Methods to manage interfaces *******************/

mtsInterfaceRequired * mtsTask::AddInterfaceRequired(const std::string & interfaceRequiredName) {
    // PK: move DEFAULT_EVENT_QUEUE_LEN somewhere else (not in mtsInterfaceProvided)
    mtsMailBox * mailBox = new mtsMailBox(interfaceRequiredName + "Events", mtsInterfaceRequired::DEFAULT_EVENT_QUEUE_LEN);
    mtsInterfaceRequired * result;
    if (mailBox) {
        // try to create and add interface
        result = this->AddInterfaceRequiredUsingMailbox(interfaceRequiredName, mailBox);
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


mtsInterfaceProvided * mtsTask::AddInterfaceProvided(const std::string & interfaceProvidedName,
                                                     mtsInterfaceQueuingPolicy queuingPolicy)
{
    mtsInterfaceProvided * interfaceProvided;
    if ((queuingPolicy == MTS_COMPONENT_POLICY)
        || (queuingPolicy == MTS_COMMANDS_SHOULD_BE_QUEUED)) {
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_BE_QUEUED);
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "AddInterfaceProvided: adding provided interface \"" << interfaceProvidedName
                                   << "\" with policy MTS_COMMANDS_SHOULD_NOT_BE_QUEUED to task \""
                                   << this->GetName() << "\". This bypasses built-ins thread safety mechanisms, make sure your commands are thread safe"
                                   << std::endl;
        interfaceProvided = new mtsInterfaceProvided(interfaceProvidedName, this, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
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
    if (TaskState == INITIALIZING) {
        WaitForState(READY, timeout);
    }
    return (TaskState >= READY);
}

bool mtsTask::WaitToTerminate(double timeout)
{
    bool ret = true;
    if (TaskState < FINISHING) {
        CMN_LOG_CLASS_INIT_WARNING << "WaitToTerminate: not finishing task \"" << this->GetName() << "\"" << std::endl;
        ret = false;
    }
    else if (TaskState == FINISHING) {
        ret = WaitForState(FINISHED, timeout);
    }

    // If task state is finished, we wait for the thread to be destroyed
    if ((TaskState == FINISHED) && Thread.IsValid()) {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitToTerminate: waiting for task \"" << this->GetName()
                                   << "\" thread to exit." << std::endl;
        Thread.Wait();
    }
    return ret;
}


void mtsTask::ToStream(std::ostream & outputStream) const
{
    outputStream << "Task name: " << Name << std::endl;
    StateTable.ToStream(outputStream);
    InterfacesProvidedOrOutput.ToStream(outputStream);
    InterfacesRequiredOrInput.ToStream(outputStream);
}
