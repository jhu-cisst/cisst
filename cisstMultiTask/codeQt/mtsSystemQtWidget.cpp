/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-17

  (C) Copyright 2013-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// system include
#include <iostream>

// cisst
#include <cisstMultiTask/mtsSystemQtWidget.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// Qt
#include <QVBoxLayout>

mtsSystemQtWidget::mtsSystemQtWidget(const std::string & name)
{
    this->setObjectName(name.c_str());
    QMMessage = new mtsMessageQtWidget();
    QMIntervalStatistics = new mtsIntervalStatisticsQtWidget();
}

void mtsSystemQtWidget::SetInterfaceRequired(mtsInterfaceRequired * interfaceRequired)
{
    QMMessage->SetInterfaceRequired(interfaceRequired);
}

void mtsSystemQtWidget::setupUi(void)
{
    QVBoxLayout * layout = new QVBoxLayout;
    this->setLayout(layout);
    layout->addWidget(QMIntervalStatistics);
    QMIntervalStatistics->setupUi();
    layout->addWidget(QMMessage);
    QMMessage->setupUi();
}

void mtsSystemQtWidget::SetValue(const mtsIntervalStatistics & value) {
    QMIntervalStatistics->SetValue(value);
}

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsSystemQtWidgetComponent, mtsComponent, std::string);

mtsSystemQtWidgetComponent::mtsSystemQtWidgetComponent(const std::string & componentName, double periodInSeconds):
    mtsSystemQtWidget(componentName),
    mtsComponent(componentName),
    TimerPeriodInMilliseconds(periodInSeconds * 1000)
{
    // Setup CISST Interface
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Component");
    if (interfaceRequired) {
        interfaceRequired->AddFunction("period_statistics", period_statistics);
        mtsSystemQtWidget::SetInterfaceRequired(interfaceRequired);
    }
    setupUi();
}

void mtsSystemQtWidgetComponent::Startup(void)
{
    startTimer(TimerPeriodInMilliseconds); // ms
}

void mtsSystemQtWidgetComponent::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }
    period_statistics(IntervalStatistics);
    QMIntervalStatistics->SetValue(IntervalStatistics);
}
