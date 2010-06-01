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

#include "displayQComponent.h"

CMN_IMPLEMENT_SERVICES(displayQComponent);


displayQComponent::displayQComponent(const std::string & taskName) :
    mtsDevice(taskName)
{
    // create the cisstMultiTask interface with commands and events
    mtsRequiredInterface * requiredInterface = AddRequiredInterface("DataGenerator");
    if (requiredInterface) {
       requiredInterface->AddFunction("GetData", Generator.GetData);
       requiredInterface->AddFunction("SetAmplitude", Generator.SetAmplitude);
    }

    // connect Qt signals to slots
    QObject::connect(CentralWidget.DialAmplitude, SIGNAL(valueChanged(int)),
                     this, SLOT(SetAmplitudeQSlot(int)));

    startTimer(50); // in milliseconds 50 secs = 20 Hz 
}


void displayQComponent::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    Generator.GetData(Data);
    CentralWidget.ValueData->setNum(Data.Data);
    CentralWidget.Plot->AddPoint(CentralWidget.TraceId, vctDouble2(Data.Timestamp(), Data.Data));
    CentralWidget.Plot->updateGL();
}


void displayQComponent::SetAmplitudeQSlot(int newValue)
{
    AmplitudeData.Data = newValue;
    Generator.SetAmplitude(AmplitudeData);
}
