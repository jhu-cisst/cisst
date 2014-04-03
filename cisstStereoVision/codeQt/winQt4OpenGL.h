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


#ifndef _winQt4OpenGL_h
#define _winQt4OpenGL_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstStereoVision/svlWindowManagerBase.h>

#include <QGLWidget>
#include <QCloseEvent>

// Forward declarations
class svlBufferImage;
class osaThreadSignal;
class svlWindowManagerQt4OpenGL;

// Always include last
#include <cisstStereoVision/svlExportQt.h>

class CISST_EXPORT svlWidgetQt4OpenGL : public QGLWidget
{
    Q_OBJECT

public:
    svlWidgetQt4OpenGL(QWidget* parent);
    virtual ~svlWidgetQt4OpenGL();

public:
    bool Create(svlBufferImage* imagebuffer, svlWindowManagerQt4OpenGL* manager, unsigned int winid);
    void Destroy();
    void UpdateImage();
    enum ByteOrder {RGB_Order, BGR_Order};
    void SetByteOrderRGB(ByteOrder &order);

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
    svlWindowManagerQt4OpenGL* Manager;
    unsigned int WinID;

    svlBufferImage* ImageBuffer;
    svlImageRGB* LatestImage;

    int WindowWidth;
    int WindowHeight;
    GLint ByteOrderVersion;

signals:
    void QSignalUpdateGL();
};


class CISST_EXPORT svlParentWidgetQt4 : public QWidget
{
    Q_OBJECT

public:
    svlParentWidgetQt4(Qt::WindowFlags flags) : QWidget(0, flags) {}

protected:
    void closeEvent(QCloseEvent* event) { event->ignore(); }
};


class CISST_EXPORT svlWindowManagerQt4OpenGL : public QObject, public cmnGenericObject, public svlWindowManagerBase
{
    Q_OBJECT
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

public:
    svlWindowManagerQt4OpenGL();
    svlWindowManagerQt4OpenGL(const svlWindowManagerQt4OpenGL& other);
    ~svlWindowManagerQt4OpenGL();

    static svlWindowManagerQt4OpenGL* New(unsigned int numofwins);
    void Delete();

    int DoModal(bool show, bool fullscreen);
    void Show(bool show, int winid);
    void SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid);
    void DrawImages();
    void DestroyThreadSafe();


protected slots:
    void QSlotCreateWindows();
    void QSlotDestroyWindows();
    void QSlotShow();
    void QSlotMousePress(QMouseEvent* event, unsigned int winid);
    void QSlotMouseRelease(QMouseEvent* event, unsigned int winid);
    void QSlotMouseMove(QMouseEvent* event, unsigned int winid);
    void QSlotKeyPress(QKeyEvent* event, unsigned int winid);

private:
    bool argShow;
    bool argFullscreen;
    int  argWinID;
    int  retSuccess;

    osaThreadSignal* DestroySignal;
    vctDynamicVector<QWidget*> ParentWidgets;
    vctDynamicVector<svlWidgetQt4OpenGL*> Windows;
    vctDynamicVector<svlBufferImage*> ImageBuffers;
    bool LButtonDown;
    bool RButtonDown;

};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlWindowManagerQt4OpenGL)

#endif // _winQt4OpenGL_h
