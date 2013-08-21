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


mtsQtWidgetIntervalStatistics::mtsQtWidgetIntervalStatistics(void)
{
    // create a layout for the widgets
    MainLayout = new QVBoxLayout(this);

    QLAverage = new QLabel("Avg: n/a");
    MainLayout->addWidget(QLAverage);
    QLStdDev = new QLabel("Dev: n/a");
    MainLayout->addWidget(QLStdDev);
    QLMin = new QLabel("Min: n/a");
    MainLayout->addWidget(QLMin);
    QLMax = new QLabel("Max: n/a");
    MainLayout->addWidget(QLMax);
    QLLoad = new QLabel("Load: n/a");
    MainLayout->addWidget(QLLoad);
    MainLayout->addStretch();
}

void mtsQtWidgetIntervalStatistics::SetValue(const mtsIntervalStatistics & newValue)
{
    const double avg = newValue.GetAvg();
    QLAverage->setText(QString("Avg: %1").arg(avg * 1000.0, 6, 'g', 4));
    QLStdDev->setText(QString("Dev: %1").arg(newValue.GetStdDev() * 1000.0, -10, 'g', 2));
    QLMin->setText(QString("Min: %1").arg(newValue.GetMin() * 1000.0, 6, 'g', 4));
    QLMax->setText(QString("Max: %1").arg(newValue.GetMax() * 1000.0, 6, 'g', 4));
    const int minLoad = newValue.MinComputeTime() / avg * 100;
    const int maxLoad = newValue.MaxComputeTime() / avg * 100;
    QLLoad->setText(QString("Load: %1:%2\%").arg(minLoad).arg(maxLoad));
}
