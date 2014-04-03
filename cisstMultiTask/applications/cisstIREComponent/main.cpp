/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2011-01-04

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnObjectRegister.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstInteractive/ireTask.h>

// Syntax:
//   cisstIREComponent                 -- starts IRE, connecting to GCM at localhost, using wxPython
//   cisstIREComponent IP_Addr         -- starts IRE, connecting to GCM at IP_Addr, using wxPython
//   cisstIREComponent IP_Addr ipython -- starts IRE, connecting to GCM at IP_Addr, using IPython

int main(int argc, char * argv[])
{
    // Enable system-wide thread-safe Logger
    mtsManagerLocal::SetLogForwarding(true);

    std::string globalComponentManagerIP;

    // Set global component manager's ip address
    if (argc < 2)
        globalComponentManagerIP = "localhost";
    else
        globalComponentManagerIP = argv[1];
    
    // Get the TaskManager instance and set operation mode
    mtsManagerLocal * taskManager;
    try {
        taskManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "ProcessIRE");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    cmnObjectRegister::Register("TaskManager", taskManager);

    IRE_Shell shell = IRE_WXPYTHON;
    if ((argc > 2) && (strncmp(argv[2], "ipy", 3) == 0))
        shell = IRE_IPYTHON;
    ireTask *ire = new ireTask("IRE", shell);  // Could add startup string as third parameter
    taskManager->AddComponent(ire);

    if (!taskManager->Connect(
            "ProcessIRE",
            "IRE",
            mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired,
            mtsManagerLocal::ProcessNameOfLCMWithGCM,
            mtsManagerComponentBase::ComponentNames::ManagerComponentServer,
            mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerProvided))
        CMN_LOG_INIT_ERROR << "Failed to connect system-wide thread-safe logger" << std::endl;

    taskManager->CreateAll();
    taskManager->StartAll();

    // Loop until IRE is exited
    while (!ire->IsTerminated())
        osaSleep(0.5 * cmn_s);  // Wait 0.5 seconds

    taskManager->Cleanup();
    return 0;
}
