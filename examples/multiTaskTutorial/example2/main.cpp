/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id$

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "sineTask.h"
#include "clockDevice.h"
#include "displayTask.h"
#include "displayUI.h"

using namespace std;

int main(void)
{
    // log configuration, see previous examples
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("sineTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("displayTask", CMN_LOG_LOD_VERY_VERBOSE);

    // determine which mode to use to save the data
    int choice = 0;
    std::cout << "Enter [b] for binary log, [t] for plain text and [c] for csv\n";
    while ((choice != 'b') && (choice != 't') && (choice != 'c')) {
        choice = cmnGetChar();
    }
    // create our two tasks
    const double PeriodSine = 1 * cmn_ms; // in milliseconds
    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    // create the task manager and the tasks/devices
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    sineTask * sineTaskObject = new sineTask("SIN", PeriodSine);
    clockDevice * clockDeviceObject = new clockDevice("CLOC");
    displayTask * displayTaskObject = new displayTask("DISP", PeriodDisplay);
    displayTaskObject->Configure();
    // add the tasks to the task manager and connect them
    taskManager->AddTask(sineTaskObject);
    taskManager->AddDevice(clockDeviceObject);
    taskManager->AddTask(displayTaskObject);
    taskManager->Connect("DISP", "DataGenerator", "SIN", "MainInterface");
    taskManager->Connect("DISP", "Clock", "CLOC", "MainInterface");
    // generate graph
    std::ofstream dotFile("example2.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();
    // add data collection for sineTask state table
    mtsCollectorState * collector = 0;
    if (choice == 'b') {
        collector =
            new mtsCollectorState("SIN",
                                  mtsCollectorBase::COLLECTOR_LOG_FORMAT_BINARY);
    } else if (choice == 't') {
        collector =
            new mtsCollectorState("SIN",
                                  mtsCollectorBase::COLLECTOR_LOG_FORMAT_PLAIN_TEXT);
    } else if (choice == 'c') {
        collector =
            new mtsCollectorState("SIN",
                                  mtsCollectorBase::COLLECTOR_LOG_FORMAT_CSV);
    }
    // specify which signal (aka state data) to collect
    if (!collector->AddSignal("SineData")) {
        std::cerr << "Can't find signal named \"SineData\"" << std::endl;
    }
    taskManager->AddTask(collector);

    taskManager->CreateAll();
    taskManager->StartAll();

    collector->SetSamplingInterval(4); // collect every other 4 sample
    collector->StartCollection(0.0); // delay in seconds

    while (!displayTaskObject->IsTerminated()) {
        osaSleep(100.0 * cmn_ms); // sleep to save CPU
    }

    taskManager->KillAll();

    osaSleep(PeriodSine * 2);
    while (!sineTaskObject->IsTerminated()) osaSleep(PeriodSine);

    // perform conversion from binary to text (csv)
    if (choice == 'b') {
        mtsCollectorState::ConvertBinaryToText(collector->GetLogFileName(),
                                               collector->GetLogFileName() + ".csv",
                                               ','); // comma separated
    }
    taskManager->Cleanup();
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
