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
    layout->setSpacing(0);
    layout->setContentsMargins(2, 2, 2, 2);
    this->setLayout(layout);

    int labelWidth;
    QSize size;

    QWPosition = new QWidget();
    QHBoxLayout * positionLayout = new QHBoxLayout();
    positionLayout->setContentsMargins(2, 2, 2, 2);
    QWPosition->setLayout(positionLayout);
    QLabel * positionLabel = new QLabel("Position");
    size = positionLabel->sizeHint();
    labelWidth = size.width();
    positionLayout->addWidget(positionLabel);
    QVRPosition = new vctQtWidgetDynamicVectorDoubleRead();
    positionLayout->addWidget(QVRPosition);
    layout->addWidget(QWPosition);

    QWVelocity = new QWidget();
    QHBoxLayout * velocityLayout = new QHBoxLayout();
    velocityLayout->setContentsMargins(2, 2, 2, 2);
    QWVelocity->setLayout(velocityLayout);
    QLabel * velocityLabel = new QLabel("Velocity");
    velocityLayout->addWidget(velocityLabel);
    size = velocityLabel->sizeHint();
    if (size.width() > labelWidth) {
        labelWidth = size.width();
    }
    QVRVelocity = new vctQtWidgetDynamicVectorDoubleRead();
    velocityLayout->addWidget(QVRVelocity);
    layout->addWidget(QWVelocity);

    QWEffort = new QWidget();
    QHBoxLayout * effortLayout = new QHBoxLayout();
    effortLayout->setContentsMargins(2, 2, 2, 2);
    QWEffort->setLayout(effortLayout);
    QLabel * effortLabel = new QLabel("Effort");
    effortLayout->addWidget(effortLabel);
    size = effortLabel->sizeHint();
    if (size.width() > labelWidth) {
        labelWidth = size.width();
    }

    QVREffort = new vctQtWidgetDynamicVectorDoubleRead();
    effortLayout->addWidget(QVREffort);
    layout->addWidget(QWEffort);

    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    positionLabel->setFixedWidth(labelWidth);
    velocityLabel->setFixedWidth(labelWidth);
    effortLabel->setFixedWidth(labelWidth);
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
