/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Dorothy Hu
  Created on: 2017-01-20

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


// system include
#include <iostream>

// Qt includes
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QComboBox>

#include <cisstVector/vctForceTorque2DQtWidget.h>
#include <cisstVector/vctPlot2DOpenGLQtWidget.h>

vctForceTorque2DQtWidget::vctForceTorque2DQtWidget(void):
    PlotIndex(0)
{
    setupUi();
}

void vctForceTorque2DQtWidget::closeEvent(QCloseEvent * event)
{
    int answer = QMessageBox::warning(this, tr("vctForceTorque2DQtWidget"),
                                      tr("Do you really want to quit this application?"),
                                      QMessageBox::No | QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        event->accept();
        QCoreApplication::exit();
    } else {
        event->ignore();
    }
}

void vctForceTorque2DQtWidget::setupUi(void)
{

    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // left side, upper/lower limit, selector and legend
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->addLayout(leftLayout);

    // upper limit
    QLUpperLimit = new QLabel("U");
    QLUpperLimit->setAlignment(Qt::AlignTop|Qt::AlignRight);
    leftLayout->addWidget(QLUpperLimit);

    leftLayout->addStretch();

    // combo box to select the plot item
    QComboBox * QPlotSelectItem = new QComboBox;
    QPlotSelectItem->addItem("Forces");
    QPlotSelectItem->addItem("Torques");
    QPlotSelectItem->setCurrentIndex(PlotIndex);
    leftLayout->addWidget(QPlotSelectItem);

    const double grey = 0.95;

    // legend
    QLabel * label;
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(grey * 255, grey * 255, grey * 255));
    label = new QLabel("Axis X");
    label->setAutoFillBackground(true);
    palette.setColor(QPalette::WindowText, Qt::red);
    label->setPalette(palette);
    leftLayout->addWidget(label);
    label = new QLabel("Axis Y");
    label->setAutoFillBackground(true);
    palette.setColor(QPalette::WindowText, Qt::green);
    label->setPalette(palette);
    leftLayout->addWidget(label);
    label = new QLabel("Axis Z");
    label->setAutoFillBackground(true);
    palette.setColor(QPalette::WindowText, Qt::blue);
    label->setPalette(palette);
    leftLayout->addWidget(label);
    label = new QLabel("Norm");
    label->setAutoFillBackground(true);
    palette.setColor(QPalette::WindowText, Qt::black);
    label->setPalette(palette);
    leftLayout->addWidget(label);

    leftLayout->addStretch();

    // lower limit
    QLLowerLimit = new QLabel("L");
    QLLowerLimit->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    leftLayout->addWidget(QLLowerLimit);

    // plot area
    QFTPlot = new vctPlot2DOpenGLQtWidget();
    QFTPlot->SetBackgroundColor(vct3(grey));
    QFTPlot->resize(QFTPlot->sizeHint());
    QFTPlot->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mainLayout->addWidget(QFTPlot);

    // create scales and signals
    // forces
    mForceScale = QFTPlot->AddScale("Forces");
    mForceSignal[0] = mForceScale->AddSignal("fx");
    mForceSignal[0]->SetColor(vct3(1.0, 0.0, 0.0));
    mForceSignal[1] = mForceScale->AddSignal("fy");
    mForceSignal[1]->SetColor(vct3(0.0, 1.0, 0.0));
    mForceSignal[2] = mForceScale->AddSignal("fz");
    mForceSignal[2]->SetColor(vct3(0.0, 0.0, 1.0));
    mFNormSignal = mForceScale->AddSignal("fnorm");
    mFNormSignal->SetColor(vct3(0.0, 0.0, 0.0));
    mF0Signal = mForceScale->AddSignal("0");
    mF0Signal->SetColor(vct3(0.5));

    // torques
    mTorqueScale = QFTPlot->AddScale("Torques");
    mTorqueSignal[0] = mTorqueScale->AddSignal("tx");
    mTorqueSignal[0]->SetColor(vct3(1.0, 0.0, 0.0));
    mTorqueSignal[1] = mTorqueScale->AddSignal("ty");
    mTorqueSignal[1]->SetColor(vct3(0.0, 1.0, 0.0));
    mTorqueSignal[2] = mTorqueScale->AddSignal("tz");
    mTorqueSignal[2]->SetColor(vct3(0.0, 0.0, 1.0));
    mT0Signal = mTorqueScale->AddSignal("0");
    mT0Signal->SetColor(vct3(0.5));

    this->setLayout(mainLayout);

    setWindowTitle("");
    resize(sizeHint());

    // setup Qt Connection
    connect(QPlotSelectItem, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotPlotIndex(int)));

    SlotPlotIndex(PlotIndex);
}

void vctForceTorque2DQtWidget::SetValue(const double & time, const vct3 & force, const vct3 & torque)
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    // plot
    if (PlotIndex == 0) {
        for (size_t i = 0; i < 3; ++i){
            mForceSignal[i]->AppendPoint(vctDouble2(time, force[i]));
        }
        mFNormSignal->AppendPoint(vctDouble2(time, force.Norm()));
        mF0Signal->AppendPoint(vctDouble2(time, 0.0));
    } else if (PlotIndex == 1) {
        for (size_t i = 0; i < 3; ++i){
            mTorqueSignal[i]->AppendPoint(vctDouble2(time, torque[i]));
        }
        mT0Signal->AppendPoint(vctDouble2(time, 0.0));
    }

    // update the lower/upper limits on the plot
    vct2 range;

    if (PlotIndex == 0) {
        range = mForceScale->GetViewingRangeY();
    } else if (PlotIndex == 1) {
        range = mTorqueScale->GetViewingRangeY();
    } else { // default
        range = vct2(-1.0, 1.0);
    }
    QString text;
    text.setNum(range[0], 'f', 3);
    QLLowerLimit->setText(text);
    text.setNum(range[1], 'f', 3);
    QLUpperLimit->setText(text);

    QFTPlot->update();
}


void vctForceTorque2DQtWidget::SlotPlotIndex(int newAxis)
{
    PlotIndex = newAxis;

    if (PlotIndex == 0) {
        for (size_t i = 0; i < 3; ++i){
            mForceSignal[i]->SetVisible(true);
            mTorqueSignal[i]->SetVisible(false);
        }
        mFNormSignal->SetVisible(true);
    } else if (PlotIndex == 1) {
        for (size_t i = 0; i < 3; ++i){
            mForceSignal[i]->SetVisible(false);
            mTorqueSignal[i]->SetVisible(true);
        }
        mFNormSignal->SetVisible(false);
    }
    QFTPlot->SetContinuousExpandYResetSlot();
}
