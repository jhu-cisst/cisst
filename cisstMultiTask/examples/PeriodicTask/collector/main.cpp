/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009-2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsCollectorEvent.h>
#include <cisstMultiTask/mtsCollectorState.h>

#include <components/sineTask.h>

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_DEBUG);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);

    // set the log level of detail on select tasks
    cmnLogger::SetMaskClass("sineTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsManagerLocal", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsManagerGlobal", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsCollectorState", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsStateTable", CMN_LOG_ALLOW_ALL);

    // get the component manager to add multiple sine generator tasks
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();

    // create the generator and its widget
    sineTask * sine = new sineTask("SIN", 5.0 * cmn_ms);
    sine->UseSeparateLogFileDefault();
    componentManager->AddComponent(sine);

    // create the state collector and connect it to the generator
    mtsCollectorState * stateCollector = new mtsCollectorState(sine->GetName(),
                                                               sine->GetDefaultStateTableName(),
                                                               mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    stateCollector->AddSignal("SineData");
    componentManager->AddComponent(stateCollector);
    stateCollector->UseSeparateLogFileDefault();
    stateCollector->Connect();

    // create an event collector
    mtsCollectorEvent * eventCollector =
        new mtsCollectorEvent("EventCollector",
                              mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    componentManager->AddComponent(eventCollector);
    eventCollector->UseSeparateLogFile("event-collector-log.txt");
    eventCollector->AddObservedComponent(sine);
    eventCollector->Connect();

    // create and start all tasks
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // start both collectors
    stateCollector->StartCollection(0.0);
    eventCollector->StartCollection(0.0);

    // run for 10 seconds
    osaSleep(10.0 * cmn_s);

    // stop the collection, this is important.  If not stop properly,
    // you will miss the last batch of data collection.
    stateCollector->StopCollection(0.0);
    eventCollector->StopCollection(0.0);

    // leave some time for the data collection to be finalized
    osaSleep(2.0 * cmn_s);

    // kill all tasks and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();

    // stop all logs
    cmnLogger::SetMask(CMN_LOG_ALLOW_NONE);

    return 0;
}
