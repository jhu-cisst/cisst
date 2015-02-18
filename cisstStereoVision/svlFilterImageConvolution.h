/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageConvolution_h
#define _svlFilterImageConvolution_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlImageProcessing.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageConvolution : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageConvolution();

    void SetKernel(const vctDynamicMatrix<double> & kernel);
    void SetKernel(const vctDynamicVector<double> & kernel_horiz, const vctDynamicVector<double> & kernel_vert);
    void SetAbsoluteResults(bool absres);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlSampleImage* OutputImage;
    vctDynamicMatrix<double> Kernel;
    vctDynamicVector<double> KernelHoriz;
    vctDynamicVector<double> KernelVert;
    bool KernelSeparable;
    bool AbsoluteResults;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageConvolution)

#endif // _svlFilterImageConvolution_h

