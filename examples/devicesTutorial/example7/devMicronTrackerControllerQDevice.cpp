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
    FrameTemp = QImage(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_Indexed8);
    ControllerWidget.setupUi(&CentralWidget);

    mtsRequiredInterface * required = AddRequiredInterface("RequiresMicronTrackerController");
    if (required) {
        required->AddFunction("ToggleCapturing", MTC.Capture);
        required->AddFunction("ToggleTracking", MTC.Track);
        required->AddFunction("GetCameraFrameLeft", MTC.GetFrameLeft);
        required->AddFunction("GetCameraFrameRight", MTC.GetFrameRight);
    }

    // connect Qt signals to slots
    QObject::connect(ControllerWidget.ButtonCapture, SIGNAL(toggled(bool)),
                     this, SLOT(MTCCaptureQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(MTCTrackQSlot(bool)));

    UpdateTimer.start(20);
}


void devMicronTrackerControllerQDevice::AddToolWidget(QWidget * toolWidget, QPoint * markerLeft, QPoint * markerRight)
{
    ControllerWidget.LayoutTools->addWidget(toolWidget);
    MarkersLeft.append(markerLeft);
    MarkersRight.append(markerRight);
}


void devMicronTrackerControllerQDevice::UpdateTimerQSlot(void)
{
    MTC.GetFrameLeft(MTC.FrameLeft);
    MTC.GetFrameRight(MTC.FrameRight);

    memcpy(FrameTemp.bits(), MTC.FrameLeft.Pointer(), FRAME_SIZE);
    FrameLeft = FrameTemp.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    memcpy(FrameTemp.bits(), MTC.FrameRight.Pointer(), FRAME_SIZE);
    FrameRight = FrameTemp.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    PainterTemp.begin(&FrameLeft);
    PainterTemp.setPen(Qt::red);
    PainterTemp.setFont(QFont("Courier", 13, QFont::Bold));
    PainterTemp.drawText(QPoint(5,18), "Left Camera");
    PainterTemp.end();

    PainterTemp.begin(&FrameRight);
    PainterTemp.setPen(Qt::red);
    PainterTemp.setFont(QFont("Courier", 13, QFont::Bold));
    PainterTemp.drawText(QPoint(5,18), "Right Camera");
    PainterTemp.end();

    for (int i = 0; i < ControllerWidget.LayoutTools->count(); i++) {
        PainterTemp.begin(&FrameLeft);
        PainterTemp.setPen(Qt::red);
        PainterTemp.setBrush(Qt::red);
        MarkerTemp = QPoint(MarkersLeft[i]->x(), MarkersLeft[i]->y());
        PainterTemp.drawEllipse(MarkerTemp, 2, 2);
        PainterTemp.end();

        PainterTemp.begin(&FrameRight);
        PainterTemp.setPen(Qt::red);
        PainterTemp.setBrush(Qt::red);
        MarkerTemp = QPoint(MarkersRight[i]->x(), MarkersRight[i]->y());
        PainterTemp.drawEllipse(MarkerTemp, 2, 2);
        PainterTemp.end();
    }

    ControllerWidget.FrameLeft->setPixmap(QPixmap::fromImage(FrameLeft));
    ControllerWidget.FrameRight->setPixmap(QPixmap::fromImage(FrameRight));
}


void devMicronTrackerControllerQDevice::MTCCaptureQSlot(bool value)
{
    MTC.Capture(mtsBool(value));
    if (value) {
        QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                         this, SLOT(UpdateTimerQSlot()));
    } else {
        QObject::disconnect(&UpdateTimer, SIGNAL(timeout()),
                            this, SLOT(UpdateTimerQSlot()));
        ControllerWidget.FrameLeft->setPixmap(QPixmap(0,0));
        ControllerWidget.FrameRight->setPixmap(QPixmap(0,0));
    }
}


void devMicronTrackerControllerQDevice::MTCTrackQSlot(bool value)
{
    MTC.Track(mtsBool(value));
}
