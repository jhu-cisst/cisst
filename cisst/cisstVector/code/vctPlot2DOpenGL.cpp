/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctPlot2DOpenGL.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif

#if (CISST_OS == CISST_DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


vctPlot2DOpenGL::vctPlot2DOpenGL(void):
    vctPlot2DBase()
{
}


void vctPlot2DOpenGL::RenderInitialize(void)
{
    glMatrixMode(GL_MODELVIEW); // set the model view matrix
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST); // disable depth test
    glDisable(GL_LIGHTING); // disable lighting
    glShadeModel(GL_SMOOTH); // smooth render
    glClearColor(static_cast<GLclampf>(this->BackgroundColor[0]),
                 static_cast<GLclampf>(this->BackgroundColor[1]),
                 static_cast<GLclampf>(this->BackgroundColor[2]),
                 static_cast<GLclampf>(1.0));
}


void vctPlot2DOpenGL::RenderResize(double width, double height)
{

    const size_t numberOfScales = this->Scales.size();
    size_t scaleIndex ;

    this->Viewport.Assign(width, height);

    for (scaleIndex = 0;
         scaleIndex < numberOfScales;
         scaleIndex++) {
        vctPlot2DBase::Scale * scale = this->Scales[scaleIndex];
        scale->Viewport.Assign(width, height);
    }
    
    GLsizei w = static_cast<GLsizei>(width);
    GLsizei h = static_cast<GLsizei>(height);

    glViewport(0 , 0, w ,h); // set up viewport
    glMatrixMode(GL_PROJECTION); // set the projection matrix
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
}


void vctPlot2DOpenGL::Render(void)
{
    size_t scaleIndex;
    const size_t numberOfScales = this->Scales.size();

    // see if translation and scale need to be updated
    this->ContinuousUpdate();

    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    for (scaleIndex =0;
         scaleIndex < numberOfScales;
         scaleIndex++) {
        this->Render(Scales[scaleIndex]);
    }
}


//void vctPlot2DOpenGL::Render(const vctPlot2DBase::VerticalLine & line)
void vctPlot2DOpenGL::Render(const vctPlot2DBase::VerticalLine * line)
{
    // render vertical lines
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(this->Translation.X(), 0.0, 0.0);
    glScaled(this->ScaleValue.X(), 1.0, 1.0);

    // todo, should check for "visible" flag
    glBegin(GL_LINE_STRIP);
    glVertex2d(line->X, this->Viewport.Y());
    glVertex2d(line->X, 0);
    glEnd();
}


void vctPlot2DOpenGL::Render(const vctPlot2DBase::Signal * signal)
{
    if (signal->Visible) {
        glColor3d(signal->Color.Element(0),
                  signal->Color.Element(1),
                  signal->Color.Element(2));
        glLineWidth(static_cast<GLfloat>(signal->LineWidth));
        const double * data = signal->Data.Element(0).Pointer();
        size_t size = signal->Data.size();
        if (signal->IndexFirst >= signal->IndexLast) {
            // circular buffer is full/split in two
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_DOUBLE, 0, data);
            // draw first part
            glDrawArrays(GL_LINE_STRIP,
                         static_cast<GLint>(signal->IndexFirst),
                         static_cast<GLsizei>(size - signal->IndexFirst));
            // draw second part
            glDrawArrays(GL_LINE_STRIP,
                         0,
                         static_cast<GLsizei>(signal->IndexLast + 1));
            glDisableClientState(GL_VERTEX_ARRAY);
            // draw between end of buffer and beginning
            glBegin(GL_LINE_STRIP);
            glVertex2d(signal->Data.Element(size - 1).X(),
                       signal->Data.Element(size - 1).Y());
            glVertex2d(signal->Data.Element(0).X(),
                       signal->Data.Element(0).Y());
            glEnd();
        } else {
            // simpler case, all points contiguous
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_DOUBLE, 0, data);
            glDrawArrays(GL_LINE_STRIP,
                         0,
                         static_cast<GLsizei>(signal->IndexLast + 1));
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }
}


void vctPlot2DOpenGL::Render(const vctPlot2DBase::Scale * scale)
{

    const size_t numberOfSignals = scale->Signals.size();
    const size_t numberOfLines = scale->VerticalLines.size();
    size_t signalIndex, vlineIndex ;
    vctPlot2DBase::Signal *signal;
    vctPlot2DBase::VerticalLine *vline;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glTranslated(this->Translation.X(), this->Translation.Y(), 0.0);
    // glScaled(this->ScaleValue.X(), this->ScaleValue.Y(), 1.0);

    glTranslated(scale->Translation.X(), scale->Translation.Y(), 0.0);
    glScaled(scale->ScaleValue.X(), scale->ScaleValue.Y(), 1.0);

    for (signalIndex = 0;
         signalIndex < numberOfSignals;
         signalIndex++) {
        signal = scale->Signals[signalIndex];
        Render(signal);
    }

    for (vlineIndex = 0;
         vlineIndex < numberOfLines;
         vlineIndex ++) {
        vline = scale->VerticalLines[vlineIndex];
        Render(vline);
    }
}
