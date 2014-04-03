/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-07-14

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsQtWidgetIntervalStatistics_h
#define _mtsQtWidgetIntervalStatistics_h

#include <cisstMultiTask/mtsIntervalStatistics.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsQtWidgetIntervalStatistics: public QTableWidget
{
    Q_OBJECT;

public:
    mtsQtWidgetIntervalStatistics(void);
    ~mtsQtWidgetIntervalStatistics(void) {};

    void SetValue(const mtsIntervalStatistics & newValue);

protected:
    QTableWidgetItem * QTWIAverage;
    QTableWidgetItem * QTWIAverageHz;
    QTableWidgetItem * QTWIStdDev;
    QTableWidgetItem * QTWIMin;
    QTableWidgetItem * QTWIMax;
    QTableWidgetItem * QTWILoadMin;
    QTableWidgetItem * QTWILoadMax;
};

#endif  // _mtsQtWidgetIntervalStatistics_h
