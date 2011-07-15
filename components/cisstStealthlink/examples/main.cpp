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

#include <cisstCommon/cmnGetChar.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstStealthlink/mtsStealthlink.h>


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS | CMN_LOG_ALLOW_WARNINGS);

    // set the log level of detail on select components
    cmnLogger::SetMaskClass("mtsStealthlink", CMN_LOG_ALLOW_ALL);

    // create the components
    mtsStealthlink * componentStealthlink = new mtsStealthlink("Stealthlink", 50.0 * cmn_ms);

    // add the components to the component manager
    mtsManagerLocal * componentManager = mtsComponentManager::GetInstance();
    componentManager->AddComponent(componentStealthlink);

    // create and start all components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE, 2.0 * cmn_s);

    // quit if any key is hit
    cmnGetChar();

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    componentManager->Cleanup();
    delete componentStealthlink;

    return 0;
}
