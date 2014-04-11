/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include "serverTask.h"
#include "clientTask.h"

#include <cisstOSAbstraction/osaSleep.h>

// Enable or disable system-wide thread-safe logging
//#define MTS_LOGGING

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

int main(int argc, char **argv)
{
#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT|MCL_FUTURE);
#endif

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    // get all messages to log file
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    // get only errors and warnings to std::cout
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("clientTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("serverTask", CMN_LOG_ALLOW_ALL);
    // enable system-wide thread-safe logging
#ifdef MTS_LOGGING
    mtsManagerLocal::SetLogForwarding(true);
#endif

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
    serverTaskBase * server;
    if (serverGeneric) {
        server = new serverTask<mtsDouble>("Server");
    } else {
        server = new serverTask<double>("Server");
    }

    clientTaskBase * client1;
    clientTaskBase * client2;
    if (clientGeneric) {
        client1 = new clientTask<mtsDouble>("Client1");
        client2 = new clientTask<mtsDouble>("Client2");
    } else {
        client1 = new clientTask<double>("Client1");
        client2 = new clientTask<double>("Client2");
    }

    server->Configure();
    client1->Configure();
    client2->Configure();

    // add the tasks to the task manager
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    componentManager->AddComponent(server);
    componentManager->AddComponent(client1);
    componentManager->AddComponent(client2);

    // connect the tasks, task.RequiresInterface -> task.ProvidesInterface
    if (!componentManager->Connect("Client1", "Required", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Failed to connect: Client1:Required-Server:Provided" << std::endl;
        return 1;
    }
    if (!componentManager->Connect("Client2", "Required", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Failed to connect: Client2:Required-Server:Provided" << std::endl;
        return 1;
    }

    // create the tasks, i.e. find the commands
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    // start the periodic Run
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // wait until the close button of the UI is pressed
    while (server->UIOpened() || client1->UIOpened() || client2->UIOpened()) {
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
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    // delete components
    delete server;
    delete client1;
    delete client2;

    componentManager->Cleanup();
    cmnLogger::Kill();
    return 0;
}
