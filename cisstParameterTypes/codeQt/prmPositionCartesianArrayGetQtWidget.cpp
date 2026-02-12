/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-07-21

  (C) Copyright 2014-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// system include
#include <iostream>

// Qt include
#include <QMessageBox>
#include <QCloseEvent>
#include <QCoreApplication>

// cisst
#include <cisstVector/vctPose3DQtWidget.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstParameterTypes/prmPositionCartesianArrayGet.h>
#include <cisstParameterTypes/prmPositionCartesianArrayGetQtWidget.h>

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(prmPositionCartesianArrayGetQtWidget, mtsComponent, std::string);

prmPositionCartesianArrayGetQtWidget::prmPositionCartesianArrayGetQtWidget(const std::string & componentName, double periodInSeconds):
    mtsComponent(componentName),
    TimerPeriodInMilliseconds(periodInSeconds)
{
    // Setup CISST Interface
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Controller");
    if (interfaceRequired) {
        interfaceRequired->AddFunction("measured_cp_array", Controller.measured_cp_array);
    }
    setupUi();
    startTimer(TimerPeriodInMilliseconds); // ms
}

void prmPositionCartesianArrayGetQtWidget::Configure(const std::string & filename)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: " << filename << std::endl;
}

void prmPositionCartesianArrayGetQtWidget::Startup(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "prmPositionCartesianArrayGetQtWidget::Startup" << std::endl;
    if (!parent()) {
        show();
    }
}

void prmPositionCartesianArrayGetQtWidget::Cleanup(void)
{
    this->hide();
    CMN_LOG_CLASS_INIT_VERBOSE << "prmPositionCartesianArrayGetQtWidget::Cleanup" << std::endl;
}

void prmPositionCartesianArrayGetQtWidget::closeEvent(QCloseEvent * event)
{
    int answer = QMessageBox::warning(this, tr("prmPositionCartesianArrayGetQtWidget"),
                                      tr("Do you really want to quit this application?"),
                                      QMessageBox::No | QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        event->accept();
        QCoreApplication::exit();
    } else {
        event->ignore();
    }
}

void prmPositionCartesianArrayGetQtWidget::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    prmPositionCartesianArrayGet poses;
    Controller.measured_cp_array(poses);
    QVPoses->Clear();
    typedef prmPositionCartesianArrayGet::Type Type;
    const Type::const_iterator end = poses.Positions().end();
    Type::const_iterator iter = poses.Positions().begin();
    for (; iter != end; ++iter) {
        QVPoses->SetValue(*iter);
    }
}

void prmPositionCartesianArrayGetQtWidget::setupUi(void)
{
    QVBoxLayout * mainLayout = new QVBoxLayout;

    QVPoses = new vctPose3DQtWidget();
    QVPoses->SetPrismaticRevoluteFactors(1.0 / cmn_mm, cmn180_PI);
    mainLayout->addWidget(QVPoses);

    this->setLayout(mainLayout);
    resize(sizeHint());
}
