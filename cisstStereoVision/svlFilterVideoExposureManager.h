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

#ifndef _svlFilterVideoExposureManager_h
#define _svlFilterVideoExposureManager_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlFilterSourceVideoCapture.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterVideoExposureManager : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterVideoExposureManager();

    virtual int SetVideoCaptureFilter(svlFilterSourceVideoCapture* sourcefilter);
    virtual svlFilterSourceVideoCapture* GetVideoCaptureFilter() const;
    virtual void SetSaturationTolerance(double tolerance);
    virtual double GetSaturationTolerance() const;
    virtual void SetSaturationThreshold(unsigned char threshold);
    virtual unsigned char GetSaturationThreshold() const;
    virtual void SetMaxGain(unsigned int maxgain);
    virtual unsigned int GetMaxGain() const;
    virtual void SetMaxShutter(unsigned int maxshutter);
    virtual unsigned int GetMaxShutter() const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

protected:
    svlFilterSourceVideoCapture* SourceFilter;
    double Tolerance;
    unsigned char Threshold;
    int MaxGain;
    int MaxShutter;

    vctDynamicVector<unsigned int> Histogram;

    virtual void GetSaturationRatio(svlSampleImage* image, const unsigned int videoch, double& saturation, unsigned int& maxval);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterVideoExposureManager)

#endif // _svlFilterVideoExposureManager_h

