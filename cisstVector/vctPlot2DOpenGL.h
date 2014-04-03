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

#ifndef _vctPlot2DOpenGL_h
#define _vctPlot2DOpenGL_h

#include <cisstVector/vctPlot2DBase.h>

// Always include last
#include <cisstVector/vctExport.h>

/*! 2D plot class using OpenGL for rendering */
class CISST_EXPORT vctPlot2DOpenGL: public vctPlot2DBase
{
 public:

    vctPlot2DOpenGL(void);
    virtual ~vctPlot2DOpenGL(void) {};

 protected:

    /*! Define rendering function for OpenGL */
    //@{
    virtual void RenderInitialize(void);
    virtual void RenderResize(double width, double height);
    virtual void Render(void); // todo, move render to base class, not virtual

    // called by Render(void)
    virtual void Render(const VerticalLine * line);
    virtual void Render(const Scale * scale);
    virtual void Render(const Signal * signal);
    //@}
};

#endif  // _vctPlot2DOpenGL_h
