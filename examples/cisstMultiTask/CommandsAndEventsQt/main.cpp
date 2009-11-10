/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

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

#include "clientQDevice.h"
#include "serverQDevice.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("CommandsAndEventsQt-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("clientQDevice", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("serverQDevice", CMN_LOG_LOD_VERY_VERBOSE);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the tasks with their respective UIs
    serverQDevice * server = new serverQDevice("Server");
    clientQDevice * client = new clientQDevice("Client");

    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddDevice(client);
    taskManager->AddDevice(server);

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
