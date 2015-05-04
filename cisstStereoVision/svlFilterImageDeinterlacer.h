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

#ifndef _svlFilterImageDeinterlacer_h
#define _svlFilterImageDeinterlacer_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlImageProcessing.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageDeinterlacer : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageDeinterlacer();

    void SetAlgorithm(svlImageProcessing::DI_Algorithm algorithm, int videoch = -1);
    svlImageProcessing::DI_Algorithm GetAlgorithm(unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    vctDynamicVector<svlImageProcessing::DI_Algorithm> Algorithm;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageDeinterlacer)

#endif // _svlFilterImageDeinterlacer_h

