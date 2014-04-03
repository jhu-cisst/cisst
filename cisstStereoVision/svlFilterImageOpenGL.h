/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Marcin Balicki
  Created on: 2011-02-18

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageOpenGL_h
#define _svlFilterImageOpenGL_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

//! this class is useful for embedding an svl stream to an OpenGL window
class CISST_EXPORT svlFilterImageOpenGL: public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 public:
    svlFilterImageOpenGL();
    enum ByteOrder {RGB_Order, BGR_Order};
    void SetByteOrderRGB(ByteOrder order);
    ByteOrder GetByteOrderRGB();

    virtual ~svlFilterImageOpenGL();

 protected:
    // rendering function for OpenGL
    void RenderInitialize(void);
    void RenderResize(double width, double height);
    void Render(void);

    void CheckGLError(const std::string & functionName);

    // Implemented by derived classes, called at the end of
    // svlFilter::Process to tell the GUI how to render (in Qt, emits
    // a signal)
    virtual void PostProcess(void) = 0;

    int GetImageHeight();
    int GetImageWidth();
    int GetWindowHeight() {return Height;}
    int GetWindowWidth()  {return Width;}

 private:
    svlSampleImage * Image;
    int Height;
    int Width;
    int ByteOrderVersion;

 protected:
    // svl derived methods
    int Initialize(svlSample * syncInput, svlSample* &syncOutput);
    int Process(svlProcInfo * procInfo, svlSample * syncInput, svlSample* &syncOutput);
    int Release();
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageOpenGL)

#endif // _svlFilterImageOpenGL_h
