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
  \brief An example interface for NDI trackers with serial interface.
  \ingroup devicesTutorial
*/

#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsCollectorState.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devNDISerial.h>
#include <cisstDevices/devNDISerialControllerQDevice.h>
#include <cisstDevices/devNDISerialToolQDevice.h>

#include <QApplication>
#include <QMainWindow>


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("example6-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("devNDISerial", CMN_LOG_LOD_RUN_WARNING);
    cmnClassRegister::SetLoD("devNDISerialControllerQDevice", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devNDISerialToolQDevice", CMN_LOG_LOD_VERY_VERBOSE);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the tasks
    devNDISerial * taskNDISerial = new devNDISerial("taskNDISerial", 50.0 * cmn_ms);
    devNDISerialControllerQDevice * taskControllerQDevice = new devNDISerialControllerQDevice("taskControllerQDevice");

    // configure the tasks
    cmnPath searchPath = std::string(CISST_SOURCE_ROOT) + "/examples/devicesTutorial/example6";
    taskNDISerial->Configure(searchPath.Find("config.xml"));

    // add the tasks to the task manager
    mtsManagerLocal * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddComponent(taskNDISerial);
    taskManager->AddComponent(taskControllerQDevice);

    // connect the tasks, e.g. RequiredInterface -> ProvidedInterface
    taskManager->Connect(taskControllerQDevice->GetName(), "Controller",
                         taskNDISerial->GetName(), "Controller");

    // add data collection for devNDISerial state table
    mtsCollectorState * taskCollector =
            new mtsCollectorState(taskNDISerial->GetName(),
                                  taskNDISerial->GetDefaultStateTableName(),
                                  mtsCollectorBase::COLLECTOR_LOG_FORMAT_CSV);

    // add interfaces for tools and populate controller widget with tool widgets
    for (unsigned int i = 0; i < taskNDISerial->GetNumberOfTools(); i++) {
        std::string toolName = taskNDISerial->GetToolName(i);
        devNDISerialToolQDevice * taskToolQDevice = new devNDISerialToolQDevice(toolName);
        taskControllerQDevice->AddToolWidget(taskToolQDevice->GetWidget());
        taskManager->AddComponent(taskToolQDevice);
        taskManager->Connect(toolName, toolName,
                             taskNDISerial->GetName(), toolName);

        taskCollector->AddSignal(toolName + "Position");
    }
    taskManager->Connect(taskControllerQDevice->GetName(), "DataCollector",
                         taskCollector->GetName(), "Control");

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();

    // create a main window to hold QWidgets
    QMainWindow * mainWindow = new QMainWindow();
    mainWindow->setCentralWidget(taskControllerQDevice->GetWidget());
    mainWindow->setWindowTitle("NDI Serial Controller");
    mainWindow->resize(0,0);
    mainWindow->show();

    // run Qt user interface
    application.exec();

    // kill all tasks and perform cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
