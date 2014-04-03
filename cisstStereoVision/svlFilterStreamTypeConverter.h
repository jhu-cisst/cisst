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

#ifndef _svlFilterStreamTypeConverter_h
#define _svlFilterStreamTypeConverter_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterStreamTypeConverter : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterStreamTypeConverter();
    svlFilterStreamTypeConverter(svlStreamType inputtype, svlStreamType outputtype);
    virtual ~svlFilterStreamTypeConverter();

    int SetType(svlStreamType inputtype, svlStreamType outputtype);
    void SetScaling(float ratio) { Scaling = ratio; }
    float GetScaling() { return Scaling; }
    void SetMono16ShiftDown(unsigned int shiftdown) { Mono16ShiftDown = shiftdown; }
    unsigned int GetMono16ShiftDown() { return Mono16ShiftDown; }

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    svlSample* OutputSample;

    float Scaling;
    unsigned int Mono16ShiftDown;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterStreamTypeConverter)

#endif // _svlFilterStreamTypeConverter_h

