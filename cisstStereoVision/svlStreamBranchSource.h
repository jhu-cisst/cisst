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

#ifndef _svlStreamBranchSource_h
#define _svlStreamBranchSource_h

#include <cisstStereoVision/svlFilterSourceBase.h>
#include <cisstStereoVision/svlSampleQueue.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class svlStreamBranchSource : public svlFilterSourceBase
{
friend class svlStreamManager;
friend class svlFilterOutput;
friend class svlStreamProc;

private:
    svlStreamBranchSource(svlStreamType type, unsigned int buffersize);
    svlStreamBranchSource();
    ~svlStreamBranchSource();

    int Initialize(svlSample* &syncOutput);
    int Process(svlProcInfo* procInfo, svlSample* &syncOutput);

    static bool IsTypeSupported(svlStreamType type);
    void SetInput(svlSample* syncInput);
    void PushSample(const svlSample* syncInput);

    bool InputBlocked;
    svlSampleQueue SampleQueue;
    svlSample* OutputSample;

public:
    int GetBufferUsage();
    double GetBufferUsageRatio();
    unsigned int GetDroppedSampleCount();
    int BlockInput(bool block);
};

#endif // _svlStreamBranchSource_h

