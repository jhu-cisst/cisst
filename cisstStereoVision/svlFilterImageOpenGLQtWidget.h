/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Marcin Balicki
  Created on: 2011-02-18

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageOpenGLQtWidget_h
#define _svlFilterImageOpenGLQtWidget_h

#include <QGLWidget>
#include <cisstStereoVision/svlFilterImageOpenGL.h>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>

//! this class is useful for embedding an svl stream inside a qt widget
class CISST_EXPORT svlFilterImageOpenGLQtWidget: public QGLWidget, public svlFilterImageOpenGL
{
    Q_OBJECT

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    svlFilterImageOpenGLQtWidget(QWidget *parent = 0);
    ~svlFilterImageOpenGLQtWidget();
    void SetEnableToolTip(bool enabled) { ToolTipEnabled = enabled;};

 protected:
    // Qt derived methods
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    // mouse events
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

    // Method to trigger rendering once the svl filter has pushed the
    // data to OpenGL ready structures.
    void PostProcess(void);

 private:
    QPoint LastPosition;
    bool ToolTipEnabled;

 signals:
    void QSignalUpdateGL();

 public slots:
    void QSlotSwapRGB();

};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageOpenGLQtWidget)

#endif // _svlFilterImageOpenGLQtWidget_h
