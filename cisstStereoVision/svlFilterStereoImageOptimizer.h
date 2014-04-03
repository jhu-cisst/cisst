/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlFilterStereoImageOptimizer_h
#define _svlFilterStereoImageOptimizer_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterStereoImageOptimizer : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterStereoImageOptimizer();
    virtual ~svlFilterStereoImageOptimizer();

    int SetDisparity(int disparity);
    int SetRegionOfInterest(const svlRect &roi);
    int SetRegionOfInterest(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);

    void RecomputeColorBalance(bool always = false);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    unsigned int Disparity_Target;
    unsigned int Disparity;
    svlRect ROI_Target;
    svlRect ROI[2];
    int ColBal_Red;
    int ColBal_Green;
    int ColBal_Blue;
    int RecomputeRatios;
    int R[2];
    int G[2];
    int B[2];
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterStereoImageOptimizer)

#endif // _svlFilterStereoImageOptimizer_h

