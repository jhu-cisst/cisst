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
#include <string>


CMN_IMPLEMENT_SERVICES(mtsTask)

/********************* Methods that call user methods *****************/

void mtsTask::DoRunInternal(void)
{
    StateTable.Start();
    this->Run();
    StateTable.Advance();
}
  
void mtsTask::StartupInternal(void) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Starting StartupInternal for " << Name << std::endl;

    bool success = true;
    // Loop through the required interfaces and bind all commands and events
    RequiredInterfacesMapType::const_iterator requiredIterator = RequiredInterfaces.begin();
    mtsDeviceInterface * connectedInterface;
    for (;
         requiredIterator != RequiredInterfaces.end();
         requiredIterator++) {
        connectedInterface = requiredIterator->second->GetConnectedInterface();
        if (connectedInterface) {
            CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: ask " << connectedInterface->GetName() 
                                       << " to allocate resources for " << this->GetName() << std::endl;
            connectedInterface->AllocateResourcesForCurrentThread();
            CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: binding commands and events" << std::endl;
            success &= requiredIterator->second->BindCommandsAndEvents();
        } else {
            CMN_LOG_CLASS_INIT_WARNING << "StartupInternal: void pointer to required interface (required not connected to provided)" << std::endl;
            success = false;
        }
    }
    // Call user-supplied startup function
    this->Startup();
    // StateChange should already be locked
    if (success) {
       TaskState = READY;       
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "StartupInternal: task " << this->GetName() << " cannot be started." << std::endl;
    }
    StateChange.Unlock();
    CMN_LOG_CLASS_INIT_VERBOSE << "Ending StartupInternal for " << this->GetName() << std::endl;
}

void mtsTask::CleanupInternal() {
    // Call user-supplied cleanup function
	this->Cleanup();
    // Perform Cleanup on all interfaces provided
    ProvidedInterfaces.ForEachVoid(&mtsDeviceInterface::Cleanup);
    // StateChange should be locked by Kill().
	TaskState = FINISHED;
    StateChange.Unlock();
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


/********************* Task constructor and destructor *****************/

mtsTask::mtsTask(const std::string & name, 
                 unsigned int sizeStateTable) :
    mtsDevice(name),
    Thread(),
    TaskState(CONSTRUCTED),
    StateChange(),
    StateTable(sizeStateTable),
    StateTables("StateTables"),
    OverranPeriod(false),
    ThreadStartData(0),
    ReturnValue(0)
{
    this->StateTables.SetOwner(*this);
    this->StateTables.AddItem(this->StateTable.GetName(),
                              &(this->StateTable),
                              CMN_LOG_LOD_INIT_ERROR);
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
    CMN_LOG_CLASS_INIT_VERBOSE << "Kill: " << this->GetName() << std::endl;

    // Generate a data collection event not to lose any data when killing a thread.
    StateTable.GenerateDataCollectionEvent();

    StateChange.Lock();
    // If we get here, we cannot be in the INITIALIZING or FINISHING
    // states because we are holding the StateChange Mutex. 
    if (TaskState == FINISHED)
        StateChange.Unlock();
    else if (TaskState == CONSTRUCTED) {
        TaskState = FINISHED;
        StateChange.Unlock();
    }
    else {
        TaskState = FINISHING;
        // Unlock StateChange in RunInternal
    }
}


/********************* Methods to query the task state ****************/

const char *mtsTask::TaskStateName(TaskStateType state) const
{
    static const char * taskStateNames[] = { "constructed", "initializing", "ready", "active", "finishing", "finished" };
    if ((state < CONSTRUCTED) || (state > FINISHED))
        return "unknown";
    else
        return taskStateNames[state];
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
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitToStart: waiting for task " << this->GetName() << " to start." << std::endl;
        // PK: Following doesn't work because WaitToStart is generally called from same thread
        // as Create, which is where the Lock was done.
        //StateChange.Lock();  // Should use TryLock with timeout
        // For now, we just use a Sleep and hope it is long enough
        osaSleep(timeout);
        if (TaskState != READY) {
            CMN_LOG_CLASS_INIT_ERROR << "WaitToStart: task " << this->GetName()
                                     << " did not start properly, state = " << TaskStateName(TaskState) << std::endl;
        }
        StateChange.Unlock();
    }
    return (TaskState >= READY);
}

bool mtsTask::WaitToTerminate(double timeout)
{
	CMN_LOG_CLASS_INIT_VERBOSE << "WaitToTerminate: " << this->GetName() << std::endl;
    if (TaskState < FINISHING) {
        return false;
    }
    if (TaskState == FINISHING) {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitToTerminate: waiting for task " << this->GetName() << " to finish." << std::endl;
        StateChange.Lock();  // Should use TryLock with timeout
        if (TaskState != FINISHED) {
            CMN_LOG_CLASS_INIT_ERROR << "WaitToTerminate: task " << this->GetName()
                                     << " did not finish properly, state = " << GetTaskStateName() << std::endl;
        }
        StateChange.Unlock();
    }
    // If task state is finished, we wait for the thread to be destroyed
    if ((TaskState == FINISHED) && Thread.IsValid()) {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitToTerminate: waiting for task " << this->GetName()
                                   << " thread to exit." << std::endl;
        Thread.Wait();
    }
	return true;
}


void mtsTask::ToStream(std::ostream & outputStream) const
{
    outputStream << "Task name: " << Name << std::endl;
    StateTable.ToStream(outputStream);
    ProvidedInterfaces.ToStream(outputStream);
    RequiredInterfaces.ToStream(outputStream);
}
