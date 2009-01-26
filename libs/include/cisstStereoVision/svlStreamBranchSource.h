/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlStreamBranchSource.h,v 1.2 2008/10/17 17:44:38 vagvoba Exp $
  
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

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SMPSRC_BUFFERS          2

class CISST_EXPORT svlStreamBranchSource : public svlFilterBase
{
friend class svlStreamManager;
friend class svlStreamControlMultiThread;

private:
    int NextFreeBufferPos;
    svlSample* DataBuffer[SMPSRC_BUFFERS];
    osaThreadSignal NewFrameEvent;

    osaStopwatch Timer;
    double ulStartTime;
    double ulFrameTime;
    double Hertz;

private:
    svlStreamBranchSource(svlStreamType type);
    svlStreamBranchSource();
    ~svlStreamBranchSource();

    int Initialize(svlSample* inputdata = 0);
    int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    int Release();

    static bool IsTypeSupported(svlStreamType type);
    void SetupSource(svlSample* inputdata, double hertz);
    void PushSample(svlSample* inputdata);
    int PullSample();
};

#endif // _svlStreamBranchSource_h

