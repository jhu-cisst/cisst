/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Dorothy Hu
  Created on: 2017-01-20

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctForceTorque2DWidget_h
#define _vctForceTorque2DWidget_h

#include <cisstVector/vctQtForwardDeclarations.h>
#include <cisstVector/vctPlot2DBase.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

#include <QWidget>

class QLabel;

// Always include last
#include <cisstVector/vctExportQt.h>

class CISST_EXPORT vctForceTorque2DWidget: public QWidget
{
    Q_OBJECT;

public:
    vctForceTorque2DWidget(void);
    ~vctForceTorque2DWidget(){}

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    void setupUi(void);

    QLabel * QLUpperLimit;
    QLabel * QLLowerLimit;

    vctPlot2DOpenGLQtWidget * QFTPlot;
    vctPlot2DBase::Signal * mForceSignal[3];
    vctPlot2DBase::Signal * mFNormSignal;
    vctPlot2DBase::Signal * mTorqueSignal[3];

    vctPlot2DBase::Scale * mForceScale;
    vctPlot2DBase::Scale * mTorqueScale;

    int PlotIndex;

 public:
    void SetValue(const double & time, const vct3 & force, const vct3 & torque);


private slots:
    void SlotPlotIndex(int newAxis);
};

CMN_DECLARE_SERVICES_INSTANTIATION(vctForceTorque2DWidget);

#endif // _vctForceTorque2DWidget_h
