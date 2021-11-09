/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-07-13

  (C) Copyright 2013-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsIntervalStatisticsQtWidget.h>

#include <QHeaderView>
#include <QScrollBar>

mtsIntervalStatisticsQtWidget::mtsIntervalStatisticsQtWidget(void):
    QTableWidget()
{
    this->setRowCount(3);
    this->setColumnCount(3);
    this->verticalHeader()->hide();
    this->horizontalHeader()->hide();
    this->verticalScrollBar()->hide();
    this->horizontalScrollBar()->hide();
#if CISST_HAS_QT4
    this->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    this->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#else
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#endif
    int colIndex = 0;

    QTWIPeriodAverage = new QTableWidgetItem();
    QTWIPeriodAverage->setToolTip("Average period");
    QTWIPeriodAverage->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWIPeriodAverage->setFlags(QTWIPeriodAverage->flags() ^ Qt::ItemIsEditable);
    this->setItem(0, colIndex, QTWIPeriodAverage);
    QTWIPeriodHz = new QTableWidgetItem();
    QTWIPeriodHz->setToolTip("Average frequency");
    QTWIPeriodHz->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWIPeriodHz->setFlags(QTWIPeriodHz->flags() ^ Qt::ItemIsEditable);
    this->setItem(1, colIndex, QTWIPeriodHz);
    QTWIPeriodRange = new QTableWidgetItem();
    QTWIPeriodRange->setToolTip("Min/max period");
    QTWIPeriodRange->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWIPeriodRange->setFlags(QTWIPeriodRange->flags() ^ Qt::ItemIsEditable);
    this->setItem(2, colIndex, QTWIPeriodRange);

    colIndex++;
    QTWILoadAverage = new QTableWidgetItem();
    QTWILoadAverage->setToolTip("Average computation time");
    QTWILoadAverage->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWILoadAverage->setFlags(QTWILoadAverage->flags() ^ Qt::ItemIsEditable);
    this->setItem(0, colIndex, QTWILoadAverage);
    QTWILoadPercent = new QTableWidgetItem();
    QTWILoadPercent->setToolTip("Compute time vs period");
    QTWILoadPercent->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWILoadPercent->setFlags(QTWILoadPercent->flags() ^ Qt::ItemIsEditable);
    this->setItem(1, colIndex, QTWILoadPercent);
    QTWILoadRange = new QTableWidgetItem();
    QTWILoadRange->setToolTip("Min/max compute time");
    QTWILoadRange->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWILoadRange->setFlags(QTWILoadRange->flags() ^ Qt::ItemIsEditable);
    this->setItem(2, colIndex, QTWILoadRange);

    colIndex++;
    QTWIInterval = new QTableWidgetItem();
    QTWIInterval->setToolTip("Interval duration");
    QTWIInterval->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWIInterval->setFlags(QTWIInterval->flags() ^ Qt::ItemIsEditable);
    this->setItem(0, colIndex, QTWIInterval);
    QTWINumberOfSamples = new QTableWidgetItem();
    QTWINumberOfSamples->setToolTip("Number of period samples");
    QTWINumberOfSamples->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWINumberOfSamples->setFlags(QTWINumberOfSamples->flags() ^ Qt::ItemIsEditable);
    this->setItem(1, colIndex, QTWINumberOfSamples);
    QTWINumberOfOverruns = new QTableWidgetItem();
    QTWINumberOfOverruns->setToolTip("Number of times computation times execeeded average period");
    QTWINumberOfOverruns->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QTWINumberOfOverruns->setFlags(QTWINumberOfOverruns->flags() ^ Qt::ItemIsEditable);
    this->setItem(2, colIndex, QTWINumberOfOverruns);

    // set value to initialize content size
    this->SetValue(mtsIntervalStatistics());

     // compute height and width, using ResizeToContents breaks auto resize, so we set a fixed size
    int height = 0;
    for (int rowIndex = 0; rowIndex < this->model()->rowCount(); rowIndex++) {
        height += this->rowHeight(rowIndex);
    }
    this->setFixedHeight(height);
    int width = 0;
    for (int colIndex = 0; colIndex < this->model()->columnCount(); colIndex++) {
        width += this->columnWidth(colIndex);
    }
    this->setFixedWidth(width);
}

void mtsIntervalStatisticsQtWidget::SetValue(const mtsIntervalStatistics & newValue)
{
    double periodAvg = newValue.PeriodAvg();
    if (periodAvg == 0.0) {
        periodAvg = 1.0;
    }
    QTWIPeriodAverage->setText(QString("%1±%2ms")
                               .arg(periodAvg * 1000.0, -6, 'f', 3)
                               .arg(newValue.PeriodStdDev() * 1000.0, -6, 'f', 3));
    QTWIPeriodHz->setText(QString("%1 KHz").arg(0.001 / periodAvg, -6, 'f', 3));
    QTWIPeriodRange->setText(QString("%1/%2ms")
                             .arg(newValue.PeriodMin() * 1000.0, -6, 'f', 3)
                             .arg(newValue.PeriodMax() * 1000.0, -6, 'f', 3));

    const double loadAvg = newValue.ComputeTimeAvg();
    const double periodPercent = 100.0 / periodAvg;
    QTWILoadAverage->setText(QString("%1±%2ms")
                             .arg(loadAvg * 1000.0, -6, 'f', 3)
                             .arg(newValue.ComputeTimeStdDev() * 1000.0, -6, 'f', 3));
    QTWILoadPercent->setText(QString("%1\%").arg(loadAvg * periodPercent , -5, 'f', 1));
    QTWILoadRange->setText(QString("%1/%2\%")
                           .arg(newValue.ComputeTimeMin() * periodPercent, -5, 'f', 1)
                           .arg(newValue.ComputeTimeMax() * periodPercent, -5, 'f', 1));

    QTWIInterval->setText(QString("%1s").arg(newValue.StatisticsInterval(), -6, 'f', 3));
    QTWINumberOfSamples->setText(QString("%1 samples").arg(newValue.NumberOfSamples(), 0, 'g', -1, '0'));
    QTWINumberOfOverruns->setText(QString("%1 > period").arg(newValue.NumberOfOverruns(), 0, 'g', -1, '0'));
}



CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsIntervalStatisticsQtWidgetComponent, mtsComponent, std::string);

mtsIntervalStatisticsQtWidgetComponent::mtsIntervalStatisticsQtWidgetComponent(const std::string & componentName, double periodInSeconds):
    mtsComponent(componentName),
    TimerPeriodInMilliseconds(periodInSeconds * 1000)
{
    // Setup CISST Interface
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Component");
    if (interfaceRequired) {
        interfaceRequired->AddFunction("period_statistics", period_statistics);
    }
}

void mtsIntervalStatisticsQtWidgetComponent::Startup(void)
{
    startTimer(TimerPeriodInMilliseconds); // ms
}

void mtsIntervalStatisticsQtWidgetComponent::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    // make sure we should update the display
    if (this->isHidden()) {
        return;
    }

    period_statistics(IntervalStatistics);
    mtsIntervalStatisticsQtWidget::SetValue(IntervalStatistics);
}
