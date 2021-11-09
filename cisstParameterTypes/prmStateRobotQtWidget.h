/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-03-22

  (C) Copyright 2017-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmStateRobotQtWidget_h
#define _prmStateRobotQtWidget_h

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstParameterTypes/prmPositionCartesianGetQtWidget.h>
#include <cisstParameterTypes/prmStateJointQtWidget.h>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class CISST_EXPORT prmStateRobotQtWidget: public QWidget
{
    Q_OBJECT;

public:
    prmStateRobotQtWidget(void);
    inline virtual ~prmStateRobotQtWidget(void) {};

    void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute);

    void setupUi(void);

protected:
    prmStateJointQtWidget * QSJWidget;
    prmPositionCartesianGetQtWidget * QPCGWidget;
};

// Widget with a component, can be used directly with cisstMultiTask component manager
class CISST_EXPORT prmStateRobotQtWidgetComponent: public prmStateRobotQtWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    prmStateRobotQtWidgetComponent(const std::string & componentName, double periodInSeconds = 50.0 * cmn_ms);
    ~prmStateRobotQtWidgetComponent() {}

    inline void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    inline void Cleanup(void) {};

private slots:
    void timerEvent(QTimerEvent * event);

private:
    int TimerPeriodInMilliseconds;
    mtsFunctionRead configuration_js;
    prmConfigurationJoint ConfigurationJoint;
    mtsFunctionRead measured_js;
    prmStateJoint StateJoint;
    mtsFunctionRead measured_cp;
    prmPositionCartesianGet Position;
};

CMN_DECLARE_SERVICES_INSTANTIATION(prmStateRobotQtWidgetComponent);

#endif  // _prmStateRobotQtWidget_h
