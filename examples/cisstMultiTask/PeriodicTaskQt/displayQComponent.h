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

#ifndef _displayQComponent_h
#define _displayQComponent_h

#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>

#include <QObject>

#include "displayQWidget.h"


class displayQComponent : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    displayQComponent(const std::string & taskName);
    ~displayQComponent(void) {};

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};

    QWidget * GetWidget(void) {
        return &CentralWidget;
    }

 protected:
    displayQWidget CentralWidget;

    struct {
       mtsFunctionRead GetData;
       mtsFunctionWrite SetAmplitude;
    } Generator;

    mtsDouble Data;
    mtsDouble AmplitudeData;

 public slots:
    void timerEvent(QTimerEvent * event);
    void SetAmplitudeQSlot(int newValue);
};

CMN_DECLARE_SERVICES_INSTANTIATION(displayQComponent);

#endif  // _displayQComponent_h
