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

#include <cisstMultiTask/mtsQtWidgetFunction.h>

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsQtWidgetFactory.h>
#include <cisstMultiTask/mtsQtWidgetGenericObject.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>


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


mtsQtWidgetFunction::mtsQtWidgetFunction(void):
    QWidget(),
    HasTimer(false),
    Enabled(true)
{
    MainLayout = new QVBoxLayout();
    MainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(MainLayout);

    ControlLayout = new QHBoxLayout();
    ControlLayout->setContentsMargins(0, 0, 0, 0);
    MainLayout->addLayout(ControlLayout);

    ControlLayout->addWidget(new QLabel("Repeat"));
    PeriodSpinBox = new QDoubleSpinBox();
    PeriodSpinBox->setSuffix("s");
    PeriodSpinBox->setSpecialValueText("never");
    PeriodSpinBox->setMinimum(0.0);
    PeriodSpinBox->setValue(0.0);
    PeriodSpinBox->setMaximum(1.0 * cmn_hour);
    PeriodSpinBox->setSingleStep(10.0 * cmn_ms);
    PeriodSpinBox->setToolTip("period between calls, set to 0 to stop");
    ControlLayout->addWidget(PeriodSpinBox);
    connect(PeriodSpinBox, SIGNAL(valueChanged(double)), this, SLOT(HandleIntervalChanged(double)));
    
    ExecuteButton = new QPushButton("Execute");
    ControlLayout->addWidget(ExecuteButton);
    connect(ExecuteButton, SIGNAL(clicked()), this, SLOT(ExecuteIfEnabled()));
    connect(ExecuteButton, SIGNAL(clicked()), this, SLOT(StopPeriodicExecution()));

    ExecutionResultLabel = new QLabel();
    ExecutionResultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    ControlLayout->addWidget(ExecutionResultLabel);

    ResultLayout = new QFormLayout;
    ResultLayout->setContentsMargins(0, 0, 0, 0);
    MainLayout->addLayout(ResultLayout);
}


void mtsQtWidgetFunction::SetExecutionResult(mtsExecutionResult result)
{
    if (result != LastResult) { 
        ExecutionResultLabel->setText(mtsExecutionResult::ToString(result.GetResult()).c_str());
    }
    LastResult = result;
}


void mtsQtWidgetFunction::SetTimer(int interval)
{
    if (HasTimer) {
        StopTimer();
    }
    TimerID = startTimer(interval);
    HasTimer = true;
}


void mtsQtWidgetFunction::ExecuteIfEnabled(void)
{
    if (this->Enabled) {
        this->Execute();
    }
}


void mtsQtWidgetFunction::StopTimer(void)
{
    killTimer(TimerID);
    HasTimer = false;
}


void mtsQtWidgetFunction::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    ExecuteIfEnabled();
}


void mtsQtWidgetFunction::SetWriteWidget(const std::string & label, const mtsGenericObject & prototype)
{
    mtsQtWidgetGenericObjectWrite * widget = mtsQtWidgetFactory::CreateWidgetWrite(&typeid(prototype));
    if (widget) {
        ResultLayout->addRow(label.c_str(), widget);
        widget->SetValue(prototype);
        WriteWidget = widget;
    } else {
        std::string message = prototype.Services()->GetName();
        CMN_LOG_CLASS_INIT_WARNING << "SetWriteWidget: can't create write widget for \"" << message << "\"" << std::endl;
        message.append(" not supported, command disabled!");
        ResultLayout->addRow(label.c_str(), new QLabel(message.c_str()));
        WriteWidget = 0;
        Enabled = false;
    }
}


void mtsQtWidgetFunction::SetReadWidget(const std::string & label, const mtsGenericObject & prototype)
{
    mtsQtWidgetGenericObjectRead * widget = mtsQtWidgetFactory::CreateWidgetRead(&typeid(prototype));
    if (widget) {
        ResultLayout->addRow(label.c_str(), widget);
        widget->SetValue(prototype);
        ReadWidget = widget;
    } else {
        std::string message = prototype.Services()->GetName();
        CMN_LOG_CLASS_INIT_WARNING << "SetReadWidget: can't create read widget for \"" << message << "\"" << std::endl;
        message.append(" not supported, command disabled!");
        ResultLayout->addRow(label.c_str(), new QLabel(message.c_str()));
        ReadWidget = 0;
        Enabled = false;
    }
}


void mtsQtWidgetFunction::HandleIntervalChanged(double newInterval)
{
    this->StopTimer();
    if (newInterval != 0.0) {
        this->SetTimer(newInterval * 1000);
    }
}


void mtsQtWidgetFunction::StopPeriodicExecution()
{
    this->StopTimer();
    this->PeriodSpinBox->setValue(0.0);
}


