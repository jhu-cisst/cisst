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

#include "devNDISerialToolQDevice.h"

CMN_IMPLEMENT_SERVICES(devNDISerialToolQDevice);


devNDISerialToolQDevice::devNDISerialToolQDevice(const std::string & taskName) :
    mtsDevice(taskName)
{
    mtsRequiredInterface * required = AddRequiredInterface(taskName);
    if (required) {
       required->AddFunction("GetPositionCartesian", NDI.GetPositionCartesian);
    }

    ToolWidget.setupUi(&CentralWidget);
    MainWindow.setCentralWidget(&CentralWidget);
    MainWindow.setWindowTitle(QString::fromStdString(taskName));
    MainWindow.show();

    UpdateTimer.start(20);

    // connect Qt signals to slots
    QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                     this, SLOT(UpdateTimerSlot()));
}


void devNDISerialToolQDevice::UpdateTimerSlot(void)
{
    NDI.GetPositionCartesian(NDI.PositionCartesian);
    ToolWidget.PositionX->setNum(NDI.PositionCartesian.Position().Translation().X());
    ToolWidget.PositionY->setNum(NDI.PositionCartesian.Position().Translation().Y());
    ToolWidget.PositionZ->setNum(NDI.PositionCartesian.Position().Translation().Z());
}
