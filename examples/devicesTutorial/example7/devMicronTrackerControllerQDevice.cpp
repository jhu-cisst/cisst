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

#include "devMicronTrackerControllerQDevice.h"

CMN_IMPLEMENT_SERVICES(devMicronTrackerControllerQDevice);


devMicronTrackerControllerQDevice::devMicronTrackerControllerQDevice(const std::string & taskName) :
    mtsDevice(taskName)
{
    mtsRequiredInterface * required = AddRequiredInterface("RequiresMicronTrackerController");
    if (required) {
        required->AddFunction("ToggleTracking", MicronTracker.Track);
    }

    ControllerWidget.setupUi(&CentralWidget);
    CreateMainWindow();

    // connect Qt signals to slots
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(MicronTrackerTrackQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonQuit, SIGNAL(clicked()),
                     &MainWindow, SLOT(close()));
}


void devMicronTrackerControllerQDevice::CreateMainWindow(void)
{
    MainWindow.setCentralWidget(&CentralWidget);
    MainWindow.adjustSize();
    MainWindow.setWindowTitle("MicronTracker Controller");
    MainWindow.show();
}


void devMicronTrackerControllerQDevice::MicronTrackerTrackQSlot(bool value)
{
    MicronTracker.Track(mtsBool(value));
}
