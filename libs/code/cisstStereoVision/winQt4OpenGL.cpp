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

#include "winQt4OpenGL.h"
#include <cisstStereoVision/svlQtObjectFactory.h>
#include <cisstStereoVision/svlBufferImage.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

#include <QtGui>

#if (CISST_OS == CISST_DARWIN)
    #include <OpenGL/gl.h>

    #define _RGB_VERSION_   GL_BGR
#else
    #include <GL/gl.h>

    #define _RGB_VERSION_   GL_RGB
#endif


/********************************/
/*** svlWidgetQt4OpenGL class ***/
/********************************/

svlWidgetQt4OpenGL::svlWidgetQt4OpenGL(QWidget* parent) :
    QGLWidget(parent),
    ImageBuffer(0),
    LatestImage(0),
    WindowWidth(0),
    WindowHeight(0)
{
    QObject::connect(this, SIGNAL(QSignalUpdateGL()), this, SLOT(updateGL()));
}

svlWidgetQt4OpenGL::~svlWidgetQt4OpenGL()
{
    Destroy();
}

bool svlWidgetQt4OpenGL::Create(svlBufferImage *imagebuffer)
{
    if (!imagebuffer) return false;

    Destroy();

    WindowWidth = imagebuffer->GetWidth();
    WindowHeight = imagebuffer->GetHeight();
    if (WindowWidth < 1 || WindowHeight < 1) return false;

    ImageBuffer = imagebuffer;

    this->resize(WindowWidth, WindowHeight);

    return true;
}

void svlWidgetQt4OpenGL::Destroy()
{
    this->close();
}

void svlWidgetQt4OpenGL::UpdateImage()
{
    if (!ImageBuffer) return;
    LatestImage = ImageBuffer->Pull(false);
    emit QSignalUpdateGL();
}

void svlWidgetQt4OpenGL::initializeGL()
{
    glEnable(GL_NORMALIZE);
    CheckGLError(__FUNCTION__);
}

void svlWidgetQt4OpenGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.6f, 0.0f, 0.0f); // provide a green background by default

    if (!ImageBuffer || !LatestImage) return;

    glEnable (GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 8);
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 ImageBuffer->GetWidth(), ImageBuffer->GetHeight(),
                 0, _RGB_VERSION_, GL_UNSIGNED_BYTE,
                 LatestImage->Pointer());

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

void svlWidgetQt4OpenGL::resizeGL(int width, int height)
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

void svlWidgetQt4OpenGL::mousePressEvent(QMouseEvent* event)
{
//    LastPosition = event->pos();
}

void svlWidgetQt4OpenGL::mouseMoveEvent(QMouseEvent* event)
{
/*
    int dx = event->x() - LastPosition.x();
    int dy = event->y() - LastPosition.y();

    if (event->buttons() & Qt::LeftButton) {
    }
    else if (event->buttons() & Qt::RightButton) {
    }

    LastPosition = event->pos();
*/
}

void svlWidgetQt4OpenGL::CheckGLError(const std::string & functionName)
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


/***************************************/
/*** svlWindowManagerQt4OpenGL class ***/
/***************************************/

CMN_IMPLEMENT_SERVICES(svlWindowManagerQt4OpenGL)

svlWindowManagerQt4OpenGL::svlWindowManagerQt4OpenGL() :
    QObject(),
    cmnGenericObject(),
    svlWindowManagerBase(),
    LButtonDown(false),
    RButtonDown(false)
{
    DestroySignal = new osaThreadSignal;
}

svlWindowManagerQt4OpenGL::svlWindowManagerQt4OpenGL(const svlWindowManagerQt4OpenGL& CMN_UNUSED(other)) :
    QObject(),
    cmnGenericObject(),
    svlWindowManagerBase()
{
    // Public copy constructor is here only to override the private QObject copy constructor
}

svlWindowManagerQt4OpenGL::~svlWindowManagerQt4OpenGL()
{
    DestroyThreadSafe();
    delete DestroySignal;
}

