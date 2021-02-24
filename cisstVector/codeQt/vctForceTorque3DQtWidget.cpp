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

#include <cisstVector/vctForceTorque3DQtWidget.h>

vctForceTorque3DQtWidget::vctForceTorque3DQtWidget(void):
    PlotIndex(0)
{
    setupUi();
}

void vctForceTorque3DQtWidget::closeEvent(QCloseEvent * event)
{
    int answer = QMessageBox::warning(this, tr("vctForceTorque3DQtWidget"),
                                      tr("Do you really want to quit this application?"),
                                      QMessageBox::No | QMessageBox::Yes);
    if (answer == QMessageBox::Yes) {
        event->accept();
        QCoreApplication::exit();
    } else {
        event->ignore();
    }
}

void vctForceTorque3DQtWidget::setupUi(void)
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // left side, upper/lower limit, selector and legend
    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(leftLayout);

    // combo box to select the plot item
    QComboBox * QPlotSelectItem = new QComboBox;
    QPlotSelectItem->addItem("Forces");
    QPlotSelectItem->addItem("Torques");
    QPlotSelectItem->setCurrentIndex(PlotIndex);
    leftLayout->addWidget(QPlotSelectItem);

    // legend
    QLabel * label;
    QPalette palette;
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
    label = new QLabel("Vector");
    label->setAutoFillBackground(true);
    palette.setColor(QPalette::WindowText, this->palette().color(QPalette::WindowText));
    label->setPalette(palette);
    leftLayout->addWidget(label);

    leftLayout->addStretch();

    // plot area
    QVector = new vctVector3DQtWidget();
    QVector->resize(QVector->sizeHint());
    QVector->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mainLayout->addWidget(QVector);

    this->setLayout(mainLayout);

    setWindowTitle("");
    resize(sizeHint());

    // setup Qt Connection
    connect(QPlotSelectItem, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotPlotIndex(int)));

    SlotPlotIndex(PlotIndex);
}

void vctForceTorque3DQtWidget::SetValue(const vct3 & force, const vct3 & torque)
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    // plot
    if (PlotIndex == 0) {
        QVector->SetValue(force);
    } else if (PlotIndex == 1) {
        QVector->SetValue(torque);
    }
}

void vctForceTorque3DQtWidget::SlotPlotIndex(int newAxis)
{
    PlotIndex = newAxis;
    QVector->SetAutoResize(true);
}
