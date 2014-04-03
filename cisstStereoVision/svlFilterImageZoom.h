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

#ifndef _svlFilterImageZoom_h
#define _svlFilterImageZoom_h

#include <cisstStereoVision/svlFilterImageCenterFinder.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageZoom : public svlFilterBase, public svlFilterImageCenterFinderInterface
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageZoom();
    virtual ~svlFilterImageZoom();

    int SetZoom(double zoom, unsigned int videoch = SVL_LEFT);
    double GetZoom(unsigned int videoch = SVL_LEFT);
    int SetCenter(int x, int y, unsigned int videoch = SVL_LEFT);
    int GetCenter(int & x, int & y, unsigned int videoch = SVL_LEFT);
    void SetInterpolation(bool enable);

    // Inherited from svlFilterImageCenterFinderInterface
    virtual int OnChangeCenter(int x, int y, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImage* OutputImage;

    bool InterpolationEnabled;
    vctFixedSizeVector<svlPoint2D, SVL_MAX_CHANNELS> Center;
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> Zoom;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageZoom)

#endif // _svlFilterImageZoom_h

