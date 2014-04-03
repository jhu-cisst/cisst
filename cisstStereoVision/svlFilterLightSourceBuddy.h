/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi & Seth Billings
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterLightSourceBuddy_h
#define _svlFilterLightSourceBuddy_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterLightSourceBuddy : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterLightSourceBuddy();

    virtual void SetCalibration(const vct3x3 & matrix);
    virtual void SetLightBalance(const vct3 & balance);
    virtual void GetCalibration(vct3x3 & matrix) const;
    virtual void GetLightBalance(vct3 & balance) const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    vct3x3 CalibMatrix;
    vct3x3 CalibMatrixInv;
    vct3x3 CorrectionMatrix;
    vct3 LightBalance;

    int InvertMatrix(const vct3x3 & matrix, vct3x3 & inverse);

protected:
    virtual void CreateInterfaces();
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterLightSourceBuddy)

#endif // _svlFilterLightSourceBuddy_h

