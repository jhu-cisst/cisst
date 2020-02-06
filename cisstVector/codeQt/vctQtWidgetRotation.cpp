/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Zihan Chen, Anton Deguet
  Created on: 2013-03-20

  (C) Copyright 2013-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstConfig.h>
#include <cisstCommon/cmnPortability.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif

#if (CISST_OS == CISST_DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include <cisstVector/vctQtWidgetRotation.h>

// all these widgets should be replaced to use static vectors/matrices
#include <cisstVector/vctQtWidgetDynamicVector.h>
#include <cisstVector/vctQtWidgetDynamicMatrix.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>


// =============================================
// vctQtWidgetRotationOpenGL
// =============================================

vctQtWidgetRotationOpenGL::vctQtWidgetRotationOpenGL(void)
{
    mRotation.SetAll(0.0);
    setFocusPolicy(Qt::StrongFocus);
    setToolTip(QString("'z' to reset orientation"));
    setMinimumHeight(100);
    setMinimumWidth(100);
    setContentsMargins(0, 0, 0, 0);
    ResetOrientation();
}

void vctQtWidgetRotationOpenGL::ResetOrientation(void)
{
    // start with default Z up, x toward left, y towards right
    mCurrentOrientation =
        vctQuatRot3(vctRodRot3(0.1 * cmnPI, 0.0, 0.0)) *
        vctQuatRot3(vctRodRot3(0.0, -0.75 * cmnPI, 0.0)) *
        vctQuatRot3(vctRodRot3(-0.5 * cmnPI, 0.0, 0.0));
    mStartMousePosition = 0;
    mDeltaOrientation = vctQuatRot3::Identity();
}

void vctQtWidgetRotationOpenGL::SetValue(const vctMatRot3 & rot)
{
    vctEulerZYXRotation3 rotEuler;
    rotEuler.From(rot);
    mRotation.X() = rotEuler.gamma() * cmn180_PI;  // x
    mRotation.Y() = rotEuler.beta() * cmn180_PI;   // y
    mRotation.Z() = rotEuler.alpha() * cmn180_PI;  // z

    // update GL display
    update();
}

void vctQtWidgetRotationOpenGL::mouseMoveEvent(QMouseEvent * event)
{
    const double sensitivity = 0.01;
    if (event->buttons() & Qt::LeftButton) {
        const vctInt2 newMousePosition(event->x(), event->y());
        if (mStartMousePosition != 0) {
            const vct2 deltaMouse = sensitivity * vct2(newMousePosition - mStartMousePosition);
            vctRodRot3 deltaRot(deltaMouse.Y(), deltaMouse.X(), 0.0);
            mDeltaOrientation.From(deltaRot);
            paintGL();
        } else {
            mStartMousePosition = newMousePosition;
        }
    }
}

void vctQtWidgetRotationOpenGL::mouseReleaseEvent(QMouseEvent *)
{
    mStartMousePosition = 0;
    // save current rotation
    mCurrentOrientation = mDeltaOrientation * mCurrentOrientation;
    mDeltaOrientation = vctQuatRot3::Identity();
}

void vctQtWidgetRotationOpenGL::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_Z:
        ResetOrientation();
        break;
    default:
        break;
    }
}

void vctQtWidgetRotationOpenGL::initializeGL(void)
{
    const QColor bgColor = palette().color(QPalette::Base);
    glClearColor(bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 1.0);
    glShadeModel(GL_SMOOTH);
}

void vctQtWidgetRotationOpenGL::paintGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // disable lighting
    glDisable(GL_LIGHTING);

    // set line width
    glLineWidth(2.0);

    // user orientation
    glTranslatef(0.0f, 0.0f, -10.0f);
    vctAxAnRot3 rot(mDeltaOrientation * mCurrentOrientation);
    glRotated(rot.Angle() * cmn180_PI, rot.Axis().X(), rot.Axis().Y(), rot.Axis().Z());

    // draw reference coordinate frame here
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF); // dashed

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    glDisable(GL_LINE_STIPPLE);

    // orientation
    glRotatef(mRotation.Z(), 0.0, 0.0, 1.0);
    glRotatef(mRotation.Y(), 0.0, 1.0, 0.0);
    glRotatef(mRotation.X(), 1.0, 0.0, 0.0);

    // draw current frame
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    glFlush();
}

void vctQtWidgetRotationOpenGL::resizeGL(int width, int height)
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


// =============================================
// vctQtWidgetRotationDoubleRead
// =============================================
vctQtWidgetRotationDoubleRead::vctQtWidgetRotationDoubleRead(const DisplayModeType displayMode):
    DisplayMode(UNDEFINED_WIDGET),
    CurrentWidget(0),
    mRevoluteFactor(cmn180_PI)
{
    // Matrix Groupbox
    MatrixWidget = new vctQtWidgetDynamicMatrixDoubleRead();
    MatrixWidget->setMinimumSize(80, 80);
    MatrixWidget->resize(MatrixWidget->sizeHint());

    // Axis Angle Groupbox
    AxisWidget = new vctQtWidgetDynamicVectorDoubleRead();
    AngleWidget = new vctQtWidgetDynamicVectorDoubleRead();

    QGridLayout * axisAngleLayout = new QGridLayout;
    axisAngleLayout->setSpacing(1);
    axisAngleLayout->setContentsMargins(0, 0, 0, 0);
    axisAngleLayout->addWidget(new QLabel("Axis"), 0, 0, 1, 1);
    axisAngleLayout->addWidget(AxisWidget, 0, 1, 1, 1);
    axisAngleLayout->addWidget(new QLabel("Angle"), 1, 0, 1, 1);
    axisAngleLayout->addWidget(AngleWidget, 1, 1, 1, 1);

    AxisAngleWidget = new QWidget();
    AxisAngleWidget->setLayout(axisAngleLayout);

    // Quaternion Groupbox
    QuaternionWidget = new vctQtWidgetDynamicVectorDoubleRead();

    // Euler ZYZ Groupbox
    EulerZYZWidget = new vctQtWidgetDynamicVectorDoubleRead();

    // Euler ZYX Groupbox
    EulerZYXWidget = new vctQtWidgetDynamicVectorDoubleRead();

    // Visualization Groupbox
    OpenGLWidget = new vctQtWidgetRotationOpenGL();
    OpenGLWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    Layout = new QVBoxLayout;
    Layout->setSpacing(1);
    Layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(Layout);
    this->setWindowTitle("vctQtWidgetRotationDoubleRead");

    // Set display mode
    SetDisplayMode(displayMode);

    // Contextual menu
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));
    setToolTip(QString("right click for menu"));
}

