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
    FrameIndexed8 = QImage(FRAME_WIDTH, FRAME_HEIGHT, QImage::Format_Indexed8);
    ControllerWidget.setupUi(&CentralWidget);

    mtsRequiredInterface * required = AddRequiredInterface("Controller");
    if (required) {
        required->AddFunction("CalibratePivot", MTC.CalibratePivot);
        required->AddFunction("ToggleCapturing", MTC.Capture);
        required->AddFunction("ToggleTracking", MTC.Track);
        required->AddFunction("GetCameraFrameLeft", MTC.GetFrameLeft);
        required->AddFunction("GetCameraFrameRight", MTC.GetFrameRight);
    }

    required = AddRequiredInterface("DataCollector");
    if (required) {
        required->AddFunction("StartCollection", Collector.Start);
        required->AddFunction("StopCollection", Collector.Stop);
    }

    // connect Qt signals to slots
    QObject::connect(ControllerWidget.ButtonCalibratePivot, SIGNAL(clicked()),
                     this, SLOT(MTCCalibratePivotQSlot()));
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(MTCTrackQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonCaptureFrameLeft, SIGNAL(toggled(bool)),
                     this, SLOT(CaptureFrameLeftQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonCaptureFrameRight, SIGNAL(toggled(bool)),
                     this, SLOT(CaptureFrameRightQSlot(bool)));

    ControllerWidget.ButtonCaptureFrameLeft->toggle();

    UpdateTimer.start(20);
}


void devMicronTrackerControllerQDevice::AddToolWidget(QWidget * toolWidget, QPoint * markerLeft, QPoint * markerRight)
{
    ControllerWidget.LayoutTools->addWidget(toolWidget);
    ControllerWidget.BoxTools->addItem(toolWidget->windowTitle());

    MarkerNames.append(toolWidget->windowTitle());
    MarkersLeft.append(markerLeft);
    MarkersRight.append(markerRight);
}


void devMicronTrackerControllerQDevice::UpdateFrameLeftQSlot(void)
{
    MTC.GetFrameLeft(MTC.FrameLeft);
    memcpy(FrameIndexed8.bits(), MTC.FrameLeft.Pointer(), FRAME_SIZE);
    FrameLeft = FrameIndexed8.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    const size_t numTools = ControllerWidget.LayoutTools->count();
    for (unsigned int i = 0; i < numTools; i++) {
        MarkerPainter.begin(&FrameLeft);
        MarkerPainter.setPen(Qt::red);
        MarkerPainter.setBrush(Qt::red);  // paint inside the ellipse
        MarkerLabel = QPoint(MarkersLeft[i]->x(), MarkersLeft[i]->y());
        MarkerPainter.drawEllipse(MarkerLabel, 2, 2);
        MarkerLabel += QPoint(3, -3);  // label offset
        MarkerPainter.setFont(QFont(MarkerPainter.font().family(), 10, QFont::DemiBold));
        MarkerPainter.drawText(MarkerLabel, MarkerNames[i]);
        MarkerPainter.end();
    }
    ControllerWidget.FrameLeft->setPixmap(QPixmap::fromImage(FrameLeft));
}


void devMicronTrackerControllerQDevice::UpdateFrameRightQSlot(void)
{
    MTC.GetFrameRight(MTC.FrameRight);
    memcpy(FrameIndexed8.bits(), MTC.FrameRight.Pointer(), FRAME_SIZE);
    FrameRight = FrameIndexed8.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    const size_t numTools = ControllerWidget.LayoutTools->count();
    for (unsigned int i = 0; i < numTools; i++) {
        MarkerPainter.begin(&FrameRight);
        MarkerPainter.setPen(Qt::red);
        MarkerPainter.setBrush(Qt::red);  // paint inside the ellipse
        MarkerLabel = QPoint(MarkersRight[i]->x(), MarkersRight[i]->y());
        MarkerPainter.drawEllipse(MarkerLabel, 2, 2);
        MarkerLabel += QPoint(3, -3);  // label offset
        MarkerPainter.setFont(QFont(MarkerPainter.font().family(), 10, QFont::DemiBold));
        MarkerPainter.drawText(MarkerLabel, MarkerNames[i]);
        MarkerPainter.end();
    }
    ControllerWidget.FrameRight->setPixmap(QPixmap::fromImage(FrameRight));
}


void devMicronTrackerControllerQDevice::MTCCalibratePivotQSlot(void)
{
    mtsStdString toolName = ControllerWidget.BoxTools->currentText().toStdString();
    MTC.CalibratePivot(toolName);
}


void devMicronTrackerControllerQDevice::MTCTrackQSlot(bool toggled)
{
    MTC.Capture(mtsBool(toggled));
    MTC.Track(mtsBool(toggled));
    qApp->beep();
}


void devMicronTrackerControllerQDevice::CaptureFrameLeftQSlot(bool toggled)
{
    if (toggled) {
        QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                         this, SLOT(UpdateFrameLeftQSlot()));
    } else {
        QObject::disconnect(&UpdateTimer, SIGNAL(timeout()),
                            this, SLOT(UpdateFrameLeftQSlot()));
        ControllerWidget.FrameLeft->clear();
        CentralWidget.parentWidget()->resize(0,0);
    }
}


void devMicronTrackerControllerQDevice::CaptureFrameRightQSlot(bool toggled)
{
    if (toggled) {
        QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                         this, SLOT(UpdateFrameRightQSlot()));
    } else {
        QObject::disconnect(&UpdateTimer, SIGNAL(timeout()),
                            this, SLOT(UpdateFrameRightQSlot()));
        ControllerWidget.FrameRight->clear();
        CentralWidget.parentWidget()->resize(0,0);
    }
}


void devMicronTrackerControllerQDevice::RecordQSlot(bool toggled)
{
    if (toggled) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RecordQSlot: starting data collection" << std::endl;
        Collector.Start();
    } else {
        CMN_LOG_CLASS_RUN_VERBOSE << "RecordQSlot: stopping data collection" << std::endl;
        Collector.Stop();
    }
}
