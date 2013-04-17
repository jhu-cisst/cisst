/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Zihan Chen
  Created on: 2013-03-20

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctQtWidgetRotation.h>


#include <cisstConfig.h>

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
    setMinimumHeight(200);
    setMinimumWidth(200);
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

void vctQtWidgetRotationOpenGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw reference coordinate frame here
    glPushMatrix();
    glTranslatef(0.4, 0.4, -5.0);
    glRotatef(-30.0, 0.0, 1.0, 0.0);
    draw3DAxis(0.05);
    glPopMatrix();

    // gl transformation here
    // x+:left  y+:up   z+: point out screen
    glTranslatef(0.0, 0.0, -10.0);

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

vctQtWidgetRotationDoubleRead::vctQtWidgetRotationDoubleRead(void)
{
    QLabel * label = new QLabel("&Display");
    combo = new QComboBox();
    combo->addItem("Matrix");
    combo->addItem("Axis-Angle");
    combo->addItem("Quaternion");
    combo->addItem("3D");
    label->setBuddy(combo);
    connect(combo, SIGNAL(activated(QString)),
            this, SLOT(slot_change_display_format(QString)));

    QHBoxLayout *typeLayout = new QHBoxLayout;
    typeLayout->addWidget(label);
    typeLayout->addWidget(combo);

    // Matrix Groupbox
    QVBoxLayout * matrixLayout = new QVBoxLayout;
    MatrixWidget = new vctQtWidgetDynamicMatrixDoubleRead();
    MatrixWidget->setMinimumSize(100, 100);
    MatrixWidget->resize(MatrixWidget->sizeHint());
    matrixLayout->addWidget(MatrixWidget);

    MatrixGroupBox = new QGroupBox("Matrix");
    MatrixGroupBox->setLayout(matrixLayout);

    // Axis Angle Groupbox
    AxisWidget = new vctQtWidgetDynamicVectorDoubleRead();
    AngleWidget = new vctQtWidgetDynamicVectorDoubleRead();

    QGridLayout * axisAngleLayout = new QGridLayout;
    axisAngleLayout->addWidget(new QLabel("Axis"), 0, 0, 1, 1);
    axisAngleLayout->addWidget(AxisWidget->GetWidget(), 0, 1, 1, 1);
    axisAngleLayout->addWidget(new QLabel("Angle"), 1, 0, 1, 1);
    axisAngleLayout->addWidget(AngleWidget->GetWidget(), 1, 1, 1, 1);

    AxisAngleGroupBox = new QGroupBox("Axis-Angle");
    AxisAngleGroupBox->setLayout(axisAngleLayout);

    // Quaternion Groupbox
    QVBoxLayout * quaternionLayout = new QVBoxLayout;
    QuaternionWidget = new vctQtWidgetDynamicVectorDoubleRead();
    quaternionLayout->addWidget(QuaternionWidget->GetWidget());

    QuaternionGroupBox = new QGroupBox("Quaternion");
    QuaternionGroupBox->setLayout(quaternionLayout);

    // Visualization Groupbox
    QVBoxLayout * openGLLayout = new QVBoxLayout;
    OpenGLWidget = new vctQtWidgetRotationOpenGL();
    openGLLayout->addWidget(OpenGLWidget);

    OpenGLGroupBox = new QGroupBox("3D");
    OpenGLGroupBox->setLayout(openGLLayout);

    // add to layout
    Layout = new QVBoxLayout;
    Layout->addLayout(typeLayout);
    Layout->addWidget(MatrixGroupBox);
    CurrentGroupBox = MatrixGroupBox;

    this->setLayout(Layout);
    this->setWindowTitle("vctQtWidgetRotationDoubleRead");
}

void vctQtWidgetRotationDoubleRead::UpdateCurrentWidget(void)
{
    // matrix
    MatrixWidget->SetValue(vctDoubleMat(this->Rotation));

    // axis angle
    vctAxAnRot3 rotAxAn;
    rotAxAn.FromRaw(Rotation);
    AxisWidget->SetValue(vctDoubleVec(rotAxAn.Axis()));
    AngleWidget->SetValue(vctDoubleVec(1, rotAxAn.Angle()));

    // quaternion
    vctQuatRot3 rotQuat;
    rotQuat.FromRaw(Rotation);
    QuaternionWidget->SetValue(vctDoubleVec(rotQuat));

    // visualization
    OpenGLWidget->SetValue(Rotation);
}

void vctQtWidgetRotationDoubleRead::slot_change_display_format(QString item)
{
    if (item == "Matrix") {
        SwitchDisplayFormat(MatrixGroupBox);
    } else if(item == "Axis-Angle") {
        SwitchDisplayFormat(AxisAngleGroupBox);
    } else if (item == "Quaternion") {
        SwitchDisplayFormat(QuaternionGroupBox);
    } else if(item == "3D") {
        SwitchDisplayFormat(OpenGLGroupBox);
    }
}

void vctQtWidgetRotationDoubleRead::SwitchDisplayFormat(QGroupBox * setBox)
{
    Layout->removeWidget(CurrentGroupBox);
    CurrentGroupBox->hide();

    CurrentGroupBox = setBox;
    Layout->addWidget(CurrentGroupBox);
    CurrentGroupBox->show();
    resize(sizeHint());
}
