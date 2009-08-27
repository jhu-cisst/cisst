/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: main.cpp 433 2009-06-09 22:10:24Z adeguet1 $ */

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
#include <cisstCommon.h>

#include "displayTask.h"

using namespace std;

int main(void)
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devSensableHD", CMN_LOG_LOD_VERY_VERBOSE);

    const double PeriodDisplay = 1.0; // in milliseconds
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    
    displayTask * displayTaskObjectFirstPair = new displayTask("DisplayFirstPair", PeriodDisplay * cmn_ms);
    taskManager->AddTask(displayTaskObjectFirstPair);

    //devSensableHDMasterSlave * sensableOmni = new devSensableHDMasterSlave("Omni", "Omni1", "Omni2");
    devSensableHDMasterSlave * sensableOmni = new devSensableHDMasterSlave("Omni", "Omni1", "Omni2", "Omni3", "Omni4");
    taskManager->AddTask(sensableOmni);

    taskManager->Connect("DisplayFirstPair", "TeleoperationParameters", 
                         "Omni", "TeleoperationParametersOmni1Omni2");

    // generate a nice tasks diagram
    std::ofstream dotFile("example1.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

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
    while (!displayTaskObjectFirstPair->IsTerminated()) osaSleep(PeriodDisplay);

    return 0;
}

