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

#include "clientQtComponent.h"
#include <cisstMultiTask/mtsInterfaceRequired.h>

CMN_IMPLEMENT_SERVICES(clientQtComponent);


clientQtComponent::clientQtComponent(const std::string & componentName):
    mtsComponent(componentName),
    EventCounter(0)
{
    // create the cisstMultiTask interface with commands and events
    mtsInterfaceRequired * required = AddInterfaceRequired("Required");
    if (required) {
        required->AddFunction("Void", Void);
        required->AddFunction("Write", Write);
        required->AddFunction("Read", Read);
        required->AddFunction("QualifiedRead", QualifiedRead);
        required->AddEventHandlerVoid(&clientQtComponent::EventVoidHandler, this, "EventVoid");
        required->AddEventHandlerWrite(&clientQtComponent::EventWriteHandler, this, "EventWrite");
    }

    // create the user interface
    ClientWidget.setupUi(&CentralWidget);
    MainWindow.setCentralWidget(&CentralWidget);
    MainWindow.setWindowTitle(QString::fromStdString(componentName));
    MainWindow.show();

    // trigger void command
    QObject::connect(ClientWidget.VoidButton, SIGNAL(clicked()),
                     this, SLOT(VoidQSlot()));
    // trigger write command
    QObject::connect(ClientWidget.WriteSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(WriteQSlot(int)));
    // trigger read command and then refresh the UI
    QObject::connect(ClientWidget.ReadButton, SIGNAL(clicked()),
                     this, SLOT(ReadQSlot()));
    QObject::connect(this, SIGNAL(ReadQSignal(int)),
                     ClientWidget.ReadValue, SLOT(setNum(int)));
    // trigger qualified read command and then refresh the UI
    QObject::connect(ClientWidget.QualifiedReadSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(QualifiedReadQSlot(int)));
    QObject::connect(this, SIGNAL(QualifiedReadQSignal(int)),
                     ClientWidget.QualifiedReadReadValue, SLOT(setNum(int)));
    // refresh event counter when events are received
    QObject::connect(this, SIGNAL(EventQSignal(int)),
                     ClientWidget.EventValue, SLOT(setNum(int)));
}


void clientQtComponent::EventVoidHandler(void)
{
    EventCounter = 0;
    emit EventQSignal(EventCounter);
}


void clientQtComponent::EventWriteHandler(const mtsInt & value)
{
    EventCounter += value.Data;
    emit EventQSignal(EventCounter);
}


void clientQtComponent::VoidQSlot(void)
{
    Void();
}


void clientQtComponent::WriteQSlot(int newValue)
{
    mtsInt data = newValue;
    Write(data);
}


void clientQtComponent::ReadQSlot(void)
{
    mtsInt data;
    Read(data);
    emit ReadQSignal(data.Data);
}


void clientQtComponent::QualifiedReadQSlot(int newValue)
{
    mtsInt qualifier, data;
    qualifier.Data = newValue;
    QualifiedRead(qualifier, data);
    emit QualifiedReadQSignal(data.Data);
}
