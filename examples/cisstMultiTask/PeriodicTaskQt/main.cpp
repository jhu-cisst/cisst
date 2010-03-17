/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLoggerQWidget.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsCollectorEvent.h>

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>

#include "displayQComponent.h"
#include "mtsCollectorQComponent.h"
#include "mtsCollectorQWidget.h"
#include "sineTask.h"

const unsigned int NumSineTasks = 2;

int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("PeriodicTaskQt-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("sineTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("displayQComponent", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsManagerLocal", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsManagerGlobal", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsCollectorQComponent", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsCollectorState", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsCollectorEvent", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsStateTable", CMN_LOG_LOD_VERY_VERBOSE);

    // create Qt user interface
    QApplication application(argc, argv);
    QWidget * mainWindow = new QWidget();
    QGridLayout * layout = new QGridLayout(mainWindow);
    mtsCollectorQWidget * collectorQWidget = new mtsCollectorQWidget();
    QPushButton * buttonQuit = new QPushButton("Quit", mainWindow);

    // configure Qt widgets
    mainWindow->setWindowTitle("Periodic Task Example");
    // add one collection widget for all sine generators
    layout->addWidget(collectorQWidget, 0, 0, 1, NumSineTasks);
    // one large quit button
    layout->addWidget(buttonQuit, 3, 0, 1, NumSineTasks);
    QObject::connect(buttonQuit, SIGNAL(clicked()),
                     QApplication::instance(), SLOT(quit()));

    // get the component manager to add multiple sine generator tasks
    mtsManagerLocal * taskManager = mtsManagerLocal::GetInstance();
    sineTask * sine;
    displayQComponent * display;
    mtsCollectorState * stateCollector;
    mtsCollectorQComponent * collectorQComponent;

    // create an event collector
    mtsCollectorEvent * eventCollector =
        new mtsCollectorEvent("EventCollector",
                              mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    taskManager->AddComponent(eventCollector);
    // add QComponent to control the event collector
    collectorQComponent = new mtsCollectorQComponent("EventCollectorQComponent");
    taskManager->AddComponent(collectorQComponent);
    // connect to the existing widget
    collectorQComponent->ConnectToWidget(collectorQWidget);
    taskManager->Connect(collectorQComponent->GetName(), "DataCollection",
                         eventCollector->GetName(), "Control");

    // create multiple sine generators along with their widget and
    // state collectors
    for (unsigned int i = 0; i < NumSineTasks; i++) {
        std::ostringstream index;
        index << i;

        // create the generator and its widget
        sine = new sineTask("SIN" + index.str(), 5.0 * cmn_ms);
        taskManager->AddComponent(sine);
        std::cout << *sine << std::endl;
        display = new displayQComponent("DISP" + index.str());
        taskManager->AddComponent(display);
        layout->addWidget(display->GetWidget(), 1, i);
        taskManager->Connect(display->GetName(), "DataGenerator",
                             sine->GetName(), "MainInterface");

        // create the state collector and connect it to the generator
        stateCollector = new mtsCollectorState(sine->GetName(),
                                               sine->GetDefaultStateTableName(),
                                               mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
        stateCollector->AddSignal("SineData");
        taskManager->AddComponent(stateCollector);
        stateCollector->Connect();
        // create the QComponent to bridge between the collection widget and the collector
        collectorQComponent = new mtsCollectorQComponent(sine->GetName() + "StateCollectorQComponent");
        taskManager->AddComponent(collectorQComponent);
        collectorQComponent->ConnectToWidget(collectorQWidget);
        taskManager->Connect(collectorQComponent->GetName(), "DataCollection",
                             stateCollector->GetName(), "Control");

        // add events to observe
        eventCollector->AddObservedComponent(sine);
    }

    // connect all interfaces for event collector
    eventCollector->Connect();

    // generate a nice tasks diagram
    std::ofstream dotFile("PeriodicTaskQt.dot");
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();
    
    // run Qt user interface
    mainWindow->resize(NumSineTasks * 220, 360);
    mainWindow->show();
    application.exec();

    // kill all tasks and perform cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
