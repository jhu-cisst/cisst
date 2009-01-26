/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlStreamTypeConverter.h,v 1.6 2008/10/17 17:44:38 vagvoba Exp $
  
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

#ifndef _svlStreamTypeConverter_h
#define _svlStreamTypeConverter_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlStreamTypeConverter : public svlFilterBase
{
public:
    svlStreamTypeConverter(svlStreamType inputtype, svlStreamType outputtype);
    virtual ~svlStreamTypeConverter();

    void SetDistanceIntensityRatio(float ratio) { DistanceScaling = ratio; }
    float GetDistanceIntensityRatio() { return DistanceScaling; }
    void SetMono16ShiftDown(unsigned int shiftdown) { Mono16ShiftDown = shiftdown; }
    unsigned int GetMono16ShiftDown() { return Mono16ShiftDown; }

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    float DistanceScaling;
    unsigned int Mono16ShiftDown;
};

#endif // _svlStreamTypeConverter_h

