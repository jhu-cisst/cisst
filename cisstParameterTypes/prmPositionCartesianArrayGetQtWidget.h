/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-07-21

  (C) Copyright 2014-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmPositionCartesianArrayGetQtWidget_h
#define _prmPositionCartesianArrayGetQtWidget_h

#include <cisstCommon/cmnUnits.h>
#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstMultiTask/mtsComponent.h>

#include <QWidget>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class CISST_EXPORT prmPositionCartesianArrayGetQtWidget : public QWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

 public:
    prmPositionCartesianArrayGetQtWidget(const std::string & componentName, double periodInSeconds = 50.0 * cmn_ms);
    ~prmPositionCartesianArrayGetQtWidget() {}

    void Configure(const std::string & filename = "");
    void Startup(void);
    void Cleanup(void);

 protected:
    virtual void closeEvent(QCloseEvent * event);

 private slots:
    void timerEvent(QTimerEvent * event);

 private:
    //! setup GUI
    void setupUi(void);
    int TimerPeriodInMilliseconds;

 protected:
    struct ControllerStruct {
        mtsFunctionRead measured_cp_array;
    } Controller;

 private:
    vctPose3DQtWidget * QVPoses;
};

CMN_DECLARE_SERVICES_INSTANTIATION(prmPositionCartesianArrayGetQtWidget);

#endif // _prmPositionCartesianArrayGetQtWidget_h
