/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-07-14

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstStealthlink/mtsStealthlink.h>
#include "mtsStealthlinkExampleComponent.h"
int main(int argc, char * argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);

    // set the log level of detail on select components
    cmnLogger::SetMaskClass("cmnXMLPath", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

    std::cerr << "Usage: " << argv[0] << " [flagForDataCollection]" << std::endl;

    // create the components
    mtsStealthlink * componentStealthlink = new mtsStealthlink("Stealthlink", 50.0 * cmn_ms);
    componentStealthlink->Configure("config.xml");

    mtsStealthlinkExampleComponent * componentExample = new mtsStealthlinkExampleComponent("Example", 50 * cmn_ms);

    // add the components to the component manager
    mtsManagerLocal * componentManager = mtsComponentManager::GetInstance();
    componentManager->AddComponent(componentStealthlink);
    componentManager->AddComponent(componentExample);

    // Connect the test component to the Stealthlink component
    if (!componentManager->Connect(componentExample->GetName(), "Stealthlink",
                                   componentStealthlink->GetName(), "Controller")) {
        CMN_LOG_INIT_ERROR << "Could not connect test component to Stealthlink component." << std::endl;
        return 0;
    }

    // Now, connect to the tools (we assume these are pre-defined in the XML file)
    if (!componentManager->Connect(componentExample->GetName(), "Pointer",
                                   componentStealthlink->GetName(), "Pointer")) {
        CMN_LOG_INIT_WARNING << "Could not connect test component to Pointer tool." << std::endl;
    }
    if (!componentManager->Connect(componentExample->GetName(), "Frame",
                                   componentStealthlink->GetName(), "Frame")) {
        CMN_LOG_INIT_WARNING << "Could not connect test component to Frame tool." << std::endl;
    }

    // Connect the registration interface
    if (!componentManager->Connect(componentExample->GetName(), "Registration",
                                   componentStealthlink->GetName(), "Registration")) {
        CMN_LOG_INIT_ERROR << "Could not connect test component to Registration interface." << std::endl;
        return 0;
    }

    // Connect the exam information interface
    if (!componentManager->Connect(componentExample->GetName(), "ExamInformation",
                                   componentStealthlink->GetName(), "ExamInformation")) {
        CMN_LOG_INIT_ERROR << "Could not connect test component to ExamInformation interface." << std::endl;
        return 0;
    }

    ///// Data Collection /////
    // collect all state data in csv file
    mtsCollectorState * collector;
    int stateCollectionFlag = 0;
    if(argc == 2)
    {
        stateCollectionFlag = atoi(argv[1]);
    }

    std::cout << "Starting ... 'q' to stop" << std::endl;

    if(stateCollectionFlag)
    {
        std::cout << "Adding collector for " << componentStealthlink->GetName() << "'s state table: "
                << componentStealthlink->GetDefaultStateTableName() << std::endl;
        std::cout << "'s' to start/stop data collection" << std::endl;
        collector =
                new mtsCollectorState(componentStealthlink->GetName(),
                                      componentStealthlink->GetDefaultStateTableName(),
                                      mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
        collector->AddSignal(); // all signals
        //collector->AddSignal("FrameData");
        //collector->AddSignal("PointerPosition");
        componentManager->AddComponent(collector);
        collector->Connect();
    }

    // create and start all components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE, 2.0 * cmn_s);

    int ch;
    bool started = false;
    while (ch != 'q') {
        osaSleep(1.0 * cmn_s);

        ch = cmnGetChar();

        switch (ch) {
        case 's':
            if(started)
            {
                collector->StopCollection(0.0);
                std::cout << "Stop data collection" << std::endl;
                started = false;

            }
            else
            {
                collector->StartCollection(0.0);
                std::cout << "Start data collection" << std::endl;
                started = true;

            }
            break;

        default:
            break;
        }

    }

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    componentManager->Cleanup();
    delete componentStealthlink;

    return 0;
}
