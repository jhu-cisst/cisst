/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DGLBase.cpp 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

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
    this->Viewport.Assign(width, height);
    GLsizei w = static_cast<GLsizei>(width);
    GLsizei h = static_cast<GLsizei>(height);
    glViewport(0 , 0, w ,h); // set up viewport
    glMatrixMode(GL_PROJECTION); // set the projection matrix
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
}


void vctPlot2DOpenGL::Render(void)
{
    size_t traceIndex;
    Trace * trace;
    const size_t numberOfTraces = this->Traces.size();
    size_t numberOfPoints;

    // see if translation and scale need to be updated
    this->ContinuousUpdate();

    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    // make sure there is no left over transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(this->Translation.X(), this->Translation.Y(), 0.0);
    glScaled(this->Scale.X(), this->Scale.Y(), 1.0);

    // plot all traces.   this needs to be updated to use glScale and glTranslate to avoid all conputations on CPU
    double * data;
    size_t size;
    for (traceIndex = 0;
         traceIndex < numberOfTraces;
         traceIndex++) {
        trace = this->Traces[traceIndex];
        trace->CriticalSectionForBuffer.Enter();
        if (trace->Visible) {            
            numberOfPoints = trace->Data.size();
            glColor3d(trace->Color.Element(0),
                      trace->Color.Element(1),
                      trace->Color.Element(2));
            glLineWidth(static_cast<GLfloat>(trace->LineWidth));
            data = trace->Data.Element(0).Pointer();
            size = trace->Data.size();
            if (trace->IndexFirst >= trace->IndexLast) {
                // circular buffer is full/split in two
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(2, GL_DOUBLE, 0, data);
                // draw first part
                glDrawArrays(GL_LINE_STRIP,
                             static_cast<GLint>(trace->IndexFirst),
                             static_cast<GLsizei>(size - trace->IndexFirst));
                // draw second part
                glDrawArrays(GL_LINE_STRIP,
                             0,
                             static_cast<GLsizei>(trace->IndexLast + 1));
                glDisableClientState(GL_VERTEX_ARRAY);
                // draw between end of buffer and beginning
                glBegin(GL_LINE_STRIP);
                glVertex2d(trace->Data.Element(size - 1).X(),
                           trace->Data.Element(size - 1).Y());
                glVertex2d(trace->Data.Element(0).X(),
                           trace->Data.Element(0).Y());
                glEnd();
            } else {
                // simpler case, all points contiguous
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(2, GL_DOUBLE, 0, data);
                glDrawArrays(GL_LINE_STRIP,
                             0,
                             static_cast<GLsizei>(trace->IndexLast + 1));
                glDisableClientState(GL_VERTEX_ARRAY);
            }
        }
        trace->CriticalSectionForBuffer.Leave();
    }

    // render vertical lines
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(this->Translation.X(), 0.0, 0.0);
    glScaled(this->Scale.X(), 1.0, 1.0);

    const VerticalLinesType::const_iterator linesEnd = VerticalLines.end();
    VerticalLinesType::const_iterator linesIter = VerticalLines.begin();
    for (;
         linesIter != linesEnd;
         ++linesIter) {
        this->Render(*(linesIter->second));
    }
}


void vctPlot2DOpenGL::Render(const vctPlot2DBase::VerticalLine & line)
{
    // todo, should check for "visible" flag
    glBegin(GL_LINE_STRIP);
    glVertex2d(line.X, this->Viewport.Y());
    glVertex2d(line.X, 0);
    glEnd();
}
