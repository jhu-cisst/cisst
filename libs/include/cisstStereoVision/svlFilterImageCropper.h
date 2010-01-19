/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageCropper.h 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterImageCropper : public svlFilterBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterImageCropper();
    virtual ~svlFilterImageCropper();

    void SetRectangle(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int videoch = SVL_LEFT);
    void SetRectangle(const svlRect & rect, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    unsigned int SetLeft[2];
    unsigned int SetRight[2];
    unsigned int SetTop[2];
    unsigned int SetBottom[2];
    unsigned int Left[2];
    unsigned int Right[2];
    unsigned int Top[2];
    unsigned int Bottom[2];

    void CheckAndFixRectangle(unsigned int videoch, unsigned int width, unsigned int height);
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterImageCropper)

#endif // _svlFilterImageCropper_h

