/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlUnsharpMask.h 105 2009-03-04 02:07:02Z bvagvol1 $
  
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

#ifndef _svlFilterUnsharpMask_h
#define _svlFilterUnsharpMask_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterUnsharpMask : public svlFilterBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterUnsharpMask();
    virtual ~svlFilterUnsharpMask();

    int SetAmount(int amount);
    int SetRadius(int radius);
    int SetThreshold(int threshold);

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    int Amount;
    int Radius;
    int Threshold;

    void FilterBlur(unsigned char* img_in, unsigned char* img_out, const int width, const int height, int radius);
    void Sharpening(unsigned char* img_in, unsigned char* img_mask, unsigned char* img_out, const int width, const int height);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterUnsharpMask)

#endif // _svlFilterUnsharpMask_h

