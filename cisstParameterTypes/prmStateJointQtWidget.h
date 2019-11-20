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

#ifndef _prmStateJointQtWidget_h
#define _prmStateJointQtWidget_h

#include <QWidget>

#include <cisstVector/vctQtWidgetDynamicVector.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstParameterTypes/prmStateJoint.h>
#include <cisstParameterTypes/prmConfigurationJoint.h>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class CISST_EXPORT prmStateJointQtWidget: public QWidget
{
    Q_OBJECT;

public:
    prmStateJointQtWidget(void);
    ~prmStateJointQtWidget(void) {};

    inline void setupUi(void) {};

    /*! Set value, this method will update the values display in the
      Qt Widget for position, velocity and effort. */
    void SetValue(const prmStateJoint & newValue);

    inline void SetConfiguration(const prmConfigurationJoint & newConfiguration) {
        mConfiguration = newConfiguration;
    }

    inline void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute) {
        mPrismaticFactor = prismatic;
        mRevoluteFactor = revolute;
        if ((prismatic == 1.0) && (revolute == 1.0)) {
            mNeedsConversion = false;
        } else {
            mNeedsConversion = true;
        }
    }

protected:
    vctQtWidgetDynamicVectorDoubleRead
        * QVRPosition,
        * QVRVelocity,
        * QVREffort;
    QWidget
        * QWPosition,
        * QWVelocity,
        * QWEffort;

    prmConfigurationJoint mConfiguration;
    double mPrismaticFactor, mRevoluteFactor;
    bool mNeedsConversion;
    vctDoubleVec mPositionFactors, mVelocityFactors, mTempVector;
};

#endif // _prmStateJointQtWidget_h
