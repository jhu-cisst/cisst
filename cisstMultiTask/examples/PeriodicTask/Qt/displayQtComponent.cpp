/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "displayQtComponent.h"

displayQtComponent::displayQtComponent(void)
{
    // connect Qt signals to slots
    QObject::connect(CentralWidget.DialAmplitude, SIGNAL(valueChanged(int)),
                     this, SLOT(SetAmplitudeQSlot(int)));

    startTimer(100); // in milliseconds 100 = 10 Hz 
}


void displayQtComponent::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    if (CentralWidget.isHidden()) {
        return;
    }
    Generator.GetData(Data);
    CentralWidget.ValueData->setNum(Data.Data);
    CentralWidget.DataSignal->AppendPoint(vctDouble2(Data.Timestamp(), Data.Data));
    CentralWidget.Plot->update();
}


void displayQtComponent::SetAmplitudeQSlot(int newValue)
{
    AmplitudeData.Data = newValue;
    Generator.SetAmplitude(AmplitudeData);
}
