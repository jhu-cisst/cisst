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
#include <cisstMultiTask/mtsTaskInterface.h>

#include <iostream>


/********************* Methods that call user methods *****************/

void mtsTask::DoRunInternal(void)
{
    StateTable.Start();
    this->Run();
    // advance all state tables (if automatic) 
    StateTables.ForEachVoid(&mtsStateTable::AdvanceIfAutomatic);
}

void mtsTask::StartupInternal(void) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Starting StartupInternal for " << Name << std::endl;

    // Loop through the required interfaces and make sure they are all connected. This extra check is probably not needed.
    bool success = true;
    RequiredInterfacesMapType::const_iterator requiredIterator = RequiredInterfaces.begin();
    mtsDeviceInterface * connectedInterface;
    for (;
         requiredIterator != RequiredInterfaces.end();
         requiredIterator++) {
        connectedInterface = requiredIterator->second->GetConnectedInterface();
        if (!connectedInterface) {
            CMN_LOG_CLASS_INIT_WARNING << "StartupInternal: void pointer to required interface (required not connected to provided)" << std::endl;
            success = false;
        }
    }
    if (success) {
        // Call user-supplied startup function
        this->Startup();
        ChangeState(READY);
    }
    else {
        CMN_LOG_CLASS_INIT_ERROR << "StartupInternal: task " << this->GetName() << " cannot be started." << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Ending StartupInternal for " << this->GetName() << std::endl;
}

void mtsTask::CleanupInternal() {
    // Call user-supplied cleanup function
    this->Cleanup();
    // Perform Cleanup on all interfaces provided
    ProvidedInterfaces.ForEachVoid(&mtsDeviceInterface::Cleanup);
    ChangeState(FINISHED);
    CMN_LOG_CLASS_INIT_VERBOSE << "Done base class CleanupInternal " << this->GetName() << std::endl;
}


/********************* Methods to process queues  *********************/

// Execute all commands in the mailbox.  This is just a temporary implementation, where
// all commands in a mailbox are executed before moving on the next mailbox.  The final
// implementation will probably look at timestamps.  We may also want to pass in a
// parameter (enum) to set the mailbox processing policy.
unsigned int mtsTask::ProcessMailBoxes(ProvidedInterfacesMapType & interfaces)
{
    unsigned int numberOfCommands = 0;
    ProvidedInterfacesMapType::iterator iterator = interfaces.begin();
    const ProvidedInterfacesMapType::iterator end = interfaces.end();
    for (;
         iterator != end;
         ++iterator) {
        numberOfCommands += iterator->second->ProcessMailBoxes();
    }
    return numberOfCommands;
}


unsigned int mtsTask::ProcessQueuedEvents(void) {
    RequiredInterfacesMapType::iterator iterator = RequiredInterfaces.begin();
    const RequiredInterfacesMapType::iterator end = RequiredInterfaces.end();
    unsigned int numberOfEvents = 0;
    for (;
         iterator != end;
         iterator++) {
        numberOfEvents += iterator->second->ProcessMailBoxes();
    }
    return numberOfEvents;
}


/**************** Methods for managing task timing ********************/

void mtsTask::Sleep(double timeInSeconds)
{
    if (Thread.IsValid())
        Thread.Sleep(timeInSeconds);
    else
        osaSleep(timeInSeconds);
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
        CMN_LOG_CLASS_INIT_WARNING << "WaitForState(" << TaskStateName(desiredState) << "): called from self for task "
                                   << this->GetName() << std::endl;
    }
    else {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: waiting for task " << this->GetName() << " to enter state "
                                   << TaskStateName(desiredState) << std::endl;
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
            CMN_LOG_CLASS_INIT_ERROR << "WaitForState: task " << this->GetName()
                                     << " did not reach state " << TaskStateName(desiredState)
                                     << ", current state = " << GetTaskStateName() << std::endl;
        }
    }
    return (TaskState == desiredState);
}

/********************* Task constructor and destructor *****************/

mtsTask::mtsTask(const std::string & name,
                 unsigned int sizeStateTable) :
    mtsDevice(name),
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
    CMN_LOG_CLASS_INIT_VERBOSE << "mtsTask destructor: deleting task " << this->GetName() << std::endl;
    if (!IsTerminated()) {
        //It is safe to call CleanupInternal() more than once.
        //Should we call the user-supplied Cleanup()?
        CleanupInternal();
    }
}


/********************* Methods to change task state ******************/

void mtsTask::Kill(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Kill: " << this->GetName() << ", current state = " << GetTaskStateName() << std::endl;

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


bool mtsTask::AddStateTable(mtsStateTable * existingStateTable, bool addProvidedInterface) {
    const std::string tableName = existingStateTable->GetName();
    const std::string interfaceName = "StateTable" + tableName;
    if (!this->StateTables.AddItem(tableName,
                                   existingStateTable,
                                   CMN_LOG_LOD_INIT_ERROR)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddStateTable: can not add state table \"" << tableName
                                 << "\" to task \"" << this->GetName() << "\"" << std::endl;
        return false;
    }
    if (addProvidedInterface) {
        mtsProvidedInterface * providedInterface = this->AddProvidedInterface(interfaceName);
        if (!providedInterface) {
            CMN_LOG_CLASS_INIT_ERROR << "AddStateTable: can no add provided interface \"" << interfaceName
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
    }
    CMN_LOG_CLASS_INIT_DEBUG << "AddStateTable: added state table \"" << tableName
                             << "\" and corresponding interface \"" << interfaceName
                             << "\" to task \"" << this->GetName() << "\"" << std::endl;
    return true;
}


/********************* Methods to manage interfaces *******************/

mtsDeviceInterface * mtsTask::AddProvidedInterface(const std::string & newInterfaceName) {
    mtsTaskInterface * newInterface = new mtsTaskInterface(newInterfaceName, this);
    if (newInterface) {
        if (ProvidedInterfaces.AddItem(newInterfaceName, newInterface)) {
            return newInterface;
        }
        CMN_LOG_CLASS_INIT_ERROR << "AddProvidedInterface: task " << this->GetName() << " unable to add interface \""
                                 << newInterfaceName << "\"" << std::endl;
        delete newInterface;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddProvidedInterface: task " << this->GetName() << " unable to create interface \""
                             << newInterfaceName << "\"" << std::endl;
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
        CMN_LOG_CLASS_INIT_WARNING << "WaitToTerminate: not finishing task " << this->GetName() << std::endl;
        ret = false;
    }
    else if (TaskState == FINISHING) {
        ret = WaitForState(FINISHED, timeout);
    }

    // If task state is finished, we wait for the thread to be destroyed
    if ((TaskState == FINISHED) && Thread.IsValid()) {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitToTerminate: waiting for task " << this->GetName()
                                   << " thread to exit." << std::endl;
        Thread.Wait();
    }
    return ret;
}


void mtsTask::ToStream(std::ostream & outputStream) const
{
    outputStream << "Task name: " << Name << std::endl;
    StateTable.ToStream(outputStream);
    ProvidedInterfaces.ToStream(outputStream);
    RequiredInterfaces.ToStream(outputStream);
}
