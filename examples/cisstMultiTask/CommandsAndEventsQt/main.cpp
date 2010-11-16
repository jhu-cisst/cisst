/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <QApplication>

#include "clientQtComponent.h"
#include "serverQtComponent.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    // add a log per thread
    osaThreadedLogFile threadedLog("CommandsAndEventsQt-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);

    // set the log level of detail on select tasks
    cmnLogger::SetMaskClass("clientQtComponent", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("serverQtComponent", CMN_LOG_ALLOW_ALL);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the tasks with their respective UIs
    serverQtComponent * server = new serverQtComponent("Server");
    clientQtComponent * client = new clientQtComponent("Client");

    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddComponent(client);
    taskManager->AddComponent(server);

    // connect the tasks, e.g. RequiredInterface -> ProvidedInterface
    taskManager->Connect("Client", "Required",
                         "Server", "Provided");

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
