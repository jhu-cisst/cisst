/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "serverTask.h"
#include "clientTask.h"

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example9Local");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("clientTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("serverTask", CMN_LOG_LOD_VERY_VERBOSE);

    // create our two components
    const double PeriodClient = 10 * cmn_ms; // in milliseconds
    const double PeriodServer = 10 * cmn_ms; // in milliseconds
    serverTask * server = new serverTask("Server", PeriodServer);
    clientTask * client = new clientTask("Client", PeriodClient);


    // add the tasks to the component manager
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    componentManager->AddComponent(client);
    componentManager->AddComponent(server);

    // connect the components, task.RequiresInterface -> task.ProvidesInterface
    componentManager->Connect("Client", "Required", "Server", "Provided");

    // create the components, i.e. find the commands
    componentManager->CreateAll();
    // start the periodic Run
    componentManager->StartAll();

    // wait until the close button of the UI is pressed
    while (server->UIOpened() || client->UIOpened()) {
        osaSleep(10.0 * cmn_ms); // sleep to save CPU
    }
    // cleanup
    componentManager->KillAll();
    componentManager->Cleanup();
    return 0;
}

/*
  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
