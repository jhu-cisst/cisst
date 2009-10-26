/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <QApplication>
#include <QMainWindow>

#include "ui_client.h"
#include "ui_server.h"
//#include "clientTask.h"
//#include "serverTask.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("QtCommandsAndEvents-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
//    cmnClassRegister::SetLoD("clientTask", CMN_LOG_LOD_VERY_VERBOSE);
//    cmnClassRegister::SetLoD("serverTask", CMN_LOG_LOD_VERY_VERBOSE);

    // create the tasks
    const double PeriodClient = 10.0 * cmn_ms;  // in milliseconds
    const double PeriodServer = 10.0 * cmn_ms;  // in milliseconds
//    serverTask * server = new serverTask("Server", PeriodServer);
//    clientTask * client = new clientTask("Client", PeriodClient);


    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    taskManager->AddTask(client);
//    taskManager->AddTask(server);

    // connect the tasks, e.g. RequiredInterface -> ProvidedInterface
//    taskManager->Connect("Client", "Required",
//                         "Server", "Provided");

    // generate a nice tasks diagram
    std::ofstream dotFile("QtCommandsAndEvents.dot");
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // create the tasks
    taskManager->CreateAll();

    // start the tasks
    taskManager->StartAll();

    // user interface in Qt
    QApplication application(argc, argv);
    QMainWindow clientMainWindow;  // client main window
    Ui::ClientWindow clientWindow;
    clientWindow.setupUi(&clientMainWindow);
    clientMainWindow.setWindowTitle("Client");
    clientMainWindow.show();
    QMainWindow serverMainWindow;  // server main window
    Ui::ClientWindow serverWindow;
    serverWindow.setupUi(&serverMainWindow);
    serverMainWindow.setWindowTitle("Server");
    serverMainWindow.show();
    application.exec();  // run gui

    // perform cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
