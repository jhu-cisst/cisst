/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: serverMain.cpp 2241 2011-01-25 20:39:21Z mjung5

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "serverTask.h"

#include <cisstOSAbstraction/osaSleep.h>

#if !CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsSocketProxyServer.h>
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
    cmnLogger::SetMaskClassMatching("serverTask", CMN_LOG_ALLOW_ALL);
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

    std::cout << "Starting server, IP = " << globalComponentManagerIP << std::endl;
    std::cout << "Using " << (useGeneric ? "mtsDouble" : "double") << std::endl;

    // Get the TaskManager instance and set operation mode
    mtsManagerLocal * componentManager;
    try {
        componentManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "ProcessServer");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }
#else
    useGeneric = true;
    if ((argc > 1) && (argv[1][0] == '1'))
        useGeneric = false;
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();
    std::cout << "Starting server" << std::endl;
    std::cout << "Using " << (useGeneric ? "mtsDouble" : "double") << std::endl;
#endif

    // create our server task
    serverTaskBase * server;
    if (useGeneric) {
        server = new serverTask<mtsDouble>("Server");
    } else {
        server = new serverTask<double>("Server");
    }

    server->Configure();
    componentManager->AddComponent(server);

#if !CISST_MTS_HAS_ICE
    mtsSocketProxyServer * serverProxy = new mtsSocketProxyServer("MyServerProxy", "Server", "Provided", 1234);
    componentManager->AddComponent(serverProxy);

    if (!componentManager->Connect("MyServerProxy", "Required", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed for server" << std::endl;
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
    while (server->UIOpened() && GCMActive) {
        Fl::lock();
        {
            Fl::check();
        }
        Fl::unlock();
        Fl::awake();

        GCMActive = componentManager->IsGCMActive();
    }

    if (!GCMActive) {
        CMN_LOG_RUN_ERROR << "Global Component Manager is disconnected" << std::endl;
    }

    // cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 20.0 * cmn_s);

    // delete components
    delete server;

    componentManager->Cleanup();

    return 0;
}
