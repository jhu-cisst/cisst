/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2010-12-10

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

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

ireTask::ireTask(const std::string &name, const std::string &startup) :
    mtsTaskContinuous(name), StartupCommands(startup)
{
    EnableDynamicComponentManagement();
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
    startup << "from cisstCommonPython import *; "
            << "from cisstMultiTaskPython import *; "
            << GetName() << " = cmnObjectRegister.FindObject('" << GetName() << "'); "
#if 0
            << "Manager = " << GetName() << ".GetInterfaceRequired('"
            << mtsManagerComponentBase::InterfaceNames::InterfaceInternalRequired << "'); "
            << "Manager.UpdateFromC(); "
#endif
            << "Manager = " << GetName() << ".GetManagerComponentServices(); "
            << StartupCommands;
    try {
        ireFramework::LaunchIREShell(startup.str().c_str(), true);
    }
    catch (...) {
        CMN_LOG_CLASS_INIT_ERROR << "Could not launch IRE shell (wxPython)" << std::endl;
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
