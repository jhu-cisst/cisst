/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-11-30

  (C) Copyright 2017-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnConstants.h>
#include <cisstConfig.h>

#include <cisstVector/vctVector3DQtWidget.h>
#include <QKeyEvent>
#include <QPainter>
#include <QTextStream>

const double vctVector3DQtWidgetDefaultMax = 0.0001;

vctVector3DQtWidget::vctVector3DQtWidget(QWidget * parent):
    vctQtOpenGLBaseWidget(parent),
    mVector(0.0, 0.0, 0.0)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setToolTip(QString("'a' to turn on/off autoresize\n'r' to reset autoresize if already in autoresize mode\n'z' to reset orientation"));

    setContentsMargins(0, 0, 0, 0);
    SetAutoResize(true);
    mAxisLength = 0.5;
    ResetOrientation();
}

void vctVector3DQtWidget::ResetOrientation(void)
{
    // start with default Z up, x toward left, y towards right
    mCurrentOrientation = Eigen::Quaterniond(
        Eigen::AngleAxisd(0.1 * cmnPI, Eigen::Vector3d::UnitX()) *
        Eigen::AngleAxisd(-0.75 * cmnPI, Eigen::Vector3d::UnitY()) *
        Eigen::AngleAxisd(-0.5 * cmnPI, Eigen::Vector3d::UnitX())
    );
    mStartMousePosition = Eigen::Vector2i::Zero();
    mDeltaOrientation = Eigen::Quaterniond::Identity();
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

void vctVector3DQtWidget::SetValue(const Eigen::Vector3d& value)
{
    mVector = value;
    mVectorNorm = mVector.norm();
    if (mAutoResize && (mVectorNorm > mMaxNorm)) {
        mMaxNorm = mVectorNorm;
        mScale = 1.0f / mMaxNorm;
    }
    // update GL display
    update();
}

void vctVector3DQtWidget::mouseMoveEvent(QMouseEvent * event)
{
    const double sensitivity = 0.01;
    if (event->buttons() & Qt::LeftButton) {
        const Eigen::Vector2i newMousePosition(event->x(), event->y());
        if (mStartMousePosition.any()) {
            const Eigen::Vector2d deltaMouse = sensitivity * (newMousePosition - mStartMousePosition).cast<double>();
            Eigen::Vector3d rodrigues(deltaMouse.x(), deltaMouse.y(), 0.0);
            mDeltaOrientation = Eigen::Quaterniond(Eigen::AngleAxis(rodrigues.norm(), rodrigues.normalized()));
            paintGL();
        } else {
            mStartMousePosition = newMousePosition;
        }
    }
}

void vctVector3DQtWidget::mouseReleaseEvent(QMouseEvent *)
{
    mStartMousePosition.fill(0);
    // save current rotation
    mCurrentOrientation = mDeltaOrientation * mCurrentOrientation;
    mDeltaOrientation = Eigen::Quaterniond::Identity();
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
    case Qt::Key_Z:
        ResetOrientation();
        break;
    default:
        break;
    }
}

void vctVector3DQtWidget::initializeGL(void)
{
    const QColor bgColor = palette().color(QPalette::Base);
    glClearColor(bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 1.0);
    glShadeModel(GL_SMOOTH);
}

void vctVector3DQtWidget::paintGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // disable lighting
    glDisable(GL_LIGHTING);

    // set line width
    glLineWidth(2.0);

    // gl transformation here
    // x+:left  y+:up   z+: point out screen
    glTranslatef(0.0f, 0.0f, -10.0f);
    Eigen::AngleAxisd rot(mDeltaOrientation * mCurrentOrientation);
    glRotated(rot.angle() * cmn180_PI, rot.axis().x(), rot.axis().y(), rot.axis().z());

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
    const QColor textColor = palette().color(QPalette::Text);
    glColor3f(textColor.redF(), textColor.greenF(), textColor.blueF());
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(mVector.x(), mVector.y(), mVector.z());
    glEnd();

    // draw X/Y/Z projections
    glEnable(GL_LINE_STIPPLE);

    glColor3f(1.0f, 0.0f, 0.0f);
    if (mVector.x() > 0.0) {
        glLineStipple(1, 0x00FF); // dashed
    } else {
        glLineStipple(1, 0x0101); // dotted
    }
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(mVector.x(), 0.0f, 0.0f);
    glVertex3f(mVector.x(), mVector.y(), 0.0f);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    if (mVector.y() > 0.0) {
        glLineStipple(1, 0x00FF); // dashed
    } else {
        glLineStipple(1, 0x0101); // dotted
    }

    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, mVector.y(), 0.0f);
    glVertex3f(mVector.x(), mVector.y(), 0.0f);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    if (mVector.z() > 0.0) {
        glLineStipple(1, 0x00FF); // dashed
    } else {
        glLineStipple(1, 0x0101); // dotted
    }
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, mVector.z());
    glVertex3f(mVector.x(), mVector.y(), mVector.z());
    glVertex3f(mVector.x(), mVector.y(), 0.0f);
    glEnd();

    glDisable(GL_LINE_STIPPLE);

    glPopMatrix();

    const int font_size = 8;
    QPainter painter(this);
    const QColor txtColor = palette().color(QPalette::Text);
    painter.setPen(txtColor);
    painter.setFont(QFont("Helvetica", font_size));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QString display_string;
    QTextStream tstream(&display_string);
    tstream.setRealNumberPrecision(2);
    tstream << mVectorNorm << "/" << mMaxNorm;

    painter.drawText(1, 1 + font_size, display_string);
    painter.end();

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
