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
    MTC.FrameLeft.SetSize(FRAME_SIZE);
    MTC.FrameRight.SetSize(FRAME_SIZE);
    MTC.MarkerProjectionLeft.SetSize(2);

    mtsRequiredInterface * required = AddRequiredInterface("RequiresMicronTrackerController");
    if (required) {
        required->AddFunction("ToggleCapturing", MTC.Capture);
        required->AddFunction("ToggleTracking", MTC.Track);
        required->AddFunction("GetCameraFrameLeft", MTC.GetFrameLeft);
        required->AddFunction("GetCameraFrameRight", MTC.GetFrameRight);
        required->AddFunction("GetPositionCartesian", MTC.GetPositionCartesian);
        required->AddFunction("GetMarkerProjectionLeft", MTC.GetMarkerProjectionLeft);
    }

    FrameLeft = QImage(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_Indexed8);
    FrameRight = QImage(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_Indexed8);
    FrameLeft.setNumColors(256);
    FrameRight.setNumColors(256);
    for (unsigned int i = 0; i < 256; i++) {
        FrameLeft.setColor(i, qRgb(i, i, i));
        FrameRight.setColor(i, qRgb(i, i, i));
    }

    ControllerWidget.setupUi(&CentralWidget);

    UpdateTimer.start(20);

    // connect Qt signals to slots
    QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                     this, SLOT(UpdateTimerQSlot()));
    QObject::connect(ControllerWidget.ButtonCapture, SIGNAL(toggled(bool)),
                     this, SLOT(MTCCaptureQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(MTCTrackQSlot(bool)));
}


void devMicronTrackerControllerQDevice::UpdateTimerQSlot(void)
{
    // draw left and right camera frames
    MTC.GetFrameLeft(MTC.FrameLeft);
    //MTC.GetFrameRight(MTC.FrameRight);

    memcpy(FrameLeft.bits(), MTC.FrameLeft.Pointer(), FRAME_SIZE);
    //memcpy(FrameRight.bits(), MTC.FrameRight.Pointer(), FRAME_SIZE);

    // draw marker
    MTC.GetMarkerProjectionLeft(MTC.MarkerProjectionLeft);

    double x = MTC.MarkerProjectionLeft.X();
    double y = MTC.MarkerProjectionLeft.Y();
    int box = 2;

    for (int i = -box; i <= box; i++) {
        for (int j = -box; j <= box; j++) {
            if (x+i >= 0 && x+i <= FRAME_WIDTH &&
                y+j >= 0 && y+j <= FRAME_HEIGHT) {
                FrameLeft.setPixel(x+i, y+j, 255);
            }
        }
    }

    ControllerWidget.FrameLeft->setPixmap(QPixmap::fromImage(FrameLeft));
    //ControllerWidget.FrameRight->setPixmap(QPixmap::fromImage(FrameRight));
}


void devMicronTrackerControllerQDevice::MTCCaptureQSlot(bool value)
{
    MTC.Capture(mtsBool(value));
}


void devMicronTrackerControllerQDevice::MTCTrackQSlot(bool value)
{
    MTC.Track(mtsBool(value));
}
