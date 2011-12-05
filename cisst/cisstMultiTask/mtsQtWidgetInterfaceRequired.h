/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsQtWidgetInterfaceRequired_h
#define _mtsQtWidgetInterfaceRequired_h

#include <cisstMultiTask/mtsQtWidgetFunction.h>

#include <QWidget>
class QToolBox;

#include <cisstMultiTask/mtsInterfaceProvided.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsQtWidgetInterfaceRequired: public QWidget
{
    Q_OBJECT;

 public:
    mtsQtWidgetInterfaceRequired(mtsInterfaceProvided * interface,
                                 mtsInterfaceRequired * executionInterface = 0);

 private:
    // mtsInterfaceProvided * Interface;
    mtsInterfaceRequired * ExecutionInterface;
    QToolBox * FunctionsWidget;

    void CreateWidgets(mtsInterfaceProvided & interface, mtsInterfaceRequired & executionInterface);
};


#endif // _mtsQtWidgetInterfaceRequired_h