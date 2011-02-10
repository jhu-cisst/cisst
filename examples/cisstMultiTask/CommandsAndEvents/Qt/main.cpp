/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <QApplication>

#include "clientQtComponent.h"
#include "serverQtComponent.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("clientQtComponent", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("serverQtComponent", CMN_LOG_ALLOW_ALL);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the components with their respective UIs
    serverQtComponent * server = new serverQtComponent("Server");
    clientQtComponent * client = new clientQtComponent("Client");

    // add the components to the component manager
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    componentManager->AddComponent(client);
    componentManager->AddComponent(server);

    // connect the components, e.g. RequiredInterface -> ProvidedInterface
    componentManager->Connect("Client", "Required",
                              "Server", "Provided");

    // create and start all components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // run Qt user interface
    application.exec();

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();
    return 0;
}
