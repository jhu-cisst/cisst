/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <QApplication>

#include "displayQDevice.h"
#include "sineTask.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("PeriodicTaskQt-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("sineTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("displayQDevice", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsCollectorState", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsStateTable", CMN_LOG_LOD_VERY_VERBOSE);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the tasks
    sineTask * sine = new sineTask("SIN", 1.0 * cmn_ms);
    displayQDevice * display = new displayQDevice("DISP");

    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddTask(sine);
    taskManager->AddDevice(display);

    // connect the tasks, e.g. RequiredInterface -> ProvidedInterface
    taskManager->Connect("DISP", "DataGenerator",
                         "SIN", "MainInterface");

    // add data collection for sineTask state table
    mtsCollectorState * collector =
        new mtsCollectorState("SIN",
                              sine->GetDefaultStateTableName(),
                              mtsCollectorBase::COLLECTOR_LOG_FORMAT_CSV);
    collector->AddSignal("SineData");
    taskManager->Connect("DISP", "DataCollection",
                         collector->GetName(), "Control");

    // generate a nice tasks diagram
    std::ofstream dotFile("PeriodicTaskQt.dot");
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();

    // run Qt user interface
    application.exec();

    // kill all tasks and perform cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
