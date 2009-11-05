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

#ifndef _devNDISerialControllerQDevice_h
#define _devNDISerialControllerQDevice_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstMultiTask/mtsFunctionVoid.h>

#include <QMainWindow>

#include "ui_devNDISerialControllerQWidget.h"


class devNDISerialControllerQDevice : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    devNDISerialControllerQDevice(const std::string & taskName);
    ~devNDISerialControllerQDevice(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    void AddToolWidget(QWidget * toolWidget, const unsigned int index);

 public slots:
    void NDIBeepQSlot(void);
    void NDIInitializeQSlot(void);
    void NDICalibratePivotQSlot(void);
    void NDITrackQSlot(bool value);
    void CollectQSlot(bool value);

 protected:
    void CreateMainWindow(void);

    Ui::ControllerWidget ControllerWidget;
    QWidget CentralWidget;
    QMainWindow MainWindow;

    struct {
        mtsFunctionWrite Beep;
        mtsFunctionVoid Initialize;
        mtsFunctionVoid Query;
        mtsFunctionVoid Enable;
        mtsFunctionVoid CalibratePivot;
        mtsFunctionWrite Track;
    } NDI;
};

CMN_DECLARE_SERVICES_INSTANTIATION(devNDISerialControllerQDevice);

#endif  // _devNDISerialControllerQDevice_h
