/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _displayQDevice_h
#define _displayQDevice_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>

#include <vctPlot2DQWidget.h>
#include <QMainWindow>
#include <QObject>
#include <QTimer>

#include "displayQWidget.h"


class displayQDevice: public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    displayQDevice(const std::string & taskName);
    ~displayQDevice(void);

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

 protected:
    displayQWidget CentralWidget;
    vctPlot2DQWidget PlotWidget;
    QMainWindow MainWindow;
    QTimer UpdateTimer;
    
    struct {
       mtsFunctionRead GetData;
       mtsFunctionWrite SetAmplitude;
    } Generator;

    struct {
        mtsFunctionVoid Start;
        mtsFunctionVoid Stop;
    } Collection;

    mtsDouble Data;
    mtsDouble AmplitudeData;

 public slots:
    void UpdateTimerQSlot(void);
    void SetAmplitudeQSlot(int newValue);
    void ToggleCollectionQSlot(bool checked);
};

CMN_DECLARE_SERVICES_INSTANTIATION(displayQDevice);

#endif  // _displayQDevice_h
