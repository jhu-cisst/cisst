/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Dorothy Hu
  Created on: 2017-01-20

  (C) Copyright 2017-2021 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <QCheckBox>
#include <QSignalMapper>

#include <cisstVector/vctForceTorque2DQtWidget.h>
#include <cisstVector/vctPlot2DOpenGLQtWidget.h>

vctForceTorque2DQtWidget::vctForceTorque2DQtWidget(void):
    mScaleIndex(0)
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

    // left side, selector and legend
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->addLayout(leftLayout);

    // combo box to select the plot item
    QComboBox * QPlotSelectItem = new QComboBox;
    QPlotSelectItem->addItem("Forces");
    QPlotSelectItem->addItem("Torques");
    QPlotSelectItem->setCurrentIndex(mScaleIndex);
    leftLayout->addWidget(QPlotSelectItem);

    // constants
    const QColor textColor = palette().color(QPalette::Text);
    const QColor baseColor = palette().color(QPalette::Base);
    const vct3 _colors[5] = {vct3(1.0, 0.0, 0.0),
                             vct3(0.0, 1.0, 0.0),
                             vct3(0.0, 0.0, 1.0),
                             vct3(textColor.redF(), textColor.greenF(), textColor.blueF()),
                             vct3(0.5)};
    const std::string _signals[5] = {"Axis X", "Axis Y", "Axis Z", "Norm", "Zero"};
    const std::string _scales[2] = {"F", "T"};

    // mapping
    mSignalMapper = new QSignalMapper();

    // legends
    QLabel * label;
    QPalette palette;

    for (size_t signal = 0;
         signal < 5;
         ++signal) {
        QHBoxLayout * signalLayout = new QHBoxLayout();
        signalLayout->setContentsMargins(0, 0, 0, 0);
        leftLayout->addLayout(signalLayout);
        // label
        label = new QLabel(_signals[signal].c_str());
        label->setAutoFillBackground(true);
        palette.setColor(QPalette::WindowText, QColor(_colors[signal].X() * 255,
                                                      _colors[signal].Y() * 255,
                                                      _colors[signal].Z() * 255));
        label->setPalette(palette);
        signalLayout->addWidget(label);
        // checkbox
        mCheckBoxes[signal] = new QCheckBox("");
        mCheckBoxes[signal]->setChecked(true);
        signalLayout->addWidget(mCheckBoxes[signal]);

        mSignalMapper->setMapping(mCheckBoxes[signal], signal);
        connect(mCheckBoxes[signal], SIGNAL(released()), mSignalMapper, SLOT(map()));
    }

    connect(mSignalMapper, SIGNAL(mapped(int)),
            this, SLOT(SlotVisibleSignal(int)));

    leftLayout->addStretch();

    // plot area
    QFTPlot = new vctPlot2DOpenGLQtWidget();
    QFTPlot->SetBackgroundColor(vct3(baseColor.redF(), baseColor.greenF(), baseColor.blueF()));
    QFTPlot->resize(QFTPlot->sizeHint());
    QFTPlot->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mainLayout->addWidget(QFTPlot);

    // create scales and signals
    for (size_t scale = 0;
         scale < 2;
         ++scale) {
        mScales[scale] = QFTPlot->AddScale(_scales[scale]);
        for (size_t signal = 0;
             signal < 5;
             ++signal) {
            mSignals[scale][signal] = mScales[scale]->AddSignal(_signals[signal]);
            mSignals[scale][signal]->SetColor(_colors[signal]);
            mVisibleSignals[scale][signal] = true;
        }
    }

    this->setLayout(mainLayout);

    setWindowTitle("");
    resize(sizeHint());

    // setup Qt Connection
    connect(QPlotSelectItem, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotScaleIndex(int)));

    SlotScaleIndex(mScaleIndex);
}

void vctForceTorque2DQtWidget::SetValue(const double & time, const vct3 & force, const vct3 & torque)
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    // plot
    vct3 toPlot;
    if (mScaleIndex == 0) {
        toPlot = force;
    } else {
        toPlot = torque;
    }

    for (size_t i = 0; i < 3; ++i){
        mSignals[mScaleIndex][i]->AppendPoint(vctDouble2(time, toPlot[i]));
    }
    mSignals[mScaleIndex][3]->AppendPoint(vctDouble2(time, toPlot.Norm()));
    mSignals[mScaleIndex][4]->AppendPoint(vctDouble2(time, 0.0));

    QFTPlot->SetDisplayYRangeScale(mScales[mScaleIndex]);

    QFTPlot->update();
}


void vctForceTorque2DQtWidget::SlotScaleIndex(int newAxis)
{
    mScaleIndex = newAxis;

    for (size_t i = 0; i < 5; ++i) {
        // hide other scale
        mSignals[(mScaleIndex + 1) % 2][i]->SetVisible(false);
        // show current scales and user preferences
        mSignals[mScaleIndex][i]->SetVisible(mVisibleSignals[mScaleIndex][i]);
        mCheckBoxes[i]->setChecked(mVisibleSignals[mScaleIndex][i]);
    }
    mScales[mScaleIndex]->AutoFitXY();
    QFTPlot->SetContinuousExpandYResetSlot();
}


void vctForceTorque2DQtWidget::SlotVisibleSignal(int index)
{
    const bool checked = mCheckBoxes[index]->isChecked();
    mSignals[mScaleIndex][index]->SetVisible(checked);
    mVisibleSignals[mScaleIndex][index] = checked;
}