svlWindowManagerQt4OpenGL* svlWindowManagerQt4OpenGL::New(unsigned int numofwins)
{
    svlWindowManagerQt4OpenGL* instance = dynamic_cast<svlWindowManagerQt4OpenGL*>(svlQtObjectFactory::Create("svlWindowManagerQt4OpenGL"));
    if (instance) instance->SetNumberOfWindows(numofwins);
    return instance;
}

void svlWindowManagerQt4OpenGL::Delete()
{
    svlQtObjectFactory::Delete(this);
}

int svlWindowManagerQt4OpenGL::DoModal(bool show, bool fullscreen)
{
    ImageBuffers.SetSize(NumOfWins);
    ImageBuffers.SetAll(0);
    for (unsigned int i = 0; i < NumOfWins; i ++) {
        ImageBuffers[i] = new svlBufferImage(Width[i], Height[i]);
    }

    argShow       = show;
    argFullscreen = fullscreen;
    QMetaObject::invokeMethod(this, "QSlotCreateWindows", Qt::BlockingQueuedConnection);

    // signal that initialization is done
    if (InitReadySignal) InitReadySignal->Raise();

    // Wait until windows need to be closed
    DestroySignal->Wait();

    QMetaObject::invokeMethod(this, "QSlotDestroyWindows", Qt::BlockingQueuedConnection);

    for (unsigned int i = 0; i < ImageBuffers.size(); i ++) {
        delete ImageBuffers[i];
        ImageBuffers[i] = 0;
    }

    return 0;
}

void svlWindowManagerQt4OpenGL::Show(bool show, int winid)
{
    argShow  = show;
    argWinID = winid;
    QMetaObject::invokeMethod(this, "QSlotShow", Qt::BlockingQueuedConnection);
}

void svlWindowManagerQt4OpenGL::SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid)
{
    if (winid >= ImageBuffers.size() || !ImageBuffers[winid]) return;

    ImageBuffers[winid]->Push(buffer, buffersize, false);
}

void svlWindowManagerQt4OpenGL::DrawImages()
{
    for (unsigned int i = 0; i < Windows.size(); i ++) {
        if (Windows[i]) Windows[i]->UpdateImage();
    }
}

void svlWindowManagerQt4OpenGL::DestroyThreadSafe()
{
    DestroySignal->Raise();
}

void svlWindowManagerQt4OpenGL::QSlotCreateWindows()
{
    QSlotDestroyWindows();

    unsigned int posx = 0;

    ParentWidgets.SetSize(NumOfWins);
    ParentWidgets.SetAll(0);
    Windows.SetSize(NumOfWins);
    Windows.SetAll(0);
    for (int i = 0; i < static_cast<int>(NumOfWins); i ++) {

        Qt::WindowFlags style = Qt::Window;
        if (argFullscreen) style = Qt::FramelessWindowHint|Qt::WindowSystemMenuHint;
        ParentWidgets[i] = new svlParentWidgetQt4(style);

        ParentWidgets[i]->setWindowTitle(QString::fromStdString(Title));

        Windows[i] = new svlWidgetQt4OpenGL(ParentWidgets[i]);
        Windows[i]->Create(ImageBuffers[i]);
        Windows[i]->show();

        if (PosX == 0 || PosY == 0) {
            // Automatically stack all the windows side-by-side
            ParentWidgets[i]->move(posx, 0);
            posx += Width[i];
        }
        else {
            ParentWidgets[i]->move(PosX[i], PosY[i]);
        }

        if (argShow) {
            ParentWidgets[i]->show();
            ParentWidgets[i]->raise();
            ParentWidgets[i]->activateWindow();
        }
    }
}

void svlWindowManagerQt4OpenGL::QSlotDestroyWindows()
{
    for (unsigned int i = 0; i < Windows.size(); i ++) {
        delete Windows[i];
        Windows[i] = 0;
    }
}

