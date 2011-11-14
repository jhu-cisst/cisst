/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#include "winQt4OpenGLStereo.h"
#include <cisstStereoVision/svlQtObjectFactory.h>
#include <cisstStereoVision/svlBufferSample.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

#include <QtGui>

#if (CISST_OS == CISST_DARWIN)
    #include <OpenGL/gl.h>

    #define _RGB_VERSION_   GL_BGR
#else
    #include <GL/gl.h>

    #define _RGB_VERSION_   GL_RGB
#endif


/**************************************/
/*** svlWidgetQt4OpenGLStereo class ***/
/**************************************/

svlWidgetQt4OpenGLStereo::svlWidgetQt4OpenGLStereo(QWidget* parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer |
                        QGL::DepthBuffer |
                        QGL::Rgba |
                        QGL::StencilBuffer |
                        QGL::AlphaChannel |
                        QGL::StereoBuffers), parent),
    Manager(0),
    ImageBuffer(0),
    LatestImage(0),
    WindowWidth(0),
    WindowHeight(0)
{
    QObject::connect(this, SIGNAL(QSignalUpdateGL()), this, SLOT(updateGL()));
}

svlWidgetQt4OpenGLStereo::~svlWidgetQt4OpenGLStereo()
{
    Destroy();
}

bool svlWidgetQt4OpenGLStereo::Create(svlBufferSample *imagebuffer,
                                      unsigned int width, unsigned int height,
                                      svlWindowManagerQt4OpenGLStereo* manager)
{
    if (!imagebuffer || width == 0 || height == 0) return false;
    Destroy();

    ImageBuffer  = imagebuffer;
    WindowWidth  = width;
    WindowHeight = height;

    this->resize(WindowWidth, WindowHeight);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);

    Manager = manager;

    return true;
}

void svlWidgetQt4OpenGLStereo::Destroy()
{
    this->close();
}

void svlWidgetQt4OpenGLStereo::UpdateImage()
{
    if (!ImageBuffer) return;
    svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(ImageBuffer->Pull(false));
    if (!img) return;
    LatestImage = img;
    emit QSignalUpdateGL();
}

void svlWidgetQt4OpenGLStereo::initializeGL()
{
    glEnable(GL_NORMALIZE);
    glEnable(GL_STEREO);
    CheckGLError(__FUNCTION__);
    TextureID[0] = TextureID[1] = 0;
}

