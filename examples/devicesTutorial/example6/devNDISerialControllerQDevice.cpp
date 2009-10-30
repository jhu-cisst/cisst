/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-10-29

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsRequiredInterface.h>

#include "devNDISerialControllerQDevice.h"

CMN_IMPLEMENT_SERVICES(devNDISerialControllerQDevice);


devNDISerialControllerQDevice::devNDISerialControllerQDevice(const std::string & taskName) :
    mtsDevice(taskName)
{
    mtsRequiredInterface * required = AddRequiredInterface("RequiresNDISerialController");
    if (required) {
        required->AddFunction("Beep", NDI.Beep);
        required->AddFunction("PortHandlesInitialize", NDI.Initialize);
        required->AddFunction("PortHandlesQuery", NDI.Query);
        required->AddFunction("PortHandlesEnable", NDI.Enable);
        required->AddFunction("ToggleTracking", NDI.Track);
    }

    ControllerWidget.setupUi(&CentralWidget);

    MainWindow.setCentralWidget(&CentralWidget);
    MainWindow.adjustSize();
    MainWindow.setWindowTitle("NDI Serial Controller");
    MainWindow.show();

    UpdateTimer.start(20);

    // connect Qt signals to slots
    QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                     this, SLOT(UpdateTimerSlot()));
    QObject::connect(ControllerWidget.ButtonBeep, SIGNAL(clicked()),
                     this, SLOT(NDIBeepSlot()));
    QObject::connect(ControllerWidget.ButtonInitialize, SIGNAL(clicked()),
                     this, SLOT(NDIInitializeSlot()));
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(NDITrackSlot(bool)));
    QObject::connect(ControllerWidget.ButtonQuit, SIGNAL(clicked()),
                     &MainWindow, SLOT(close()));
}


void devNDISerialControllerQDevice::AddToolWidget(QWidget * toolWidget, const unsigned int index)
{
    ControllerWidget.gridLayout->addWidget(toolWidget, 6 + index, 1, 1, 2);
    MainWindow.adjustSize();
}


void devNDISerialControllerQDevice::UpdateTimerSlot(void)
{
}


void devNDISerialControllerQDevice::NDIBeepSlot(void)
{
    mtsInt data = ControllerWidget.NumberOfBeeps->value();
    NDI.Beep(data);
}


void devNDISerialControllerQDevice::NDIInitializeSlot(void)
{
    NDI.Initialize();
    NDI.Query();
    NDI.Enable();
    mtsInt data = 2;
    NDI.Beep(data);
}


void devNDISerialControllerQDevice::NDITrackSlot(bool value)
{
    mtsBool data = value;
    NDI.Track(data);
}
