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

#include <cisstStereoVision/svlSampleQueue.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <string.h> // for memcpy


/********************************/
/*** svlSampleQueue class *******/
/********************************/

svlSampleQueue::svlSampleQueue(svlStreamType type, int length) :
    Type(type),
    Length(std::max(3, length)), // buffer size is greater or equal to 3
    LockedPos(0),
    Tail(1),
    Head(1),
    BufferUsage(0),
    DroppedSamples(0)
{
    int i;

    Buffer.SetSize(Length);
    Buffer.SetAll(0);
    BackwardPos.SetSize(Length);
    ForwardPos.SetSize(Length);

    // Create samples in the buffer
    for (i = 0; i < Length; i ++) {
        Buffer[i] = svlSample::GetNewFromType(type);
    }

    // Initialize chained list of samples
    for (i = 2; i < Length; i ++) {
        BackwardPos[i] = i - 1;
        ForwardPos[i - 1] = i;
    }
    BackwardPos[1] = Length - 1;
    ForwardPos[Length - 1] = 1;
}

svlSampleQueue::svlSampleQueue() :
    Type(svlTypeInvalid),
    Length(0)
{
}

svlSampleQueue::~svlSampleQueue()
{
    for (int i = 0; i < Length; i ++) {
        if (Buffer[i]) delete Buffer[i];
    }
}

svlStreamType svlSampleQueue::GetType()
{
    return Type;
}

int svlSampleQueue::GetLength()
{
    return Length;
}

int svlSampleQueue::GetUsage()
{
    return BufferUsage;
}

double svlSampleQueue::GetUsageRatio()
{
    return static_cast<double>(BufferUsage) / (Length - 1);
}

int svlSampleQueue::GetDroppedSampleCount()
{
    return DroppedSamples;
}

bool svlSampleQueue::PreAllocate(const svlSample & sample)
{
    for (int i = 0; i < Length; i ++) {
        if (Buffer[i]->SetSize(sample) != SVL_OK) return false;
    }
    return true;
}

bool svlSampleQueue::Push(const svlSample & sample)
{
    if (Buffer[Head]->CopyOf(sample) != SVL_OK) return false;

    CS.Enter();
        Head = ForwardPos[Head];
        if (Head != Tail) {
            BufferUsage ++;
        }
        else {
            Tail = ForwardPos[Head];
            DroppedSamples ++;
        }
    CS.Leave();

    NewSampleEvent.Raise();

    return true;
}

svlSample* svlSampleQueue::Pull(double timeout)
{
    // Make sure a new frame has arrived since the last call
    if (BufferUsage <= 1) {
        if (!NewSampleEvent.Wait(timeout)) {
            return 0;
        }
    }

    CS.Enter();
        // Swap "tail" with "LockedPos" in the chained list
        ForwardPos[LockedPos] = ForwardPos[Tail];
        BackwardPos[LockedPos] = BackwardPos[Tail];
        ForwardPos[BackwardPos[Tail]] = LockedPos;
        BackwardPos[ForwardPos[Tail]] = LockedPos;

        LockedPos = Tail;
        Tail = ForwardPos[Tail];

        BufferUsage --;
    CS.Leave();

    return Buffer[LockedPos];
}

svlSample* svlSampleQueue::Peek()
{
    return Buffer[LockedPos];
}

