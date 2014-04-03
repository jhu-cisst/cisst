/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2013-11-14

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsComponentViewer.h>

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

int main(int argc, char ** argv)
{
#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT|MCL_FUTURE);
#endif

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

    std::string ipGCM;
    if (argc == 2) {
        ipGCM = argv[1]; // connect to GCM with specified ip
    } else {
        ipGCM = "localhost"; // localhost by default
    }

    std::cout << "Connecting to Global Component Manager: " << ipGCM << " ... " << std::flush;

    mtsManagerLocal * localManager = 0;;
    const std::string thisProcessName = "cisstComponentViewer";
    try {
        localManager = mtsManagerLocal::GetInstance(ipGCM, thisProcessName);
    } catch (...) {
        std::cerr << "Failed" << std::endl;
        std::cerr << "Check if Global Component Manager is running." << std::endl;
        return 1;
    }
    std::cout << "Connected" << std::endl;

    mtsComponentViewer * componentViewer = new mtsComponentViewer("ComponentViewer");

    localManager->AddComponent(componentViewer);

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
