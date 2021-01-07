/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-01-03

  (C) Copyright 2019-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmInputDataQtWidget_h
#define _prmInputDataQtWidget_h

#include <QWidget>

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstParameterTypes/prmInputData.h>
#include <cisstVector/vctPlot2DOpenGLQtWidget.h>
#include <cisstVector/vctQtWidgetDynamicVector.h>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class QSpinBox;

class CISST_EXPORT prmInputDataQtWidget: public QWidget
{
    Q_OBJECT;

public:
    prmInputDataQtWidget(const std::string & name = "prmInputDataQtWidget");
    ~prmInputDataQtWidget(void) {};

    inline void setupUi(void) {};

    /*! Set value, this method will update the values displayed in the
      Qt Widget. */
    void SetValue(const prmInputData & newValue);

private slots:
    void SlotPlotIndex(int newAxis);

protected:
    vctQtWidgetDynamicVectorDoubleRead * QVRAnalogInputsWidget;
    vctQtWidgetDynamicVectorBoolRead * QVRDigitalInputsWidget;

    vctPlot2DOpenGLQtWidget * QVPlot;
    vctPlot2DBase::Signal * AnalogSignal;
    QSpinBox * QSBPlotIndex;
    int PlotIndex;
};

// Widget with a component, can be used directly with cisstMultiTask component manager
class CISST_EXPORT prmInputDataQtWidgetComponent: public prmInputDataQtWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    prmInputDataQtWidgetComponent(const std::string & componentName, double periodInSeconds = 50.0 * cmn_ms);
    ~prmInputDataQtWidgetComponent() {}

    inline void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    inline void Cleanup(void) {};

signals:
    void SignalInputDataEvent(prmInputData);                               
private slots:
    void timerEvent(QTimerEvent * event);
    void SlotInputDataEvent(prmInputData);
private:
    void InputDataEventHandler(const prmInputData & inputData);
    int TimerPeriodInMilliseconds;
    prmInputData mInputData;
};

Q_DECLARE_METATYPE(prmInputData);

CMN_DECLARE_SERVICES_INSTANTIATION(prmInputDataQtWidgetComponent);

#endif // _prmInputDataQtWidget_h
