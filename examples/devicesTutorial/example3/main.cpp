/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

/*
  Author(s):  Gorkem Sevinc, Anton Deguet
  Created on: 2009-07-06

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devSensableHDMasterSlave.h>
#if CISST_HAS_IRE
#include <cisstInteractive.h>
#endif
#include <cisstCommon.h>

#include "displayTask.h"

#define USE_AS_SERVER 0

using namespace std;


int main(int argc, char * argv[])

{
#if USE_AS_SERVER
/*    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " GlobalManagerIP ServerTaskIP" << std::endl;
        exit(-1);
    }
    std::string globalTaskManagerIP(argv[1]);
    std::string serverTaskIP(argv[2]);
*/
#endif

    std::string globalTaskManagerIP("10.0.0.100");
    std::string serverTaskIP("10.0.0.101");

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example3-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devSensableHDMasterSlave", CMN_LOG_LOD_VERY_VERBOSE);

    const double PeriodDisplay = 1.0; // in milliseconds
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    displayTask * displayTaskObjectFirstPair = new displayTask("DisplayFirstPair", PeriodDisplay * cmn_ms);
    taskManager->AddTask(displayTaskObjectFirstPair);
    devSensableHDMasterSlave * sensableOmni = new devSensableHDMasterSlave("RemoteOmni", "Omni1", "Omni2");
    // devSensableHDMasterSlave * sensableOmni = new devSensableHDMasterSlave("Omni", "Omni1", "Omni2", "Omni3", "Omni4");
    taskManager->AddTask(sensableOmni);

#if CISST_HAS_IRE
    cmnObjectRegister::Register("TaskManager", taskManager);

    cout << "*** Launching IRE shell (C++ Thread) ***" << endl;
    osaThread IreThread;
    if (argc != 1)   // if any parameters, use IPython
        IreThread.Create<char *> (&ireFramework::RunIRE_IPython, "");
    else             // else use wxPython
        IreThread.Create<char *> (&ireFramework::RunIRE_wxPython, "");
#endif

#if USE_AS_SERVER
    taskManager->SetGlobalTaskManagerIP(globalTaskManagerIP);
    taskManager->SetServerTaskIP(serverTaskIP);
    taskManager->SetTaskManagerType(mtsTaskManager::TASK_MANAGER_CLIENT); // client of task manager
#endif


    taskManager->Connect("DisplayFirstPair", "TeleoperationParameters", 
                         "RemoteOmni", "TeleoperationParametersOmni1Omni2");

    // generate a nice tasks diagram
    std::ofstream dotFile("example1.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    osaSleep(0.5 * cmn_ms);

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();

    do
    {
        osaSleep(10.0 * cmn_ms);
    } while(!displayTaskObjectFirstPair->GetExitFlag());
    // cleanup
    taskManager->KillAll();

    osaSleep(PeriodDisplay * 2);

#if CISST_HAS_IRE
    // Cleanup and exit
    IreThread.Wait();
#endif
    // while (!displayTaskObjectFirstPair->IsTerminated()) osaSleep(PeriodDisplay);
    taskManager->Cleanup();
    return 0;
}

