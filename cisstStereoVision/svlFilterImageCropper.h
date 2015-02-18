/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageCropper_h
#define _svlFilterImageCropper_h

#include <cisstStereoVision/svlFilterImageCenterFinder.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageCropper : public svlFilterBase, public svlFilterImageCenterFinderInterface
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageCropper();
    virtual ~svlFilterImageCropper();

    int SetRectangle(int left, int top, int right, int bottom, unsigned int videoch = SVL_LEFT);
    int SetRectangle(const svlRect & rect, unsigned int videoch = SVL_LEFT);
    int SetCorner(int x, int y, unsigned int videoch = SVL_LEFT);
    int SetCenter(int x, int y, unsigned int videoch = SVL_LEFT);
    svlRect GetRectangle(unsigned int videoch = SVL_LEFT);

    // Inherited from svlFilterImageCenterFinderInterface
    virtual int OnChangeCenter(int x, int y, unsigned int videoch = SVL_LEFT);
    virtual int OnChangeCenterRect(const svlRect & rect, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImage* OutputImage;

    vctFixedSizeVector<svlRect, 2> Rectangles;
    vctFixedSizeVector<bool, 2> Enabled;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageCropper)

#endif // _svlFilterImageCropper_h

