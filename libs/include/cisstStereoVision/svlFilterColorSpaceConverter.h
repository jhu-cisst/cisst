/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlColorSpaceConverter.h 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterColorSpaceConverter_h
#define _svlFilterColorSpaceConverter_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

enum svlColorSpace
{
    svlColorSpaceRGB,
    svlColorSpaceHSV,
    svlColorSpaceHSL,
    svlColorSpaceYUV
};

class CISST_EXPORT svlFilterColorSpaceConverter : public svlFilterBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterColorSpaceConverter();
    virtual ~svlFilterColorSpaceConverter();

    void SetConversion(svlColorSpace input, svlColorSpace output);
    void SetChannelMask(bool ch1, bool ch2, bool ch3);

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    svlColorSpace InputColorSpace;
    svlColorSpace OutputColorSpace;
    bool Channel1, Channel2, Channel3;

    void ConvertColorSpace(unsigned char* buffer, unsigned int numofpixels);
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterColorSpaceConverter)

#endif // _svlFilterColorSpaceConverter_h

