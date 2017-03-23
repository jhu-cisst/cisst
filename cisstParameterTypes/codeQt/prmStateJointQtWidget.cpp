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

#include <cisstParameterTypes/prmStateJointQtWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

prmStateJointQtWidget::prmStateJointQtWidget(void):
    QWidget()
{
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setSpacing(1);
    this->setLayout(layout);

    QWPosition = new QWidget();
    QHBoxLayout * positionLayout = new QHBoxLayout();
    QWPosition->setLayout(positionLayout);
    positionLayout->addWidget(new QLabel("Position"));
    QVRPosition = new vctQtWidgetDynamicVectorDoubleRead();
    positionLayout->addWidget(QVRPosition);
    layout->addWidget(QWPosition);

    QWVelocity = new QWidget();
    QHBoxLayout * velocityLayout = new QHBoxLayout();
    QWVelocity->setLayout(velocityLayout);
    velocityLayout->addWidget(new QLabel("Velocity"));
    QVRVelocity = new vctQtWidgetDynamicVectorDoubleRead();
    velocityLayout->addWidget(QVRVelocity);
    layout->addWidget(QWVelocity);

    QWEffort = new QWidget();
    QHBoxLayout * effortLayout = new QHBoxLayout();
    QWEffort->setLayout(effortLayout);
    effortLayout->addWidget(new QLabel("Effort"));
    QVREffort = new vctQtWidgetDynamicVectorDoubleRead();
    effortLayout->addWidget(QVREffort);
    layout->addWidget(QWEffort);

    layout->addStretch();
}

void prmStateJointQtWidget::SetValue(const prmStateJoint & newValue)
{
    if (newValue.Position().size() != 0) {
        QWPosition->show();
        QVRPosition->SetValue(newValue.Position());
    } else {
        QWPosition->hide();
    }
    if (newValue.Velocity().size() != 0) {
        QWVelocity->show();
        QVRVelocity->SetValue(newValue.Velocity());
    } else {
        QWVelocity->hide();
    }
    if (newValue.Effort().size() != 0) {
        QWEffort->show();
        QVREffort->SetValue(newValue.Effort());
    } else {
        QWEffort->hide();
    }
}
