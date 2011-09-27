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

#ifndef _mtsMicronTrackerControllerQDevice_h
#define _mtsMicronTrackerControllerQDevice_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsVector.h>

#include <QImage>
#include <QList>
#include <QPainter>

#include "ui_mtsMicronTrackerControllerQtWidget.h"


class mtsMicronTrackerControllerQDevice : public QObject, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    mtsMicronTrackerControllerQDevice(const std::string & taskName);
    ~mtsMicronTrackerControllerQDevice(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    void AddToolWidget(QWidget * toolWidget, QPoint * markerLeft, QPoint * markerRight);

    QWidget * GetWidget(void) {
        return &CentralWidget;
    }

 protected:
    static const unsigned int FrameWidth = 640;
    static const unsigned int FrameHeight = 480;
    static const unsigned int FrameSize = FrameWidth * FrameHeight;

    Ui::mtsMicronTrackerControllerQtWidget ControllerWidget;
    QWidget CentralWidget;

    struct {
        mtsFunctionWrite CalibratePivot;
        mtsFunctionWrite Capture;
        mtsFunctionWrite Track;
        mtsFunctionRead GetFrameLeft;
        mtsFunctionRead GetFrameRight;
        mtsFunctionWrite ComputeCameraModel;

        mtsUCharVec FrameLeft;
        mtsUCharVec FrameRight;
    } MTC;

    struct {
        mtsFunctionVoid Start;
        mtsFunctionVoid Stop;
    } Collector;

    QImage FrameIndexed8;
    QImage FrameRGB;
    QPainter MarkerPainter;
    QPoint MarkerPosition;
    QList<QString> MarkerNames;
    QList<QPoint *> MarkersLeft;
    QList<QPoint *> MarkersRight;

 public slots:
    void timerEvent(QTimerEvent * event);
    void PaintImage(QImage & frameIndexed8, QList<QPoint *> & markers);
    void MTCCalibratePivotQSlot(void);
    void MTCComputeCameraModelQSlot(void);
    void MTCTrackQSlot(bool toggled);
    void RecordQSlot(bool toggled);
    void ScreenshotQSlot(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMicronTrackerControllerQDevice);

#endif  // _mtsMicronTrackerControllerQDevice_h
