/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */ /* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2013 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <iostream>

#if CISST_HAS_SAFETY_PLUGINS
#include "dict.h"
#include "statemachine.h"
#endif

std::runtime_error mtsTask::UnknownException("Unknown mtsTask exception");

/********************* Methods that call user methods *****************/

void mtsTask::DoRunInternal(void)
{
    RunEventCalled = false;
    StateTables.ForEachVoid(&mtsStateTable::StartIfAutomatic);
#if CISST_HAS_SAFETY_PLUGINS
    double tic = 0.0, toc = 0.0;
    const SF::Event * e = 0;
    SF::State::StateType state = SF::State::INVALID;
#endif
    try {
        // Make sure following is called
        if (InterfaceProvidedToManager)
            InterfaceProvidedToManager->ProcessMailBoxes();
#if CISST_HAS_SAFETY_PLUGINS
        if (!GetSafetyCoordinator) {
            tic = osaGetTime();
            this->Run();
            toc = osaGetTime();
        } else {
            state = GetSafetyCoordinator->GetComponentState(this->GetName(), e);
            if (state == SF::State::INVALID) {
                //tic = osaGetTime(); // still need to be updated for ExecTimeUser
                CMN_LOG_CLASS_RUN_ERROR << "Invalid state: " << SF::State::GetStringState(state) << std::endl;
                toc = tic = 0.0;
            } else {
                // handle state transition
                if (LastState != state) {
                    if (LastState == SF::State::NORMAL) {
                        if (state == SF::State::WARNING)
                            this->OnNormal2Warning(e);
                        else
                            this->OnNormal2Error(e);
                    } else if (LastState == SF::State::WARNING) {
                        if (state == SF::State::NORMAL)
                            this->OnWarning2Normal(e);
                        else
                            this->OnWarning2Error(e);
                    } else if (LastState == SF::State::ERROR) {
                        if (state == SF::State::WARNING)
                            this->OnError2Warning(e);
                        else
                            this->OnError2Normal(e);
                    }
                }
                // update last state
                LastState = state;

                tic = osaGetTime();
                switch (state) {
                case SF::State::NORMAL:  this->RunNormal(); break;
                case SF::State::WARNING: this->RunWarning(e); break;
                case SF::State::ERROR:   this->RunError(e); break;
                default:
                    CMN_LOG_CLASS_RUN_ERROR << "Invalid state: " << SF::State::GetStringState(state) << std::endl;
                }
                toc = osaGetTime();
            }
        }
        this->StateTableMonitor.ExecTimeUser = toc - tic;
#else
        this->Run();
#endif
    }
    catch (const std::exception &excp) {
        OnRunException(excp);
    }
    catch (...) {
        OnRunException(mtsTask::UnknownException);
    }

#if CISST_HAS_SAFETY_PLUGINS
    // Generate completion event of thread exception if onset event has occurred earlier.
    state = GetSafetyCoordinator->GetComponentState(this->GetName(), e);
    if (state == SF::State::ERROR) {
        CMN_ASSERT(e);
        if (e->GetName().compare("EVT_THREAD_EXCEPTION") == 0) {
            std::stringstream ss;
            ss << "Component \"" << GetName() << "\" goes back to NORMAL state (exception resolved)";
            CMN_LOG_CLASS_RUN_WARNING << ss.str() << std::endl;
            // Inform casros of this offset event
            GetSafetyCoordinator->GenerateEvent("/EVT_THREAD_EXCEPTION",
                                                SF::State::STATEMACHINE_FRAMEWORK,
                                                ss.str(),
                                                this->Name);
        }
    }
#endif

    // advance all state tables (if automatic)
    // MJ: mtsStateTable::Advance processes installed and active filters
    StateTables.ForEachVoid(&mtsStateTable::AdvanceIfAutomatic);

    RunEvent();  // only generates event if RunEventCalled is false
}

#if CISST_HAS_SAFETY_PLUGINS
void mtsTask::RunNormal(void)
{
    this->Run();
}

void mtsTask::RunWarning(const SF::Event * CMN_UNUSED(e))
{
    this->Run();
}

void mtsTask::RunError(const SF::Event * CMN_UNUSED(e))
{
    this->Run();
}
#endif

void mtsTask::RunEventHandler(void)
{
    RunEventCalled = false;
    if (this->State == mtsComponentState::INITIALIZING && !Thread.IsValid()) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RunEventHandler: initializing thread for " << this->GetName() << std::endl;
        // This is only executed once, to produce same behavior as RunInternal
        Thread.CreateFromCurrentThread();
        StartupInternal();
    }
    else if (this->State == mtsComponentState::ACTIVE)
        DoRunInternal();
    else
        RunEvent();
}

