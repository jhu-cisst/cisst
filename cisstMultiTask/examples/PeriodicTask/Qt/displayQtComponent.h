/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _displayQtComponent_h
#define _displayQtComponent_h

#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>

#include <QObject>

#include "displayQtWidget.h"

// This class is not a cisst component, but instances of this class
// are part of another cisst component (mainQtComponent).

class displayQtComponent : public QObject
{
    Q_OBJECT;

 public:
    displayQtComponent(void);
    ~displayQtComponent(void) {};

    QWidget * GetWidget(void) {
        return &CentralWidget;
    }

    struct {
       mtsFunctionRead GetData;
       mtsFunctionWrite SetAmplitude;
    } Generator;

 protected:
    displayQtWidget CentralWidget;

    mtsDouble Data;
    mtsDouble AmplitudeData;

 public slots:
    void timerEvent(QTimerEvent * event);
    void SetAmplitudeQSlot(int newValue);
};

#endif  // _displayQtComponent_h
