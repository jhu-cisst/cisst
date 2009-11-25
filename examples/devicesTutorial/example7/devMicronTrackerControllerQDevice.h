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

#ifndef _devMicronTrackerControllerQDevice_h
#define _devMicronTrackerControllerQDevice_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstMultiTask/mtsVector.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>

#include <QImage>
#include <QTimer>

#include "ui_devMicronTrackerControllerQWidget.h"


class devMicronTrackerControllerQDevice : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    devMicronTrackerControllerQDevice(const std::string & taskName);
    ~devMicronTrackerControllerQDevice(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    QWidget * GetCentralWidget(void) {
        return &CentralWidget;
    }

 protected:
    enum { FRAME_WIDTH = 640 };
    enum { FRAME_HEIGHT = 480 };
    enum { FRAME_SIZE = FRAME_WIDTH * FRAME_HEIGHT };

    Ui::ControllerWidget ControllerWidget;
    QWidget CentralWidget;
    QTimer UpdateTimer;

    struct {
        mtsFunctionWrite Capture;
        mtsFunctionWrite Track;
        mtsFunctionRead GetFrameLeft;
        mtsFunctionRead GetFrameRight;
        mtsFunctionRead GetPositionCartesian;
        mtsFunctionRead GetMarkerProjectionLeft;
        mtsUCharVec FrameLeft;
        mtsUCharVec FrameRight;
        prmPositionCartesianGet PositionCartesian;
        mtsDoubleVec MarkerProjectionLeft;
    } MTC;

    QImage FrameLeft;
    QImage FrameRight;

 public slots:
    void UpdateTimerQSlot(void);
    void MTCCaptureQSlot(bool value);
    void MTCTrackQSlot(bool value);
};

CMN_DECLARE_SERVICES_INSTANTIATION(devMicronTrackerControllerQDevice);

#endif  // _devMicronTrackerControllerQDevice_h
