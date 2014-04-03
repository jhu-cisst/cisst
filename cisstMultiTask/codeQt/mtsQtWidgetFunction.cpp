/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <QFrame>
#include <QPushButton>
#include <QToolButton>
#include <QDoubleSpinBox>

#include <QDebug>


mtsQtFunctionContainerWidget::mtsQtFunctionContainerWidget():
    QFrame()
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);

    FunctionWidgetContainer = new QWidget();
    FunctionWidgetContainer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    TitleBar = new QWidget();
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);
    TitleBar->setLayout(layout);
    TitleBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    TitleBar->setMinimumWidth(250);

    ToggleButton = new QToolButton();
    ToggleButton->setArrowType(Qt::RightArrow);
    ToggleButton->setAutoRaise(true);
    connect(ToggleButton, SIGNAL(clicked()), this, SLOT(ToggleCollapsed()));
    layout->addWidget(ToggleButton);

    TitleLabel = new QLabel();
    TitleLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    layout->addWidget(TitleLabel);

    layout = new QHBoxLayout();
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);

    layout->addWidget(TitleBar);
    layout->addWidget(FunctionWidgetContainer);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void mtsQtFunctionContainerWidget::ToggleCollapsed()
{
    if(FunctionWidgetContainer->isVisible()) {
        ToggleButton->setArrowType(Qt::RightArrow);
        FunctionWidgetContainer->hide();
    } else {
        ToggleButton->setArrowType(Qt::LeftArrow);
        FunctionWidgetContainer->show();
    }
}

void mtsQtFunctionContainerWidget::SetFunctionWidget(QWidget * widget, const QString& name)
{
    this->layout()->removeWidget(FunctionWidgetContainer);
    delete FunctionWidgetContainer;

    TitleLabel->setText(name);

    FunctionWidgetContainer = new QWidget();
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    FunctionWidgetContainer->setLayout(layout);
    layout->addWidget(widget);

    this->layout()->addWidget(FunctionWidgetContainer);
    FunctionWidgetContainer->hide();
    layout->addStretch();
}


mtsQtFunctionListContainerWidget::mtsQtFunctionListContainerWidget():
    QWidget()
{
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}


void mtsQtFunctionListContainerWidget::addItem(QWidget * widget, const QString& name)
{
    mtsQtFunctionContainerWidget * containerWidget = new mtsQtFunctionContainerWidget();
    containerWidget->SetFunctionWidget(widget, name);
    layout()->addWidget(containerWidget);
}


mtsQtWidgetFunction::mtsQtWidgetFunction(void):
    QWidget(),
    HasTimer(false),
    Enabled(true)
{
    MainLayout = new QVBoxLayout();
    MainLayout->setSpacing(1);
    MainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(MainLayout);

    ControlLayout = new QHBoxLayout();
    ControlLayout->setSpacing(1);
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
    ControlLayout->addStretch();

    ResultLayout = new QFormLayout;
    ResultLayout->setSpacing(1);
    ResultLayout->setContentsMargins(0, 0, 0, 0);
    ResultLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    ResultLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    ResultLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ResultLayout->setLabelAlignment(Qt::AlignLeft);
    MainLayout->addLayout(ResultLayout);
    MainLayout->addStretch();
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
        std::string message = std::string("type \"") + prototype.Services()->GetName() + "\"";
        CMN_LOG_CLASS_INIT_WARNING << "SetWriteWidget: can't create write widget for " << message << std::endl;
        message.append(" not supported.");
        ResultLayout->addRow(label.c_str(), new QLabel(message.c_str()));
        WriteWidget = 0;
        DisableExecute();
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
        std::string message = std::string("type \"") + prototype.Services()->GetName() + "\"";
        CMN_LOG_CLASS_INIT_WARNING << "SetReadWidget: can't create read widget for " << message << std::endl;
        message.append(" not supported.");
        ResultLayout->addRow(label.c_str(), new QLabel(message.c_str()));
        ReadWidget = 0;
        DisableExecute();
    }
}


void mtsQtWidgetFunction::DisableExecute(void)
{
    Enabled = false;
    ExecuteButton->setDisabled(true);
    PeriodSpinBox->setDisabled(true);
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
    // EventInformation = new mtsEventInformationWidget();
    std::cerr << CMN_LOG_DETAILS << " fix this, events are broken now" << std::endl;
    // Layout->addWidget(EventInformation);
}


