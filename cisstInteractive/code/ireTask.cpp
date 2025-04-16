/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2010-12-10

  (C) Copyright 2010-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnObjectRegister.h>
#include <cisstInteractive/ireTask.h>
#include <cisstInteractive/ireFramework.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstOSAbstraction/osaThreadSignal.h>   // PK TEMP

CMN_IMPLEMENT_SERVICES(ireTaskConstructorArg)

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(ireTask, mtsTaskContinuous, ireTaskConstructorArg)

ireTask::ireTask(const std::string &name, IRE_Shell shell, const std::string &startup) :
    mtsTaskContinuous(name), Shell(shell), StartupCommands(startup)
{
    Initialize();
}

ireTask::ireTask(const ireTaskConstructorArg &arg) :
    mtsTaskContinuous(arg.Name), Shell(arg.Shell), StartupCommands(arg.Startup)
{
    Initialize();
}

void ireTask::Initialize(void)
{
    EnableDynamicComponentManagement();
    // For receiving system-wide logs
    mtsInterfaceRequired * required = AddInterfaceRequired(
        mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired,
        MTS_OPTIONAL);
    if (required) {
        required->AddEventHandlerWrite(&ireTask::Log, this, 
                                       mtsManagerComponentBase::EventNames::PrintLog);
    }
    SetInitializationDelay(10.0);  // Allow up to 10 seconds for it to start
}

ireTask::~ireTask()
{
    this->Cleanup();
}

void ireTask::Startup(void)
{
    // PK TEMP: Set callbacks for osaThreadSignal::Wait
    osaThreadSignal::SetWaitCallbacks(Thread.GetId(), &ireFramework::UnblockThreads, &ireFramework::BlockThreads);

    cmnObjectRegister::Register(GetName(), this);
    std::stringstream startup;
    startup << "import cisstCommonPython as cisstCommon; "
            << "import cisstMultiTaskPython as cisstMultiTask; "
            << "LCM = cisstMultiTask.mtsManagerLocal.GetInstance(); "
            // Could use LCM.GetComponent instead of cmnObjectRegister
            << GetName() << " = cisstCommon.cmnObjectRegister.FindObject('" << GetName() << "'); "
            << StartupCommands;
    try {
        // Don't use cmnCallbackStreambuf because we will use system-wide log
        ireFramework::LaunchIREShell(startup.str().c_str(), true, (Shell == IRE_IPYTHON), true);
    }
    catch (...) {
        if (Shell == IRE_IPYTHON)
            CMN_LOG_CLASS_INIT_ERROR << "Could not launch IPython shell" << std::endl;
        else
            CMN_LOG_CLASS_INIT_ERROR << "Could not launch IRE shell (wxPython)" << std::endl;
        return;
    }
    while (ireFramework::IsStarting()) {
        // Need following call to give the IRE thread some time to execute.
        ireFramework::JoinIREShell(0.1);
    }
}

void ireTask::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
    // Need following call to give the IRE thread some time to execute.
    ireFramework::JoinIREShell(0.1);
    if (ireFramework::IsFinished())
        Kill();
}

void ireTask::Cleanup(void)
{
    cmnObjectRegister::Remove(this->GetName());
}

void ireTask::Log(const mtsLogMessage & log)
{
    ireFramework::PrintLog(log.Message, log.Length);
}
