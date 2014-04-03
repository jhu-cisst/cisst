/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2011-12-16

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>

#include <iostream>
#include <fstream>

class LoggerTask: public mtsTaskPeriodic
{
    std::ofstream FileLog;
public:
    LoggerTask() : mtsTaskPeriodic("LoggerTask", 10 * cmn_ms)
    {
        // For receiving system-wide logs from MCS
        mtsInterfaceRequired * required = AddInterfaceRequired(
            mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired,
            MTS_OPTIONAL); // MJ: in case previous version of GCM is used
        if (required) {
            required->AddEventHandlerWrite(&LoggerTask::Log, this,
                                           mtsManagerComponentBase::EventNames::PrintLog);
        }

        FileLog.open("cisstSystemLog.txt");
    }

    ~LoggerTask() {
        if (FileLog.is_open()) {
            FileLog.close();
        }
    }

    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
    }

    void Log(const mtsLogMessage & log)
    {
        std::string s(log.Message, log.Length);
        FileLog << s << std::flush;
        std::cout << s << std::flush;
    }
};

int main(int argc, char * argv[])
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL); // for cisstLog.txt
    mtsManagerLocal::SetLogForwarding(true); // enable thread-safe system-wide logging

    std::string ipGCM;
    if (argc == 2) {
        ipGCM = argv[1]; // connect to GCM with specified ip
    } else {
        ipGCM = "localhost"; // localhost by default
    }

    std::cout << "Connecting to Global Component Manager: " << ipGCM << " ... " << std::flush;

    mtsManagerLocal * localManager = 0;;
    const std::string thisProcessName = "cisstLogger";
    try {
        localManager = mtsManagerLocal::GetInstance(ipGCM, thisProcessName);
    } catch (...) {
        std::cerr << "Failed" << std::endl;
        std::cerr << "Check if Global Component Manager is running." << std::endl;
        return 1;
    }
    std::cout << "Connected" << std::endl;

    LoggerTask logger;
    localManager->AddComponent(&logger);

    // Connect to GCM for system-wide logging
    localManager->Connect(thisProcessName,
                          logger.GetName(),
                          mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired,
                          // GCM:MCS:LoggerRequired
                          mtsManagerLocal::ProcessNameOfLCMWithGCM,
                          mtsManagerComponentBase::ComponentNames::ManagerComponentServer,
                          mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerProvided);

    localManager->CreateAll();
    localManager->WaitForStateAll(mtsComponentState::READY);

    localManager->StartAll();
    localManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // loop until 'q' is pressed
    int key = ' ';
    std::cout << "Press 'q' to quit" << std::endl;
    while (key != 'q') {
        key = cmnGetChar();
    }
    std::cout << "Quitting ..." << std::endl;

    // cleanup
    localManager->KillAll();
    localManager->WaitForStateAll(mtsComponentState::FINISHED, 5.0 * cmn_s);
    localManager->Cleanup();

    return 0;
}
