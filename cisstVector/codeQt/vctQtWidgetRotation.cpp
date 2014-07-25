/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Zihan Chen
  Created on: 2013-03-20

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

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
    orientation.SetAll(0.0);
    setMinimumHeight(100);
    setMinimumWidth(100);
    setContentsMargins(0, 0, 0, 0);
}

void vctQtWidgetRotationOpenGL::SetValue(const vctMatRot3 & rot)
{
    vctEulerZYXRotation3 rotEuler;
    rotEuler.From(rot);
    orientation.X() = rotEuler.gamma() * 180.0 / cmnPI;  // x
    orientation.Y() = rotEuler.beta() * 180.0 / cmnPI;   // y
    orientation.Z() = rotEuler.alpha() * 180.0 / cmnPI;  // z

    // update GL display
    updateGL();
}


void vctQtWidgetRotationOpenGL::initializeGL(void)
{
    qglClearColor(Qt::lightGray);
    glShadeModel(GL_SMOOTH);
}

void vctQtWidgetRotationOpenGL::paintGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw reference coordinate frame here
    glPushMatrix();
    glTranslatef(0.4f, 0.4f, -5.0f);
    glRotatef(-30.0, 0.0, 1.0, 0.0);
    draw3DAxis(0.05);
    glPopMatrix();

    // gl transformation here
    // x+:left  y+:up   z+: point out screen
    glTranslatef(0.0f, 0.0f, -10.0f);

    // orientation
    glRotatef(orientation.Z(), 0.0, 0.0, 1.0);
    glRotatef(orientation.Y(), 0.0, 1.0, 0.0);
    glRotatef(orientation.X(), 1.0, 0.0, 0.0);

    draw3DAxis(0.4);

    glFlush();
}

void vctQtWidgetRotationOpenGL::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void vctQtWidgetRotationOpenGL::draw3DAxis(const double scale)
{
    // draw
    glPushMatrix();

    // disable lighting
    glDisable(GL_LIGHTING);

    // set line width
    glLineWidth(scale * 10.0);

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



// =============================================
// vctQtWidgetRotationDoubleRead
// =============================================
vctQtWidgetRotationDoubleRead::vctQtWidgetRotationDoubleRead(const DisplayModeType displayMode):
    DisplayMode(UNDEFINED_WIDGET),
    CurrentWidget(0)
{
    // Matrix Groupbox
    MatrixWidget = new vctQtWidgetDynamicMatrixDoubleRead();
    MatrixWidget->setMinimumSize(80, 80);
    MatrixWidget->resize(MatrixWidget->sizeHint());

    // Axis Angle Groupbox
    AxisWidget = new vctQtWidgetDynamicVectorDoubleRead();
    AngleWidget = new vctQtWidgetDynamicVectorDoubleRead();

    QGridLayout * axisAngleLayout = new QGridLayout;
    axisAngleLayout->setSpacing(0);
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
    Layout->setSpacing(0);
    this->setLayout(Layout);
    this->setContentsMargins(0, 0, 0, 0);
    this->setWindowTitle("vctQtWidgetRotationDoubleRead");

    // Set display mode
    SetDisplayMode(displayMode);

    // myWidget is any QWidget-derived class
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));
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
    menu.addAction(matrix);
    menu.addAction(axisAngle);
    menu.addAction(quaternion);
    menu.addAction(eulerZYZ);
    menu.addAction(eulerZYX);
    menu.addAction(openGL);

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
        AngleWidget->SetValue(vctDoubleVec(1, rotAxAn.Angle() * cmn180_PI));
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
        EulerZYZWidget->SetValue(vctDoubleVec(rotEulerZYZ.GetAnglesInDegrees()));
        break;
    case EULERZYX_WIDGET:
        rotEulerZYX.FromRaw(Rotation);
        EulerZYXWidget->SetValue(vctDoubleVec(rotEulerZYX.GetAnglesInDegrees()));
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
