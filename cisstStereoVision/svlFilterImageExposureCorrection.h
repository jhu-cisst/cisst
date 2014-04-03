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

#ifndef _svlFilterImageExposureCorrection_h
#define _svlFilterImageExposureCorrection_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlImageProcessing.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageExposureCorrection : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageExposureCorrection();

    virtual void SetBrightness(const double & brightness);
    virtual void SetContrast(const double & contrast);
    virtual void SetGamma(const double & gamma);
    virtual void GetContrast(double & contrast) const;
    virtual void GetBrightness(double & brightness) const;
    virtual void GetGamma(double & gamma) const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

protected:
    double Brightness;
    double Contrast;
    double Gamma;

    vctDynamicVector<svlImageProcessing::Internals> Exposure;

protected:
    virtual void CreateInterfaces();
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageExposureCorrection)

#endif // _svlFilterImageExposureCorrection_h

