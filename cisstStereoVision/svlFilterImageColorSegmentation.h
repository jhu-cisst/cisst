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

#ifndef _svlFilterImageColorSegmentation_h
#define _svlFilterImageColorSegmentation_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageColorSegmentation : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageColorSegmentation();
    virtual ~svlFilterImageColorSegmentation();

    void AddColor(int x, int y, int z, unsigned char threshold, unsigned char label);

protected:
    virtual int OnConnectInput(svlFilterInput &input, svlStreamType type);
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlSampleImage* OutputImage;

    vctDynamicVector<vctInt3> Color;
    vctDynamicVector<svlColorSpace> ColorSpace;
    vctDynamicVector<unsigned char> ColorThreshold;
    vctDynamicVector<unsigned char> ColorLabel;

    vctDynamicVector<unsigned char> NormSqrtLUT;
    vctDynamicVector< vctDynamicMatrix<unsigned char> > DistanceMap;

    void ComputeSegmentation(svlSampleImage* image, unsigned int videoch);

    // TO DO: make it public once filter is fully implemented
    void AddColor(svlColorSpace colorspace, int x, int y, int z, unsigned char threshold, unsigned char label);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageColorSegmentation)

#endif // _svlFilterImageColorSegmentation_h

