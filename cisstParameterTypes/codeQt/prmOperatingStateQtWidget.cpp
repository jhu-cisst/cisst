/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-01-15

  (C) Copyright 2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmOperatingStateQtWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>

prmOperatingStateQtWidget::prmOperatingStateQtWidget(void):
    QWidget()
{
    // to be able to signal/slot prmOperatingState
    qRegisterMetaType<prmOperatingState>();

    QVBoxLayout * layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(2, 2, 2, 2);
    this->setLayout(layout);

    QHBoxLayout * topLayout = new QHBoxLayout();
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(1, 1, 1, 1);
    layout->addLayout(topLayout);

    QLState = new QLabel("State");
    topLayout->addWidget(QLState);

    QLTime = new QLabel("Time");
    QLTime->setStyleSheet("QLabel { background-color: rgb(255, 100, 100) }");
    topLayout->addWidget(QLTime);

    QHBoxLayout * bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(0);
    bottomLayout->setContentsMargins(1, 1, 1, 1);
    layout->addLayout(bottomLayout);

    QLIsHomed = new QLabel("Homed");
    QLIsHomed->setStyleSheet("QLabel { background-color: rgb(255, 100, 100) }");
    bottomLayout->addWidget(QLIsHomed);

    QLIsBusy = new QLabel("Busy");
    bottomLayout->addWidget(QLIsBusy);
}

void prmOperatingStateQtWidget::SetValue(const prmOperatingState & newValue)
{
    // timestamp & valid
    QTime time(0, 0);
    QLTime->setText(time.addSecs(static_cast<int>(newValue.Timestamp())).toString("hh:mm:ss"));
    if (newValue.Valid()) {
        QLTime->setStyleSheet("QLabel { background-color: rgb(100, 255, 100) }");
    } else {
        QLTime->setStyleSheet("QLabel { background-color: rgb(255, 100, 100) }");
    }

    // state
    std::string state;
    try {
        state = prmOperatingState::EnumToString(newValue.State());
    } catch (...) {
        state = "VOID";
    }
    QLState->setText(QString(state.c_str()));
    if (newValue.IsHomed()) {
        QLIsHomed->setStyleSheet("QLabel { background-color: rgb(100, 255, 100) }");
    } else {
        QLIsHomed->setStyleSheet("QLabel { background-color: rgb(255, 100, 100) }");
    }
    if (newValue.IsBusy()) {
        QLIsBusy->setText("Busy");
    } else {
        QLIsBusy->setText("Available");
    }
}