void svlWidgetQt4OpenGLStereo::paintGL()
{
    if (!LatestImage) return;

    for (unsigned int i = 0; i < 2; i ++) {

        std::cerr << (int)(format().stereo()) << std::endl;
    	glDeleteTextures(1, &(TextureID[i]));
        glGenTextures(1, &(TextureID[i]));

        glBindTexture(GL_TEXTURE_2D, TextureID[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, 3,
                     LatestImage->GetWidth(i), LatestImage->GetHeight(i),
                     0, _RGB_VERSION_, GL_UNSIGNED_BYTE,
                     LatestImage->GetUCharPointer(i));

        if (i) glDrawBuffer(GL_BACK_RIGHT);
        else   glDrawBuffer(GL_BACK_LEFT);

        glClearColor(0.0f, 0.6f, 0.0f, 0.0f); // provide a green background by default
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable (GL_TEXTURE_2D);

        glPushMatrix();
        glRotatef(-90.0, 0.0, 0.0, 1.0);
        glTranslatef(-WindowHeight, 0.0, 0.0);

        glBegin (GL_QUADS);
        {
            glTexCoord2f(0.0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(WindowHeight, 0.0, 0.0);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(WindowHeight, WindowWidth, 0.0);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(0.0, WindowWidth, 0.0);
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        CheckGLError(__FUNCTION__);
    }
}

void svlWidgetQt4OpenGLStereo::resizeGL(int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, WindowWidth, WindowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WindowWidth, 0.0, WindowHeight, 0.0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    CheckGLError(__FUNCTION__);
}

void svlWidgetQt4OpenGLStereo::mousePressEvent(QMouseEvent* event)
{
    QMetaObject::invokeMethod(Manager, "QSlotMousePress", Qt::AutoConnection,
                              Q_ARG(QMouseEvent*, event));
}

void svlWidgetQt4OpenGLStereo::mouseReleaseEvent(QMouseEvent* event)
{
    QMetaObject::invokeMethod(Manager, "QSlotMouseRelease", Qt::AutoConnection,
                              Q_ARG(QMouseEvent*, event));
}

void svlWidgetQt4OpenGLStereo::mouseMoveEvent(QMouseEvent* event)
{
    QMetaObject::invokeMethod(Manager, "QSlotMouseMove", Qt::AutoConnection,
                              Q_ARG(QMouseEvent*, event));
}

void svlWidgetQt4OpenGLStereo::keyPressEvent(QKeyEvent* event)
{
    QMetaObject::invokeMethod(Manager, "QSlotKeyPress", Qt::AutoConnection,
                              Q_ARG(QKeyEvent*, event));
}

void svlWidgetQt4OpenGLStereo::CheckGLError(const std::string & functionName)
{
    for (GLenum gl_error = glGetError(); gl_error; gl_error = glGetError()) {
        switch (gl_error) {
            case (GL_INVALID_ENUM):
                CMN_LOG_INIT_ERROR << "svlFilterImageOpenGL::CheckGLError - invalid OpenGL ENUM: "
                                   << gl_error << " for function " << functionName << std::endl;
            break;

            case (GL_INVALID_VALUE):
                CMN_LOG_INIT_ERROR << "svlFilterImageOpenGL::CheckGLError - invalid OpenGL VALUE: "
                                   << gl_error << " for function " << functionName << std::endl;
            break;

            case (GL_INVALID_OPERATION):
                CMN_LOG_INIT_ERROR << "svlFilterImageOpenGL::CheckGLError - invalid OpenGL OPERATION: "
                                   << gl_error << " for function " << functionName << std::endl;
                printf("%s: %u caught: Invalid OPERATION\n", __FUNCTION__, gl_error);
            break;

            case (GL_STACK_OVERFLOW):
                CMN_LOG_INIT_ERROR << "svlFilterImageOpenGL::CheckGLError - OpenGL GL_STACK_OVERFLOW: "
                                   << gl_error << " for function " << functionName << std::endl;
            break;

            case (GL_STACK_UNDERFLOW):
                CMN_LOG_INIT_ERROR << "svlFilterImageOpenGL::CheckGLError - OpenGL GL_STACK_UNDERFLOW: "
                                   << gl_error << " for function " << functionName << std::endl;
            break;

            case (GL_OUT_OF_MEMORY):
                CMN_LOG_INIT_ERROR << "svlFilterImageOpenGL::CheckGLError - OpenGL GL_OUT_OF_MEMORY: "
                                   << gl_error << " for function " << functionName << std::endl;
            break;

            default:
                CMN_LOG_INIT_ERROR << "svlFilterImageOpenGL::CheckGLError - invalid OpenGL something: "
                                   << gl_error << " for function " << functionName << std::endl;
        }
    }
}


/*********************************************/
/*** svlWindowManagerQt4OpenGLStereo class ***/
/*********************************************/

CMN_IMPLEMENT_SERVICES(svlWindowManagerQt4OpenGLStereo)

svlWindowManagerQt4OpenGLStereo::svlWindowManagerQt4OpenGLStereo() :
    QObject(),
    cmnGenericObject(),
    svlWindowManagerBase(),
    DestroySignal(0),
    ParentWidget(0),
    Window(0),
    ImageBuffer(0),
    LButtonDown(false),
    RButtonDown(false)
{
    DestroySignal = new osaThreadSignal;

    QGLFormat fmt;
    fmt.setStereo(true);
    QGLFormat::setDefaultFormat(fmt);
}

svlWindowManagerQt4OpenGLStereo::svlWindowManagerQt4OpenGLStereo(const svlWindowManagerQt4OpenGLStereo& CMN_UNUSED(other)) :
    QObject(),
    cmnGenericObject(),
    svlWindowManagerBase()
{
    // Public copy constructor is here only to override the private QObject copy constructor
}

svlWindowManagerQt4OpenGLStereo::~svlWindowManagerQt4OpenGLStereo()
{
    DestroyThreadSafe();
    delete DestroySignal;
}

svlWindowManagerQt4OpenGLStereo* svlWindowManagerQt4OpenGLStereo::New()
{
    svlWindowManagerQt4OpenGLStereo* instance = dynamic_cast<svlWindowManagerQt4OpenGLStereo*>(svlQtObjectFactory::Create("svlWindowManagerQt4OpenGLStereo"));
    if (instance) instance->SetNumberOfWindows(1);
    return instance;
}

void svlWindowManagerQt4OpenGLStereo::Delete()
{
    svlQtObjectFactory::Delete(this);
}

int svlWindowManagerQt4OpenGLStereo::DoModal(bool show, bool fullscreen)
{
    ImageBuffer = new svlBufferSample(svlTypeImageRGBStereo);

    argShow       = show;
    argFullscreen = fullscreen;
    QMetaObject::invokeMethod(this, "QSlotCreateWindow", Qt::BlockingQueuedConnection);

    // signal that initialization is done
    if (InitReadySignal) InitReadySignal->Raise();

    // Wait until windows need to be closed
    DestroySignal->Wait();

    QMetaObject::invokeMethod(this, "QSlotDestroyWindow", Qt::BlockingQueuedConnection);

    delete ImageBuffer;
    ImageBuffer = 0;

    return 0;
}

void svlWindowManagerQt4OpenGLStereo::Show(bool show, int CMN_UNUSED(winid))
{
    argShow  = show;
    QMetaObject::invokeMethod(this, "QSlotShow", Qt::BlockingQueuedConnection);
}

void svlWindowManagerQt4OpenGLStereo::SetImage(svlSampleImageRGBStereo* image)
{
    if (!ImageBuffer || !image) return;
    ImageBuffer->Push(image);
}

void svlWindowManagerQt4OpenGLStereo::SetImageBuffer(unsigned char* CMN_UNUSED(buffer),
                                                     unsigned int CMN_UNUSED(buffersize),
                                                     unsigned int CMN_UNUSED(winid))
{
    // Needs to be implemented for compatibility with baseclass
}

void svlWindowManagerQt4OpenGLStereo::DrawImages()
{
    if (Window) Window->UpdateImage();
}

void svlWindowManagerQt4OpenGLStereo::DestroyThreadSafe()
{
    DestroySignal->Raise();
}

void svlWindowManagerQt4OpenGLStereo::QSlotCreateWindow()
{
    QSlotDestroyWindow();

    Qt::WindowFlags style = Qt::Window;
    if (argFullscreen) style = Qt::FramelessWindowHint|Qt::WindowSystemMenuHint;
    ParentWidget = new svlParentWidgetQt4Stereo(style);
    ParentWidget->showFullScreen();
    ParentWidget->setWindowTitle(QString::fromStdString(Title));

    Window = new svlWidgetQt4OpenGLStereo(ParentWidget);
    Window->Create(ImageBuffer, Width[0], Height[0], this);
//    Window->showFullScreen();
    Window->show();

    ParentWidget->move(PosX[0], PosY[0]);

    if (argShow) {
        ParentWidget->show();
        ParentWidget->raise();
        ParentWidget->activateWindow();
    }
}

void svlWindowManagerQt4OpenGLStereo::QSlotDestroyWindow()
{
    delete Window;
    Window = 0;
}

void svlWindowManagerQt4OpenGLStereo::QSlotShow()
{
    if (ParentWidget) {
        if (argShow) ParentWidget->show();
        else ParentWidget->hide();
    }
}

void svlWindowManagerQt4OpenGLStereo::QSlotMousePress(QMouseEvent* event)
{
    SetMousePos(event->x(), event->y());

    if (event->buttons() & Qt::LeftButton) {
        LButtonDown = true;
        OnUserEvent(0, false, winInput_LBUTTONDOWN);
    }
    if (event->buttons() & Qt::RightButton) {
        RButtonDown = true;
        OnUserEvent(0, false, winInput_RBUTTONDOWN);
    }
}

void svlWindowManagerQt4OpenGLStereo::QSlotMouseRelease(QMouseEvent* event)
{
    SetMousePos(event->x(), event->y());

    if (LButtonDown && ~(event->buttons() & Qt::LeftButton)) {
        LButtonDown = false;
        OnUserEvent(0, false, winInput_LBUTTONUP);
    }
    if (RButtonDown && ~(event->buttons() & Qt::RightButton)) {
        RButtonDown = false;
        OnUserEvent(0, false, winInput_RBUTTONUP);
    }
}

void svlWindowManagerQt4OpenGLStereo::QSlotMouseMove(QMouseEvent* event)
{
    SetMousePos(event->x(), event->y());
    OnUserEvent(0, false, winInput_MOUSEMOVE);
}

void svlWindowManagerQt4OpenGLStereo::QSlotKeyPress(QKeyEvent* event)
{
    std::string str = event->text().toStdString();

    unsigned int code;

    if (str.empty()) {
    // Special keys

        code = event->key();
        if (code >= Qt::Key_F1 && code <= Qt::Key_F12) { // F1-F12
            OnUserEvent(0, false, winInput_KEY_F1 + (code - Qt::Key_F1));
            return;
        }
        switch (code) {
            case Qt::Key_PageUp:
                OnUserEvent(0, false, winInput_KEY_PAGEUP);
            break;

            case Qt::Key_PageDown:
                OnUserEvent(0, false, winInput_KEY_PAGEDOWN);
            break;

            case Qt::Key_Home:
                OnUserEvent(0, false, winInput_KEY_HOME);
            break;

            case Qt::Key_End:
                OnUserEvent(0, false, winInput_KEY_END);
            break;

            case Qt::Key_Insert:
                OnUserEvent(0, false, winInput_KEY_INSERT);
            break;

            case Qt::Key_Delete:
                OnUserEvent(0, false, winInput_KEY_DELETE);
            break;

            case Qt::Key_Left:
                OnUserEvent(0, false, winInput_KEY_LEFT);
            break;

            case Qt::Key_Right:
                OnUserEvent(0, false, winInput_KEY_RIGHT);
            break;

            case Qt::Key_Up:
                OnUserEvent(0, false, winInput_KEY_UP);
            break;

            case Qt::Key_Down:
                OnUserEvent(0, false, winInput_KEY_DOWN);
            break;
        }
    }
    else {
    // ASCII codes

        code = str.c_str()[0];
        if (code >= 48 && code <= 57) { // ascii numbers
            OnUserEvent(0, true, code);
            return;
        }
        if (code >= 97 && code <= 122) { // ascii letters
            OnUserEvent(0, true, code);
            return;
        }
        if (code == 13 ||
            code == 32) { // special characters with correct ascii code
            OnUserEvent(0, true, code);
            return;
        }
    }
}

