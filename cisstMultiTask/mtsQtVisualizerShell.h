/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Praneeth Sadda
  Created on:	2012-05-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _mtsQtVisualizerShell_h
#define _mtsQtVisualizerShell_h

#include <QWidget>

#include <cisstVector/vctPlot2DOpenGLQtWidget.h>

#include <cisstMultiTask/mtsManagerGlobal.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

class CISST_EXPORT mtsQtVisualizerShell: public QWidget
{
    Q_OBJECT;

public:
    mtsQtVisualizerShell(mtsManagerGlobal * globalManager, vctPlot2DOpenGLQtWidget * visualizer = 0, QWidget * parent = 0);

private:
    vctPlot2DOpenGLQtWidget * Visualizer;
};

#endif
