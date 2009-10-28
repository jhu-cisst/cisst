/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-10-27

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsRequiredInterface.h>

#include "proxyQt.h"

CMN_IMPLEMENT_SERVICES(proxyQt);


proxyQt::proxyQt(const std::string & taskName) :
    mtsDevice(taskName)
{
    mtsRequiredInterface * required;
    required = AddRequiredInterface("RequiresNDISerial");
    if (required) {
        required->AddFunction("Beep", NDIBeep);
        required->AddFunction("PortHandlesInitialize", NDIInitialize);
        required->AddFunction("PortHandlesQuery", NDIQuery);
        required->AddFunction("PortHandlesEnable", NDIEnable);
        required->AddFunction("ToggleTracking", NDITrack);
    }
    required = AddRequiredInterface("02-34802401");
    if (required) {
       required->AddFunction("GetPositionCartesian", GetNDIPosition);
    }

    UI.setupUi(&MainWindow);
    MainWindow.setWindowTitle("devNDISerial");
    MainWindow.show();

    UpdateTimer.start(20);

    // connect Qt signals to slots
    QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                     this, SLOT(UpdateTimerSlot()));
    QObject::connect(UI.ButtonBeep, SIGNAL(clicked()),
                     this, SLOT(NDIBeepSlot()));
    QObject::connect(UI.ButtonInitialize, SIGNAL(clicked()),
                     this, SLOT(NDIInitializeSlot()));
    QObject::connect(UI.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(NDITrackSlot(bool)));
}


void proxyQt::UpdateTimerSlot(void)
{
    GetNDIPosition(NDIPosition);
    UI.PositionX->setNum(NDIPosition.Position().Translation().X());
    UI.PositionY->setNum(NDIPosition.Position().Translation().Y());
    UI.PositionZ->setNum(NDIPosition.Position().Translation().Z());
}


void proxyQt::NDIBeepSlot(void)
{
    mtsInt data = 1;
    NDIBeep(data);
}


void proxyQt::NDIInitializeSlot(void)
{
    NDIInitialize();
    NDIQuery();
    NDIEnable();
}


void proxyQt::NDITrackSlot(bool value)
{
    mtsBool data = value;
    NDITrack(data);
}
