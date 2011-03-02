/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFilterImageOpenGLOpenGLQtWidget.cpp -1   $

  Author(s):  Marcin Balicki
  Created on: 2011-02-18

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <QtGui>
#include <QtOpenGL>
#include <QTime>
#include <QImage>

#include <math.h>
#include <QImage>
#include <cisstStereoVision/svlFilterImageOpenGLQtWidget.h>


CMN_IMPLEMENT_SERVICES(svlFilterImageOpenGLQtWidget);


svlFilterImageOpenGLQtWidget::svlFilterImageOpenGLQtWidget(QWidget * parent):
    QGLWidget(parent),
    svlFilterImageOpenGL()
{
    QObject::connect(this, SIGNAL(QSignalUpdateGL()), this, SLOT(updateGL()));
}


svlFilterImageOpenGLQtWidget::~svlFilterImageOpenGLQtWidget()
{
    makeCurrent();
    Release();
}


void svlFilterImageOpenGLQtWidget::initializeGL()
{
    this->RenderInitialize();
}


void svlFilterImageOpenGLQtWidget::paintGL()
{
    this->Render();
}


void svlFilterImageOpenGLQtWidget::resizeGL(int width, int height)
{
    this->RenderResize(width, height);
}


void svlFilterImageOpenGLQtWidget::mousePressEvent(QMouseEvent * event)
{
    LastPosition = event->pos();
}


void svlFilterImageOpenGLQtWidget::mouseMoveEvent(QMouseEvent * event)
{
    int dx = event->x() - LastPosition.x();
    int dy = event->y() - LastPosition.y();

    if (event->buttons() & Qt::LeftButton) {

    }

    else if (event->buttons() & Qt::RightButton) {

    }
    LastPosition = event->pos();
}


void svlFilterImageOpenGLQtWidget::PostProcess(void)
{
    emit QSignalUpdateGL();
}
