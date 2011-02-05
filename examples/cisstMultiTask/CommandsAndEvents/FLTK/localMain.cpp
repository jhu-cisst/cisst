/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: localMain.cpp 2276 2011-01-31 16:52:55Z adeguet1 $

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "serverTask.h"
#include "clientTask.h"

int main(int argc, char **argv)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    // get all message to log file
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    // get only errors and warnings to std::cout
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("clientTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("serverTask", CMN_LOG_ALLOW_ALL);

    // Command line parameter:
    //    1 -- server uses mtsDouble, client uses double
    //    2 -- server uses double, client uses mtsDouble
    //    3 -- server uses double, client uses double
    //    default: server uses mtsDouble, client uses mtsDouble
    bool clientGeneric = true;
    bool serverGeneric = true;
    if (argc > 1) {
        if (argv[1][0] == '1') {
            clientGeneric = false;
        } else if (argv[1][0] == '2') {
            serverGeneric = false;
        } else if (argv[1][0] == '3') {
            clientGeneric = false;
            serverGeneric = false;
        }
    }

    // create our two tasks
    const double PeriodClient = 10 * cmn_ms; // in milliseconds
    const double PeriodServer = 10 * cmn_ms; // in milliseconds

    serverTaskBase * server;
    if (serverGeneric) {
        server = new serverTask<mtsDouble>("Server", PeriodServer);
    } else {
        server = new serverTask<double>("Server", PeriodServer);
    }

    clientTaskBase * client;
    if (clientGeneric) {
        client = new clientTask<mtsDouble>("Client", PeriodClient);
    } else {
        client = new clientTask<double>("Client", PeriodClient);
    }

    server->Configure();
    client->Configure();

    // add the tasks to the task manager
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    // reconfiguration test 1
#if 0
#if CISST_MTS_HAS_ICE
    componentManager = mtsComponentManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    componentManager->AddComponent(client);
    componentManager->AddComponent(server);
    // reconfiguration test 2
#if 0
#if CISST_MTS_HAS_ICE
    componentManager = mtsTaskManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    // connect the tasks, task.RequiresInterface -> task.ProvidesInterface
    if (!componentManager->Connect("Client", "Required", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Failed to connect: Client:Required-Server:Provided" << std::endl;
        return 1;
    }

    // reconfiguration test 3
#if 0
#if CISST_MTS_HAS_ICE
    componentManager = mtsComponentManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    // create the tasks, i.e. find the commands
    componentManager->CreateAll();
    // start the periodic Run
    componentManager->StartAll();

    // reconfiguration test 4
#if 0
#if CISST_MTS_HAS_ICE
    componentManager = mtsComponentManager::GetInstance("localhost", "localMainTest");
#endif
#endif

    // wait until the close button of the UI is pressed
    while (server->UIOpened() || client->UIOpened()) {
        Fl::lock();
        {
            Fl::check();
        }
        Fl::unlock();
        Fl::awake();
        osaSleep(5.0 * cmn_ms); // sleep to save CPU
    }
    // cleanup
    componentManager->KillAll();
    componentManager->Cleanup();
    return 0;
}
