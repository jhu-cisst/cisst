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

#include <QtGui>
#include <QtOpenGL>
#include <QTime>
#include <QImage>

#include <math.h>
#include <QImage>
#include <cisstStereoVision/svlFilterImageQtWidget.h>


CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageQtWidget, svlFilterBase)


svlFilterImageQtWidget::svlFilterImageQtWidget(QWidget * parent):
    QGLWidget(parent),
    svlFilterBase(),
    Image(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
    QObject::connect(this, SIGNAL(QSignalUpdateGL()), this, SLOT(updateGL()));
}


svlFilterImageQtWidget::~svlFilterImageQtWidget()
{
    makeCurrent();
    Release();
}


void svlFilterImageQtWidget::initializeGL()
{
    glEnable(GL_NORMALIZE);
    glViewport(0,0,width(),height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), 0.0, height(), 0.0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    CheckGLError();
}


void svlFilterImageQtWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.6f, 0.0f, 0.0f);

    int w = width();
    int h = height();
    if (!Image) {
        return;
    }
    int iw = Image->GetWidth();
    int ih = Image->GetHeight();

    glEnable (GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 8);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, iw, ih, 0, GL_RGB, GL_UNSIGNED_BYTE, Image->GetUCharPointer());

    glPushMatrix();
    glRotatef(-90.0, 0.0, 0.0, 1.0);
    glTranslatef(-h, 0.0, 0.0);

    glBegin (GL_QUADS);
    {
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(h, 0.0, 0.0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(h, w, 0.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.0, w, 0.0);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    CheckGLError();
}


void svlFilterImageQtWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, 0.0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void svlFilterImageQtWidget::mousePressEvent(QMouseEvent * event)
{
    LastPosition = event->pos();
}


void svlFilterImageQtWidget::mouseMoveEvent(QMouseEvent * event)
{
    int dx = event->x() - LastPosition.x();
    int dy = event->y() - LastPosition.y();

    if (event->buttons() & Qt::LeftButton) {

    }

    else if (event->buttons() & Qt::RightButton) {

    }
    LastPosition = event->pos();
}


int svlFilterImageQtWidget::Initialize(svlSample * syncInput, svlSample* &syncOutput)
{
    Release();

    Image = dynamic_cast<svlSampleImage*>(svlSampleImage::GetNewFromType(syncInput->GetType()));
    svlSampleImage * imagePointer = dynamic_cast<svlSampleImage*>(syncInput);

    //    Image = dynamic_cast<svlSampleImage*>  (new svlSampleImageRGB());
    for (unsigned int i = 0; i < Image->GetVideoChannels(); i++) {
        Image->SetSize(i, imagePointer->GetWidth(), imagePointer->GetHeight());
    }

    syncOutput = syncInput;

    return SVL_OK;
}


int svlFilterImageQtWidget::Process(svlProcInfo * procInfo, svlSample * syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage * imagePointer = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = imagePointer->GetVideoChannels();
    unsigned int idx;

    _OnSingleThread(procInfo)
    {
        for (unsigned int i = 0; i < videochannels; i++) {
            memcpy(Image->GetUCharPointer(i), imagePointer->GetUCharPointer(i), Image->GetDataSize(i));
        }
        emit QSignalUpdateGL();
    }

    return SVL_OK;
}


int svlFilterImageQtWidget::Release()
{
    return SVL_OK;
}


void svlFilterImageQtWidget::CheckGLError()
{
    for (GLenum gl_error = glGetError();
	 (gl_error);
	 gl_error = glGetError()) {
        switch (gl_error){
        case (GL_INVALID_ENUM):
            printf("%s: %u caught: Invalid ENUM\n", __FUNCTION__,gl_error);
            break;
        case (GL_INVALID_VALUE):
            printf("%s: %u caught: Invalid VALUE\n", __FUNCTION__,gl_error);
            break;
        case (GL_INVALID_OPERATION):
            printf("%s: %u caught: Invalid OPERATION\n", __FUNCTION__,gl_error);
            break;
        case (GL_STACK_OVERFLOW):
            printf("%s: %u caught:  GL_STACK_OVERFLOW\n", __FUNCTION__,gl_error);
            break;

        case (GL_STACK_UNDERFLOW):
            printf("%s: %u caught:  GL_STACK_UNDERFLOW\n", __FUNCTION__,gl_error);
            break;
        case (GL_OUT_OF_MEMORY):
            printf("%s: %u caught:  GL_OUT_OF_MEMORY\n", __FUNCTION__,gl_error);
            break;
        default:
            printf("%s: %u caught: NO IDEA\n", __FUNCTION__,gl_error);
        }
    }
}
