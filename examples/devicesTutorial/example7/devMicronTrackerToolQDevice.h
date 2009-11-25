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

#ifndef _devMicronTrackerToolQDevice_h
#define _devMicronTrackerToolQDevice_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>

#include <QTimer>

#include <cisstDevices/devNDISerialToolQWidget.h>


class devMicronTrackerToolQDevice : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    devMicronTrackerToolQDevice(const std::string & taskName);
    ~devMicronTrackerToolQDevice(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    QWidget * GetToolWidget(void) {
        return &CentralWidget;
    }

 protected:
    Ui::devNDISerialToolQWidget ToolWidget;
    QWidget CentralWidget;
    QTimer UpdateTimer;

    struct {
        mtsFunctionRead GetPositionCartesian;
        prmPositionCartesianGet PositionCartesian;
    } NDI;

 public slots:
    void UpdateTimerQSlot(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(devMicronTrackerToolQDevice);

#endif  // _devMicronTrackerToolQDevice_h
