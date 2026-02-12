/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-12-09

  (C) Copyright 2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmPositionJointSetQtWidget_h
#define _prmPositionJointSetQtWidget_h

#include <QPushButton>

#include <cisstVector/vctQtWidgetDynamicVector.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstParameterTypes/prmStateJoint.h>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class CISST_EXPORT prmPositionJointSetQtWidget: public QWidget
{
    Q_OBJECT;

public:
    prmPositionJointSetQtWidget(void);
    ~prmPositionJointSetQtWidget(void) {};

    void setupUi(void);

    inline void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute) {
        mPrismaticFactor = prismatic;
        mRevoluteFactor = revolute;
        if ((prismatic == 1.0) && (revolute == 1.0)) {
            mNeedsConversion = false;
        } else {
            mNeedsConversion = true;
        }
    }

    inline void Read(void) {
        SlotRead();
    }

    mtsFunctionRead * measured_js;
    mtsFunctionRead * configuration_js;
    mtsFunctionWrite * move_jp;

protected:
    vctQtWidgetDynamicVectorDoubleWrite * QVWPosition;
    QPushButton * QPBRead;
    QPushButton * QPBMove;

    double mPrismaticFactor, mRevoluteFactor;
    bool mNeedsConversion;
    vctDoubleVec mFactors, mTemp1, mTemp2;

 private slots:
    void SlotRead(void);
    void SlotSetPositionGoalJoint(void);
};

#endif // _prmPositionJointSetQtWidget_h
