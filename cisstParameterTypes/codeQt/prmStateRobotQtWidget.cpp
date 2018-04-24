/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-03-22

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstParameterTypes/prmStateRobotQtWidget.h>

#include <QVBoxLayout>

prmStateRobotQtWidget::prmStateRobotQtWidget(void):
    QWidget()
{
    QSJWidget = new prmStateJointQtWidget();
    QPCGWidget = new prmPositionCartesianGetQtWidget();
}

void prmStateRobotQtWidget::SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute)
{
    QSJWidget->SetPrismaticRevoluteFactors(prismatic, revolute);
    QPCGWidget->SetPrismaticRevoluteFactors(prismatic, revolute);
}

void prmStateRobotQtWidget::setupUi(void)
{
    QVBoxLayout * layout = new QVBoxLayout;
    this->setLayout(layout);
    layout->addWidget(QSJWidget);
    layout->addWidget(QPCGWidget);
    QSJWidget->setupUi();
}



CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(prmStateRobotQtWidgetComponent, mtsComponent, std::string);

prmStateRobotQtWidgetComponent::prmStateRobotQtWidgetComponent(const std::string & componentName, double periodInSeconds):
    mtsComponent(componentName),
    TimerPeriodInMilliseconds(periodInSeconds * 1000)
{
    // Setup CISST Interface
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Component");
    if (interfaceRequired) {
        interfaceRequired->AddFunction("GetStateJoint", GetStateJoint);
        interfaceRequired->AddFunction("GetPositionCartesian", GetPositionCartesian);
    }
    setupUi();
}

void prmStateRobotQtWidgetComponent::Startup(void)
{
    startTimer(TimerPeriodInMilliseconds); // ms
}

void prmStateRobotQtWidgetComponent::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }
    GetStateJoint(StateJoint);
    QSJWidget->SetValue(StateJoint);
    GetPositionCartesian(Position);
    QPCGWidget->SetValue(Position);
}
