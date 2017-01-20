/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):
  Created on: 2016-01-20

  (C) Copyright 2016-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctForceTorqueQtWidget_h
#define _vctForceTorqueQtWidget_h

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctQtForwardDeclarations.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

#include <QWidget>
#include <QtOpenGL>

class QWidget;
class QVBoxLayout;

// Always include last
#include <cisstVector/vctExportQt.h>

/*!
  Widget to visualize rotation using 3 axes in OpenGL
  \todo use rotation matrix to rotate axis instead of euler angles
  \todo use a GL list to create the axes once and re-use later
  \todo remove reference frame?
*/
class CISST_EXPORT vctForceTorqueQtWidgetOpenGL: public QGLWidget
{
    Q_OBJECT;

public:
    vctForceTorqueQtWidgetOpenGL(void);
    inline ~vctForceTorqueQtWidgetOpenGL(void) {};

    void SetValue(const vct3 & force, const vct3 & torque);

protected:
    void initializeGL(void) {};
    void paintGL(void) {};
    void resizeGL(int width, int height) {};
    void draw3DAxis(const double scale) {};
    vct3 orientation; // should be replaced by rotation matrix using column-first storage order, isn't OpenGL Fortran like?
};


/*! Qt Widget to display a rotation using cisstVector. */
class CISST_EXPORT vctForceTorqueQtWidget: public QWidget
{
    Q_OBJECT;

 public:
    /*! Possible display modes.  See SetDisplayMode method.  Please
      note that UNDEFINED_WIDGET should never be used. */
    typedef enum {UNDEFINED_WIDGET,
                  VECTOR_WIDGET, // text display using two vectors of 3 elements + norm
                  PLOT_2D_WIDGET,   // 2D plot
                  PLOT_3D_WIDGET  // 3D display
    } DisplayModeType;

    /*! Constructor.  Default display mode is rotation matrix.  See
      also SetDisplayMode. */
    vctForceTorqueQtWidget(const DisplayModeType displayMode = VECTOR_WIDGET);

    inline ~vctForceTorqueQtWidget(void) {};

    /*! Set the rotation value to be displayed.  This method assumes
      the rotation matrix is valid, i.e. normalized and will not
      perform any check or normalization. */
    void SetValue(const vct3 & force,
                  const vct3 & torque) {
        this->Force.Assign(force);
        this->Torque.Assign(torque);
        this->UpdateCurrentWidget();
    }

    /*! Set the display mode, i.e. the widget used to represent the
      rotation.  Options are rotation matrix, axis and angle, quaternion
      (displayed in order x, y, z, w), Euler angles (ZYZ or ZYX, in degrees),
      and 3D OpenGL based using red, green and blue axes.
      Please note that the display mode UNDEFINED_WIDGET will be silently ignored. */
    void SetDisplayMode(const DisplayModeType displayMode);

 protected slots:
    /*! Contextual menu showed when the user right clicks on the widget */
    void ShowContextMenu(const QPoint & position);

 protected:
    /*! Current display mode, somewhat redundant with the
      CurrentWidget pointer.  The current widget should always be
      set by changing the DisplayMode. */
    DisplayModeType DisplayMode;

    /*! Update the content of the current widget.  This method is
      called when the user provides a new rotation with SetValue or
      when the widget used to display the rotation is changed using
      SetDisplayMode. */
    void UpdateCurrentWidget(void);

    /*! Internal representation for the rotation to display. */
    vct3 Force, Torque;

    // Vector widget
    vctQtWidgetDynamicVectorDoubleRead * ForceWidget;
    vctQtWidgetDynamicVectorDoubleRead * TorqueWidget;
    QWidget * ForceTorqueWidget;

#if 0 // Anton

    // replace what is below by widgets for 2D plot and 3D

    // Quaternion
    vctQtWidgetDynamicVectorDoubleRead * QuaternionWidget;

    // Euler Angles (in degrees)
    vctQtWidgetDynamicVectorDoubleRead * EulerZYZWidget;
    vctQtWidgetDynamicVectorDoubleRead * EulerZYXWidget;

    // Visualization
    vctQtWidgetRotationOpenGL * OpenGLWidget;
#endif

    // current widget
    QWidget * CurrentWidget;
    QVBoxLayout * Layout;
};

#endif  // _vctForceTorqueQtWidget_h
