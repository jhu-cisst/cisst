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

#include "svlSampleBuffer.h"
#include <cisstOSAbstraction/osaSleep.h>
#include <string.h> // for memcpy


/*********************************/
/*** svlSampleBuffer class *******/
/*********************************/

svlSampleBuffer::svlSampleBuffer(svlStreamType type, int buffersize) :
    Type(type),
    BufferSize(std::max(3, buffersize)), // buffer size is greater or equal to 3
    LockedPos(-1),
    Tail(-1),
    Head(0),
    BufferUsage(0),
    DroppedSamples(0)
{
    int i;

    Buffer.SetSize(BufferSize);
    Buffer.SetAll(0);
    Used.SetSize(BufferSize);
    Used.SetAll(false);
    BackwardPos.SetSize(BufferSize);
    ForwardPos.SetSize(BufferSize);

    // Create samples in the buffer
    for (i = 0; i < BufferSize; i ++) {
        Buffer[i] = svlSample::GetNewFromType(type);
    }

    // Initialize chained list of samples
    for (i = 1; i < BufferSize; i ++) {
        BackwardPos[i] = i - 1;
        ForwardPos[i - 1] = i;
    }
    BackwardPos[0] = BufferSize - 1;
    ForwardPos[BufferSize - 1] = 0;
}

svlSampleBuffer::~svlSampleBuffer()
{
    for (int i = 0; i < BufferSize; i ++) {
        if (Buffer[i]) delete Buffer[i];
    }
}

svlStreamType svlSampleBuffer::GetType()
{
    return Type;
}

int svlSampleBuffer::GetBufferSize()
{
    return BufferSize;
}

int svlSampleBuffer::GetBufferUsage()
{
    return BufferUsage;
}

double svlSampleBuffer::GetBufferUsageRatio()
{
    return static_cast<double>(BufferUsage) / (BufferSize - 1);
}

int svlSampleBuffer::GetDroppedSampleCount()
{
    return DroppedSamples;
}

bool svlSampleBuffer::PreAllocate(const svlSample & sample)
{
    for (int i = 0; i < BufferSize; i ++) {
        if (Buffer[i]->SetSize(sample) != SVL_OK) return false;
    }
    return true;
}

bool svlSampleBuffer::Push(const svlSample & sample)
{
    bool dropped = Used[Head];

    if (Buffer[Head]->CopyOf(sample) != SVL_OK) return false;

    CS.Enter();
        Head = Tail = ForwardPos[Head];
        if (dropped) DroppedSamples ++;
        else BufferUsage ++;
    CS.Leave();

    NewSampleEvent.Raise();

    return true;
}

svlSample* svlSampleBuffer::Pull(double timeout)
{
    // Make sure a new frame has arrived since the last call
    if (BufferUsage < 1 && !NewSampleEvent.Wait(timeout)) return SVL_FAIL;

    CS.Enter();
        if (LockedPos >= 0) {
            ForwardPos[LockedPos] = ForwardPos[Tail];
            BackwardPos[LockedPos] = BackwardPos[Tail];
            ForwardPos[BackwardPos[Tail]] = LockedPos;
            BackwardPos[ForwardPos[Tail]] = LockedPos;

            LockedPos = Tail;
            Tail = ForwardPos[Tail];
            BufferUsage --;
        }
        else {
            // First pulled frame
            LockedPos = 0;
            Tail = 1;
            BufferUsage --;
        }
    CS.Leave();

    return Buffer[LockedPos];
}

