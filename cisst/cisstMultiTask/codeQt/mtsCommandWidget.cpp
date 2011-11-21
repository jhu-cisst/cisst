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

#include <QObject>
#include <QLabel>
#include <QGroupBox>


mtsExecutionResultWidget::mtsExecutionResultWidget(void)
    : QWidget()
{
    QLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    Label = new QLabel();
    Label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    layout->addWidget(Label);
    setLayout(layout);
    setFocusPolicy(Qt::StrongFocus);
}


void mtsExecutionResultWidget::SetValue(mtsExecutionResult result)
{
    Label->setText(mtsExecutionResult::ToString(result.GetResult()).c_str());
}


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
    QLayout* grouplayout = new QVBoxLayout();
    CheckBox = new QCheckBox("Execute periodically");
    grouplayout->addWidget(CheckBox);
    Container = new QWidget();
    QLayout* containerLayout = new QHBoxLayout();
    containerLayout->addWidget(new QLabel("Execute every"));
    SpinBox = new QSpinBox();
    SpinBox->setMaximum(1000000);
    SpinBox->setMinimum(1);
    SpinBox->setValue(1000);
    containerLayout->addWidget(SpinBox);
    containerLayout->addWidget(new QLabel("milliseconds"));
    Container->setLayout(containerLayout);
    Container->setEnabled(false);
    grouplayout->addWidget(Container);
    QGroupBox* groupBox = new QGroupBox();
    groupBox->setLayout(grouplayout);
    QLayout* layout = new QVBoxLayout();
    layout->addWidget(groupBox);
    setLayout(layout);
    connect(CheckBox, SIGNAL(stateChanged(int)), this, SLOT(HandleStateChanged(int)));
    connect(SpinBox, SIGNAL(valueChanged(int)), this, SLOT(HandleIntervalChanged(int)));
}


void mtsPeriodicExecutionWidget::HandleStateChanged(int enabled)
{
    Container->setEnabled(enabled);
    if(enabled) {
        CommandWidget->SetTimer(SpinBox->value());
    } else {
        CommandWidget->StopTimer();
    }
}


