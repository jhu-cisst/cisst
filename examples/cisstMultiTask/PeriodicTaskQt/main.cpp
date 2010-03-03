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

#include <cisstCommon/cmnLoggerQWidget.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>

#include "displayQComponent.h"
#include "mtsCollectorQComponent.h"
#include "sineTask.h"

const unsigned int NumSineTasks = 3;


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
    cmnClassRegister::SetLoD("mtsCollectorState", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsStateTable", CMN_LOG_LOD_VERY_VERBOSE);

    // create Qt user interface
    QApplication application(argc, argv);
    QWidget * mainWindow = new QWidget();
    QGridLayout * layout = new QGridLayout(mainWindow);
    mtsCollectorQWidget * collectorQWidget = new mtsCollectorQWidget();
    QPushButton * buttonQuit = new QPushButton("Quit", mainWindow);

    // configure Qt widgets
    mainWindow->setWindowTitle("Periodic Task Example");
    layout->addWidget(collectorQWidget, 0, 0, 1, NumSineTasks);
    layout->addWidget(buttonQuit, 3, 0, 1, NumSineTasks);
    QObject::connect(buttonQuit, SIGNAL(clicked()),
                     qApp, SLOT(quit()));

    //
    mtsManagerLocal * taskManager = mtsManagerLocal::GetInstance();
    sineTask * sine;
    displayQComponent * display;
    mtsCollectorState * collector;
    mtsCollectorQComponent * collectorQComponent;

    //
    for (unsigned int i = 0; i < NumSineTasks; i++) {
        std::ostringstream index;
        index << i;

        sine = new sineTask("SIN" + index.str(), 1.0 * cmn_ms);
        display = new displayQComponent("DISP" + index.str());
        collectorQComponent = new mtsCollectorQComponent("DataCollection" + index.str());

        taskManager->AddComponent(sine);
        taskManager->AddComponent(display);
        taskManager->AddComponent(collectorQComponent);

        layout->addWidget(display->GetWidget(), 1, i);
        collector = new mtsCollectorState(sine->GetName(),
                                          sine->GetDefaultStateTableName(),
                                          mtsCollectorBase::COLLECTOR_LOG_FORMAT_CSV);
        collector->AddSignal("SineData");
        collectorQComponent->ConnectToWidget(collectorQWidget);

        taskManager->Connect(display->GetName(), "DataGenerator",
                             sine->GetName(), "MainInterface");
        taskManager->Connect(collectorQComponent->GetName(), "DataCollection",
                             collector->GetName(), "Control");
    }

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
