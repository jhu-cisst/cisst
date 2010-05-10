/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DQWidget.h 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPlot2DFLTK_h
#define _vctPlot2DFLTK_h

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include <vctPlot2DGLBase.h>


class vctPlot2DFLTK: public Fl_Gl_Window, public vctPlot2DGLBase
{
public:
    vctPlot2DFLTK(const int x, const int y,
		  const int width, const int height,
		  const char * title);
    ~vctPlot2DFLTK(void) {};

protected:

    /*! Methods required for FLTK */
    void initgl(void);
    void draw(void);
};


#endif // _vctPlot2DFLTK_h