mtsQtWidgetFunctionVoid::mtsQtWidgetFunctionVoid(mtsFunctionVoid * function):
    mtsQtWidgetFunction(),
    Function(function)
{
}

void mtsQtWidgetFunctionVoid::CreateArgumentsWidgets(void)
{
}

void mtsQtWidgetFunctionVoid::Execute(void)
{
    SetExecutionResult((*Function)());
}


mtsQtWidgetFunctionVoidReturn::mtsQtWidgetFunctionVoidReturn(mtsFunctionVoidReturn * function):
    mtsQtWidgetFunction(),
    Function(function)
{
}

void mtsQtWidgetFunctionVoidReturn::CreateArgumentsWidgets(void)
{
    ReadValue = dynamic_cast<mtsGenericObject *>(Function->GetResultPrototype()->Services()->Create());
    SetReadWidget("Result:", *(Function->GetResultPrototype()));
}

void mtsQtWidgetFunctionVoidReturn::Execute(void)
{
    SetExecutionResult((*Function)(*ReadValue));
    ReadWidget->SetValue(*ReadValue);
}


mtsQtWidgetFunctionWrite::mtsQtWidgetFunctionWrite(mtsFunctionWrite * function):
    mtsQtWidgetFunction(),
    Function(function)
{
}

void mtsQtWidgetFunctionWrite::CreateArgumentsWidgets(void)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(Function->GetArgumentPrototype()->Services()->Create());
    SetWriteWidget("Argument:", *(Function->GetArgumentPrototype()));
}

void mtsQtWidgetFunctionWrite::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult((*Function)(*WriteValue));
}


mtsQtWidgetFunctionWriteReturn::mtsQtWidgetFunctionWriteReturn(mtsFunctionWriteReturn * function):
    mtsQtWidgetFunction(),
    Function(function)
{
}

void mtsQtWidgetFunctionWriteReturn::CreateArgumentsWidgets(void)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(Function->GetArgumentPrototype()->Services()->Create());
    SetWriteWidget("Argument:", *(Function->GetArgumentPrototype()));
    ReadValue = dynamic_cast<mtsGenericObject *>(Function->GetResultPrototype()->Services()->Create());
    SetReadWidget("Result:", *(Function->GetResultPrototype()));
}

void mtsQtWidgetFunctionWriteReturn::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult((*Function)(*WriteValue, *ReadValue));
    ReadWidget->SetValue(*ReadValue);
}


mtsQtWidgetFunctionRead::mtsQtWidgetFunctionRead(mtsFunctionRead * function):
    mtsQtWidgetFunction(),
    Function(function)
{
}

void mtsQtWidgetFunctionRead::CreateArgumentsWidgets(void)
{
    ReadValue = dynamic_cast<mtsGenericObject *>(Function->GetArgumentPrototype()->Services()->Create());
    SetReadWidget("Result:", *(Function->GetArgumentPrototype()));
}

void mtsQtWidgetFunctionRead::Execute(void)
{
    SetExecutionResult((*Function)(*ReadValue));
    ReadWidget->SetValue(*ReadValue);
}


mtsQtWidgetFunctionQualifiedRead::mtsQtWidgetFunctionQualifiedRead(mtsFunctionQualifiedRead * function):
    mtsQtWidgetFunction(),
    Function(function)
{
}

void mtsQtWidgetFunctionQualifiedRead::CreateArgumentsWidgets(void)
{
    WriteValue = dynamic_cast<mtsGenericObject *>(Function->GetArgument1Prototype()->Services()->Create());
    SetWriteWidget("Qualifier:", *(Function->GetArgument1Prototype()));
    ReadValue = dynamic_cast<mtsGenericObject *>(Function->GetArgument2Prototype()->Services()->Create());
    SetReadWidget("Result:", *(Function->GetArgument2Prototype()));
}

void mtsQtWidgetFunctionQualifiedRead::Execute(void)
{
    WriteWidget->GetValue(*WriteValue);
    SetExecutionResult((*Function)(*WriteValue, *ReadValue));
    ReadWidget->SetValue(*ReadValue);
}
