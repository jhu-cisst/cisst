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

#include <cisstVector/vctVector3DQtWidget.h>
#include <QKeyEvent>

const double vctVector3DQtWidgetDefaultMax = 0.0001;

vctVector3DQtWidget::vctVector3DQtWidget(QWidget * parent):
    vctQtOpenGLBaseWidget(parent),
    mVector(0.0)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setToolTip(QString("'a' to turn on/off autoresize\n'r' to reset autoresize if already in autoresize mode"));

    setContentsMargins(0, 0, 0, 0);
    SetAutoResize(true);
    mAxisLength = 0.5;
}


void vctVector3DQtWidget::SetAutoResize(const bool autoResize)
{
    if (autoResize) {
        // re-initialize to default, i.e. fairly small force
        mMaxNorm = vctVector3DQtWidgetDefaultMax;
        mScale = 1.0f / mMaxNorm;
    }
    mAutoResize = autoResize;
}


void vctVector3DQtWidget::SetValue(const vct3 & value)
{
    mVector = value;
    const double norm = mVector.Norm();
    if (mAutoResize && (norm > mMaxNorm)) {
        mMaxNorm = norm;
        mScale = 1.0f / mMaxNorm;
    }
    // update GL display
    update();
}


void vctVector3DQtWidget::initializeGL(void)
{
    const double grey = 0.95;
    glClearColor(grey, grey, grey, 1.0);
    glShadeModel(GL_SMOOTH);
}


void vctVector3DQtWidget::paintGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // gl transformation here
    // x+:left  y+:up   z+: point out screen
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(-70.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-135.0f, 0.0f, 0.0f, 1.0f);

    // disable lighting
    glDisable(GL_LIGHTING);

    // set line width
    glLineWidth(2.0);

    // draw X/Y/Z axis, using half
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(mAxisLength, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, mAxisLength, 0.0f);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, mAxisLength);
    glEnd();

    glPushMatrix();

    if (mAutoResize) {
        glScalef(mScale, mScale, mScale);
    }

    // draw line for force
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(mVector.X(), mVector.Y(), mVector.Z());
    glEnd();

    // draw X/Y/Z projections
    glEnable(GL_LINE_STIPPLE);

    if (mVector.X() > 0.0) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glLineStipple(1, 0x00FF); // dashed
    } else {
        glColor3f(0.8f, 0.0f, 0.0f);
        glLineStipple(1, 0x0101); // dotted
    }
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(mVector.X(), 0.0f, 0.0f);
    glVertex3f(mVector.X(), mVector.Y(), 0.0f);
    glEnd();

    if (mVector.Y() > 0.0) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glLineStipple(1, 0x00FF); // dashed
    } else {
        glColor3f(0.0f, 0.8f, 0.0f);
        glLineStipple(1, 0x0101); // dotted
    }
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, mVector.Y(), 0.0f);
    glVertex3f(mVector.X(), mVector.Y(), 0.0f);
    glEnd();

    if (mVector.Z() > 0.0) {
        glColor3f(0.0f, 0.0f, 1.0f);
        glLineStipple(1, 0x00FF); // dashed
    } else {
        glColor3f(0.0f, 0.0f, 0.8f);
        glLineStipple(1, 0x0101); // dotted
    }
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, mVector.Z());
    glVertex3f(mVector.X(), mVector.Y(), mVector.Z());
    glVertex3f(mVector.X(), mVector.Y(), 0.0f);
    glEnd();

    glDisable(GL_LINE_STIPPLE);

    glPopMatrix();

    glFlush();
}


void vctVector3DQtWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLdouble dWt;
    GLdouble dHt;

    if (width > height) {
        dHt = 1.0;
        dWt = static_cast<double>(width) / static_cast<double>(height);
    } else {
        dHt = static_cast<double>(height) / static_cast<double>(width);
        dWt = 1.0;
    }

    glOrtho(-dWt, dWt, -dHt, dHt, 8.0, 12.0);

    glMatrixMode(GL_MODELVIEW);
}

void vctVector3DQtWidget::keyPressEvent(QKeyEvent * event)
{
    switch(event->key()) {
    case Qt::Key_A:
        // toggle autoresize
        SetAutoResize(!mAutoResize);
        break;
    case Qt::Key_R:
        // reset autoresize if already in autoresize mode
        if (mAutoResize) {
            SetAutoResize(true);
        }
        break;
    default:
        break;
    }
}
