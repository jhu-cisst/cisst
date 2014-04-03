/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageResizer : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageResizer();
    virtual ~svlFilterImageResizer();

    int SetOutputSize(unsigned int width, unsigned int height, unsigned int videoch = SVL_LEFT);
    int SetOutputRatio(double widthratio, double heightratio, unsigned int videoch = SVL_LEFT);
    void SetInterpolation(const bool & enable);
    void GetInterpolation(bool & enable) const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImage* OutputImage;

    bool EqualSize;
    double WidthRatio[2];
    double HeightRatio[2];
    unsigned int Width[2];
    unsigned int Height[2];
    bool InterpolationEnabled;
    vctDynamicVector<unsigned char> Internals[2];

protected:
    virtual void CreateInterfaces();
    virtual void SetOutputDimensionLCommand(const vctInt2 & dimension);
    virtual void SetOutputDimensionRCommand(const vctInt2 & dimension);
    virtual void SetOutputRatioLCommand(const vctDouble2 & ratio);
    virtual void SetOutputRatioRCommand(const vctDouble2 & ratio);
    virtual void GetOutputDimensionLCommand(vctInt2 & dimension) const;
    virtual void GetOutputDimensionRCommand(vctInt2 & dimension) const;
    virtual void GetOutputRatioLCommand(vctDouble2 & ratio) const;
    virtual void GetOutputRatioRCommand(vctDouble2 & ratio) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageResizer)

#endif // _svlFilterImageResizer_h

