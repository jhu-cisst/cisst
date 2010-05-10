/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DQWidget.cpp 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctPlot2DQWidget.h"


vctPlot2DQWidget::vctPlot2DQWidget(QWidget * parent):
    QGLWidget(parent),
    vctPlot2DGLBase()
{
}


void vctPlot2DQWidget::initializeGL(void)
{
    vctPlot2DGLBase::RenderInitialize();
}


void vctPlot2DQWidget::resizeGL(int width, int height)
{
    vctPlot2DGLBase::RenderResize(width, height);
}


void vctPlot2DQWidget::paintGL(void)
{
    vctPlot2DGLBase::Render();
}
