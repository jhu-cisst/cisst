/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2011-07-14

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstStealthlink/mtsStealthlink.h>
#include "mtsStealthlinkExampleComponent.h"

int main(int CMN_UNUSED(argc), char * CMN_UNUSED(argv[]))
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    // set the log level of detail on select components
    cmnLogger::SetMaskClass("cmnXMLPath", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsStealthlink", CMN_LOG_ALLOW_ALL);

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

    // Connect the registration interfaces
    if (!componentManager->Connect(componentExample->GetName(), "Registration",
                                   componentStealthlink->GetName(), "Registration")) {
        CMN_LOG_INIT_ERROR << "Could not connect test component to Registration interface." << std::endl;
        return 0;
    }
 
    // create and start all components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE, 2.0 * cmn_s);

    while (1) {
        osaSleep(1.0 * cmn_s);
    }

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    componentManager->Cleanup();
    delete componentStealthlink;

    return 0;
}
