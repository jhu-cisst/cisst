/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-11-09

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "displayQtWidget.h"


displayQtWidget::displayQtWidget(void)
{
    // create the widgets
    Plot = new vctPlot2DOpenGLQtWidget(this);
    vctPlot2DBase::Scale * scale = Plot->AddScale("Scale");
    DataSignal = scale->AddSignal("Data");
    DialAmplitude = new QDial(this);
    LabelAmplitude = new QLabel("Amplitude", this);
    ValueAmplitude = new QLabel("1", this);
    LabelData = new QLabel("Data", this);
    ValueData = new QLabel("0.0", this);

    // configure the widgets
    DialAmplitude->setNotchesVisible(true);
    DialAmplitude->setRange(1, 100);
    LabelAmplitude->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    LabelData->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    // create a layout for the widgets
    CentralLayout = new QGridLayout(this);
    CentralLayout->setRowStretch(0, 1);
    CentralLayout->setColumnStretch(1, 1);
    CentralLayout->addWidget(Plot, 0, 0, 1, 2);
    CentralLayout->addWidget(DialAmplitude, 1, 1, 1, 2);
    CentralLayout->addWidget(LabelAmplitude, 2, 0);
    CentralLayout->addWidget(ValueAmplitude, 2, 1);
    CentralLayout->addWidget(LabelData, 3, 0);
    CentralLayout->addWidget(ValueData, 3, 1);

    // connect Qt signals to slots
    QObject::connect(DialAmplitude, SIGNAL(valueChanged(int)),
                     ValueAmplitude, SLOT(setNum(int)));
}