void svlWindowManagerQt4OpenGL::QSlotShow()
{
    if (argWinID < 0) {
        for (unsigned int i = 0; i < ParentWidgets.size(); i ++) {
            if (ParentWidgets[i]) {
                if (argShow) ParentWidgets[i]->show();
                else ParentWidgets[i]->hide();
            }
        }
    }
    else {
        if (argWinID < static_cast<int>(ParentWidgets.size()) && ParentWidgets[argWinID]) {
            if (argShow) ParentWidgets[argWinID]->show();
            else ParentWidgets[argWinID]->hide();
        }
    }
}

/*
int svlWindowManagerQt4OpenGL::FilterMessage(unsigned int winid, MSG* msg)
{
	PAINTSTRUCT ps;
    unsigned int code;
	HDC hdc;

    switch (msg->message) {
	    case WM_PAINT:
	    case WM_USER_PAINT:
            // Critical section: starts
//            csImage.Enter();

		        hdc = BeginPaint(msg->hwnd, &ps);
                if (ImageBuffers[winid]) {
                    SetDIBitsToDevice(hdc,
                                      0, 0,
                                      Width[winid], Height[winid],
                                      0, 0,
                                      0, Height[winid],
                                      ImageBuffers[winid],
                                      &(BitmapInfos[winid]),
                                      DIB_RGB_COLORS);
                }
    		    EndPaint(msg->hwnd, &ps);

//            csImage.Leave();
            // Critical section: ends
		break;

        case WM_MOUSEMOVE:
            SetMousePos(static_cast<short>(LOWORD(msg->lParam)), static_cast<short>(HIWORD(msg->lParam)));
            OnUserEvent(winid, false, winInput_MOUSEMOVE);
        break;

        case WM_LBUTTONDOWN:
            if (!LButtonDown && !RButtonDown) {
                LButtonDown = true;
                SetCapture(msg->hwnd);
            }
            OnUserEvent(winid, false, winInput_LBUTTONDOWN);
        break;

        case WM_LBUTTONUP:
            OnUserEvent(winid, false, winInput_LBUTTONUP);
            if (LButtonDown && !RButtonDown) {
                LButtonDown = false;
                ReleaseCapture();
            }
        break;

        case WM_RBUTTONDOWN:
            if (!LButtonDown && !RButtonDown) {
                RButtonDown = true;
                SetCapture(msg->hwnd);
            }
            OnUserEvent(winid, false, winInput_RBUTTONDOWN);
        break;

        case WM_RBUTTONUP:
            OnUserEvent(winid, false, winInput_RBUTTONUP);
            if (!LButtonDown && RButtonDown) {
                RButtonDown = false;
                ReleaseCapture();
            }
        break;

	    case WM_KEYDOWN:
            code = static_cast<unsigned int>(msg->wParam);
            if (code >= 48 && code <= 57) { // ascii numbers
                OnUserEvent(winid, true, code);
                break;
            }
            if (code >= 65 && code <= 90) { // ascii letters
                OnUserEvent(winid, true, code + 32);
                break;
            }
            if (code == 13 ||
                code == 32) { // special characters with correct ascii code
                OnUserEvent(winid, true, code);
                break;
            }
            if (code >= 112 && code <= 123) { // F1-F12
                OnUserEvent(winid, false, code);
                break;
            }
            if ((msg->lParam | 0x800000) == 0) {
                return 0; // yet to be processed
            }
            else {
                // Virtual keys
		        switch (msg->wParam) {
		            case winInput_KEY_PAGEUP:
		            case winInput_KEY_PAGEDOWN:
		            case winInput_KEY_HOME:
		            case winInput_KEY_END:
		            case winInput_KEY_INSERT:
		            case winInput_KEY_DELETE:
		            case winInput_KEY_LEFT:
		            case winInput_KEY_RIGHT:
		            case winInput_KEY_UP:
		            case winInput_KEY_DOWN:
                        OnUserEvent(winid, false, static_cast<unsigned int>(msg->wParam));
		            break;

		            default:
			            return 0; // yet to be processed
		        }
            }
	    break;

        case WM_USER_DESTROY:
            // Critical section: starts
            csImage.Enter();

                Destroy();

            csImage.Leave();
            // Critical section: ends
        break;

        default:
            return 0; // yet to be processed
    }
    return 1; // already processed
}
*/
