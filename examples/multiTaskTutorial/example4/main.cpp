/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id$

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstInteractive.h>
#include <cisstMultiTask.h>

#include <Python.h>

#include "sineTask.h"
#include "clockDevice.h"
#include "displayTask.h"
#include "displayUI.h"

using namespace std;

#if 1  // PKAZ
// Launch IRE in C++-created thread (using osaThread)
// Note that it is best to do all IRE actions in the Run() method,
// including the calls to ireFramework::Instance (which should occur
// when LaunchIREShell is called) and FinalizeShell, because otherwise
// the IRE (Python interpreter) would be called from multiple threads.
class IreLaunch {
    bool useIPython;
public:
    IreLaunch(bool useIPy = false) : useIPython(useIPy) {}
    ~IreLaunch() {}
    void *Run(char *startup) {
        try {
            CMN_LOG(3) << "Using " << (useIPython?"IPython":"wxPython") << std::endl;
            ireFramework::LaunchIREShell(startup, false, useIPython);
        }
        catch (...) {
            cout << "*** ERROR:  could not launch IRE shell ***" << endl;
        }
        ireFramework::FinalizeShell();
        return this;
    }
};
#endif

int main(int argc, char **argv)
{
    // log configuration, see previous examples
    cmnLogger::SetLoD(10);
    cmnLogger::GetMultiplexer()->AddChannel(cout, 10);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(10);
    cmnClassRegister::SetLoD("sineTask", 10);
    cmnClassRegister::SetLoD("displayTask", 10);

    // create our two tasks
    const double PeriodSine = 1 * cmn_ms; // in milliseconds
    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    // create the task manager and the tasks/devices
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    sineTask * sineTaskObject =
        new sineTask("SIN", PeriodSine);
    clockDevice * clockDeviceObject =
        new clockDevice("CLOC");
    displayTask * displayTaskObject =
        new displayTask("DISP", PeriodDisplay);
    displayTaskObject->Configure();
    // add the tasks to the task manager and connect them
    taskManager->AddTask(sineTaskObject);
    taskManager->AddDevice(clockDeviceObject);
    taskManager->AddTask(displayTaskObject);
    taskManager->Connect("DISP", "DataGenerator", "SIN", "MainInterface");
    taskManager->Connect("DISP", "Clock", "CLOC", "MainInterface");

    taskManager->CreateAll();
    taskManager->StartAll();

    // we should not need this ...
    osaSleep(2.0); // seconds

    // start a python shell, will resume after shell is closed
    cmnObjectRegister::Register("TaskManager", taskManager);

#if 1  // PKAZ
    IreLaunch IRE(argc != 1);  // if any parameters, use IPython
    cout << "*** Launching IRE shell (C++ Thread) ***" << endl;
    osaThread IreThread;
    IreThread.Create<IreLaunch,  char *> (&IRE, &IreLaunch::Run, "from example4 import *");
    // Wait for IRE to initialize itself
    while (ireFramework::IsStarting())
        osaSleep(0.5 * cmn_s);  // Wait 0.5 seconds
    // Loop until IRE and display task are both exited
    while (ireFramework::IsActive() || !displayTaskObject->IsTerminated())
        osaSleep(0.5 * cmn_s);  // Wait 0.5 seconds
    // Cleanup and exit
    IreThread.Wait();
#else
    cmnPath path;
    path.AddFromEnvironment("PYTHONPATH");
    std::string exampleFile = path.Find("example4.py", cmnPath::READ);
    std::cout << "Using file " << exampleFile << std::endl;
    FILE * fileDescriptor;
    fileDescriptor = fopen(exampleFile.c_str(), "r");
    if (fileDescriptor == 0) {
        std::cout << "Can't open " << exampleFile << std::endl;
	} else {
        Py_Initialize();
        std::cout << "Starting Python shell with " << exampleFile << std::endl;
        // #if (CISST_OS != CISST_WINDOWS)
        PyRun_SimpleFile(fileDescriptor, exampleFile.c_str());
        // #endif
        PyRun_InteractiveLoop(fileDescriptor, exampleFile.c_str());
        fclose(fileDescriptor);
        Py_Finalize();
    }
    while (!displayTaskObject->IsTerminated()) {
        osaSleep(100.0 * cmn_ms); // sleep to save CPU
    }
#endif

    osaSleep(PeriodDisplay * 2);
    sineTaskObject->Kill();
    displayTaskObject->Kill();
    osaSleep(PeriodDisplay * 2);
    while (!sineTaskObject->IsTerminated()) osaSleep(PeriodDisplay);
    while (!displayTaskObject->IsTerminated()) osaSleep(PeriodDisplay);

    return 0;
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
