/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-07-14

  (C) Copyright 2013-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsIntervalStatisticsQtWidget_h
#define _mtsIntervalStatisticsQtWidget_h

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsIntervalStatistics.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsIntervalStatisticsQtWidget: public QTableWidget
{
    Q_OBJECT;

public:
    mtsIntervalStatisticsQtWidget(void);
    ~mtsIntervalStatisticsQtWidget(void) {};

    inline void setupUi(void) {};
    void SetValue(const mtsIntervalStatistics & newValue);

protected:
    QTableWidgetItem * QTWIPeriodAverage;
    QTableWidgetItem * QTWIPeriodHz;
    QTableWidgetItem * QTWIPeriodRange;
    QTableWidgetItem * QTWILoadAverage;
    QTableWidgetItem * QTWILoadPercent;
    QTableWidgetItem * QTWILoadRange;
    QTableWidgetItem * QTWIInterval;
    QTableWidgetItem * QTWINumberOfSamples;
    QTableWidgetItem * QTWINumberOfOverruns;
};

// Widget with a component, can be used directly with cisstMultiTask component manager
class CISST_EXPORT mtsIntervalStatisticsQtWidgetComponent: public mtsIntervalStatisticsQtWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsIntervalStatisticsQtWidgetComponent(const std::string & componentName, double periodInSeconds = 50.0 * cmn_ms);
    ~mtsIntervalStatisticsQtWidgetComponent() {}

    inline void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    inline void Cleanup(void) {};

private slots:
    void timerEvent(QTimerEvent * event);

private:
    int TimerPeriodInMilliseconds;
    mtsFunctionRead period_statistics;
    mtsIntervalStatistics IntervalStatistics;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsIntervalStatisticsQtWidgetComponent);


#endif  // _mtsIntervalStatisticsQtWidget_h