void mtsPeriodicExecutionWidget::HandleIntervalChanged(int newInterval)
{
    CommandWidget->StopTimer();
    CommandWidget->SetTimer(SpinBox->value());
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


bool mtsCommandWidget::Constructed = false;
std::map<const std::type_info*, ArgumentWidget* (*)(), TypeInfoComparator> mtsCommandWidget::InputWidgets;
std::map<const std::type_info*, ArgumentWidget* (*)(), TypeInfoComparator> mtsCommandWidget::OutputWidgets;


mtsCommandWidget::mtsCommandWidget(void)
    : QWidget(), HasTimer(false)
{
    Layout = new QFormLayout();
    setLayout(Layout);

    if(!Constructed) {
        CreateDefaultWidgetBindings();
        Constructed = true;
    }
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


void mtsCommandWidget::CreateDefaultWidgetBindings(void)
{
    InputWidgets[&typeid(mtsInt)] = &ArgumentWidget::CreateIntInputWidget;
    InputWidgets[&typeid(mtsBool)] = &ArgumentWidget::CreateBoolInputWidget;
    InputWidgets[&typeid(mtsDouble)] = &ArgumentWidget::CreateDoubleInputWidget;
    InputWidgets[&typeid(mtsStdString)] = &ArgumentWidget::CreateStdStringInputWidget;

    OutputWidgets[&typeid(mtsInt)] = &ArgumentWidget::CreateIntOutputWidget;
    OutputWidgets[&typeid(mtsBool)] = &ArgumentWidget::CreateBoolOutputWidget;
    OutputWidgets[&typeid(mtsDouble)] = &ArgumentWidget::CreateDoubleOutputWidget;
    OutputWidgets[&typeid(mtsStdString)] = &ArgumentWidget::CreateStdStringOutputWidget;
}


void mtsCommandWidget::AddWriteArgument(const char * label, const mtsGenericObject & prototype)
{
    TypeInfoMap::const_iterator iterator = InputWidgets.find(&typeid(prototype));
    if (iterator != InputWidgets.end()) {
        ArgumentWidget * widget = (iterator->second)(); // object factory for widget
        Layout->addRow(label, widget);
        Arguments.push_back(widget);
    }
}


void mtsCommandWidget::AddReadArgument(const char * label, const mtsGenericObject& prototype)
{
    TypeInfoMap::const_iterator iterator = OutputWidgets.find(&typeid(prototype));
    if (iterator != OutputWidgets.end()) {
        ArgumentWidget * widget = (iterator->second)();
        Layout->addRow(label, widget);
        Arguments.push_back(widget);
    }
}


void mtsCommandWidget::AddExecutionWidgets(bool CMN_UNUSED(allowPeriodicExecution))
{
    ExecutionResult = new mtsExecutionResultWidget();
    Layout->addRow("Execution result", ExecutionResult);
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
    ExecutionResult->SetValue(function());
    ExecuteButton->setEnabled(true);
}


CommandVoidReturnWidget::CommandVoidReturnWidget(mtsFunctionVoidReturn& command)
    : mtsCommandWidget(), function(command)
{
    AddReadArgument("Result", *(command.GetResultPrototype()));
    AddExecutionWidgets();
}


void CommandVoidReturnWidget::Execute(void)
{
    ExecutionResult->SetValue(function(Arguments[0]->GetValueRef()));
    ExecuteButton->setEnabled(true);
    Arguments[0]->SetValue(Arguments[0]->GetValueRef());
}


CommandWriteWidget::CommandWriteWidget(mtsFunctionWrite & command)
    : mtsCommandWidget(), function(command)
{
    AddWriteArgument("Argument", *(command.GetArgumentPrototype()));
    AddExecutionWidgets();
}

void CommandWriteWidget::Execute(void)
{
    ExecutionResult->SetValue(function(Arguments[0]->GetValue()));
    ExecuteButton->setEnabled(true);
}

CommandWriteReturnWidget::CommandWriteReturnWidget(mtsFunctionWriteReturn& command)
    : mtsCommandWidget(), function(command)
{
    AddWriteArgument("Argument", *(command.GetArgumentPrototype()));
    AddReadArgument("Result", *(command.GetResultPrototype()));
    AddExecutionWidgets();
}

void CommandWriteReturnWidget::Execute(void)
{
    ExecutionResult->SetValue(function(Arguments[0]->GetValue(), Arguments[1]->GetValueRef()));
    ExecuteButton->setEnabled(true);
    Arguments[1]->SetValue(Arguments[1]->GetValueRef());
}

CommandReadWidget::CommandReadWidget(mtsFunctionRead& command)
    : mtsCommandWidget(), function(command)
{
    AddReadArgument("Result", *(command.GetArgumentPrototype()));
    AddExecutionWidgets(true);
}

void CommandReadWidget::Execute(void)
{
    ExecutionResult->SetValue(function(Arguments[0]->GetValueRef()));
    ExecuteButton->setEnabled(true);
    Arguments[0]->SetValue(Arguments[0]->GetValueRef());
}

CommandQualifiedReadWidget::CommandQualifiedReadWidget(mtsFunctionQualifiedRead& command)
    : mtsCommandWidget(), function(command)
{
    AddWriteArgument("Qualifier", *(command.GetArgument1Prototype()));
    AddReadArgument("Result", *(command.GetArgument2Prototype()));
    AddExecutionWidgets(true);
}

void CommandQualifiedReadWidget::Execute(void)
{
    ExecutionResult->SetValue(function(Arguments[0]->GetValue(), Arguments[1]->GetValueRef()));
    ExecuteButton->setEnabled(true);
    Arguments[1]->SetValue(Arguments[1]->GetValueRef());
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
