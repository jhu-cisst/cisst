/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask.h>

#include <QApplication>

#include "clientQDevice.h"


int main(int argc, char * argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " GlobalManagerIP ServerTaskIP" << std::endl;
        exit(-1);
    }

    std::string globalTaskManagerIP(argv[1]);
    std::string serverTaskIP(argv[2]);

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("QtCommandsAndEventsClient-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("clientQDevice", CMN_LOG_LOD_VERY_VERBOSE);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create our client task
    clientQDevice * client = new clientQDevice("Client");

    // Get the TaskManager instance and set operation mode
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddDevice(client);
    taskManager->SetGlobalTaskManagerIP(globalTaskManagerIP);
    taskManager->SetServerTaskIP(serverTaskIP);

    // Set the type of task manager either as a server or as a client.
    // mtsTaskManager::SetTaskManagerType() should be called before
    // mtsTaskManager::Connect()
    taskManager->SetTaskManagerType(mtsTaskManager::TASK_MANAGER_CLIENT);

    //
    // TODO: Hide this waiting routine inside mtsTaskManager using events or other things.
    //
    osaSleep(0.5 * cmn_s);

    // Connect the tasks across networks
    taskManager->Connect("Client", "Required", "Server", "Provided");

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
