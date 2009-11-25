/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-10-29

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsRequiredInterface.h>
#include <cisstParameterTypes/prmString.h>

#include <cisstDevices/devNDISerialControllerQDevice.h>

CMN_IMPLEMENT_SERVICES(devNDISerialControllerQDevice);


devNDISerialControllerQDevice::devNDISerialControllerQDevice(const std::string & taskName) :
    mtsDevice(taskName)
{
    mtsRequiredInterface * required = AddRequiredInterface("RequiresNDISerialController");
    if (required) {
        required->AddFunction("Beep", NDI.Beep);
        required->AddFunction("PortHandlesInitialize", NDI.Initialize);
        required->AddFunction("PortHandlesQuery", NDI.Query);
        required->AddFunction("PortHandlesEnable", NDI.Enable);
        required->AddFunction("CalibratePivot", NDI.CalibratePivot);
        required->AddFunction("ToggleTracking", NDI.Track);
    }

    ControllerWidget.setupUi(&CentralWidget);
    CentralWidget.setWindowTitle(QString::fromStdString(taskName));
    CentralWidget.adjustSize();

    // connect Qt signals to slots
    QObject::connect(ControllerWidget.ButtonBeep, SIGNAL(clicked()),
                     this, SLOT(NDIBeepQSlot()));
    QObject::connect(ControllerWidget.ButtonInitialize, SIGNAL(clicked()),
                     this, SLOT(NDIInitializeQSlot()));
    QObject::connect(ControllerWidget.ButtonCalibratePivot, SIGNAL(clicked()),
                     this, SLOT(NDICalibratePivotQSlot()));
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(NDITrackQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonCollect, SIGNAL(toggled(bool)),
                     this, SLOT(CollectQSlot(bool)));
}


void devNDISerialControllerQDevice::AddToolWidget(QWidget * toolWidget)
{
    ControllerWidget.LayoutTools->addWidget(toolWidget);
    ControllerWidget.BoxTools->addItem(toolWidget->windowTitle());
    CentralWidget.adjustSize();
}


void devNDISerialControllerQDevice::NDIBeepQSlot(void)
{
    mtsInt numberOfBeeps = ControllerWidget.NumberOfBeeps->value();
    NDI.Beep(numberOfBeeps);
}


void devNDISerialControllerQDevice::NDIInitializeQSlot(void)
{
    NDI.Initialize();
    NDI.Query();
    NDI.Enable();
    NDI.Beep(mtsInt(2));
}


void devNDISerialControllerQDevice::NDICalibratePivotQSlot(void)
{
    prmString toolName(512);
    toolName.Set(ControllerWidget.BoxTools->currentText().toStdString());
    NDI.CalibratePivot(toolName);
    NDI.Beep(mtsInt(2));
}


void devNDISerialControllerQDevice::NDITrackQSlot(bool value)
{
    NDI.Track(mtsBool(value));
}

void devNDISerialControllerQDevice::CollectQSlot(bool value)
{
    if (value) {
        CMN_LOG_CLASS_RUN_ERROR << "CollectQSlot: data collection is not yet implemented" << std::endl;
//        mtsTaskManager::GetInstance()->GetTask("devNDISerialCollectorStateTable")->SetSamplingInterval(10);
//        mtsTaskManager::GetInstance()->GetTask("devNDISerialCollectorStateTable")->StartCollection();
    } else {
//        mtsTaskManager::GetInstance()->GetTask("devNDISerialCollectorStateTable")->StopCollection();
    }
}
