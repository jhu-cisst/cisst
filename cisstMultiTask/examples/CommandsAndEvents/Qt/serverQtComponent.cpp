/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "serverQtComponent.h"

#include <cisstMultiTask/mtsInterfaceProvided.h>

CMN_IMPLEMENT_SERVICES(serverQtComponent);


serverQtComponent::serverQtComponent(const std::string & componentName):
    mtsComponent(componentName),
    VoidCounter(0),
    ReadValue(0),
    QualifiedReadValue(0)
{
    // create the cisstMultiTask interface with commands and events
    mtsInterfaceProvided * provided = AddInterfaceProvided("Provided");
    if (provided) {
        provided->AddCommandVoid(&serverQtComponent::Void, this, "Void");
        provided->AddCommandWrite(&serverQtComponent::Write, this, "Write");
        provided->AddCommandRead(&serverQtComponent::Read, this, "Read");
        provided->AddCommandQualifiedRead(&serverQtComponent::QualifiedRead, this, "QualifiedRead");
        provided->AddEventVoid(EventVoid, "EventVoid");
        provided->AddEventWrite(EventWrite, "EventWrite", mtsInt());
    }

    // create the user interface
    ServerWidget.setupUi(&CentralWidget);
    MainWindow.setCentralWidget(&CentralWidget);
    MainWindow.setWindowTitle(QString::fromStdString(componentName));
    MainWindow.show();

    // connect Qt signals to slots
    QObject::connect(this, SIGNAL(VoidQSignal(int)),
                     ServerWidget.VoidValue, SLOT(setNum(int)));
    QObject::connect(this, SIGNAL(WriteQSignal(int)),
                     ServerWidget.WriteValue, SLOT(setNum(int)));
    QObject::connect(ServerWidget.ReadSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(ReadQSlot(int)));
    QObject::connect(ServerWidget.QualifiedReadSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(QualifiedReadQSlot(int)));
    QObject::connect(ServerWidget.EventVoidButton, SIGNAL(clicked()),
                     this, SLOT(EventVoidQSlot()));
    QObject::connect(ServerWidget.EventWriteSlider, SIGNAL(sliderMoved(int)),
                     this, SLOT(EventWriteQSlot(int)));
}


void serverQtComponent::Void(void)
{
    VoidCounter++;
    emit VoidQSignal(VoidCounter);
}


void serverQtComponent::Write(const mtsInt & data)
{
    emit WriteQSignal(data.Data);
}


void serverQtComponent::Read(mtsInt & placeHolder) const
{
    placeHolder.Data = ReadValue;
}


void serverQtComponent::QualifiedRead(const mtsInt & data, mtsInt & placeHolder) const
{
    placeHolder.Data = data.Data + QualifiedReadValue;
}


void serverQtComponent::ReadQSlot(int newValue)
{
    ReadValue = newValue;
}


void serverQtComponent::QualifiedReadQSlot(int newValue)
{
    QualifiedReadValue = newValue;
}


void serverQtComponent::EventVoidQSlot(void)
{
    EventVoid();
}


void serverQtComponent::EventWriteQSlot(int newValue)
{
    mtsInt payload;
    payload.Data = newValue;
    EventWrite(payload);
}
