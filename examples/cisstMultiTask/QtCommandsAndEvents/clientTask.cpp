/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "clientTask.h"

CMN_IMPLEMENT_SERVICES(clientTask);


clientTask::clientTask(const std::string & taskName) :
    mtsDevice(taskName),
    EventCounter(0)
{
    mtsRequiredInterface * required = AddRequiredInterface("Required");
    if (required) {
        required->AddFunction("Void", Void);
        required->AddFunction("Write", Write);
        required->AddFunction("Read", Read);
        required->AddFunction("QualifiedRead", QualifiedRead);
        required->AddEventHandlerVoid(&clientTask::EventVoidHandler, this, "EventVoid", false);
        required->AddEventHandlerWrite(&clientTask::EventWriteHandler, this, "EventWrite", false);
    }

    ClientWindow.setupUi(&MainWindow);
    MainWindow.setWindowTitle("Client");
    MainWindow.setFixedSize(500, 205);
    MainWindow.move(100, 100);
    MainWindow.show();

    // trigger void command
    QObject::connect(ClientWindow.VoidButton, SIGNAL(clicked()),
                     this, SLOT(VoidQtSlot()));
    // trigger write command
    QObject::connect(ClientWindow.WriteSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(WriteQtSlot(int)));
    // trigger read command and then refresh the UI
    QObject::connect(ClientWindow.ReadButton, SIGNAL(clicked()),
                     this, SLOT(ReadQtSlot()));
    QObject::connect(this, SIGNAL(ReadQtSignal(int)),
                     ClientWindow.ReadValue, SLOT(setNum(int)));
    // trigger qualified read command and then refresh the UI
    QObject::connect(ClientWindow.QualifiedReadSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(QualifiedReadQtSlot(int)));
    QObject::connect(this, SIGNAL(QualifiedReadQtSignal(int)),
                     ClientWindow.QualifiedReadReadValue, SLOT(setNum(int)));
    // refresh event counter when events are received
    QObject::connect(this, SIGNAL(EventQtSignal(int)),
                     ClientWindow.EventValue, SLOT(setNum(int)));
}


void clientTask::EventVoidHandler(void)
{
    EventCounter = 0;
    emit EventQtSignal(EventCounter);
}


void clientTask::EventWriteHandler(const mtsInt & value)
{
    EventCounter += value.Data;
    emit EventQtSignal(EventCounter);
}


void clientTask::VoidQtSlot(void)
{
    Void();
}


void clientTask::WriteQtSlot(int newValue)
{
    mtsInt data = newValue;
    Write(data);
}


void clientTask::ReadQtSlot(void)
{
    mtsInt data;
    Read(data);
    emit ReadQtSignal(data.Data);
}


void clientTask::QualifiedReadQtSlot(int newValue)
{
    mtsInt qualifier, data;
    qualifier.Data = newValue;
    QualifiedRead(qualifier, data);
    emit QualifiedReadQtSignal(data.Data);
}
