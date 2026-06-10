/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif

#if (CISST_OS == CISST_DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cisstVector/vctPlot2DOpenGL.h>

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
    Viewport = Eigen::Vector2d(width, height);

    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         scale++) {
        scale->second->Viewport.x() = width;
        scale->second->Viewport.y() = height;
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
    // see if translation and scale need to be updated
    this->ContinuousUpdate();
    // clear
    glClear(GL_COLOR_BUFFER_BIT);
    // plot all scales
    const ScalesType::iterator end = Scales.end();
    for (ScalesType::iterator scale = Scales.begin();
         scale != end;
         scale++) {
        this->Render(scale->second.get());
    }
}

void vctPlot2DOpenGL::Render(const vctPlot2DBase::VerticalLine * line)
{
    // render vertical lines
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // todo, should check for "visible" flag
    glBegin(GL_LINE_STRIP);
    glVertex2d(line->X, this->Viewport.y());
    glVertex2d(line->X, 0);
    glEnd();
}

void vctPlot2DOpenGL::Render(const vctPlot2DBase::Signal * signal)
{
    if (signal->Visible) {
        glColor3d(signal->Color.x(),
                  signal->Color.y(),
                  signal->Color.z());
        glLineWidth(static_cast<GLfloat>(signal->LineWidth));
        const double * data = signal->data.data();
        size_t size = signal->data.cols();
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
            glVertex2d(signal->data.col(size - 1).x(),
                       signal->data.col(size - 1).y());
            glVertex2d(signal->data.col(0).x(),
                       signal->data.col(0).y());
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
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(scale->Translation.x(), scale->Translation.y(), 0.0);
    glScaled(scale->ScaleValue.x(), scale->ScaleValue.y(), 1.0);

    // signals
    {
        const vctPlot2DBase::Scale::SignalsType::const_iterator
            end = scale->Signals.end();
        vctPlot2DBase::Scale::SignalsType::const_iterator
            iter = scale->Signals.begin();
        for (; iter != end; ++iter) {
            Render(iter->second.get());
        }
    }

    // lines
    {
        const vctPlot2DBase::Scale::VerticalLinesType::const_iterator
            end = scale->VerticalLines.end();
        vctPlot2DBase::Scale::VerticalLinesType::const_iterator
            iter = scale->VerticalLines.begin();
        for (; iter != end; ++iter) {
            Render(iter->second.get());
        }
    }
}
