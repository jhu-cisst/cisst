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

#ifndef _devNDISerialToolQDevice_h
#define _devNDISerialToolQDevice_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>

#include <QTimer>

#include "ui_devNDISerialToolQWidget.h"


class devNDISerialToolQDevice : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    devNDISerialToolQDevice(const std::string & taskName);
    ~devNDISerialToolQDevice(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    QWidget * GetToolWidget(void) {
        return &CentralWidget;
    }

 public slots:
    void UpdateTimerQSlot(void);

 protected:
    Ui::ToolWidget ToolWidget;
    QWidget CentralWidget;
    QTimer UpdateTimer;

    struct {
        mtsFunctionRead GetPositionCartesian;
        prmPositionCartesianGet PositionCartesian;
    } NDI;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devNDISerialToolQDevice);

#endif  // _devNDISerialToolQDevice_h
