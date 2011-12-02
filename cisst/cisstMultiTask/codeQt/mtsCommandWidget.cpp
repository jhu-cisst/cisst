/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCommandWidget.h>
#include <cisstMultiTask/mtsCommandWidgets.h>

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsQtWidgetFactory.h>
#include <cisstMultiTask/mtsQtWidgetGenericObject.h>

#include <QObject>
#include <QLabel>
#include <QGroupBox>


mtsExecuteButton::mtsExecuteButton(void)
    : QPushButton("Execute")
{
    connect(this, SIGNAL(clicked()), this, SLOT(Disable()));
}


void mtsExecuteButton::Disable(void)
{
    setEnabled(false);
}


void mtsExecuteButton::Enable(void)
{
    setEnabled(true);
}


mtsPeriodicExecutionWidget::mtsPeriodicExecutionWidget(mtsCommandWidget* parent)
    : CommandWidget(parent)
{
    QLayout * groupLayout = new QHBoxLayout();
    CheckBox = new QCheckBox("Execute periodically");
    groupLayout->addWidget(CheckBox);
    SpinBox = new QDoubleSpinBox();
    SpinBox->setMaximum(1.0 * cmn_hour);
    SpinBox->setMinimum(50.0 * cmn_ms);
    SpinBox->setValue(1.0 * cmn_s);
    SpinBox->setSingleStep(10.0 * cmn_ms);
    groupLayout->addWidget(SpinBox);
    groupLayout->addWidget(new QLabel("secs"));
    Container = new QWidget();
    Container->setLayout(groupLayout);
    Container->setEnabled(false);
    QGroupBox * groupBox = new QGroupBox();
    groupBox->setLayout(groupLayout);
    QLayout * layout = new QVBoxLayout();
    layout->addWidget(groupBox);
    setLayout(layout);
    connect(CheckBox, SIGNAL(stateChanged(int)), this, SLOT(HandleStateChanged(int)));
    connect(SpinBox, SIGNAL(valueChanged(double)), this, SLOT(HandleIntervalChanged(double)));
}


void mtsPeriodicExecutionWidget::HandleStateChanged(int enabled)
{
    Container->setEnabled(enabled);
    if(enabled) {
        CommandWidget->SetTimer(SpinBox->value() * 1000);
    } else {
        CommandWidget->StopTimer();
    }
}


void mtsPeriodicExecutionWidget::HandleIntervalChanged(double newInterval)
{
    CommandWidget->StopTimer();
    CommandWidget->SetTimer(SpinBox->value() * 1000);
}


bool mtsPeriodicExecutionWidget::ExecutePeriodically(void) const
{
    return CheckBox->isChecked();
}


double mtsPeriodicExecutionWidget::ExecutionInterval(void) const
{
    return SpinBox->value();
}


mtsEventInformationWidget::mtsEventInformationWidget(void)
    : SecondsSinceLastEvent(0), EventFired(false)
{
    QLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    TimeSinceLastEvent = new QLabel("N/A");
    layout->addWidget(TimeSinceLastEvent);
    setLayout(layout);
    TimerID = startTimer(1000);
}


void mtsEventInformationWidget::ResetTimeSinceLastEvent(void)
{
    killTimer(TimerID);
    EventFired = true;
    SecondsSinceLastEvent = 0;
    TimeOfLastEvent = QTime::currentTime();
    TimeSinceLastEvent->setText("0 seconds since last event");
    TimerID = startTimer(1000);
}


void mtsEventInformationWidget::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    if (SecondsSinceLastEvent >= 120) {
        QString s;
        TimeSinceLastEvent->setText(s.sprintf("Last event occured at %s", TimeOfLastEvent.toString("h:mm:ss ap")));
    } else {
        ++SecondsSinceLastEvent;
        if (SecondsSinceLastEvent == 1) {
            TimeSinceLastEvent->setText("1 second since last event");
        } else {
            QString s;
            TimeSinceLastEvent->setText(s.sprintf("%d seconds since last event", SecondsSinceLastEvent));
        }
    }
}


