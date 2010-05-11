/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DOpenGLQtWidget.cpp 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctPlot2DOpenGLQtWidget.h>


vctPlot2DOpenGLQtWidget::vctPlot2DOpenGLQtWidget(QWidget * parent):
    QGLWidget(parent),
    vctPlot2DOpenGL()
{
}


void vctPlot2DOpenGLQtWidget::initializeGL(void)
{
    vctPlot2DOpenGL::RenderInitialize();
}


void vctPlot2DOpenGLQtWidget::resizeGL(int width, int height)
{
    vctPlot2DOpenGL::RenderResize(width, height);
}


void vctPlot2DOpenGLQtWidget::paintGL(void)
{
    vctPlot2DOpenGL::Render();
}
