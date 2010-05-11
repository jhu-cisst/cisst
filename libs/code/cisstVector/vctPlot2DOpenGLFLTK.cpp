/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DOpenGLFLTK.cpp 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctPlot2DOpenGLFLTK.h>

vctPlot2DOpenGLFLTK::vctPlot2DOpenGLFLTK(const int x, const int y,
			     const int width, const int height,
			     const char * title):
    Fl_Gl_Window(x, y, width, height, title),
    vctPlot2DOpenGLBase()
{
    this->Viewport.X() = width;
    this->Viewport.Y() = height;
}


void vctPlot2DOpenGLFLTK::initgl(void)
{
    vctPlot2DOpenGLBase::RenderInitialize();
    vctPlot2DOpenGLBase::RenderResize(this->Viewport.X(), this->Viewport.Y());
}


void vctPlot2DOpenGLFLTK::draw(void)
{
    if (!valid()) {
	initgl();
	valid(1);
    }
    vctPlot2DOpenGLBase::Render();
}
