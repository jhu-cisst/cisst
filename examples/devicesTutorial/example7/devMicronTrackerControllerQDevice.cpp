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

#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <QDir>
#include <QString>

#include "devMicronTrackerControllerQDevice.h"

CMN_IMPLEMENT_SERVICES(devMicronTrackerControllerQDevice);


devMicronTrackerControllerQDevice::devMicronTrackerControllerQDevice(const std::string & taskName) :
    mtsDevice(taskName)
{
    MTC.FrameLeft.SetSize(FrameSize);
    MTC.FrameRight.SetSize(FrameSize);
    FrameIndexed8 = QImage(FrameWidth, FrameHeight, QImage::Format_Indexed8);
    ControllerWidget.setupUi(&CentralWidget);

    mtsInterfaceRequired * required = AddInterfaceRequired("Controller");
    if (required) {
        required->AddFunction("CalibratePivot", MTC.CalibratePivot);
        required->AddFunction("ToggleCapturing", MTC.Capture);
        required->AddFunction("ToggleTracking", MTC.Track);
        required->AddFunction("GetCameraFrameLeft", MTC.GetFrameLeft);
        required->AddFunction("GetCameraFrameRight", MTC.GetFrameRight);
        required->AddFunction("ComputeCameraModel", MTC.ComputeCameraModel);
    }

    required = AddInterfaceRequired("DataCollector");
    if (required) {
        required->AddFunction("StartCollection", Collector.Start);
        required->AddFunction("StopCollection", Collector.Stop);
    }

    // connect Qt signals to slots
    QObject::connect(ControllerWidget.ButtonCalibratePivot, SIGNAL(clicked()),
                     this, SLOT(MTCCalibratePivotQSlot()));
    QObject::connect(ControllerWidget.ButtonComputeCameraModel, SIGNAL(clicked()),
                     this, SLOT(MTCComputeCameraModelQSlot()));
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(MTCTrackQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonRecord, SIGNAL(toggled(bool)),
                     this, SLOT(RecordQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonScreenshot, SIGNAL(clicked()),
                     this, SLOT(ScreenshotQSlot()));

    ControllerWidget.ButtonCaptureFrameLeft->toggle();

    startTimer(20);
}


void devMicronTrackerControllerQDevice::AddToolWidget(QWidget * toolWidget, QPoint * markerLeft, QPoint * markerRight)
{
    ControllerWidget.LayoutTools->addWidget(toolWidget);
    ControllerWidget.BoxTools->addItem(toolWidget->windowTitle());

    MarkerNames.append(toolWidget->windowTitle());
    MarkersLeft.append(markerLeft);
    MarkersRight.append(markerRight);
}


void devMicronTrackerControllerQDevice::timerEvent(QTimerEvent * event)
{
    if (ControllerWidget.ButtonCaptureFrameLeft->isChecked()) {
        MTC.GetFrameLeft(MTC.FrameLeft);
        memcpy(FrameIndexed8.bits(), MTC.FrameLeft.Pointer(), FrameSize);
        PaintImage(FrameIndexed8, MarkersLeft);
        ControllerWidget.FrameLeft->setPixmap(QPixmap::fromImage(FrameRGB));
    } else {
        ControllerWidget.FrameLeft->clear();
    }

    if (ControllerWidget.ButtonCaptureFrameRight->isChecked()) {
        MTC.GetFrameRight(MTC.FrameRight);
        memcpy(FrameIndexed8.bits(), MTC.FrameRight.Pointer(), FrameSize);
        PaintImage(FrameIndexed8, MarkersRight);
        ControllerWidget.FrameRight->setPixmap(QPixmap::fromImage(FrameRGB));
    } else {
        ControllerWidget.FrameRight->clear();
    }

    CentralWidget.parentWidget()->resize(0,0);
}


void devMicronTrackerControllerQDevice::PaintImage(QImage & frameIndexed8, QList<QPoint *> & markers)
{
    FrameRGB = frameIndexed8.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    const size_t numTools = ControllerWidget.LayoutTools->count();
    for (unsigned int i = 0; i < numTools; i++) {
        MarkerPainter.begin(&FrameRGB);
        MarkerPainter.setPen(Qt::red);
        MarkerPainter.setBrush(Qt::red);  // paint inside the ellipse
        MarkerPosition = QPoint(markers[i]->x(), markers[i]->y());
        MarkerPainter.drawEllipse(MarkerPosition, 2, 2);
        MarkerPosition += QPoint(3, -3);  // label offset
        MarkerPainter.setFont(QFont(MarkerPainter.font().family(), 10, QFont::DemiBold));
        MarkerPainter.drawText(MarkerPosition, MarkerNames[i]);
        MarkerPainter.end();
    }
}


void devMicronTrackerControllerQDevice::MTCCalibratePivotQSlot(void)
{
    mtsStdString toolName = ControllerWidget.BoxTools->currentText().toStdString();
    MTC.CalibratePivot(mtsStdString(toolName));
}


void devMicronTrackerControllerQDevice::MTCComputeCameraModelQSlot(void)
{
    MTC.ComputeCameraModel(mtsStdString("MicronTrackerLeftRectification.dat"));
}


void devMicronTrackerControllerQDevice::MTCTrackQSlot(bool toggled)
{
    MTC.Capture(mtsBool(toggled));
    MTC.Track(mtsBool(toggled));
    qApp->beep();
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


void devMicronTrackerControllerQDevice::ScreenshotQSlot(void)
{
    QPixmap leftCamera = QPixmap::grabWidget(ControllerWidget.FrameLeft);
    QPixmap rightCamera = QPixmap::grabWidget(ControllerWidget.FrameRight);

    CMN_LOG_CLASS_RUN_VERBOSE << "ScreenshotQSlot: screenshot captured" << std::endl;
    qApp->beep();

    std::string dateTime;
    osaGetDateTimeString(dateTime);

    QString leftPath = QDir::currentPath() + "/LeftCamera-" + dateTime.c_str() + ".tif";
    if (!leftPath.isEmpty()) {
        leftCamera.save(leftPath, "tif");
    }
    QString rightPath = QDir::currentPath() + "/RightCamera-" + dateTime.c_str() + ".tif";
    if (!rightPath.isEmpty()) {
        rightCamera.save(rightPath, "tif");
    }
}
