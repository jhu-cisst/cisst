/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPath.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include <cisstMultiTask/mtsCollectorFactory.h>
#include <cisstMultiTask/mtsCollectorFactoryQtWidget.h>
#include <cisstMultiTask/mtsComponentViewerQt.h>
#include <cisstMultiTask/mtsCommandLineOptionsQt.h>

#include <QApplication>

#include "mainQtComponent.h"
#include <components/sineTask.h>
#include <components/sineTaskWithDelay.h>

int main(int argc, char *argv[]) {
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_DEBUG);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);

    // set the log level of detail on select tasks
    cmnLogger::SetMaskClass("sineTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsManagerLocal", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsManagerGlobal", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsCollectorQtComponent", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsCollectorState", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsCollectorEvent", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("mtsStateTable", CMN_LOG_ALLOW_ALL);

    // create Qt application
    QApplication application(argc, argv);

    // command line options
    mtsCommandLineOptionsQt options;
    if (!options.Parse(argc, argv, std::cerr)) {
        return -1;
    }

    // get the component manager to add multiple sine generator tasks
    mtsManagerLocal *componentManager = mtsManagerLocal::GetInstance();

    mainQtComponent *mainQt = new mainQtComponent("mainQt");
    componentManager->AddComponent(mainQt);

    sineTask *sine;
    sineTaskWithDelay *sineWithDelay;

    // create multiple sine generators along with their widget and
    // state collectors
    for (unsigned int i = 0; i < NumSineTasks; i++) {
        std::ostringstream index;
        index << i;

        // create the generator andconnect to its display widget
        sine = new sineTask("SIN" + index.str(), 5.0 * cmn_ms);
        sine->UseSeparateLogFileDefault();
        componentManager->AddComponent(sine);
        std::string displayName("DISP" + index.str());
        // add delay, just for the second graph
        if (i == 1) {
            sineWithDelay =
                new sineTaskWithDelay("SIN-DELAY" + index.str(), 5.0 * cmn_ms);
            sineWithDelay->SetLatency(2.0 * cmn_s);
            componentManager->AddComponent(sineWithDelay);
            componentManager->Connect(sineWithDelay->GetName(), "MainInterface",
                                      sine->GetName(), "MainInterface");
            componentManager->Connect(mainQt->GetName(), displayName,
                                      sineWithDelay->GetName(), "MainInterface");
        } else {
            componentManager->Connect(mainQt->GetName(), displayName, sine->GetName(),
                                      "MainInterface");
        }
    }

    options.Apply();

    // create and start all tasks
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // run Qt event loop
    application.exec();

    // kill all tasks and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();

    // stop all logs
    cmnLogger::SetMask(CMN_LOG_ALLOW_NONE);

    return 0;
}
