/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-17

  (C) Copyright 2013-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsSystemQtWidget_h
#define _mtsSystemQtWidget_h

#include <cisstMultiTask/mtsComponent.h>

#include <QTextEdit>

#include <cisstMultiTask/mtsMessageQtWidget.h>
#include <cisstMultiTask/mtsIntervalStatisticsQtWidget.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

// Widget without the component, can be included in another widget
class CISST_EXPORT mtsSystemQtWidget: public QWidget
{
    Q_OBJECT;

public:
    mtsSystemQtWidget(const std::string & name = "mtsSystemQtWidget");
    inline virtual ~mtsSystemQtWidget() {}

    void SetInterfaceRequired(mtsInterfaceRequired * interfaceRequired);
    void setupUi(void);
    void SetValue(const mtsIntervalStatistics & value);

protected:
    mtsMessageQtWidget * QMMessage;
    mtsIntervalStatisticsQtWidget * QMIntervalStatistics;
};

// Widget with a component, can be used directly with cisstMultiTask component manager
class CISST_EXPORT mtsSystemQtWidgetComponent: public mtsSystemQtWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsSystemQtWidgetComponent(const std::string & componentName, double periodInSeconds = 50.0 * cmn_ms);
    ~mtsSystemQtWidgetComponent() {}

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

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSystemQtWidgetComponent);

#endif // _mtsSystemQtWidget_h
