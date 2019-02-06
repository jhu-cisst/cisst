/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-01-03

  (C) Copyright 2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmInputDataQtWidget_h
#define _prmInputDataQtWidget_h

#include <QWidget>
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
    prmInputDataQtWidget(void);
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

#endif // _prmInputDataQtWidget_h
