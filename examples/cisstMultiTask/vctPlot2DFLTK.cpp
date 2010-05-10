/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DFLTK.cpp 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctPlot2DFLTK.h"

vctPlot2DFLTK::vctPlot2DFLTK(const int x, const int y,
			     const int width, const int height,
			     const char * title):
    Fl_Gl_Window(x, y, width, height, title),
    vctPlot2DGLBase()
{
    this->Viewport.X() = width;
    this->Viewport.Y() = height;
}


void vctPlot2DFLTK::initgl(void)
{
    vctPlot2DGLBase::RenderInitialize();
    vctPlot2DGLBase::RenderResize(this->Viewport.X(), this->Viewport.Y());
}


void vctPlot2DFLTK::draw(void)
{
    if (!valid()) {
	initgl();
	valid(1);
    }
    vctPlot2DGLBase::Render();
}
