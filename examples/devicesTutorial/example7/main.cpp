/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-11-06

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief An example interface for Claron Micron Tracker.
  \ingroup devicesTutorial
*/

#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsCollectorState.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devMicronTracker.h>

#include <QApplication>
#include <QMainWindow>

#include "devMicronTrackerControllerQDevice.h"
#include "devMicronTrackerToolQDevice.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    // add a log per thread
    osaThreadedLogFile threadedLog("example7-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);

    // set the log level of detail on select components
    cmnLogger::SetMaskClass("devMicronTracker", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("devMicronTrackerControllerQDevice", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("devMicronTrackerToolQDevice", CMN_LOG_ALLOW_ALL);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the components
    devMicronTracker * componentMicronTracker = new devMicronTracker("componentMicronTracker", 50.0 * cmn_ms);
    devMicronTrackerControllerQDevice * componentControllerQDevice = new devMicronTrackerControllerQDevice("componentControllerQDevice");

    // configure the components
    cmnPath searchPath = std::string(CISST_SOURCE_ROOT) + "/examples/devicesTutorial/example7";
    componentMicronTracker->Configure(searchPath.Find("config.xml"));

    // add the components to the component manager
    mtsManagerLocal * componentManager = mtsComponentManager::GetInstance();
    componentManager->AddComponent(componentMicronTracker);
    componentManager->AddComponent(componentControllerQDevice);

    // connect the components, e.g. RequiredInterface -> ProvidedInterface
    componentManager->Connect(componentControllerQDevice->GetName(), "Controller",
                              componentMicronTracker->GetName(), "Controller");

    // add data collection for devMicronTracker state table
    mtsCollectorState * componentCollector =
        new mtsCollectorState(componentMicronTracker->GetName(),
                              componentMicronTracker->GetDefaultStateTableName(),
                              mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);

    // add interfaces for tools and populate controller widget with tool widgets
    for (unsigned int i = 0; i < componentMicronTracker->GetNumberOfTools(); i++) {
        std::string toolName = componentMicronTracker->GetToolName(i);
        devMicronTrackerToolQDevice * componentToolQDevice = new devMicronTrackerToolQDevice(toolName);
        componentControllerQDevice->AddToolWidget(componentToolQDevice->GetWidget(),
                                                  componentToolQDevice->GetMarkerProjectionLeft(),
                                                  componentToolQDevice->GetMarkerProjectionRight());
        componentManager->AddComponent(componentToolQDevice);
        componentManager->Connect(toolName, toolName,
                                  componentMicronTracker->GetName(), toolName);

        componentCollector->AddSignal(toolName + "Position");
    }
    componentManager->AddComponent(componentCollector);
    componentCollector->Connect();
    componentManager->Connect(componentControllerQDevice->GetName(), "DataCollector",
                              componentCollector->GetName(), "Control");

    // create and start all components
    componentManager->CreateAll();
    componentManager->StartAll();

    // create a main window to hold QWidgets
    QMainWindow * mainWindow = new QMainWindow();
    mainWindow->setCentralWidget(componentControllerQDevice->GetWidget());
    mainWindow->setWindowTitle("MicronTracker Controller");
    mainWindow->resize(0,0);
    mainWindow->show();

    // run Qt user interface
    application.exec();

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->Cleanup();

    return 0;
}
