/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-10-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief An example for NDI trackers with serial interface
  \ingroup devicesTutorial
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devNDiSerial.h>

#include <QApplication>

#include "devNDISerialControllerQDevice.h"
#include "devNDISerialToolQDevice.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("example6-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("devNDiSerial", CMN_LOG_LOD_VERY_VERBOSE);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the tasks
    devNDiSerial * devNDiSerialTask = new devNDiSerial("devNDiSerial", "COM3");
    devNDISerialControllerQDevice * controllerQDevice = new devNDISerialControllerQDevice("controllerQDevice");

    // configure the tasks
    devNDiSerialTask->Configure();

    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddTask(devNDiSerialTask);
    taskManager->AddDevice(controllerQDevice);

    // connect the tasks, i.e. RequiredInterface -> ProvidedInterface
    taskManager->Connect("controllerQDevice", "RequiresNDISerialController",
                         "devNDiSerial", "ProvidesNDISerialController");

    // add interfaces for tools and populate controller widget with tool widgets
    const unsigned int numberOfTools = devNDiSerialTask->GetNumberOfTools();
    for (unsigned int i = 0; i < numberOfTools; i++) {
        std::string toolName = devNDiSerialTask->GetToolName(i);
        devNDISerialToolQDevice * toolQDevice = new devNDISerialToolQDevice(toolName);
        controllerQDevice->AddToolWidget(toolQDevice->GetToolWidget(), i);
        taskManager->AddDevice(toolQDevice);
        taskManager->Connect(toolName, toolName,
                             "devNDiSerial", toolName);
    }

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
