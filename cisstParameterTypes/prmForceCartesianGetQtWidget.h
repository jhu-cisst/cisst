/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-12-12

  (C) Copyright 2017-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmForceCartesianGetQtWidget_h
#define _prmForceCartesianGetQtWidget_h

#include <cisstCommon/cmnUnits.h>
#include <cisstVector/vctQtWidgetFrame.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstParameterTypes/prmForceCartesianGet.h>

#include <QWidget>


// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class QLabel;

class CISST_EXPORT prmForceCartesianGetQtWidget: public QWidget
{
    Q_OBJECT;

public:
    prmForceCartesianGetQtWidget(void);
    ~prmForceCartesianGetQtWidget(void) {};

    inline void setupUi(void) {};

    /*! Set value, this method will update the values displayed in the
      Qt Widget */
    void SetValue(const prmForceCartesianGet & newValue);

protected:
    vctForceTorqueQtWidget * QForce;
    QLabel * QLReferenceFrame;
    QLabel * QLTime; // time and valid using background color
};

// Widget with a component, can be used directly with cisstMultiTask component manager
class CISST_EXPORT prmForceCartesianGetQtWidgetComponent: public prmForceCartesianGetQtWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    prmForceCartesianGetQtWidgetComponent(const std::string & componentName, double periodInSeconds = 50.0 * cmn_ms);
    ~prmForceCartesianGetQtWidgetComponent() {}

    inline void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    inline void Cleanup(void) {};

private slots:
    void timerEvent(QTimerEvent * event);

private:
    int TimerPeriodInMilliseconds;
    mtsFunctionRead measured_cf;
    prmForceCartesianGet m_measured_cf;
};

CMN_DECLARE_SERVICES_INSTANTIATION(prmForceCartesianGetQtWidgetComponent);

#endif // _prmForceCartesianGetQtWidget_h
