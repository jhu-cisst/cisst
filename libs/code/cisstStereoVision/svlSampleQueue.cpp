/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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


/********************************/
/*** svlSampleQueue class *******/
/********************************/

svlSampleQueue::svlSampleQueue(svlStreamType type, int length) :
    Type(type),
    Length(std::max(3, length)), // buffer size is greater or equal to 3
    Tail(0),
    Head(0),
    BufferUsage(0),
    DroppedSamples(0)
{
    Buffer.SetSize(Length);

    // Create samples in the buffer
    ReadBuffer = svlSample::GetNewFromType(type);
    for (int i = 0; i < Length; i ++) {
        Buffer[i] = svlSample::GetNewFromType(type);
    }
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
    if (ReadBuffer) delete ReadBuffer;
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

bool svlSampleQueue::PreAllocate(const svlSample* sample)
{
    if (ReadBuffer->SetSize(sample) != SVL_OK) return false;
    for (int i = 0; i < Length; i ++) {
        if (Buffer[i]->SetSize(sample) != SVL_OK) return false;
    }
    return true;
}

bool svlSampleQueue::Push(const svlSample* sample)
{
    CS.Enter();

        Head ++;
        if (Head >= Length) Head = 0;

        if (Head == Tail) {
            Tail ++;
            if (Tail >= Length) Tail = 0;

            DroppedSamples ++;
        }
        else {
            BufferUsage ++;
        }

        if (BufferUsage == 1) NewSampleEvent.Raise();

    CS.Leave();

    if (Buffer[Head]->CopyOf(sample) != SVL_OK) return false;

    return true;
}

svlSample* svlSampleQueue::Pull(double timeout)
{
    CS.Enter();

        if (Tail == Head) {
            CS.Leave();
            NewSampleEvent.Wait(timeout);
            return 0;
        }

        // Swap 'ReadBuffer' with 'Tail'
        svlSample* temp = ReadBuffer;
        ReadBuffer = Buffer[Tail];
        Buffer[Tail] = temp;

        Tail ++; if (Tail >= Length) Tail = 0;

        BufferUsage --;

    CS.Leave();

    return ReadBuffer;
}

svlSample* svlSampleQueue::Peek()
{
    return ReadBuffer;
}

