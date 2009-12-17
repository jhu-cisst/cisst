/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlSampleBuffer_h
#define _svlSampleBuffer_h

#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstStereoVision/svlStreamDefs.h>


class svlSampleBuffer
{
public:
    svlSampleBuffer(svlStreamType type, int buffersize);
    ~svlSampleBuffer();

    svlStreamType GetType();
    int GetBufferSize();
    int GetBufferUsage();
    double GetBufferUsageRatio();
    int GetDroppedSampleCount();

    bool PreAllocate(const svlSample & sample);

    bool Push(const svlSample & sample);
    svlSample* Pull(double timeout = 5.0);

private:
    svlSampleBuffer() {}

    svlStreamType Type;
    const int BufferSize;
    vctDynamicVector<bool> Used;
    vctDynamicVector<int> BackwardPos;
    vctDynamicVector<int> ForwardPos;
    int LockedPos;
    int Tail;
    int Head;
    int BufferUsage;
    int DroppedSamples;
    vctDynamicVector<svlSample*> Buffer;
    osaCriticalSection CS;
    osaThreadSignal NewSampleEvent;
};

#endif // _svlSampleBuffer_h

