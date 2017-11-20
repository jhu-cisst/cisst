/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-08-24

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <QFont>
#include <QLabel>
#include <QSpacerItem>
#include <QComboBox>

#include <cisstVector/vctForceTorque2DWidget.h>
#include <cisstVector/vctQtWidgetDynamicVector.h>
#include <cisstVector/vctPlot2DOpenGLQtWidget.h>

vctForceTorque2DWidget::vctForceTorque2DWidget(void):
    InternalTime(0.0),
    PlotIndex(0)
{
    setupUi();
}

void vctForceTorque2DWidget::closeEvent(QCloseEvent * event)
{
    int answer = QMessageBox::warning(this, tr("vctForceTorque2DWidget"),
                                      tr("Do you really want to quit this application?"),
                                      QMessageBox::No | QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        event->accept();
        QCoreApplication::exit();
    } else {
        event->ignore();
    }
}

void vctForceTorque2DWidget::setupUi()
{

    QVBoxLayout * tab1Layout = new QVBoxLayout;

    // Combo box to select the plot item
    QComboBox * QPlotSelectItem = new QComboBox;
    QPlotSelectItem->addItem("Forces");
    QPlotSelectItem->addItem("Torques");
    QPlotSelectItem->setCurrentIndex(PlotIndex);

    // Upper and lower limits of the plot
    QVBoxLayout * plotLabelLayout = new QVBoxLayout;
    UpperLimit = new QLabel("U");
    UpperLimit->setAlignment(Qt::AlignTop|Qt::AlignRight);
    LowerLimit = new QLabel("L");
    LowerLimit->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    plotLabelLayout->addWidget(UpperLimit);
    plotLabelLayout->addWidget(LowerLimit);

    // Initilize ft sensor plot
    std::cout << "Set up" << std::endl;
    SetupSensorPlot();

    // Add combo box, upper/lower limits labels and plot to a single layout
    QHBoxLayout * sensorPlotLayout = new QHBoxLayout;
    sensorPlotLayout->addWidget(QPlotSelectItem);
    sensorPlotLayout->addLayout(plotLabelLayout);
    sensorPlotLayout->addWidget(QFTPlot);

    // Tab1 layout order
    tab1Layout->addLayout(sensorPlotLayout);

    this->setLayout(tab1Layout);

    setWindowTitle("sawOptoForce Sensor(N, N-mm)");
    resize(sizeHint());

    // setup Qt Connection
    connect(QPlotSelectItem, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotPlotIndex(int)));

    SlotPlotIndex(PlotIndex);
}


void vctForceTorque2DWidget::SetupSensorPlot()
{
    // Plot to show force/torque graph
    QFTPlot = new vctPlot2DOpenGLQtWidget();
    QFTPlot->SetBackgroundColor(vct3(0.0, 0, 0));
    QFTPlot->resize(QFTPlot->sizeHint());
    QFTPlot->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    ForceScale = QFTPlot->AddScale("Forces");
    TorqueScale = QFTPlot->AddScale("Torques");
    ForceSignal[0] = ForceScale->AddSignal("fx");
    ForceSignal[0]->SetColor(vctDouble3(1.0, 0.0, 0.0));
    ForceSignal[1] = ForceScale->AddSignal("fy");
    ForceSignal[1]->SetColor(vctDouble3(0.0, 1.0, 0.0));
    ForceSignal[2] = ForceScale->AddSignal("fz");
    ForceSignal[2]->SetColor(vctDouble3(0.0, 0.0, 1.0));
    TorqueSignal[0] = TorqueScale->AddSignal("tx");
    TorqueSignal[0]->SetColor(vctDouble3(1.0, 0.0, 0.0));
    TorqueSignal[1] = TorqueScale->AddSignal("ty");
    TorqueSignal[1]->SetColor(vctDouble3(0.0, 1.0, 0.0));
    TorqueSignal[2] = TorqueScale->AddSignal("tz");
    TorqueSignal[2]->SetColor(vctDouble3(0.0, 0.0, 1.0));
    FNormSignal = ForceScale->AddSignal("fnorm");
    FNormSignal->SetColor(vctDouble3(1.0, 1.0, 1.0));
}

void vctForceTorque2DWidget::SetValue(const double & time, const vct3 & force, const vct3 & torque)
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    vctDouble3 forceOnly(0.0), torqueOnly(0.0);
    forceOnly.Assign(force);
    torqueOnly.Assign(torque);

    if (PlotIndex == 0) {
        ForceSignal[0]->AppendPoint(vctDouble2(time,
                                               forceOnly.Element(0)));
        ForceSignal[1]->AppendPoint(vctDouble2(time,
                                               forceOnly.Element(1)));
        ForceSignal[2]->AppendPoint(vctDouble2(time,
                                               forceOnly.Element(2)));
        FNormSignal->AppendPoint(vctDouble2(time,
                                            forceOnly.Norm()));
    } else if (PlotIndex == 1) {
        TorqueSignal[0]->AppendPoint(vctDouble2(time,
                                                torqueOnly.Element(0)));
        TorqueSignal[1]->AppendPoint(vctDouble2(time,
                                                torqueOnly.Element(1)));
        TorqueSignal[2]->AppendPoint(vctDouble2(time,
                                                torqueOnly.Element(2)));
    }
    // Update the lower/upper limits on the plot
    vct2 range;

    if (PlotIndex == 0) {
        range = ForceScale->GetViewingRangeY();
    } else if (PlotIndex == 1) {
        range = TorqueScale->GetViewingRangeY();
    } else { // default
        range = vctDouble2(0.0, 0.0);
    }
    QString text;
    text.setNum(range[0], 'f', 2);
    LowerLimit->setText(text);
    text.setNum(range[1], 'f', 2);
    UpperLimit->setText(text);
  
    QFTPlot->update();
}

void vctForceTorque2DWidget::SlotPlotIndex(int newAxis)
{
    PlotIndex = newAxis;

    if (PlotIndex == 0) {
        for (int i = 0; i < 3; ++i){
            ForceSignal[i]->SetVisible(true);
            TorqueSignal[i]->SetVisible(false);
        }
        FNormSignal->SetVisible(true);
    } else if (PlotIndex == 1) {
        for (int i = 0; i < 3; ++i){
            ForceSignal[i]->SetVisible(false);
            TorqueSignal[i]->SetVisible(true);
        }
        FNormSignal->SetVisible(false);
    }
    QFTPlot->SetContinuousExpandYResetSlot();
}
