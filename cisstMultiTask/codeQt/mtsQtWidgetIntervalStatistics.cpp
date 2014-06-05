/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
    this->setRowCount(4);
    this->setColumnCount(3);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->verticalScrollBar()->hide();
    this->horizontalScrollBar()->hide();
#if CISST_HAS_QT4
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
    int verticalHeight = this->verticalHeader()->sizeHint().height() * this->rowCount();
    this->setFixedHeight(verticalHeight);
    QLabel * label;

    label = new QLabel("Average");
    this->setCellWidget(0, 0, label);
    QTWIAverage = new QTableWidgetItem();
    QTWIAverage->setTextAlignment(Qt::AlignRight);
    QTWIAverage->setFlags(QTWIAverage->flags() ^ Qt::ItemIsEditable);
    this->setItem(0, 1, QTWIAverage);
    QTWIAverageHz = new QTableWidgetItem();
    QTWIAverageHz->setTextAlignment(Qt::AlignRight);
    QTWIAverageHz->setFlags(QTWIAverageHz->flags() ^ Qt::ItemIsEditable);
    this->setItem(0, 2, QTWIAverageHz);

    label = new QLabel("Std dev");
    this->setCellWidget(1, 0, label);
    QTWIStdDev = new QTableWidgetItem();
    QTWIStdDev->setTextAlignment(Qt::AlignRight);
    QTWIStdDev->setFlags(QTWIStdDev->flags() ^ Qt::ItemIsEditable);
    this->setItem(1, 1, QTWIStdDev);

    label = new QLabel("Range");
    this->setCellWidget(2, 0, label);
    QTWIMin = new QTableWidgetItem();
    QTWIMin->setTextAlignment(Qt::AlignRight);
    QTWIMin->setFlags(QTWIMin->flags() ^ Qt::ItemIsEditable);
    this->setItem(2, 1, QTWIMin);
    QTWIMax = new QTableWidgetItem();
    QTWIMax->setTextAlignment(Qt::AlignRight);
    QTWIMax->setFlags(QTWIMax->flags() ^ Qt::ItemIsEditable);
    this->setItem(2, 2, QTWIMax);

    label = new QLabel("Load");
    this->setCellWidget(3, 0, label);
    QTWILoadMin = new QTableWidgetItem();
    QTWILoadMin->setTextAlignment(Qt::AlignRight);
    QTWILoadMin->setFlags(QTWILoadMin->flags() ^ Qt::ItemIsEditable);
    this->setItem(3, 1, QTWILoadMin);
    QTWILoadMax = new QTableWidgetItem();
    QTWILoadMax->setTextAlignment(Qt::AlignRight);
    QTWILoadMax->setFlags(QTWILoadMax->flags() ^ Qt::ItemIsEditable);
    this->setItem(3, 2, QTWILoadMax);
}

void mtsQtWidgetIntervalStatistics::SetValue(const mtsIntervalStatistics & newValue)
{
    const double avg = newValue.GetAvg();
    QTWIAverage->setText(QString("%1 ms").arg(avg * 1000.0, -6, 'f', 3));
    QTWIAverageHz->setText(QString("%1 KHz").arg(0.001 / avg, -6, 'f', 3));
    QTWIStdDev->setText(QString("%1 ms").arg(newValue.GetStdDev() * 1000.0, -6, 'f', 3));
    QTWIMin->setText(QString("%1 ms").arg(newValue.GetMin() * 1000.0, -6, 'f', 3));
    QTWIMax->setText(QString("%1 ms").arg(newValue.GetMax() * 1000.0, -6, 'f', 3));
    const double minLoad = newValue.MinComputeTime() / avg * 100.0;
    const double maxLoad = newValue.MaxComputeTime() / avg * 100.0;
    QTWILoadMin->setText(QString("%1\%").arg(minLoad, -4, 'f', 0));
    QTWILoadMax->setText(QString("%1\%").arg(maxLoad, -4, 'f', 0));
}
