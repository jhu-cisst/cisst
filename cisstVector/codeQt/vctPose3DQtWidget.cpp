/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-09-04

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstConfig.h>

#include <cisstVector/vctPose3DQtWidget.h>
#include <QKeyEvent>

vctPose3DQtWidget::vctPose3DQtWidget(QWidget * parent):
    vctQtOpenGLBaseWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setToolTip(QString("'a' to turn on/off autoresize\n'r' to reset autoresize if already in autoresize mode\n'1' to plot XY\n'2' to plot XZ\n'3' to plot YZ"));
    setContentsMargins(0, 0, 0, 0);
    mX = 0;
    mY = 1;
    SetBackgroundColor(vct3(1.0));
    SetAutoResize(true);
}

void vctPose3DQtWidget::SetAutoResize(const bool autoResize)
{
    // reset BB used to auto-center
    mBB.Empty = true;
    mAutoResize = autoResize;
}

void vctPose3DQtWidget::ResetSize(void)
{
    mBB.Empty = true;
}

void vctPose3DQtWidget::SetValue(const vct3 & value)
{
    // update bounding box
    if (mBB.Empty) {
        mBB.MinCorner = value;
        mBB.MaxCorner = value;
        mBB.Empty = false;
    } else {
        mBB.MinCorner.ElementwiseMinOf(mBB.MinCorner, value);
        mBB.MaxCorner.ElementwiseMaxOf(mBB.MaxCorner, value);
    }

    // save all values for display
    mPoses.push_back(value);

    // update GL display
    update();
}

void vctPose3DQtWidget::SetBackgroundColor(const vctDouble3 & colorInRange0To1)
{
    vctDouble3 clippedColor;
    clippedColor.ClippedAboveOf(colorInRange0To1, 1.0);
    clippedColor.ClippedBelowOf(clippedColor, 0.0);
    mBackgroundColor.Assign(clippedColor);
}

void vctPose3DQtWidget::keyPressEvent(QKeyEvent * event)
{
    switch(event->key()) {
    case Qt::Key_1:
        mX = 0;
        mY = 1;
        mBB.Empty = true;
        break;
    case Qt::Key_2:
        mX = 0;
        mY = 2;
        mBB.Empty = true;
        break;
    case Qt::Key_3:
        mX = 1;
        mY = 2;
        mBB.Empty = true;
        break;
    case Qt::Key_A:
        SetAutoResize(!mAutoResize);
        break;
    case Qt::Key_R:
        ResetSize();
        break;
    default:
        break;
    }
}

void vctPose3DQtWidget::initializeGL(void)
{
    glMatrixMode(GL_MODELVIEW); // set the model view matrix
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST); // disable depth test
    glDisable(GL_LIGHTING); // disable lighting
    glShadeModel(GL_SMOOTH); // smooth render
    glClearColor(static_cast<GLclampf>(mBackgroundColor[0]),
                 static_cast<GLclampf>(mBackgroundColor[1]),
                 static_cast<GLclampf>(mBackgroundColor[2]),
                 static_cast<GLclampf>(1.0));
}

void vctPose3DQtWidget::paintGL(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // update translation and scale based on viewport and bounding box
    if (mAutoResize && !mBB.Empty) {
        // scale to fit viewport without changing ratio
        vct3 sizeBB;
        sizeBB.DifferenceOf(mBB.MaxCorner, mBB.MinCorner);
        if ((sizeBB[mX] > 0.0) && (sizeBB[mY] > 0.0)) {
            const double ratioX = mViewport.X() / sizeBB[mX];
            const double ratioY = mViewport.Y() / sizeBB[mY];
            mViewportScale = (ratioY > ratioX ? ratioX : ratioY) * 0.98; // add some padding
        } else {
            mViewportScale = 1.0;
        }
        // take center of BB for display
        vct3 centerBB;
        centerBB.SumOf(mBB.MinCorner, mBB.MaxCorner);
        centerBB.Multiply(mViewportScale * 0.5); // middle and scale to viewport
        vct2 centerVP(mViewport);
        centerVP.Divide(2.0);
        mViewportTranslation.DifferenceOf(centerVP, vctDouble2(centerBB[mX], centerBB[mY]));
    }
    glTranslated(mViewportTranslation.X(), mViewportTranslation.Y(), 0.0);
    glScaled(mViewportScale, mViewportScale, 1.0);
    typedef std::list<vct3> PosesType;
    const PosesType::const_iterator end = mPoses.end();
    glColor3d(0.1, 0.1, 0.1);
    glPointSize(3.0);
    glBegin(GL_POINTS); {
        for (PosesType::const_iterator pose = mPoses.begin();
             pose != end;
             ++pose) {
            glVertex2d(pose->Element(mX), pose->Element(mY));
        }
    } glEnd();
    // bounding box
    glBegin(GL_LINE_STRIP); {
        glVertex2d(mBB.MinCorner[mX], mBB.MinCorner[mY]);
        glVertex2d(mBB.MaxCorner[mX], mBB.MinCorner[mY]);
        glVertex2d(mBB.MaxCorner[mX], mBB.MaxCorner[mY]);
        glVertex2d(mBB.MinCorner[mX], mBB.MaxCorner[mY]);
        glVertex2d(mBB.MinCorner[mX], mBB.MinCorner[mY]);
    } glEnd();
    // zero values
    if (mBB.MinCorner[mY] * mBB.MaxCorner[mY] < 0.0) {
        glBegin(GL_LINE_STRIP); {
            glVertex2d(mBB.MinCorner[mX], 0.0);
            glVertex2d(mBB.MaxCorner[mX], 0.0);
        } glEnd();
    }
    if (mBB.MinCorner[mX] * mBB.MaxCorner[mX] < 0.0) {
        glBegin(GL_LINE_STRIP); {
            glVertex2d(0.0, mBB.MinCorner[mY]);
            glVertex2d(0.0, mBB.MaxCorner[mY]);
        } glEnd();
    }

}


void vctPose3DQtWidget::resizeGL(int width, int height)
{
    mViewport.Assign(width, height);
    GLsizei w = static_cast<GLsizei>(width);
    GLsizei h = static_cast<GLsizei>(height);
    glViewport(0 , 0, w , h); // set up viewport
    glMatrixMode(GL_PROJECTION); // set the projection matrix
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
}