void mtsQtWidgetFunction::AddEventWidgets(void)
{
    EventInformation = new mtsEventInformationWidget();
    std::cerr << CMN_LOG_DETAILS << " fix this, events are broken now" << std::endl;
    // Layout->addWidget(EventInformation);
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateCommandVoidWidget(mtsFunctionVoid & command)
{
    return new mtsQtWidgetFunctionVoid(command);
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateCommandVoidReturnWidget(mtsFunctionVoidReturn & command)
{
    return new mtsQtWidgetFunctionVoidReturn(command);
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateCommandWriteWidget(mtsFunctionWrite & command)
{
    return new mtsQtWidgetFunctionWrite(command);
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateCommandWriteReturnWidget(mtsFunctionWriteReturn & command)
{
    return new mtsQtWidgetFunctionWriteReturn(command);
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateCommandReadWidget(mtsFunctionRead & command)
{
    return new mtsQtWidgetFunctionRead(command);
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateCommandQualifiedReadWidget(mtsFunctionQualifiedRead & command)
{
    return new mtsQtWidgetFunctionQualifiedRead(command);
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateEventVoidWidget(void)
{
    return new CommandEventVoidWidget();
}


mtsQtWidgetFunction * mtsQtWidgetFunction::CreateEventWriteWidget(void)
{
    return new CommandEventWriteWidget();
}



mtsQtWidgetFunctionVoid::mtsQtWidgetFunctionVoid(mtsFunctionVoid & function):
    mtsQtWidgetFunction(),
    Function(function)
{
}

void mtsQtWidgetFunctionVoid::Execute(void)
{
    SetExecutionResult(Function());
}


mtsQtWidgetFunctionVoidReturn::mtsQtWidgetFunctionVoidReturn(mtsFunctionVoidReturn & function):
    mtsQtWidgetFunction(),
    Function(function)
{
    ReadValue = dynamic_cast<mtsGenericObject *>(function.GetResultPrototype()->Services()->Create());
    SetReadWidget("Result:", *(function.GetResultPrototype()));
}

void mtsQtWidgetFunctionVoidReturn::Execute(void)
{
    SetExecutionResult(Function(*ReadValue));
    ReadWidget->SetValue(*ReadValue);
}


mtsQtWidgetFunctionWrite::mtsQtWidgetFunctionWrite(mtsFunctionWrite & function):
    mtsQtWidgetFunction(),
    Function(function)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(function.GetArgumentPrototype()->Services()->Create());
    SetWriteWidget("Argument:", *(function.GetArgumentPrototype()));
}

void mtsQtWidgetFunctionWrite::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult(Function(*WriteValue));
}


mtsQtWidgetFunctionWriteReturn::mtsQtWidgetFunctionWriteReturn(mtsFunctionWriteReturn & function):
    mtsQtWidgetFunction(),
    Function(function)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(function.GetArgumentPrototype()->Services()->Create());
    SetWriteWidget("Argument:", *(function.GetArgumentPrototype()));
    ReadValue = dynamic_cast<mtsGenericObject *>(function.GetResultPrototype()->Services()->Create());
    SetReadWidget("Result:", *(function.GetResultPrototype()));
}

void mtsQtWidgetFunctionWriteReturn::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult(Function(*WriteValue, *ReadValue));
    ReadWidget->SetValue(*ReadValue);
}


mtsQtWidgetFunctionRead::mtsQtWidgetFunctionRead(mtsFunctionRead & function):
    mtsQtWidgetFunction(),
    Function(function)
{
    ReadValue = dynamic_cast<mtsGenericObject *>(function.GetArgumentPrototype()->Services()->Create());
    SetReadWidget("Result:", *(function.GetArgumentPrototype()));
}

void mtsQtWidgetFunctionRead::Execute(void)
{
    SetExecutionResult(Function(*ReadValue));
    ReadWidget->SetValue(*ReadValue);
}


mtsQtWidgetFunctionQualifiedRead::mtsQtWidgetFunctionQualifiedRead(mtsFunctionQualifiedRead & function):
    mtsQtWidgetFunction(),
    Function(function)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(function.GetArgument1Prototype()->Services()->Create());
    SetWriteWidget("Qualifier:", *(function.GetArgument1Prototype()));
    ReadValue = dynamic_cast<mtsGenericObject *>(function.GetArgument2Prototype()->Services()->Create());
    SetReadWidget("Result:", *(function.GetArgument2Prototype()));
}

void mtsQtWidgetFunctionQualifiedRead::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult(Function(*WriteValue, *ReadValue));
    ReadWidget->SetValue(*ReadValue);
}



CommandEventVoidWidget::CommandEventVoidWidget(void)
    : mtsQtWidgetFunction()
{
    AddEventWidgets();
}

void CommandEventVoidWidget::Execute(void)
{
    EventInformation->ResetTimeSinceLastEvent();
}

CommandEventWriteWidget::CommandEventWriteWidget(void)
    : mtsQtWidgetFunction()
{
    AddEventWidgets();
    //AddReadArgument("Value", *(command.GetArgument2Prototype()));
}

void CommandEventWriteWidget::Execute(void)
{
    EventInformation->ResetTimeSinceLastEvent();
}
