/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Dorothy Hu
  Created on: 2017-01-20

  (C) Copyright 2017-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctForceTorque2DQtWidget_h
#define _vctForceTorque2DQtWidget_h

#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctPlot2DBase.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

#include <QWidget>

class QLabel;
class QSignalMapper;
class QCheckBox;

// Always include last
#include <cisstVector/vctExportQt.h>

class CISST_EXPORT vctForceTorque2DQtWidget: public QWidget
{
    Q_OBJECT;

public:
    vctForceTorque2DQtWidget(void);
    ~vctForceTorque2DQtWidget(){}

    void SetValue(const double & time, const vct3 & force, const vct3 & torque);

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    void setupUi(void);

    vctPlot2DOpenGLQtWidget * QFTPlot;
    // signals 0, 1, 2 are for x, y, z.  3 is for norm and 4 for zero
    vctPlot2DBase::Signal * mSignals[2][5];
    bool mVisibleSignals[2][5];

    // scale 0 is for forces, 1 is for torques
    vctPlot2DBase::Scale * mScales[2];

    int mScaleIndex; // 0 for forces, 1 for torques

    QSignalMapper * mSignalMapper;
    QCheckBox * mCheckBoxes[5];

private slots:
    void SlotScaleIndex(int newAxis);
    void SlotVisibleSignal(int);
};

CMN_DECLARE_SERVICES_INSTANTIATION(vctForceTorque2DQtWidget);

#endif // _vctForceTorque2DQtWidget_h
