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

#include <cisstMultiTask/mtsRequiredInterface.h>

#include "displayQDevice.h"

CMN_IMPLEMENT_SERVICES(displayQDevice);


displayQDevice::displayQDevice(const std::string & taskName) :
    mtsDevice(taskName)
{
    // create the cisstMultiTask interface with commands and events
    mtsRequiredInterface * requiredInterface = AddRequiredInterface("DataGenerator");
    if (requiredInterface) {
       requiredInterface->AddFunction("GetData", Generator.GetData);
       requiredInterface->AddFunction("SetAmplitude", Generator.SetAmplitude);
    }

    // create the user interface
    MainWindow.setCentralWidget(&CentralWidget);
    MainWindow.adjustSize();
    MainWindow.setWindowTitle("Periodic Task Example");
    MainWindow.show();

    UpdateTimer.start(20);

    // connect Qt signals to slots
    QObject::connect(&UpdateTimer, SIGNAL(timeout()),
                     this, SLOT(UpdateTimerQSlot()));
    QObject::connect(CentralWidget.DialAmplitude, SIGNAL(valueChanged(int)),
                     this, SLOT(SetAmplitudeQSlot(int)));
    QObject::connect(CentralWidget.ButtonQuit, SIGNAL(clicked()),
                     &MainWindow, SLOT(close()));
}


void displayQDevice::UpdateTimerQSlot(void)
{
    Generator.GetData(Data);
    CentralWidget.ValueData->setNum(Data.Data);
}


void displayQDevice::SetAmplitudeQSlot(int newValue)
{
    AmplitudeData.Data = newValue;
    Generator.SetAmplitude(AmplitudeData);
}
