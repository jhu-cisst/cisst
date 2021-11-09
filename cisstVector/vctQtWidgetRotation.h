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


#ifndef _vctQtWidgetRotation_h
#define _vctQtWidgetRotation_h

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctForwardDeclarationsQt.h>
#include <cisstVector/vctTransformationTypes.h>

class QVBoxLayout;

// Always include last
#include <cisstVector/vctExportQt.h>


/*!
  Widget to visualize rotation using 3 axes in OpenGL
  \todo use rotation matrix to rotate axis instead of euler angles
  \todo use a GL list to create the axes once and re-use later
  \todo remove reference frame?
*/
class CISST_EXPORT vctQtWidgetRotationOpenGL: public vctQtOpenGLBaseWidget
{
    Q_OBJECT;

public:
    vctQtWidgetRotationOpenGL(void);
    inline ~vctQtWidgetRotationOpenGL(void) {};

    void SetValue(const vctMatRot3 & rotation);

protected:
    void ResetOrientation(void);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);

    void initializeGL(void);
    void paintGL(void);
    void resizeGL(int width, int height);

    vct3 mRotation;
    vctQuatRot3 mCurrentOrientation, mDeltaOrientation;
    vctInt2 mStartMousePosition;
};


/*! Qt Widget to display a rotation using cisstVector. */
class CISST_EXPORT vctQtWidgetRotationDoubleRead: public QWidget
{
    Q_OBJECT;

 public:
    /*! Possible display modes.  See SetDisplayMode method.  Please
      note that UNDEFINED_WIDGET should never be used. */
    typedef enum {UNDEFINED_WIDGET, MATRIX_WIDGET, AXIS_ANGLE_WIDGET, QUATERNION_WIDGET,
                  EULERZYZ_WIDGET, EULERZYX_WIDGET, OPENGL_WIDGET} DisplayModeType;

    /*! Constructor.  Default display mode is rotation matrix.  See
      also SetDisplayMode. */
    vctQtWidgetRotationDoubleRead(const DisplayModeType displayMode = MATRIX_WIDGET);

    inline ~vctQtWidgetRotationDoubleRead(void) {};

    /*! Set the rotation value to be displayed.  This method assumes
      the rotation matrix is valid, i.e. normalized and will not
      perform any check or normalization. */
    template <class _containerType>
    void SetValue(const vctMatrixRotation3ConstBase<_containerType> & rotation) {
        this->Rotation.FromRaw(rotation);
        this->UpdateCurrentWidget();
    }

    /*! Set the display mode, i.e. the widget used to represent the
      rotation.  Options are rotation matrix, axis and angle, quaternion
      (displayed in order x, y, z, w), Euler angles (ZYZ or ZYX, in degrees),
      and 3D OpenGL based using red, green and blue axes.
      Please note that the display mode UNDEFINED_WIDGET will be silently ignored. */
    void SetDisplayMode(const DisplayModeType displayMode);

    inline void SetPrismaticRevoluteFactors(const double & CMN_UNUSED(prismatic), const double & revolute) {
        mRevoluteFactor = revolute;
        this->UpdateCurrentWidget();
    }

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
    vctMatRot3 Rotation;

    // Matrix
    vctQtWidgetDynamicMatrixDoubleRead * MatrixWidget;

    // Axis Angle
    vctQtWidgetDynamicVectorDoubleRead * AxisWidget;
    vctQtWidgetDynamicVectorDoubleRead * AngleWidget;
    QWidget * AxisAngleWidget;

    // Quaternion
    vctQtWidgetDynamicVectorDoubleRead * QuaternionWidget;

    // Euler Angles (in degrees)
    vctQtWidgetDynamicVectorDoubleRead * EulerZYZWidget;
    vctQtWidgetDynamicVectorDoubleRead * EulerZYXWidget;

    // Visualization
    vctQtWidgetRotationOpenGL * OpenGLWidget;

    // current widget
    QWidget * CurrentWidget;
    QVBoxLayout * Layout;

    // conversion factor
    double mRevoluteFactor;
};

#endif // _vctQtWidgetRotation_h
