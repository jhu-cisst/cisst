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

#include <cisstMultiTask/mtsRequiredInterface.h>

#include <QString>

#include "devMicronTrackerToolQDevice.h"

CMN_IMPLEMENT_SERVICES(devMicronTrackerToolQDevice);


devMicronTrackerToolQDevice::devMicronTrackerToolQDevice(const std::string & taskName) :
    mtsDevice(taskName)
{
    MTC.MarkerProjectionLeft.SetSize(2);
    MTC.MarkerProjectionRight.SetSize(2);

    ToolWidget.setupUi(&CentralWidget);
    ToolWidget.ToolGroup->setTitle(QString::fromStdString(taskName));
    CentralWidget.setWindowTitle(QString::fromStdString(taskName));

    mtsRequiredInterface * required = AddRequiredInterface(taskName);
    if (required) {
       required->AddFunction("GetPositionCartesian", MTC.GetPositionCartesian);
       required->AddFunction("GetMarkerProjectionLeft", MTC.GetMarkerProjectionLeft);
       required->AddFunction("GetMarkerProjectionRight", MTC.GetMarkerProjectionRight);
    }

    // connect Qt signals to slots
    QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                     this, SLOT(UpdateTimerQSlot()));

    UpdateTimer.start(20);
}


void devMicronTrackerToolQDevice::UpdateTimerQSlot(void)
{
    MTC.GetPositionCartesian(MTC.PositionCartesian);
    if (MTC.PositionCartesian.Valid()) {
        ToolWidget.PositionX->setNum(MTC.PositionCartesian.Position().Translation().X());
        ToolWidget.PositionY->setNum(MTC.PositionCartesian.Position().Translation().Y());
        ToolWidget.PositionZ->setNum(MTC.PositionCartesian.Position().Translation().Z());
    } else {
        ToolWidget.PositionX->setNum(0.0);
        ToolWidget.PositionY->setNum(0.0);
        ToolWidget.PositionZ->setNum(0.0);
    }

    MTC.GetMarkerProjectionLeft(MTC.MarkerProjectionLeft);
    MTC.GetMarkerProjectionRight(MTC.MarkerProjectionRight);
    MarkerLeft.setX(MTC.MarkerProjectionLeft.X());
    MarkerLeft.setY(MTC.MarkerProjectionLeft.Y());
    MarkerRight.setX(MTC.MarkerProjectionRight.X());
    MarkerRight.setY(MTC.MarkerProjectionRight.Y());
}
