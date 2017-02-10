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
    QFont font;
    font.setBold(true);
    font.setPointSize(12);

    QVBoxLayout * tab1Layout = new QVBoxLayout;
    QLabel * instructionsLabel = new QLabel("This widget displays the forces and torques values sensed by the optoForce Sensor.\nUnits - Force(N), Torque(N-mm) \nValue in the brackets of each header displays the max F/T(-value,+value).\n\tRed is Fx or Tx\n \tGreen is Fy or Ty\n\tBlue is Fz or Tz \n\tWhite is FNorm (Torques does not display a norm).");

    // Spacers
    QSpacerItem * vSpacer = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Force/Torque display widgets and layouts
    // QVBoxLayout * spinBoxLayout = new QVBoxLayout;
    // QHBoxLayout * ftValuesLayout = new QHBoxLayout;

    // QLabel * ftLabel = new QLabel("Values");
    // ftValuesLayout->addWidget(ftLabel);

    // QFTSensorValues = new vctQtWidgetDynamicVectorDoubleRead();
    // QFTSensorValues->SetPrecision(4);

    // ftValuesLayout->addWidget(QFTSensorValues);
    // spinBoxLayout->addLayout(ftValuesLayout);

    // Combo box to select the plot item
    QComboBox * QPlotSelectItem = new QComboBox;
    QPlotSelectItem->addItem("Forces");
    QPlotSelectItem->addItem("Torques");

    // Upper and lower limits of the plot
    QVBoxLayout * plotLabelLayout = new QVBoxLayout;
    UpperLimit = new QLabel("U");
    UpperLimit->setAlignment(Qt::AlignTop|Qt::AlignRight);
    LowerLimit = new QLabel("L");
    LowerLimit->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    plotLabelLayout->addWidget(UpperLimit);
    plotLabelLayout->addWidget(LowerLimit);

    // Initilize ft sensor plot
    SetupSensorPlot();

    // Add combo box, upper/lower limits labels and plot to a single layout
    QHBoxLayout * sensorPlotLayout = new QHBoxLayout;
    sensorPlotLayout->addWidget(QPlotSelectItem);
    sensorPlotLayout->addLayout(plotLabelLayout);
    sensorPlotLayout->addWidget(QFTPlot);

    // Layout for Error Messages
    QHBoxLayout * errorLayout = new QHBoxLayout;
    QLabel *errorLabel = new QLabel("Info");

    errorLayout->addWidget(errorLabel);
    // Layout containing rebias button
    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();

    // Tab1 layout order
    tab1Layout->addWidget(instructionsLabel);
    // tab1Layout->addLayout(spinBoxLayout);
    tab1Layout->addLayout(sensorPlotLayout);
    tab1Layout->addSpacerItem(vSpacer);
    tab1Layout->addLayout(errorLayout);
    tab1Layout->addLayout(buttonLayout);
    tab1Layout->addSpacerItem(vSpacer);

    this->setLayout(tab1Layout);

    setWindowTitle("sawOptoForce Sensor(N, N-mm)");
    resize(sizeHint());

    // setup Qt Connection
    connect(QPlotSelectItem, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotPlotIndex(int)));

    QPlotSelectItem->setCurrentIndex(FNorm);
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
    ForceSignal[0]->SetVisible(false);
    ForceSignal[1] = ForceScale->AddSignal("fy");
    ForceSignal[1]->SetColor(vctDouble3(0.0, 1.0, 0.0));
    ForceSignal[1]->SetVisible(false);
    ForceSignal[2] = ForceScale->AddSignal("fz");
    ForceSignal[2]->SetColor(vctDouble3(0.0, 0.0, 1.0));
    ForceSignal[2]->SetVisible(false);
    TorqueSignal[0] = TorqueScale->AddSignal("tx");
    TorqueSignal[0]->SetColor(vctDouble3(1.0, 0.0, 0.0));
    TorqueSignal[0]->SetVisible(false);
    TorqueSignal[1] = TorqueScale->AddSignal("ty");
    TorqueSignal[1]->SetColor(vctDouble3(0.0, 1.0, 0.0));
    TorqueSignal[1]->SetVisible(false);
    TorqueSignal[2] = TorqueScale->AddSignal("tz");
    TorqueSignal[2]->SetColor(vctDouble3(0.0, 0.0, 1.0));
    TorqueSignal[2]->SetVisible(false);
    FNormSignal = ForceScale->AddSignal("fnorm");
    FNormSignal->SetColor(vctDouble3(1.0, 1.0, 1.0));
    FNormSignal->SetVisible(true);
}

void vctForceTorque2DWidget::SetValue(const double & time, const vct3 & force, const vct3 & torque)
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    // text output
    // QFTSensorValues->SetValue(vctDoubleVec(ForceSensor.ForceTorque.Force()));

    // missing all the code to plot, see old widget to do it

    vctDouble3 forceOnly(0.0), torqueOnly(0.0);
    forceOnly.Assign(force);
    torqueOnly.Assign(torque);
    
    if(PlotIndex == 0){           // If Forces
        ForceSignal[0]->AppendPoint(vctDouble2(time,
                                               forceOnly.Element(0)));
        ForceSignal[1]->AppendPoint(vctDouble2(time,
                                               forceOnly.Element(1)));
        ForceSignal[2]->AppendPoint(vctDouble2(time,
                                               forceOnly.Element(2)));
        FNormSignal->AppendPoint(vctDouble2(time,
                                            forceOnly.Norm()));
    }else if(PlotIndex == 1){           // If Torques
        TorqueSignal[0]->AppendPoint(vctDouble2(time,
                                                torqueOnly.Element(0)));
        TorqueSignal[1]->AppendPoint(vctDouble2(time,
                                                torqueOnly.Element(1)));
        TorqueSignal[2]->AppendPoint(vctDouble2(time,
                                                torqueOnly.Element(2)));
    }
    // Update the lower/upper limits on the plot
    vct2 range;

    if(PlotIndex==0){
        range = ForceScale->GetViewingRangeY();
    }else if(PlotIndex==1){
        range = TorqueScale->GetViewingRangeY();
    } else{ //default
        range= vctDouble2(0.0, 0.0);
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

    for(int i=0; i <5; ++i){
        ForceSignal[i]->SetVisible(false);
        TorqueSignal[i]->SetVisible(false);
    }
    if(PlotIndex == 0) {
        ForceSignal[0]->SetVisible(true);
        ForceSignal[1]->SetVisible(true);
        ForceSignal[2]->SetVisible(true);
        FNormSignal->SetVisible(true);
    } else if(PlotIndex == 1) {
        TorqueSignal[0]->SetVisible(true);
        TorqueSignal[1]->SetVisible(true);
        TorqueSignal[2]->SetVisible(true);
    } else{
        for(int i=0; i <5; ++i){
            ForceSignal[i]->SetVisible(false);
            TorqueSignal[i]->SetVisible(false);
        }
    }
    QFTPlot->SetContinuousExpandYResetSlot();
}