mtsExecutionResult mtsTask::RunEvent(bool check)
{
    if (check && RunEventCalled)
        return mtsExecutionResult::COMMAND_DISABLED;
    RunEventCalled = true;
    return RunEventInternal();
}

// ChangeStateEventHandler
// This event handler is in the ExecIn required interface, which means that it is only called
// if this task is getting its thread of execution from another task (via RunEvent).
// Thus, we do the following:
//   1) If the other task is initializing (i.e., Create called), we also call Create; this shouldn't
//      be necessary, since most likely the application code will create this task (either by
//      calling Create directly, or by calling mtsManagerLocal::CreateAll).
//   2) If this task is ACTIVE, and the other task is suspended (i.e., state is READY), we
//      also suspend this task. Note that we could also automatically start this task when
//      the other task becomes ACTIVE, but for now we are not doing that; instead, the user
//      will have to manually re-start this task.
//   3) If the other task is being killed (state is FINISHING) or is already killed (state is
//      FINISHED), then we also kill this task.
void mtsTask::ChangeStateEventHandler(const mtsComponentState &newState)
{
    if ((this->State == mtsComponentState::CONSTRUCTED) &&
        (newState == mtsComponentState::INITIALIZING)) {
        CMN_LOG_CLASS_RUN_VERBOSE << "ChangeStateEventHandler: calling Create for " << this->GetName() << std::endl;
        this->Create();
    }
    else if ((this->State == mtsComponentState::ACTIVE) &&
             (newState == mtsComponentState::READY)) {
        CMN_LOG_CLASS_RUN_VERBOSE << "ChangeStateEventHandler: calling Suspend for " << this->GetName() << std::endl;
        this->Suspend();
    }
    else if ((newState == mtsComponentState::FINISHING) ||
             (newState == mtsComponentState::FINISHED)) {
        CMN_LOG_CLASS_RUN_VERBOSE << "ChangeStateEventHandler: calling Kill for " << this->GetName() << std::endl;
        this->Kill();
    }
    // Forward this to any tasks connected to the ExecOut interface
    ChangeStateEvent(newState);
}

