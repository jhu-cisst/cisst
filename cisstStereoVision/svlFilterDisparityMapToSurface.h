/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterDisparityMapToSurface_h
#define _svlFilterDisparityMapToSurface_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlCameraGeometry.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterDisparityMapToSurface : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterDisparityMapToSurface();

    int SetCameraGeometry(const svlCameraGeometry & geometry);
    int SetROI(const svlRect & rect);
    int SetROI(int left, int top, int right, int bottom);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlSampleImage3DMap OutputSurface;
    svlCameraGeometry Geometry;
    svlRect ROI;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterDisparityMapToSurface)

#endif // _svlFilterDisparityMapToSurface_h

