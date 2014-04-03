/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "clientTask.h"
#include <cisstOSAbstraction/osaSleep.h>

#if !CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsSocketProxyClient.h>
#endif

// Enable or disable system-wide thread-safe logging
//#define MTS_LOGGING

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

int main(int argc, char * argv[])
{
#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT|MCL_FUTURE);
#endif
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    // get all message to log file
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    // get only errors and warnings to std::cout
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("clientTask", CMN_LOG_ALLOW_ALL);
    // enable system-wide thread-safe logging
#ifdef MTS_LOGGING
    mtsManagerLocal::SetLogForwarding(true);
#endif

    bool useGeneric;

#if CISST_MTS_HAS_ICE
    std::string globalComponentManagerIP;

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [GlobalManagerIP] [flag]" << std::endl;
        std::cerr << "       GlobalManagerIP is set as 127.0.0.1 by default" << std::endl;
        std::cerr << "       flag = 1 to use double instead of mtsDouble" << std::endl;
    }

    // Set global component manager's ip and argument type
    // If flag is not specified, or not 1, then use generic type (mtsDouble)
    if (argc == 1) {
        globalComponentManagerIP = "localhost";
        useGeneric = true;
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
        useGeneric = true;
    } else if (argc == 3) {
        globalComponentManagerIP = argv[1];
        useGeneric = (argv[2][0] != '1');
    } else {
        exit(-1);
    }

    std::cout << "Starting client, IP = " << globalComponentManagerIP << std::endl;
    std::cout << "Using " << (useGeneric ? "mtsDouble" : "double") << std::endl;

    // Get the TaskManager instance and set operation mode
    mtsManagerLocal * componentManager;
    try {
        componentManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "ProcessClient");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }
#else
    useGeneric = true;
    if ((argc > 1) && (argv[1][0] == '1'))
        useGeneric = false;
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();
    std::cout << "Starting client" << std::endl;
    std::cout << "Using " << (useGeneric ? "mtsDouble" : "double") << std::endl;
#endif


    // create our client task
    clientTaskBase * client1;
    clientTaskBase * client2;
    if (useGeneric) {
        client1 = new clientTask<mtsDouble>("Client1");
        client2 = new clientTask<mtsDouble>("Client2");
    } else {
        client1 = new clientTask<double>("Client1");
        client2 = new clientTask<double>("Client2");
    }

    client1->Configure();
    client2->Configure();
    componentManager->AddComponent(client1);
    componentManager->AddComponent(client2);

#if CISST_MTS_HAS_ICE
    // Connect the tasks across networks
    if (!componentManager->Connect("ProcessClient", "Client1", "Required", "ProcessServer", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed for client 1" << std::endl;
        return 1;
    }

    if (!componentManager->Connect("ProcessClient", "Client2", "Required", "ProcessServer", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed for client 2" << std::endl;
        return 1;
    }
#else
    mtsSocketProxyClient * clientProxy = new mtsSocketProxyClient("MyClientProxy", "localhost", 1234);
    componentManager->AddComponent(clientProxy);

    if (!componentManager->Connect("Client1", "Required", "MyClientProxy", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed for client 1" << std::endl;
        return 1;
    }

    if (!componentManager->Connect("Client2", "Required", "MyClientProxy", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed for client 2" << std::endl;
        return 1;
    }
#endif

    // create the tasks, i.e. find the commands
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    // start the periodic Run
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    bool GCMActive = true;
    while (client1->UIOpened() || client2->UIOpened()) {
        Fl::lock();
        {
            Fl::check();
        }
        Fl::unlock();
        Fl::awake();
        osaSleep(5.0 * cmn_ms);

        GCMActive = componentManager->IsGCMActive();
        if (!GCMActive)
            break;
    }

    if (!GCMActive) {
        CMN_LOG_RUN_ERROR << "Global Component Manager is disconnected" << std::endl;
    }

    // cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 20.0 * cmn_s);

    // delete components
    delete client1;
    delete client2;

    componentManager->Cleanup();

    return 0;
}
