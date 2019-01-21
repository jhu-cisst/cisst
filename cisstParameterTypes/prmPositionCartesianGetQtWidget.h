/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-12-12

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmPositionCartesianGetQtWidget_h
#define _prmPositionCartesianGetQtWidget_h

#include <QWidget>
#include <cisstVector/vctQtWidgetFrame.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class QLabel;

class CISST_EXPORT prmPositionCartesianGetQtWidget: public QWidget
{
    Q_OBJECT;

public:
    prmPositionCartesianGetQtWidget(void);
    ~prmPositionCartesianGetQtWidget(void) {};

    inline void setupUi(void) {};

    /*! Set value, this method will update the values display in the
      Qt Widget for position, velocity and effort. */
    void SetValue(const prmPositionCartesianGet & newValue);

    inline void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute) {
        QFPosition->SetPrismaticRevoluteFactors(prismatic, revolute);
    }

protected:
    vctQtWidgetFrameDoubleRead * QFPosition;
    QLabel * QLReferenceFrame;
    QLabel * QLTime; // time and valid using background color
};

#endif // _prmPositionCartesianGetQtWidget_h