void mtsTask::StartupInternal(void) {
    CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: started for task \"" << this->GetName() << "\"" << std::endl;

    // Loop through the required interfaces and make sure they are all connected. This extra check is probably not needed.
    bool success = true;
    InterfacesRequiredMapType::const_iterator requiredIterator = InterfacesRequired.begin();
    const mtsInterfaceProvided * connectedInterface;
    for (;
         requiredIterator != InterfacesRequired.end();
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
    RunEventCalled = false;
    if (success) {
        try {
            // Call user-supplied startup function
            this->Startup();
        }
        catch (const std::exception &excp) {
            OnStartupException(excp);
        }
        catch (...) {
            OnStartupException(mtsTask::UnknownException);
        }
        ChangeState(mtsComponentState::READY);
    }
    else {
        CMN_LOG_CLASS_INIT_ERROR << "StartupInternal: task \"" << this->GetName() << "\" cannot be started." << std::endl;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: ended for task \"" << this->GetName() << "\"" << std::endl;
    RunEvent();
}


void mtsTask::CleanupInternal() {
    // Call user-supplied cleanup function
    this->Cleanup();

    // Kill each state table
    StateTables.ForEachVoid(&mtsStateTable::Cleanup);

    // Perform Cleanup on all interfaces provided
    InterfacesProvided.ForEachVoid(&mtsInterfaceProvided::Cleanup);

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
    if (this->State.State() == newState)
        return;

    // If this component is providing the thread, inform
    // any dependent components
    if (!(ExecIn && ExecIn->GetConnectedInterface()))
        ChangeStateEvent(mtsComponentState(newState));

//#if CISST_HAS_SAFETY_PLUGINS
    //mtsComponentState oldState = this->State;
//#endif

    StateChange.Lock();
    this->State = newState;
    StateChange.Unlock();
    StateChangeSignal.Raise();

//#if CISST_HAS_SAFETY_PLUGINS
    //// If state transition involves the ACTIVE state, notify Safety Framework of the transition.
    //if (oldState == mtsComponentState::ACTIVE && newState != mtsComponentState::ACTIVE) {
        //GCMInstance->ProcessEvent_ComponentFramework(SF::State::ON_EXIT);
    //} else if (oldState != mtsComponentState::ACTIVE && newState == mtsComponentState::ACTIVE) {
        //GCMInstance->ProcessEvent_ComponentFramework(SF::State::ON_ENTRY);
    //}
//#endif

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
    if (CheckForOwnThread()) {
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
            if (LCM->IsLogAllowed()) {
                CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: waited for " << curTime-startTime
                                        << " seconds." << std::endl;
            }
        } else {
            if (LCM->IsLogAllowed()) {
                // some cases are not really errors
                if ((desiredState == mtsComponentState::READY) && (this->State == mtsComponentState::ACTIVE)) {
                    CMN_LOG_CLASS_INIT_VERBOSE << "WaitForState: task \"" << this->GetName()
                                               << "\" did not reach state \"" << desiredState
                                               << "\", current state is \"" << this->State << "\"" << std::endl;

                } else {
                    CMN_LOG_CLASS_INIT_ERROR << "WaitForState: task \"" << this->GetName()
                                             << "\" did not reach state \"" << desiredState
                                             << "\", current state is \"" << this->State << "\"" << std::endl;
                }
            }
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
    StateTableMonitor(sizeStateTable, mtsStateTable::NameOfStateTableForMonitoring),
#endif
    OverranPeriod(false),
    ThreadStartData(0),
    ReturnValue(0),
    RunEventCalled(false)
{
    this->AddStateTable(&this->StateTable);
#if CISST_HAS_SAFETY_PLUGINS
    this->AddStateTable(&this->StateTableMonitor);

    // set owner name for state table.  State table does not run any filter without its owner component name.
    // Owner (component) name is to look up an instance of filters via Safety Coordinator
    // that manages all the filter instances based on its owner name.
    StateTableMonitor.SetOwnerComponentName(name);

    mtsInterfaceProvided * provided = GetInterfaceProvided(
        mtsStateTable::GetNameOfStateTableInterface(StateTableMonitor.GetName()));
    CMN_ASSERT(provided);
    // Make Period available through the command pattern
    // MJTODO: Rename exec to dutycycle
    provided->AddCommandReadState(this->StateTableMonitor, this->StateTableMonitor.Period, "GetPeriod");
    provided->AddCommandReadState(this->StateTableMonitor, this->StateTableMonitor.ExecTimeUser, "GetExecTimeUser");
    provided->AddCommandReadState(this->StateTableMonitor, this->StateTableMonitor.ExecTimeTotal, "GetExecTimeTotal");
    // Add fault notification event
    provided->AddEventWrite(this->GenerateMonitorEvent, SF::Dict::MonitorNames::MonitorEvent, std::string());

    // Initialize containers for framework filters
    StatusException.Count     = 0;
    StatusException.Timestamp = 0.0;
    StatusOverrun.Count     = 0;
    StatusOverrun.Timestamp = 0.0;
    StatusOverrun.Duration  = 0.0;

    // Wrap internal status information with command pattern, which framework filters use.
    StateTableMonitor.AddData(StatusException.Count,     "ExceptionCount");
    StateTableMonitor.AddData(StatusException.Timestamp, "ExceptionTimestamp");
    StateTableMonitor.AddData(StatusOverrun.Count,       "OverrunCount");
    StateTableMonitor.AddData(StatusOverrun.Timestamp,   "OverrunTimestamp");
    StateTableMonitor.AddData(StatusOverrun.Duration,    "OverrunDuration");
    provided->AddCommandReadState(StateTableMonitor, StatusException.Count,     "GetExceptionCount");
    provided->AddCommandReadState(StateTableMonitor, StatusException.Timestamp, "GetExceptionTimestamp");
    provided->AddCommandReadState(StateTableMonitor, StatusOverrun.Count,       "GetOverrunCount");
    provided->AddCommandReadState(StateTableMonitor, StatusOverrun.Timestamp,   "GetOverrunTimestamp");
    provided->AddCommandReadState(StateTableMonitor, StatusOverrun.Duration,    "GetOverrunDuration");
    // [SFUPDATE]

    LastState = SF::State::NORMAL;
#endif

    this->InterfaceProvidedToManagerCallable = new mtsCallableVoidMethod<mtsTask>(&mtsTask::ProcessManagerCommandsIfNotActive, this);
    // ExecIn interface is optional; does not need a mailbox
    ExecIn = this->AddInterfaceRequiredUsingMailbox(mtsManagerComponentBase::InterfaceNames::InterfaceExecIn, 0, MTS_OPTIONAL);
    if (ExecIn) {
        // Can add functions to set period, check if hard real-time, etc.
        ExecIn->AddEventHandlerVoid(&mtsTask::RunEventHandler, this, "RunEvent", MTS_EVENT_NOT_QUEUED);
        ExecIn->AddEventHandlerWrite(&mtsTask::ChangeStateEventHandler, this, "ChangeStateEvent", MTS_EVENT_NOT_QUEUED);
    }
    else
        CMN_LOG_CLASS_INIT_ERROR << "Failed to add ExecIn interface to " << this->GetName() << std::endl;
    // ExecOut interface
    ExecOut = this->AddInterfaceProvided(mtsManagerComponentBase::InterfaceNames::InterfaceExecOut);
    if (ExecOut) {
        // Can add commands to set period, check if hard real-time, etc.
        ExecOut->AddEventVoid(RunEventInternal, "RunEvent");
        ExecOut->AddEventWrite(ChangeStateEvent, "ChangeStateEvent", this->State);
    }
    else
        CMN_LOG_CLASS_INIT_ERROR << "Failed to add ExecOut interface to " << this->GetName() << std::endl;
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
    if (this->State == mtsComponentState::FINISHED)
        return;

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
        if (InterfacesProvided.AddItem(interfaceProvidedName, interfaceProvided)) {
#if CISST_HAS_SAFETY_PLUGINS
            mtsSafetyCoordinator * sc = mtsManagerLocal::GetInstance()->GetCoordinator();
            if (sc)
                if (!sc->AddInterface(Name, interfaceProvidedName, SF::GCM::PROVIDED_INTERFACE))
                    CMN_LOG_CLASS_INIT_ERROR << "Failed to add provided interface \"" << interfaceProvidedName << "\" to Safety Coordinator." << std::endl;
#endif
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

void mtsTask::OnStartupException(const std::exception &excp)
{
#if !CISST_HAS_SAFETY_PLUGINS
    CMN_LOG_CLASS_RUN_WARNING << "Task " << this->GetName() << " caught startup exception: " << excp.what() << std::endl;
#else
    std::stringstream ss;
    ss << "Task " << this->GetName() << " caught startup exception: " << excp.what() << std::endl;
    CMN_LOG_CLASS_RUN_WARNING << ss.str();

    GetSafetyCoordinator->GenerateEvent("EVT_THREAD_EXCEPTION",
                                        SF::State::STATEMACHINE_FRAMEWORK,
                                        ss.str(),
                                        this->Name);
#endif
}

void mtsTask::OnRunException(const std::exception &excp)
{
#if !CISST_HAS_SAFETY_PLUGINS
    CMN_LOG_CLASS_RUN_WARNING << "Task " << this->GetName() << " caught run exception: " << excp.what() << std::endl;
#else
    std::stringstream ss;
    ss << "Task " << this->GetName() << " caught run exception: " << excp.what() << std::endl;
    CMN_LOG_CLASS_RUN_WARNING << ss.str();

    GetSafetyCoordinator->GenerateEvent("EVT_THREAD_EXCEPTION",
                                        SF::State::STATEMACHINE_FRAMEWORK,
                                        ss.str(),
                                        this->Name);
#endif
}

void mtsTask::SetInitializationDelay(double delay)
{
    this->InitializationDelay = delay;
}

#if CISST_HAS_SAFETY_PLUGINS
#if 0
void mtsTask::HandlerException(const std::string & CMN_UNUSED(name), const std::string & CMN_UNUSED(what))
{
    ++StatusException.Count;

    StatusException.Timestamp = osaGetTime();
}

void mtsTask::HandlerOverrun(const std::string & CMN_UNUSED(name), const std::string & CMN_UNUSED(what))
{
    ++StatusOverrun.Count;

    StatusOverrun.Duration  = 0.0; // TODO: get actual overrun time
    StatusOverrun.Timestamp = osaGetTime();
}
#endif

void mtsTask::OnNormal2Warning(const SF::Event * e)
{
    std::stringstream ss;
    ss << "State transition: NORMAL to WARNING";
    if (e)
        ss << ", " << *e;
    ss << std::endl;

    CMN_LOG_CLASS_RUN_VERBOSE << ss.str();
}

void mtsTask::OnNormal2Error(const SF::Event * e)
{
    std::stringstream ss;
    ss << "State transition: NORMAL to ERROR";
    if (e)
        ss << ", " << *e;
    ss << std::endl;

    CMN_LOG_CLASS_RUN_VERBOSE << ss.str();
}

void mtsTask::OnWarning2Normal(const SF::Event * e)
{
    std::stringstream ss;
    ss << "State transition: WARNING to NORMAL";
    if (e)
        ss << ", " << *e;
    ss << std::endl;

    CMN_LOG_CLASS_RUN_VERBOSE << ss.str();
}

void mtsTask::OnWarning2Error(const SF::Event * e)
{
    std::stringstream ss;
    ss << "State transition: WARNING to ERROR";
    if (e)
        ss << ", " << *e;
    ss << std::endl;

    CMN_LOG_CLASS_RUN_VERBOSE << ss.str();
}

void mtsTask::OnError2Warning(const SF::Event * e)
{
    std::stringstream ss;
    ss << "State transition: ERROR to WARNING";
    if (e)
        ss << ", " << *e;
    ss << std::endl;

    CMN_LOG_CLASS_RUN_VERBOSE << ss.str();
}

void mtsTask::OnError2Normal(const SF::Event * e)
{
    std::stringstream ss;
    ss << "State transition: ERROR to NORMAL";
    if (e)
        ss << ", " << *e;
    ss << std::endl;

    CMN_LOG_CLASS_RUN_VERBOSE << ss.str();
}

// Obsoleted
#if 0
void mtsTask::SetOverranPeriod(bool overran)
{
    this->OverranPeriod = overran;

    // Generate event to inform the safety supervisor of the thread overrun fault
    // of this component.
    if (!this->GenerateMonitorEvent.IsValid()) {
        CMN_LOG_CLASS_RUN_WARNING << "Monitor event cannot be generated: task \"" << this->GetName() << "\"" << std::endl;
        return;
    }

    // MJTODO: How/when to reset overrun flag??
    std::cout  << "mtsTask::SetOverranPeriod() ---- MONITORING EVENT: TASK \"" << this->GetName() << "\" overran" << std::endl;
}
#endif

SF::State::StateType mtsTask::GetComponentState(void) const
{
    return GetSafetyCoordinator->GetComponentState(this->GetName());
}

SF::State::StateType mtsTask::GetProvidedInterfaceState(const std::string & interfaceName) const
{
    return GetSafetyCoordinator->GetInterfaceState(this->GetName(),
                                                   interfaceName,
                                                   SF::GCM::PROVIDED_INTERFACE);
}

SF::State::StateType mtsTask::GetProvidedInterfaceState(const std::string & interfaceName, const SF::Event* & e) const
{
    return GetSafetyCoordinator->GetInterfaceState(this->GetName(),
                                                   interfaceName,
                                                   e,
                                                   SF::GCM::PROVIDED_INTERFACE);
}

SF::State::StateType mtsTask::GetRequiredInterfaceState(const std::string & interfaceName) const
{
    return GetSafetyCoordinator->GetInterfaceState(this->GetName(),
                                                   interfaceName,
                                                   SF::GCM::REQUIRED_INTERFACE);
}

SF::State::StateType mtsTask::GetRequiredInterfaceState(const std::string & interfaceName, const SF::Event* & e) const
{
    return GetSafetyCoordinator->GetInterfaceState(this->GetName(),
                                                   interfaceName,
                                                   e,
                                                   SF::GCM::REQUIRED_INTERFACE);
}

bool mtsTask::GetLatestDataFromStateTable(const std::string & signalName, double & value) const
{
    typedef mtsStateTable::Accessor<double> AccessorType;
    AccessorType * accessor = dynamic_cast<AccessorType*>(this->StateTableMonitor.GetAccessor(signalName));
    if (!accessor) {
        CMN_LOG_CLASS_RUN_ERROR << "GetLatestDataFromStateTable: failed to get accessor for \"" << signalName << "\"" << std::endl;
        return false;
    }
    mtsDouble _value;
    accessor->GetLatest(_value);
    value = _value.Data;

    return true;
}

bool mtsTask::GetLatestDataFromStateTable(const std::string & signalName, SF::DoubleVecType & values) const
{
    typedef mtsStateTable::Accessor<std::vector<double> > AccessorType;
    AccessorType * accessor = dynamic_cast<AccessorType*>(this->StateTableMonitor.GetAccessor(signalName));
    if (!accessor) {
        CMN_LOG_CLASS_RUN_ERROR << "GetLatestDataFromStateTable: failed to get accessor for \"" << signalName << "\"" << std::endl;
        return false;
    }
    mtsStdDoubleVecProxy _values;
    accessor->GetLatest(_values);
    values = _values.Data;

    return true;
}
#endif
