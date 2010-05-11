/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DGLBase.cpp 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctPlot2DGLBase.h"

#if (CISST_OS == CISST_WINDOWS)
  #include <windows.h>
#endif

#if (CISST_OS == CISST_DARWIN)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif


vctPlot2DGLBase::vctPlot2DGLBase(void):
    vctPlot2DBase()
{
}


void vctPlot2DGLBase::RenderInitialize(void)
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


void vctPlot2DGLBase::RenderResize(double width, double height)
{
    this->Viewport.Assign(width, height);
    GLdouble w = static_cast<GLdouble>(width);
    GLdouble h = static_cast<GLdouble>(height);
	glViewport(0 , 0, w ,h); // set up viewport
	glMatrixMode(GL_PROJECTION); // set the projection matrix
	glLoadIdentity();
	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
}


void vctPlot2DGLBase::Render(void)
{
    size_t traceIndex;
    Trace * trace;
    const size_t numberOfTraces = this->Traces.size();
    size_t pointIndex;
    size_t numberOfPoints;

    // make sure there is no left over transformation
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    // for now, find the min and max for X and Y for all traces
    vctDouble2 min, max;
    min.Assign(this->Traces[0]->Data.Element(0));
    max.Assign(min);
    for (traceIndex = 0;
         traceIndex < numberOfTraces;
         traceIndex++) {
        this->Traces[traceIndex]->UpdateMinAndMax(min, max);
    }
    vctDouble2 scale;
    scale.ElementwiseRatioOf(this->Viewport, max - min);
    vctDouble2 offset;
    offset.ElementwiseProductOf(min, scale);
    offset.NegationSelf();

    // plot all traces
    for (traceIndex = 0;
         traceIndex < numberOfTraces;
         traceIndex++) {
        trace = this->Traces[traceIndex];
        numberOfPoints = trace->Data.size();
        glColor3d(trace->Color.Element(0),
                  trace->Color.Element(1),
                  trace->Color.Element(2));
        glLineWidth(trace->LineWidth);
        glBegin(GL_LINE_STRIP);
        for (pointIndex = trace->IndexFirst;
             pointIndex != trace->IndexLast;
             pointIndex = (pointIndex + 1) % numberOfPoints) {
            glVertex2d((trace->Data.Element(pointIndex).X() * scale.X()) + offset.X(),
                       (trace->Data.Element(pointIndex).Y() * scale.Y()) + offset.Y());
        }
        glEnd();
    }
}
