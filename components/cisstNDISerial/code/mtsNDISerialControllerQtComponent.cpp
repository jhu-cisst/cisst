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

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstNDISerial/mtsNDISerialControllerQtComponent.h>

CMN_IMPLEMENT_SERVICES(mtsNDISerialControllerQtComponent);


mtsNDISerialControllerQtComponent::mtsNDISerialControllerQtComponent(const std::string & taskName) :
    mtsComponent(taskName)
{
    ControllerWidget.setupUi(&CentralWidget);
    CentralWidget.setWindowTitle(QString::fromStdString(taskName));

    mtsInterfaceRequired * required = AddInterfaceRequired("Controller");
    if (required) {
        required->AddFunction("Beep", NDI.Beep);
        required->AddFunction("PortHandlesInitialize", NDI.Initialize);
        required->AddFunction("PortHandlesQuery", NDI.Query);
        required->AddFunction("PortHandlesEnable", NDI.Enable);
        required->AddFunction("CalibratePivot", NDI.CalibratePivot);
        required->AddFunction("ToggleTracking", NDI.Track);
    }

    required = AddInterfaceRequired("DataCollector");
    if (required) {
        required->AddFunction("StartCollection", Collector.Start);
        required->AddFunction("StopCollection", Collector.Stop);
    }

    // connect Qt signals to slots
    QObject::connect(ControllerWidget.ButtonBeep, SIGNAL(clicked()),
                     this, SLOT(NDIBeepQSlot()));
    QObject::connect(ControllerWidget.ButtonInitialize, SIGNAL(clicked()),
                     this, SLOT(NDIInitializeQSlot()));
    QObject::connect(ControllerWidget.ButtonCalibratePivot, SIGNAL(clicked()),
                     this, SLOT(NDICalibratePivotQSlot()));
    QObject::connect(ControllerWidget.ButtonTrack, SIGNAL(toggled(bool)),
                     this, SLOT(NDITrackQSlot(bool)));
    QObject::connect(ControllerWidget.ButtonRecord, SIGNAL(toggled(bool)),
                     this, SLOT(RecordQSlot(bool)));
}


void mtsNDISerialControllerQtComponent::AddToolWidget(QWidget * toolWidget)
{
    ControllerWidget.LayoutTools->addWidget(toolWidget);
    ControllerWidget.BoxTools->addItem(toolWidget->windowTitle());
}


void mtsNDISerialControllerQtComponent::NDIBeepQSlot(void)
{
    mtsInt numberOfBeeps = ControllerWidget.NumberOfBeeps->value();
    NDI.Beep(numberOfBeeps);
}


void mtsNDISerialControllerQtComponent::NDIInitializeQSlot(void)
{
    NDI.Initialize();
    NDI.Query();
    NDI.Enable();
    qApp->beep();
}


void mtsNDISerialControllerQtComponent::NDICalibratePivotQSlot(void)
{
    mtsStdString toolName = ControllerWidget.BoxTools->currentText().toStdString();
    NDI.CalibratePivot(toolName);
}


void mtsNDISerialControllerQtComponent::NDITrackQSlot(bool toggled)
{
    NDI.Track(mtsBool(toggled));
    qApp->beep();
}


void mtsNDISerialControllerQtComponent::RecordQSlot(bool toggled)
{
    if (toggled) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RecordQSlot: starting data collection" << std::endl;
        Collector.Start();
    } else {
        CMN_LOG_CLASS_RUN_VERBOSE << "RecordQSlot: stopping data collection" << std::endl;
        Collector.Stop();
    }
}