void vctQtWidgetRotationDoubleRead::ShowContextMenu(const QPoint & pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    QMenu menu;
    QAction * matrix = new QAction("Matrix", this);
    QAction * axisAngle = new QAction("Axis Angle", this);
    QAction * quaternion = new QAction("Quaternion", this);
    QAction * eulerZYZ = new QAction("Euler ZYZ", this);
    QAction * eulerZYX = new QAction("Euler ZYX", this);
    QAction * openGL = new QAction("3D", this);
    if (DisplayMode != MATRIX_WIDGET) {
        menu.addAction(matrix);
    }
    if (DisplayMode != AXIS_ANGLE_WIDGET) {
        menu.addAction(axisAngle);
    }
    if (DisplayMode != QUATERNION_WIDGET) {
        menu.addAction(quaternion);
    }
    if (DisplayMode != EULERZYZ_WIDGET) {
        menu.addAction(eulerZYZ);
    }
    if (DisplayMode != EULERZYX_WIDGET) {
        menu.addAction(eulerZYX);
    }
    if (DisplayMode != OPENGL_WIDGET) {
        menu.addAction(openGL);
    }

    QAction * selectedItem = menu.exec(globalPos);
    if (selectedItem) {
        if (selectedItem == matrix) {
            SetDisplayMode(MATRIX_WIDGET);
        } else if (selectedItem == axisAngle) {
            SetDisplayMode(AXIS_ANGLE_WIDGET);
        } else if (selectedItem == quaternion) {
            SetDisplayMode(QUATERNION_WIDGET);
        } else if (selectedItem == eulerZYZ) {
            SetDisplayMode(EULERZYZ_WIDGET);
        } else if (selectedItem == eulerZYX) {
            SetDisplayMode(EULERZYX_WIDGET);
        } else if (selectedItem == openGL) {
            SetDisplayMode(OPENGL_WIDGET);
        }
    }
}

void vctQtWidgetRotationDoubleRead::UpdateCurrentWidget(void)
{
    vctAxAnRot3 rotAxAn;
    vctQuatRot3 rotQuat;
    vctEulerZYZRotation3 rotEulerZYZ;
    vctEulerZYXRotation3 rotEulerZYX;
    // compute the value based on the internal Rotation
    switch (DisplayMode) {
    case MATRIX_WIDGET:
        MatrixWidget->SetValue(vctDoubleMat(this->Rotation));
        break;
    case AXIS_ANGLE_WIDGET:
        rotAxAn.FromRaw(Rotation);
        AxisWidget->SetValue(vctDoubleVec(rotAxAn.Axis()));
        AngleWidget->SetValue(vctDoubleVec(1, rotAxAn.Angle() * mRevoluteFactor));
        break;
    case QUATERNION_WIDGET:
        rotQuat.FromRaw(Rotation);
        QuaternionWidget->SetValue(vctDoubleVec(rotQuat));
        break;
    case OPENGL_WIDGET:
        OpenGLWidget->SetValue(Rotation);
        break;
    case EULERZYZ_WIDGET:
        rotEulerZYZ.FromRaw(Rotation);
        EulerZYZWidget->SetValue(vctDoubleVec(rotEulerZYZ.GetAngles() * mRevoluteFactor));
        break;
    case EULERZYX_WIDGET:
        rotEulerZYX.FromRaw(Rotation);
        EulerZYXWidget->SetValue(vctDoubleVec(rotEulerZYX.GetAngles() * mRevoluteFactor));
        break;
    default:
        break;
    }
}

void vctQtWidgetRotationDoubleRead::SetDisplayMode(const DisplayModeType displayMode)
{
    // should never allow anyone to use undefined
    if (displayMode == UNDEFINED_WIDGET) {
        return;
    }

    // if the mode is unchanged, nothing to do
    if (displayMode == this->DisplayMode) {
        return;
    }

    // set the new display mode
    this->DisplayMode = displayMode;

    // mostly for initialization, there was no widget defined prior this call
    if (CurrentWidget) {
        Layout->removeWidget(CurrentWidget);
        CurrentWidget->hide();
    }

    // set the new current widget, these have been created in the ctor
    switch (displayMode) {
    case MATRIX_WIDGET:
        CurrentWidget = MatrixWidget;
        break;
    case AXIS_ANGLE_WIDGET:
        CurrentWidget = AxisAngleWidget;
        break;
    case QUATERNION_WIDGET:
        CurrentWidget = QuaternionWidget;
        break;
    case EULERZYZ_WIDGET:
        CurrentWidget = EulerZYZWidget;
        break;
    case EULERZYX_WIDGET:
        CurrentWidget = EulerZYXWidget;
        break;
    case OPENGL_WIDGET:
        CurrentWidget = OpenGLWidget;
        break;
    default:
        break;
    }
    Layout->addWidget(CurrentWidget);
    UpdateCurrentWidget();
    CurrentWidget->show();
    repaint();
}
