/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-12-12

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmPositionCartesianGetQtWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTime>

prmPositionCartesianGetQtWidget::prmPositionCartesianGetQtWidget(void):
    QWidget()
{
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(2, 2, 2, 2);
    this->setLayout(layout);

    QHBoxLayout * topLayout = new QHBoxLayout();
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(1, 0, 0, 0);
    layout->addLayout(topLayout);

    QLReferenceFrame = new QLabel("Name");
    topLayout->addWidget(QLReferenceFrame);

    topLayout->addStretch();

    QLTime = new QLabel("Time");
    QLTime->setStyleSheet("QLabel { background-color: rgb(255, 100, 100) }");
    topLayout->addWidget(QLTime);

    QFPosition = new vctQtWidgetFrameDoubleRead(vctQtWidgetRotationDoubleRead::OPENGL_WIDGET);
    layout->addWidget(QFPosition);
}

void prmPositionCartesianGetQtWidget::SetValue(const prmPositionCartesianGet & newValue)
{
    // reference frame
    if (!newValue.MovingFrame().empty()) {
        std::string referenceFrame = newValue.MovingFrame();
        if (!newValue.ReferenceFrame().empty()) {
            referenceFrame.append("/");
            referenceFrame.append(newValue.ReferenceFrame());
        }
        QLReferenceFrame->setText(referenceFrame.c_str());
        QLReferenceFrame->show();
    } else {
        QLReferenceFrame->hide();
    }
    // position
    QFPosition->SetValue(newValue.Position());
    // timestamp & valid
    QTime time(0, 0);
    QLTime->setText(time.addSecs(static_cast<int>(newValue.Timestamp())).toString("hh:mm:ss"));
    if (newValue.Valid()) {
        QLTime->setStyleSheet("QLabel { background-color: rgb(50, 255, 50) }");
    } else {
        QLTime->setStyleSheet("QLabel { background-color: rgb(255, 100, 100) }");
    }
}
