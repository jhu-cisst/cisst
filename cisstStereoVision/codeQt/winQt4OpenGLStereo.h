/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifndef _winQt4OpenGLStereo_h
#define _winQt4OpenGLStereo_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstStereoVision/svlWindowManagerBase.h>

#include <QGLWidget>
#include <QCloseEvent>

// Always include last!
#include <cisstStereoVision/svlExportQt.h>

// Forward declarations
class svlBufferSample;
class osaThreadSignal;
class svlWindowManagerQt4OpenGLStereo;


class CISST_EXPORT svlWidgetQt4OpenGLStereo : public QGLWidget
{
    Q_OBJECT

public:
    svlWidgetQt4OpenGLStereo(QWidget* parent);
    virtual ~svlWidgetQt4OpenGLStereo();

public:
    bool Create(svlBufferSample* imagebuffer,
                unsigned int width, unsigned int height,
                svlWindowManagerQt4OpenGLStereo* manager);
    void Destroy();
    void UpdateImage();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

    void CheckGLError(const std::string & functionName);

private:
    svlWindowManagerQt4OpenGLStereo* Manager;

    svlBufferSample* ImageBuffer;
    svlSampleImageRGBStereo* LatestImage;

    unsigned int TextureID[2];
    int WindowWidth;
    int WindowHeight;

signals:
    void QSignalUpdateGL();
};


class CISST_EXPORT svlParentWidgetQt4Stereo : public QWidget
{
    Q_OBJECT

public:
    svlParentWidgetQt4Stereo(Qt::WindowFlags flags) : QWidget(0, flags) {}

protected:
    void closeEvent(QCloseEvent* event) { event->ignore(); }
};


class CISST_EXPORT svlWindowManagerQt4OpenGLStereo : public QObject, public cmnGenericObject, public svlWindowManagerBase
{
    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

public:
    svlWindowManagerQt4OpenGLStereo();
    svlWindowManagerQt4OpenGLStereo(const svlWindowManagerQt4OpenGLStereo& other);
    ~svlWindowManagerQt4OpenGLStereo();

    static svlWindowManagerQt4OpenGLStereo* New();
    void Delete();

    int DoModal(bool show, bool fullscreen);
    void Show(bool show, int winid);
    void SetImage(svlSampleImageRGBStereo* image);
    void SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid);
    void DrawImages();
    void DestroyThreadSafe();


protected slots:
    void QSlotCreateWindow();
    void QSlotDestroyWindow();
    void QSlotShow();
    void QSlotMousePress(QMouseEvent* event);
    void QSlotMouseRelease(QMouseEvent* event);
    void QSlotMouseMove(QMouseEvent* event);
    void QSlotKeyPress(QKeyEvent* event);

private:
    bool argShow;
    bool argFullscreen;
    int  retSuccess;

    osaThreadSignal* DestroySignal;
    QWidget* ParentWidget;
    svlWidgetQt4OpenGLStereo* Window;
    svlBufferSample* ImageBuffer;
    bool LButtonDown;
    bool RButtonDown;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlWindowManagerQt4OpenGLStereo)

#endif // _winQt4OpenGLStereo_h