mtsCommandWidget::mtsCommandWidget(void)
    : QWidget(), HasTimer(false)
{
    Layout = new QFormLayout();
    setLayout(Layout);
    Layout->setContentsMargins(0, 0, 0, 0);
    ExecutionResultLabel = new QLabel();
    ExecutionResultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    Layout->addWidget(ExecutionResultLabel);
}


void mtsCommandWidget::SetExecutionResult(mtsExecutionResult result)
{
    if (result != LastResult) { 
        ExecutionResultLabel->setText(mtsExecutionResult::ToString(result.GetResult()).c_str());
    }
    LastResult = result;
}


void mtsCommandWidget::SetTimer(int interval)
{
    if (HasTimer) {
        StopTimer();
    }
    TimerID = startTimer(interval);
    HasTimer = true;
}


void mtsCommandWidget::StopTimer(void)
{
    killTimer(TimerID);
    HasTimer = false;
}


void mtsCommandWidget::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    Execute();
}


void mtsCommandWidget::SetWriteWidget(const std::string & label, const mtsGenericObject & prototype)
{
    mtsQtWidgetGenericObjectWrite * widget = mtsQtWidgetFactory::CreateWidgetWrite(&typeid(prototype));
    if (widget) {
        Layout->addRow(label.c_str(), widget);
        WriteWidget = widget;
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "SetWriteWidget: can't create write widget for \"" << prototype.Services()->GetName() << "\"" << std::endl;
    }
}


void mtsCommandWidget::SetReadWidget(const std::string & label, const mtsGenericObject & prototype)
{
    mtsQtWidgetGenericObjectRead * widget = mtsQtWidgetFactory::CreateWidgetRead(&typeid(prototype));
    if (widget) {
        Layout->addRow(label.c_str(), widget);
        ReadWidget = widget;
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "SetReadWidget: can't create read widget for \"" << prototype.Services()->GetName() << "\"" << std::endl;
    }
}


void mtsCommandWidget::AddExecutionWidgets(bool CMN_UNUSED(allowPeriodicExecution))
{
    Layout->addRow("Execution result", ExecutionResultLabel);
    //if(allowPeriodicExecution) {
    PeriodicExecution = new mtsPeriodicExecutionWidget(this);
    Layout->addWidget(PeriodicExecution);
    //}
    ExecuteButton = new mtsExecuteButton();
    Layout->addWidget(ExecuteButton);
    connect(ExecuteButton, SIGNAL(clicked()), this, SLOT(Execute()));
}


void mtsCommandWidget::AddEventWidgets(void)
{
    EventInformation = new mtsEventInformationWidget();
    Layout->addWidget(EventInformation);
}


mtsCommandWidget * mtsCommandWidget::CreateCommandVoidWidget(mtsFunctionVoid & command)
{
    return new CommandVoidWidget(command);
}


mtsCommandWidget * mtsCommandWidget::CreateCommandVoidReturnWidget(mtsFunctionVoidReturn & command)
{
    return new CommandVoidReturnWidget(command);
}


mtsCommandWidget * mtsCommandWidget::CreateCommandWriteWidget(mtsFunctionWrite& command)
{
    return new CommandWriteWidget(command);
}


mtsCommandWidget * mtsCommandWidget::CreateCommandWriteReturnWidget(mtsFunctionWriteReturn& command)
{
    return new CommandWriteReturnWidget(command);
}


mtsCommandWidget * mtsCommandWidget::CreateCommandReadWidget(mtsFunctionRead& command)
{
    return new CommandReadWidget(command);
}


mtsCommandWidget * mtsCommandWidget::CreateCommandQualifiedReadWidget(mtsFunctionQualifiedRead& command)
{
    return new CommandQualifiedReadWidget(command);
}


mtsCommandWidget * mtsCommandWidget::CreateEventVoidWidget(void)
{
    return new CommandEventVoidWidget();
}


mtsCommandWidget * mtsCommandWidget::CreateEventWriteWidget(void)
{
    return new CommandEventWriteWidget();
}


