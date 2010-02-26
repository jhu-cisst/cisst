/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageResizer_h
#define _svlFilterImageResizer_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterImageResizer : public svlFilterBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterImageResizer();
    virtual ~svlFilterImageResizer();

    int SetOutputSize(unsigned int width, unsigned int height, unsigned int videoch = SVL_LEFT);
    int SetOutputRatio(double widthratio, double heightratio, unsigned int videoch = SVL_LEFT);
    void EnableInterpolation(bool enable = true) { InterpolationEnabled = enable; }

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    double WidthRatio[2];
    double HeightRatio[2];
    unsigned int Width[2];
    unsigned int Height[2];
    unsigned char *TempBuffer[2];
    bool InterpolationEnabled;

    int ResampleMono8(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                      unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight);
    int ResampleAndInterpolateHMono8(unsigned char* src, const unsigned int srcwidth,
                                     unsigned char* dst, const unsigned int dstwidth,
                                     const unsigned int height);
    int ResampleAndInterpolateVMono8(unsigned char* src, const unsigned int srcheight,
                                     unsigned char* dst, const unsigned int dstheight,
                                     const unsigned int width);

    int ResampleRGB24(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                      unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight);
    int ResampleAndInterpolateHRGB24(unsigned char* src, const unsigned int srcwidth,
                                     unsigned char* dst, const unsigned int dstwidth,
                                     const unsigned int height);
    int ResampleAndInterpolateVRGB24(unsigned char* src, const unsigned int srcheight,
                                     unsigned char* dst, const unsigned int dstheight,
                                     const unsigned int width);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageResizer)

#endif // _svlFilterImageResizer_h

