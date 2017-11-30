/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-11-30

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstConfig.h>

#include <cisstVector/vctForceTorque3DQtWidget.h>

vctForceTorque3DQtWidget::vctForceTorque3DQtWidget(void):
    mForce(0.0),
    mTorque(0.0)
{
    setMinimumHeight(100);
    setMinimumWidth(100);
    setContentsMargins(0, 0, 0, 0);
}

void vctForceTorque3DQtWidget::SetValue(const vct3 & force, const vct3 & torque)
{
    mForce = force;
    mTorque = torque;
    // update GL display
    update();
}


void vctForceTorque3DQtWidget::initializeGL(void)
{
    const double grey = 0.95;
    glClearColor(grey, grey, grey, 1.0);
    glShadeModel(GL_SMOOTH);
}

void vctForceTorque3DQtWidget::paintGL(void)
{
    const int side = qMin(width(), height());
    glViewport((width() - side) / 2, (height() - side) / 2, side, side);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // gl transformation here
    // x+:left  y+:up   z+: point out screen
    glTranslatef(0.0f, 0.0f, -10.0f);

    draw3DAxis(0.4);

    glFlush();
}

void vctForceTorque3DQtWidget::resizeGL(int width, int height)
{
    const int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void vctForceTorque3DQtWidget::draw3DAxis(const float scale)
{
    // draw
    glPushMatrix();

    // disable lighting
    glDisable(GL_LIGHTING);

    // set line width
    glLineWidth(1.0);

    // RGB 3d axes
    // x axis: red
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(scale, 0.0f, 0.0f);
    glEnd();

    // y axis: green
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, scale, 0.0f);
    glEnd();

    // z axis: blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, scale);
    glEnd();

    glPopMatrix();
}
