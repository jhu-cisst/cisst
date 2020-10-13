/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Dorothy Hu
  Created on: 2017-01-20

  (C) Copyright 2017-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctForceTorque3DQtWidget_h
#define _vctForceTorque3DQtWidget_h

#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctVector3DQtWidget.h>
#include <QWidget>

class QLabel;

// Always include last
#include <cisstVector/vctExportQt.h>

class CISST_EXPORT vctForceTorque3DQtWidget: public QWidget
{
    Q_OBJECT;

public:
    vctForceTorque3DQtWidget(void);
    ~vctForceTorque3DQtWidget(){}

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    void setupUi(void);
    vctVector3DQtWidget * QVector;

    // force vs. torque
    int PlotIndex;

 public:
    void SetValue(const vct3 & force, const vct3 & torque);


private slots:
    void SlotPlotIndex(int newAxis);
};

CMN_DECLARE_SERVICES_INSTANTIATION(vctForceTorque3DQtWidget);

#endif // _vctForceTorque3DQtWidget_h
