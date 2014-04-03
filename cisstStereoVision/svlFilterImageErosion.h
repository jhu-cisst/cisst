/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

 Author(s):  Balazs Vagvolgyi
 Created on: 2011

 (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
 Reserved.

 --- begin cisst license - do not edit ---

 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.

 --- end cisst license ---

 */

#ifndef _svlFilterImageErosion_h
#define _svlFilterImageErosion_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageErosion : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageErosion();
    virtual ~svlFilterImageErosion();

    void SetIterations(unsigned int iterations);
    unsigned int GetIterations() const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImage* OutputImage;
    svlSampleImage* TempImage;
    unsigned int Iterations;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageErosion)

#endif // _svlFilterImageErosion_h

