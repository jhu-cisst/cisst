/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "serverTask.h"
#include "clientTask.h"

int main(int argc, char **argv)
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example9Local");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsInterfaceProvided", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("clientTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("serverTask", CMN_LOG_LOD_VERY_VERBOSE);

    // Command line parameter:
    //    1 -- server uses mtsDouble, client uses double
    //    2 -- server uses double, client uses mtsDouble
    //    3 -- server uses double, client uses double
    //    default: server uses mtsDouble, client uses mtsDouble
    bool clientGeneric = true;
    bool serverGeneric = true;
    if (argc > 1) {
        if (argv[1][0] == '1')
            clientGeneric = false;
        else if (argv[1][0] == '2')
            serverGeneric = false;
        else if (argv[1][0] == '3') {
            clientGeneric = false;
            serverGeneric = false;
        }
    }

    // create our two tasks
    const double PeriodClient = 10 * cmn_ms; // in milliseconds
    const double PeriodServer = 10 * cmn_ms; // in milliseconds

    serverTaskBase *server;
    if (serverGeneric)
        server = new serverTask<mtsDouble>("Server", PeriodServer);
    else
        server = new serverTask<double>("Server", PeriodServer);

    clientTaskBase *client;
    if (clientGeneric)
        client = new clientTask<mtsDouble>("Client", PeriodClient);
    else
        client = new clientTask<double>("Client", PeriodClient);

    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    // reconfiguration test 1
#if 0
#if CISST_MTS_HAS_ICE
    taskManager = mtsTaskManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    taskManager->AddTask(client);
    taskManager->AddTask(server);
    // reconfiguration test 2
#if 0
#if CISST_MTS_HAS_ICE
    taskManager = mtsTaskManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    // connect the tasks, task.RequiresInterface -> task.ProvidesInterface
    if (!taskManager->Connect("Client", "Required", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Failed to connect: Client:Required-Server:Provided" << std::endl;
        return 1;
    }

    // reconfiguration test 3
#if 0
#if CISST_MTS_HAS_ICE
    taskManager = mtsTaskManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();

    // reconfiguration test 4
#if 1
#if CISST_MTS_HAS_ICE
    taskManager = mtsTaskManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    // wait until the close button of the UI is pressed
    while (server->UIOpened() || client->UIOpened()) {
        osaSleep(10.0 * cmn_ms); // sleep to save CPU
    }
    // cleanup
    taskManager->KillAll();
    taskManager->Cleanup();
    return 0;
}

/*
  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
