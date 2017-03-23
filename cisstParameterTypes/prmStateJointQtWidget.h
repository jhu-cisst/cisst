/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-03-22

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

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

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class CISST_EXPORT prmStateJointQtWidget: public QWidget
{
    Q_OBJECT;

public:
    prmStateJointQtWidget(void);
    ~prmStateJointQtWidget(void) {};

    inline void setupUi(void) {};
    void SetValue(const prmStateJoint & newValue);

protected:
    vctQtWidgetDynamicVectorDoubleRead
        * QVRPosition,
        * QVRVelocity,
        * QVREffort;
    QWidget
        * QWPosition,
        * QWVelocity,
        * QWEffort;
};

#endif // _prmStateJointQtWidget_h
