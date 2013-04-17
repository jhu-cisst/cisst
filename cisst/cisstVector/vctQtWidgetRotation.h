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


#ifndef _vctQtWidgetRotation_h
#define _vctQtWidgetRotation_h

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctQtForwardDeclarations.h>
#include <cisstVector/vctTransformationTypes.h>

#include <QWidget>
#include <QtOpenGL>

class QGroupBox;
class QComboBox;
class QVBoxLayout;

// Maybe we should make this class public after all
class vctQtWidgetRotationOpenGL;

// Always include last
#include <cisstVector/vctExportQt.h>


class CISST_EXPORT vctQtWidgetRotationOpenGL: public QGLWidget
{
    Q_OBJECT;

public:
    vctQtWidgetRotationOpenGL(void);
    inline ~vctQtWidgetRotationOpenGL(void) {};

    void SetValue(const vctMatRot3 & rotation);

protected:
   void initializeGL(void);
   void paintGL(void);
   void resizeGL(int width, int height);

   void draw3DAxis(const double scale);

   // protected thing here
   vct3 orientation;
};


class CISST_EXPORT vctQtWidgetRotationDoubleRead: public QWidget
{
    Q_OBJECT;

 public:
    vctQtWidgetRotationDoubleRead(void);
    inline ~vctQtWidgetRotationDoubleRead(void) {};

    //! set value
    template <class _containerType>
    void SetValue(const vctMatrixRotation3ConstBase<_containerType> & rotation) {
        this->Rotation.FromRaw(rotation);
        this->UpdateCurrentWidget();
    }

 protected slots:
    void slot_change_display_format(QString item);

 protected:

    void UpdateCurrentWidget(void);
    void SwitchDisplayFormat(QGroupBox * setBox);

    vctMatRot3 Rotation;
    QComboBox * combo;

    // Matrix
    vctQtWidgetDynamicMatrixDoubleRead * MatrixWidget;
    QGroupBox * MatrixGroupBox;

    // Axis Angle
    vctQtWidgetDynamicVectorDoubleRead * AxisWidget;
    vctQtWidgetDynamicVectorDoubleRead * AngleWidget;
    QGroupBox * AxisAngleGroupBox;

    // Quaternion
    vctQtWidgetDynamicVectorDoubleRead * QuaternionWidget;
    QGroupBox * QuaternionGroupBox;

    // Visualization
    vctQtWidgetRotationOpenGL * OpenGLWidget;
    QGroupBox * OpenGLGroupBox;

    // current box
    QGroupBox * CurrentGroupBox;
    QVBoxLayout * Layout;
};

#endif // _vctQtWidgetRotation_h
