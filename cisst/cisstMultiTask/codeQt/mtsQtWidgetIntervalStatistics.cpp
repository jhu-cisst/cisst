/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2013-07-13

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsQtWidgetIntervalStatistics.h>

#include <QHeaderView>
#include <QScrollBar>

mtsQtWidgetIntervalStatistics::mtsQtWidgetIntervalStatistics(void):
    QTableWidget()
{
    this->setRowCount(7);
    this->setColumnCount(2);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    /*
    this->verticalScrollBar()->setDisabled(true);
    this->horizontalScrollBar()->setDisabled(true);
    */
    this->verticalScrollBar()->hide();
    this->horizontalScrollBar()->hide();
   /* int verticalHeight = 400;

        this->horizontalHeader()->sizeHint().height()
        + this->verticalHeader()->sizeHint().height();

    this->setFixedHeight(verticalHeight);
    this->setMinimumHeight(verticalHeight);
    */
    // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QLabel * label;
    int row = 0;

    label = new QLabel("Avg:");
    this->setCellWidget(row, 0, label);
    QTWIAverage = new QTableWidgetItem();
    QTWIAverage->setTextAlignment(Qt::AlignRight);
    QTWIAverage->setFlags(QTWIAverage->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, QTWIAverage);
    row++;

    label = new QLabel("Avg:");
    this->setCellWidget(row, 0, label);
    QTWIAverageHz = new QTableWidgetItem();
    QTWIAverageHz->setTextAlignment(Qt::AlignRight);
    QTWIAverageHz->setFlags(QTWIAverageHz->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, QTWIAverageHz);
    row++;

    label = new QLabel("StdDev:");
    this->setCellWidget(row, 0, label);
    QTWIStdDev = new QTableWidgetItem();
    QTWIStdDev->setTextAlignment(Qt::AlignRight);
    QTWIStdDev->setFlags(QTWIStdDev->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, QTWIStdDev);
    row++;

    label = new QLabel("Min:");
    this->setCellWidget(row, 0, label);
    QTWIMin = new QTableWidgetItem();
    QTWIMin->setTextAlignment(Qt::AlignRight);
    QTWIMin->setFlags(QTWIMin->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, QTWIMin);
    row++;

    label = new QLabel("Max:");
    this->setCellWidget(row, 0, label);
    QTWIMax = new QTableWidgetItem();
    QTWIMax->setTextAlignment(Qt::AlignRight);
    QTWIMax->setFlags(QTWIMax->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, QTWIMax);
    row++;

    label = new QLabel("LoadMin:");
    this->setCellWidget(row, 0, label);
    QTWILoadMin = new QTableWidgetItem();
    QTWILoadMin->setTextAlignment(Qt::AlignRight);
    QTWILoadMin->setFlags(QTWILoadMin->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, QTWILoadMin);
    row++;

    label = new QLabel("LoadMax:");
    this->setCellWidget(row, 0, label);
    QTWILoadMax = new QTableWidgetItem();
    QTWILoadMax->setTextAlignment(Qt::AlignRight);
    QTWILoadMax->setFlags(QTWILoadMax->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, QTWILoadMax);
    row++;
}

void mtsQtWidgetIntervalStatistics::SetValue(const mtsIntervalStatistics & newValue)
{
    const double avg = newValue.GetAvg();
    QTWIAverage->setText(QString("%1 ms").arg(avg * 1000.0, -6, 'f', 2));
    QTWIAverageHz->setText(QString("%1 KHz").arg(0.001 / avg, -6, 'f', 2));
    QTWIStdDev->setText(QString("%1 ms").arg(newValue.GetStdDev() * 1000.0, -6, 'f', 2));
    QTWIMin->setText(QString("%1 ms").arg(newValue.GetMin() * 1000.0, -6, 'f', 2));
    QTWIMax->setText(QString("%1 ms").arg(newValue.GetMax() * 1000.0, -6, 'f', 2));
    const double minLoad = newValue.MinComputeTime() / avg * 100.0;
    const double maxLoad = newValue.MaxComputeTime() / avg * 100.0;
    QTWILoadMin->setText(QString("%1\%").arg(minLoad, -4, 'f', 0));
    QTWILoadMax->setText(QString("%1\%").arg(maxLoad, -4, 'f', 0));
}
