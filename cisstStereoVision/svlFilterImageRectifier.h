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

#ifndef _svlFilterImageRectifier_h
#define _svlFilterImageRectifier_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlImageProcessing.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageRectifier : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageRectifier();
    virtual ~svlFilterImageRectifier();

    int LoadTable(const std::string &filepath, unsigned int videoch = SVL_LEFT, int exponentlen = 3);
    //changed from "vctFixedSizeVector<double,5> k", to "vctFixedSizeVector<double,7> k"
    int SetTableFromCameraCalibration(unsigned int height,unsigned int width,vct3x3 R,vct2 f, vct2 c, vctFixedSizeVector<double,7> k, double alpha, unsigned int videoch);
    vctFixedSizeVector<svlImageProcessing::Internals, SVL_MAX_CHANNELS> GetTables(){return Tables;};
    void EnableInterpolation(bool enable = true);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlSampleImage* OutputImage;

    vctFixedSizeVector<svlImageProcessing::Internals, SVL_MAX_CHANNELS> Tables;
    bool InterpolationEnabled;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageRectifier)

#endif // _svlFilterImageRectifier_h

