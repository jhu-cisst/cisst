/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageSampler.h,v 1.12 2008/10/17 17:44:38 vagvoba Exp $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _svlImageSampler_h
#define _svlImageSampler_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class svlImageSampler;

class CISST_EXPORT svlImageSamplerCallbackBase
{
friend class svlImageSampler;

public:
    svlImageSamplerCallbackBase() { FrameCounter = 0; }
    virtual ~svlImageSamplerCallbackBase() {}

    // Callback
    virtual void FrameCallback(svlSampleImageBase * CMN_UNUSED(image),
                               svlBMPFileHeader * CMN_UNUSED(fileheader1), svlDIBHeader * CMN_UNUSED(dibheader1),
                               svlBMPFileHeader * CMN_UNUSED(fileheader2), svlDIBHeader * CMN_UNUSED(dibheader2)) {}

    inline unsigned int GetFrameCounter(void) { return FrameCounter; }
    inline void ResetCounter(void) { FrameCounter = 0; }

private:
    unsigned int FrameCounter;

    virtual void PostCallback() { FrameCounter ++; }
};

class CISST_EXPORT svlImageSampler : public svlFilterBase
{
public:
    svlImageSampler();
    virtual ~svlImageSampler();

    void SetCallback(svlImageSamplerCallbackBase* callbackobj);
    void SetDistanceIntensityRatio(float ratio) { DistanceScaling = ratio; }
    float GetDistanceIntensityRatio() { return DistanceScaling; }

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    svlImageSamplerCallbackBase* CallbackObj;
    svlBMPFileHeader* FileHeader[2];
    svlDIBHeader* DIBHeader[2];
    svlSampleImageBase* ImageBuffer;
    float DistanceScaling;
};

#endif // _svlImageSampler_h

