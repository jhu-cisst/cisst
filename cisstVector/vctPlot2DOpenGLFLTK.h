/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPlot2DOpenGLFLTK_h
#define _vctPlot2DOpenGLFLTK_h

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include <cisstVector/vctPlot2DOpenGL.h>

// Always include last
#include <cisstVector/vctExportFLTK.h>

class CISST_EXPORT vctPlot2DOpenGLFLTK: public Fl_Gl_Window, public vctPlot2DOpenGL
{
public:
    vctPlot2DOpenGLFLTK(const int x, const int y,
                        const int width, const int height,
                        const char * title);
    ~vctPlot2DOpenGLFLTK(void) {};

protected:

    /*! Methods required for FLTK */
    //@{
    void initgl(void);
    void draw(void);
    int handle(int event);
    //@}
};


#endif // _vctPlot2DOpenGLFLTK_h
