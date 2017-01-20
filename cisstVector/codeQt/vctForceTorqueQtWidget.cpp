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

#include <cisstVector/vctForceTorqueQtWidget.h>

// all these widgets should be replaced to use static vectors/matrices
#include <cisstVector/vctQtWidgetDynamicVector.h>
#include <cisstVector/vctDynamicVectorTypes.h>

#if 0

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

#endif


vctForceTorqueQtWidget::vctForceTorqueQtWidget(const DisplayModeType displayMode):
    DisplayMode(UNDEFINED_WIDGET),
    CurrentWidget(0)
{
    // Force Torque Groupbox
    ForceWidget = new vctQtWidgetDynamicVectorDoubleRead();
    TorqueWidget = new vctQtWidgetDynamicVectorDoubleRead();

    QGridLayout * forceTorqueLayout = new QGridLayout;
    forceTorqueLayout->setSpacing(0);
    forceTorqueLayout->addWidget(new QLabel("Force"), 0, 0, 1, 1);
    forceTorqueLayout->addWidget(ForceWidget, 0, 1, 1, 1);
    forceTorqueLayout->addWidget(new QLabel("Torque"), 1, 0, 1, 1);
    forceTorqueLayout->addWidget(TorqueWidget, 1, 1, 1, 1);

    ForceTorqueWidget = new QWidget();
    ForceTorqueWidget->setLayout(forceTorqueLayout);

#if 0 // Anton, for 2D and 3D widgets
    
    // Visualization Groupbox
    OpenGLWidget = new vctQtWidgetRotationOpenGL();
    OpenGLWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
#endif
    
    Layout = new QVBoxLayout;
    Layout->setSpacing(0);
    this->setLayout(Layout);
    this->setContentsMargins(0, 0, 0, 0);
    this->setWindowTitle("vctForceTorqueQtWidget");

    // Set display mode
    SetDisplayMode(displayMode);

    // myWidget is any QWidget-derived class
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));
}

void vctForceTorqueQtWidget::ShowContextMenu(const QPoint & pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    QMenu menu;
    QAction * vector = new QAction("Text", this);
    QAction * plot2D = new QAction("2D", this);
    QAction * plot3D = new QAction("3D", this);
    menu.addAction(vector);
    menu.addAction(plot2D);
    menu.addAction(plot3D);

    QAction * selectedItem = menu.exec(globalPos);
    if (selectedItem) {
        if (selectedItem == vector) {
            SetDisplayMode(VECTOR_WIDGET);
        } else if (selectedItem == plot2D) {
            SetDisplayMode(PLOT_2D_WIDGET);
        } else if (selectedItem == plot3D) {
            SetDisplayMode(PLOT_3D_WIDGET);
        }
    }
}

void vctForceTorqueQtWidget::UpdateCurrentWidget(void)
{
    // compute the value based on the internal values
    switch (DisplayMode) {
    case VECTOR_WIDGET:
        ForceWidget->SetValue(this->Force);
        TorqueWidget->SetValue(this->Torque);
        break;
    case PLOT_2D_WIDGET:
        std::cerr << CMN_LOG_DETAILS << " to be implemented " << std::endl;
        break;
    case PLOT_3D_WIDGET:
        std::cerr << CMN_LOG_DETAILS << " to be implemented " << std::endl;
        break;
    default:
        break;
    }
}

void vctForceTorqueQtWidget::SetDisplayMode(const DisplayModeType displayMode)
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
    case VECTOR_WIDGET:
        CurrentWidget = ForceTorqueWidget;
        break;
    case PLOT_2D_WIDGET:
        std::cerr << CMN_LOG_DETAILS << " to be implemented " << std::endl;
        break;
    case PLOT_3D_WIDGET:
        std::cerr << CMN_LOG_DETAILS << " to be implemented " << std::endl;
        break;
    default:
        break;
    }
    Layout->addWidget(CurrentWidget);
    UpdateCurrentWidget();
    CurrentWidget->show();
    repaint();
}
