/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-11-09

  (C) Copyright 2009-2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _displayQtWidget_h
#define _displayQtWidget_h

#include <QDial>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <cisstVector/vctPlot2DOpenGLQtWidget.h>


class displayQtWidget: public QWidget
{
    Q_OBJECT;

 public:
    displayQtWidget(void);
    ~displayQtWidget(void) {};

    QDial * DialAmplitude;
    QGridLayout * CentralLayout;
    vctPlot2DOpenGLQtWidget * Plot;
    vctPlot2DBase::Signal * DataSignal;
    QLabel * LabelAmplitude;
    QLabel * ValueAmplitude;
    QLabel * LabelData;
    QLabel * ValueData;
};

#endif  // _displayQtWidget_h
