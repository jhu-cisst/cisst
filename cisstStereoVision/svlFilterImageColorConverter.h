/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlFilterImageColorConverter_h
#define _svlFilterImageColorConverter_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageColorConverter : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageColorConverter();

    void SetConversion(svlColorSpace input, svlColorSpace output);
    void SetChannelMask(bool ch1, bool ch2, bool ch3);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlColorSpace InputColorSpace;
    svlColorSpace OutputColorSpace;
    bool Channel1, Channel2, Channel3;

    void ConvertColorSpace(unsigned char* buffer, unsigned int numofpixels);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageColorConverter)

#endif // _svlFilterImageColorConverter_h

