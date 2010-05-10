/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctPlot2DGLBase.h 1238 2010-02-27 03:16:01Z auneri1 $

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctPlot2DGLBase_h
#define _vctPlot2DGLBase_h

#include <map>
#include <string>

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

#include "vctPlot2DBase.h"

class vctPlot2DGLBase: public vctPlot2DBase
{
public:

    vctPlot2DGLBase(void);
    ~vctPlot2DGLBase(void) {};

 protected:

    /*! Define rendering function for OpenGL */
    virtual void RenderInitialize(void);
    virtual void RenderResize(double width, double height);
    virtual void Render(void);

};

#endif  // _vctPlot2DGLBase_h