CommandVoidWidget::CommandVoidWidget(mtsFunctionVoid & command)
    : mtsCommandWidget(), function(command)
{
    AddExecutionWidgets();
}


void CommandVoidWidget::Execute(void)
{
    SetExecutionResult(function());
    ExecuteButton->setEnabled(true);
}


CommandVoidReturnWidget::CommandVoidReturnWidget(mtsFunctionVoidReturn& command)
    : mtsCommandWidget(), function(command)
{
    ReadValue = dynamic_cast<mtsGenericObject *>(command.GetResultPrototype()->Services()->Create());
    std::cout << CMN_LOG_DETAILS << " -- add error handling here" << std::endl;
    SetReadWidget("Result", *(command.GetResultPrototype()));
    AddExecutionWidgets();
}

void CommandVoidReturnWidget::Execute(void)
{
    SetExecutionResult(function(*ReadValue));
    ExecuteButton->setEnabled(true);
    ReadWidget->SetValue(*ReadValue);
}


CommandWriteWidget::CommandWriteWidget(mtsFunctionWrite & command)
    : mtsCommandWidget(), function(command)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(command.GetArgumentPrototype()->Services()->Create());
    std::cout << CMN_LOG_DETAILS << " -- add error handling here" << std::endl;
    SetWriteWidget("Argument", *(command.GetArgumentPrototype()));
    AddExecutionWidgets();
}

void CommandWriteWidget::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult(function(*WriteValue));
    ExecuteButton->setEnabled(true);
}


CommandWriteReturnWidget::CommandWriteReturnWidget(mtsFunctionWriteReturn& command)
    : mtsCommandWidget(), function(command)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(command.GetArgumentPrototype()->Services()->Create());
    ReadValue = dynamic_cast<mtsGenericObject *>(command.GetResultPrototype()->Services()->Create());
    SetWriteWidget("Argument", *(command.GetArgumentPrototype()));
    SetReadWidget("Result", *(command.GetResultPrototype()));
    AddExecutionWidgets();
}

void CommandWriteReturnWidget::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult(function(*WriteValue, *ReadValue));
    ExecuteButton->setEnabled(true);
    ReadWidget->SetValue(*ReadValue);
}


CommandReadWidget::CommandReadWidget(mtsFunctionRead & command)
    : mtsCommandWidget(), function(command)
{
    ReadValue = dynamic_cast<mtsGenericObject *>(command.GetArgumentPrototype()->Services()->Create());
    SetReadWidget("Result", *(command.GetArgumentPrototype()));
    AddExecutionWidgets(true);
}

void CommandReadWidget::Execute(void)
{
    SetExecutionResult(function(*ReadValue));
    ExecuteButton->setEnabled(true);
    ReadWidget->SetValue(*ReadValue);
}


CommandQualifiedReadWidget::CommandQualifiedReadWidget(mtsFunctionQualifiedRead& command)
    : mtsCommandWidget(), function(command)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(command.GetArgument1Prototype()->Services()->Create());
    ReadValue = dynamic_cast<mtsGenericObject *>(command.GetArgument2Prototype()->Services()->Create());
    SetWriteWidget("Qualifier", *(command.GetArgument1Prototype()));
    SetReadWidget("Result", *(command.GetArgument2Prototype()));
    AddExecutionWidgets(true);
}

void CommandQualifiedReadWidget::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult(function(*WriteValue, *ReadValue));
    ExecuteButton->setEnabled(true);
    ReadWidget->SetValue(*ReadValue);
}

CommandEventVoidWidget::CommandEventVoidWidget(void)
    : mtsCommandWidget()
{
    AddEventWidgets();
}

void CommandEventVoidWidget::Execute(void)
{
    EventInformation->ResetTimeSinceLastEvent();
}

CommandEventWriteWidget::CommandEventWriteWidget(void)
    : mtsCommandWidget()
{
    AddEventWidgets();
    //AddReadArgument("Value", *(command.GetArgument2Prototype()));
}

void CommandEventWriteWidget::Execute(void)
{
    EventInformation->ResetTimeSinceLastEvent();
}
