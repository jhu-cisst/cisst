/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "TestComponent.h"

#include <QApplication>
#include <QMainWindow>

#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsQtVisualizerShell.h>

int main(int argc, char** argv)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cerr, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);

    mtsManagerGlobal globalManager;

    mtsManagerLocal * manager = mtsManagerLocal::GetInstance(globalManager);
    TestComponent * testComponent = new TestComponent();
    testComponent->Configure("");
    manager->AddComponent(testComponent);

    QApplication app(argc, argv);

    QMainWindow win;
    win.setCentralWidget(new mtsQtVisualizerShell(&globalManager));
    win.show();

    manager->CreateAll();
    manager->WaitForStateAll(mtsComponentState::READY, 5.0 * cmn_s);
    manager->StartAll();
    manager->WaitForStateAll(mtsComponentState::ACTIVE, 5.0 * cmn_s);

    app.exec();

    return 0;
}
