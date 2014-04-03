/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "sineTask.h"
#include "displayTask.h"
#include "UITask.h"
#include "displayUI.h"

using namespace std;

// If the following macro is enabled, network-related feature is disabled.
//#define LOCAL_VERSION

bool IsGlobalTaskManager = false;
bool IsServerTask = false;
string GlobalTaskManagerIP;
string ServerTaskIP;

void help(const char * programName)
{
    cerr << endl 
         << "Usage: multiTaskTutorialExample1-3 [OPTIONS] [ServerIP_1] [ServerIP_2]" << endl 
         << endl
         << "[OPTIONS]" << endl
         << "  -s,    run a server task manager (global task manager)" << endl
         << "  -cs,   run a client task manager with a server task" << endl
         << "  -cc,   run a client task manager with a client task" << endl
         << endl
         << "[ServerIP_1] Global Task Manager IP address (default: localhost)" << endl
         << "[ServerIP_2] Server Task IP address (default: localhost)"<< endl
         << endl;
}

void ParseOption(const char * argv)
{
    if (strcmp(argv, "-s") == 0) {
        IsGlobalTaskManager = true;
    } else if (strcmp(argv, "-cs") == 0 || strcmp(argv, "-cc") == 0) {
        IsGlobalTaskManager = false;

        // Create a server task
        if (strcmp(argv, "-cs") == 0) {
            IsServerTask = true;
        } 
        // Create a client task
        else {
            IsServerTask = false;
        }
    }
}

void ParseIP(const char * arg1, const char * arg2)
{
    GlobalTaskManagerIP = arg1;
    ServerTaskIP = arg2;
}

int main(int argc, char * argv[])
{
    string serverTaskName = "SIN", clientTaskName = "DISP";

#ifndef LOCAL_VERSION
    // Check arguments
    if (argc == 2) {
        if (strcmp(argv[1], "-s") == 0) {
            ParseOption(argv[1]);
        } else {
            help(argv[0]);
            return 1;
        }
    } else if (argc == 4) {
        if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-cs") == 0 || strcmp(argv[1], "-cc") == 0) {
            ParseOption(argv[1]);
            ParseIP(argv[2], argv[3]);
        } else {
            help(argv[0]);
            return 1;
        }
    } else {
        help(argv[0]);
        return 1;
    }
#endif

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-3_");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("sineTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("displayTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);

#ifndef LOCAL_VERSION
    //-------------------------------------------------------------------------
    // Create default local tasks
    //-------------------------------------------------------------------------
    // Get the TaskManager instance and set operation mode
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    const double PeriodDisplay = 50 * cmn_ms;

    sineTask * sineTaskObject = NULL;
    displayTask * displayTaskObject = NULL;
    UITask * UITaskObject = NULL;

    if (IsGlobalTaskManager) {
        UITaskObject = new UITask("UITask", PeriodDisplay);
        UITaskObject->Configure();

        taskManager->AddTask(UITaskObject);

        taskManager->SetTaskManagerType(mtsTaskManager::TASK_MANAGER_SERVER);
    } else {
        //-------------------------------------------------------------------------
        // Create a task which works over networks
        //-------------------------------------------------------------------------
        const double PeriodSine = 1 * cmn_ms;        

        if (IsServerTask) {
            sineTaskObject = new sineTask(serverTaskName, PeriodSine);
            UITaskObject = new UITask("UITask", PeriodDisplay);
            UITaskObject->Configure();
            
            taskManager->AddTask(UITaskObject);
            taskManager->AddTask(sineTaskObject);
        } else {
            displayTaskObject = new displayTask(clientTaskName, PeriodDisplay);
            displayTaskObject->Configure();

            taskManager->AddTask(displayTaskObject);        
        }

        taskManager->SetGlobalTaskManagerIP(GlobalTaskManagerIP);
        taskManager->SetServerTaskIP(ServerTaskIP);

        // Set the type of task manager either as a server or as a client.
        // mtsTaskManager::SetTaskManagerType() should be called before
        // mtsTaskManager::Connect()
        taskManager->SetTaskManagerType(mtsTaskManager::TASK_MANAGER_CLIENT);

        //
        // TODO: Hide this waiting routine inside mtsTaskManager using events or other things.
        //
        osaSleep(0.5 * cmn_s);

        // Connect the tasks across networks
        if (!IsServerTask) {
            taskManager->Connect(clientTaskName, "DataGenerator", serverTaskName, "MainInterface");
        }
    }

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();

    if (IsGlobalTaskManager) {
        while (1) {
            osaSleep(10 * cmn_ms);
            if (UITaskObject->IsTerminated()) {
                break;
            }
        }
    } else {
        if (IsServerTask) {
            while (1) {
                osaSleep(10 * cmn_ms);
                if (UITaskObject->IsTerminated()) {
                    break;
                }
            }
        } else {
            while (1) {
                osaSleep(10 * cmn_ms);
                if (displayTaskObject->IsTerminated()) {
                    break;
                }
            }
        }
    }

    // cleanup
    taskManager->KillAll();

    osaSleep(PeriodDisplay * 2);

    if (IsGlobalTaskManager) {
        while (!UITaskObject->IsTerminated()) osaSleep(PeriodDisplay);
    } else {
        if (IsServerTask) {
            while (!sineTaskObject->IsTerminated()) osaSleep(PeriodDisplay);
            while (!UITaskObject->IsTerminated()) osaSleep(PeriodDisplay);
        } else {
            while (!displayTaskObject->IsTerminated()) osaSleep(PeriodDisplay);
        }
    }

    // Deallocate user-created objects.
    // MJUNG: Currently, there is no mechanism to deallocate resources created and
    // managed by the task manager--e.g., interfaces, command objects, function
    // pointers, and so on.
    
    // As a temporary solution, I added task deletion commands here to close 
    // the ICE application safely and cleanly. However, this is definitely not a proper
    // way to delete a task registered to the task manager; it's brutal. So it can
    // cause another errors or problems related to cisstMultiTask library itself.
    if (sineTaskObject) delete sineTaskObject;
    if (displayTaskObject) delete displayTaskObject;
    if (UITaskObject) delete UITaskObject;

#else
    // create our two tasks
    const double PeriodSine = 1 * cmn_ms; // in milliseconds
    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    sineTask * sineTaskObject = new sineTask("SIN", PeriodSine);
    
    displayTask * displayTaskObject = new displayTask("DISP", PeriodDisplay);
    displayTaskObject->Configure();

    // add the tasks to the task manager
    taskManager->AddTask(sineTaskObject);
    taskManager->AddTask(displayTaskObject);

    // connect the tasks, task.RequiresInterface -> task.ProvidesInterface
    taskManager->Connect("DISP", "DataGenerator", "SIN", "MainInterface");

    // generate a nice tasks diagram
    std::ofstream dotFile("example1-3.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();

    // wait until the close button of the UI is pressed
    while (!displayTaskObject->IsTerminated()) {
        osaSleep(10.0 * cmn_ms); // sleep to save CPU
    }
    // cleanup
    taskManager->KillAll();

    osaSleep(PeriodSine * 2);
    while (!sineTaskObject->IsTerminated()) osaSleep(PeriodSine);
#endif

    return 0;
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
