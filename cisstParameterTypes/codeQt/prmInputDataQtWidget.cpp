/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-01-03

  (C) Copyright 2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmInputDataQtWidget.h>

#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

prmInputDataQtWidget::prmInputDataQtWidget(void):
    QWidget()
{
    QVBoxLayout * dataLayout = new QVBoxLayout;
    this->setLayout(dataLayout);

    // text display
    QHBoxLayout * digitalLayout = new QHBoxLayout;
    QLabel * digitalLabel = new QLabel("Digital Signals");
    digitalLayout->addWidget(digitalLabel);
    QVRDigitalInputsWidget = new vctQtWidgetDynamicVectorBoolRead();
    digitalLayout->addWidget(QVRDigitalInputsWidget);
    dataLayout->addLayout(digitalLayout);

    QHBoxLayout * analogLayout = new QHBoxLayout;
    QLabel * analogLabel = new QLabel("Analog Signals");
    analogLayout->addWidget(analogLabel);
    QVRAnalogInputsWidget = new vctQtWidgetDynamicVectorDoubleRead();
    analogLayout->addWidget(QVRAnalogInputsWidget);
    dataLayout->addLayout(analogLayout);

    // plot
    QVPlot = new vctPlot2DOpenGLQtWidget();
    vctPlot2DBase::Scale * scaleSignal = QVPlot->AddScale("signal");
    AnalogSignal = scaleSignal->AddSignal("analog");
    AnalogSignal->SetColor(vctDouble3(1.0, 0.0, 0.0));
    QVPlot->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    dataLayout->addWidget(QVPlot);

    QSBPlotIndex = new QSpinBox();
    QSBPlotIndex->setRange(0, -1);
    QSBPlotIndex->setEnabled(false);
    dataLayout->addWidget(QSBPlotIndex);

    connect(QSBPlotIndex, SIGNAL(valueChanged(int)), this, SLOT(SlotPlotIndex(int)));
}

void prmInputDataQtWidget::SetValue(const prmInputData & newValue)
{
    // update text display
    QVRAnalogInputsWidget->SetValue(newValue.AnalogInputs());
    QVRDigitalInputsWidget->SetValue(newValue.DigitalInputs());

    // check if analog signals are provided
    const int nbAnalogInputs = newValue.AnalogInputs().size();
    if (nbAnalogInputs > 0) {
        // check for new size
        if (nbAnalogInputs != (QSBPlotIndex->maximum() + 1)) {
            QSBPlotIndex->setMaximum(nbAnalogInputs - 1);
            QSBPlotIndex->setEnabled(true);
            PlotIndex = 0;
        }
        AnalogSignal->AppendPoint(vctDouble2(newValue.Timestamp(), newValue.AnalogInputs()[PlotIndex]));
        QVPlot->update();
    } else {
        QSBPlotIndex->setMaximum(-1);
        QSBPlotIndex->setEnabled(false);
    }
}

void prmInputDataQtWidget::SlotPlotIndex(int newAxis)
{
    PlotIndex = newAxis;
    QVPlot->SetContinuousExpandYResetSlot();
}
