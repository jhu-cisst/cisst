/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "sineTask.h"
#include "displayTask.h"
#include "UITask.h"
#include "displayUI.h"

using namespace std;

/*
    Server task : SIN - provided interface

    Client task : DISP - required interface
*/

bool IsGlobalTaskManager = false;
bool IsServerTask = false;
string GlobalTaskManagerIP;
string ServerTaskIP;

void help()
{
    cerr << endl 
         << "Usage: multiTaskTutorialExample1-2 [OPTIONS] [ServerIP_1] [ServerIP_2]" << endl 
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

    // Check arguments
    if (argc == 2) {
        if (strcmp(argv[1], "-s") == 0) {
            ParseOption(argv[1]);
        } else {
            help();
            return 1;
        }
    } else if (argc == 4) {
        if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-cs") == 0 || strcmp(argv[1], "-cc") == 0) {
            ParseOption(argv[1]);
            ParseIP(argv[2], argv[3]);
        } else {
            help();
            return 1;
        }
    } else {
        help();
        return 1;
    }

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-2_");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClass("sineTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("displayTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

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

        taskManager->AddComponent(UITaskObject);
    } else {
        //-------------------------------------------------------------------------
        // Create a task which works over networks
        //-------------------------------------------------------------------------
        const double PeriodSine = 1 * cmn_ms;        

        if (IsServerTask) {
            sineTaskObject = new sineTask(serverTaskName, PeriodSine);
            UITaskObject = new UITask("UITask", PeriodDisplay);
            UITaskObject->Configure();
            
            taskManager->AddComponent(UITaskObject);
            taskManager->AddComponent(sineTaskObject);
        } else {
            displayTaskObject = new displayTask(clientTaskName, PeriodDisplay);
            displayTaskObject->Configure();

            taskManager->AddComponent(displayTaskObject);        
        }

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
