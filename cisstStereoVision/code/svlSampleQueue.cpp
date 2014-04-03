/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


/****************************/
/*** svlSampleQueue class ***/
/****************************/

svlSampleQueue::svlSampleQueue(svlStreamType type, unsigned int size) :
    Type(type),
    Size(std::max(size, 2u)), // TO DO: check why it doesn't work when min=1
    DroppedSamples(0),
    UnusedItems(Size, 0)
{
    PullItem = svlSample::GetNewFromType(type);
    for (std::list<svlSample*>::iterator it = UnusedItems.begin();
         it != UnusedItems.end();
         ++ it) {
        *it = svlSample::GetNewFromType(type);
    }
}

svlSampleQueue::~svlSampleQueue()
{
    delete PullItem;
    for (std::list<svlSample*>::iterator it = UnusedItems.begin();
         it != UnusedItems.end();
         ++ it) {
        delete *it;
    }
}

bool svlSampleQueue::Push(const svlSample* sample)
{
    if (sample->GetType() != Type) return false;

    svlSample* push_item;

    CS.Enter();
        if (UnusedItems.empty()) {
            push_item = BufferedItems.back();
            BufferedItems.pop_back();
            DroppedSamples ++;
        }
        else {
            push_item = UnusedItems.front();
            UnusedItems.pop_front();
        }
    CS.Leave();

    push_item->CopyOf(sample);

    CS.Enter();
        BufferedItems.push_front(push_item);
        if (BufferedItems.size() == 1) NewSampleEvent.Raise();
    CS.Leave();

    return true;
}

svlSample* svlSampleQueue::Pull(double timeout)
{
    bool is_event_reset = false;

    if (BufferedItems.empty()) {
        if (NewSampleEvent.Wait(timeout) == false) return 0;
        // A successful wait automatically resets the event
        is_event_reset = true;
    }

    CS.Enter();
        UnusedItems.push_front(PullItem);
        PullItem = BufferedItems.back();
        BufferedItems.pop_back();

        // Reset event when buffer is empty
        if (BufferedItems.empty() && !is_event_reset) NewSampleEvent.Wait(0.0);
    CS.Leave();

    return PullItem;
}

svlStreamType svlSampleQueue::GetType()
{
    return Type;
}

unsigned int svlSampleQueue::GetLength()
{
    return Size;
}

unsigned int svlSampleQueue::GetUsage()
{
    return Size - static_cast<unsigned int>(UnusedItems.size());
}

double svlSampleQueue::GetUsageRatio()
{
    return static_cast<double>(GetUsage()) / GetLength();
}

unsigned int svlSampleQueue::GetDroppedSampleCount()
{
    return DroppedSamples;
}

svlSample* svlSampleQueue::Peek()
{
    return PullItem;
}

/*
svlSampleQueu2::svlSampleQueu2(svlStreamType type, int length) :
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

svlSampleQueu2::svlSampleQueu2() :
    Type(svlTypeInvalid),
    Length(0)
{
}

svlSampleQueu2::~svlSampleQueu2()
{
    for (int i = 0; i < Length; i ++) {
        if (Buffer[i]) delete Buffer[i];
    }
    if (ReadBuffer) delete ReadBuffer;
}

svlStreamType svlSampleQueu2::GetType()
{
    return Type;
}

int svlSampleQueu2::GetLength()
{
    return Length;
}

int svlSampleQueu2::GetUsage()
{
    return BufferUsage;
}

double svlSampleQueu2::GetUsageRatio()
{
    return static_cast<double>(BufferUsage) / (Length - 1);
}

int svlSampleQueu2::GetDroppedSampleCount()
{
    return DroppedSamples;
}

bool svlSampleQueu2::PreAllocate(const svlSample* sample)
{
    if (ReadBuffer->SetSize(sample) != SVL_OK) return false;
    for (int i = 0; i < Length; i ++) {
        if (Buffer[i]->SetSize(sample) != SVL_OK) return false;
    }
    return true;
}

bool svlSampleQueu2::Push(const svlSample* sample)
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

std::cerr << "svlSampleQueu2::Push: " << std::fixed << sample->GetTimestamp() << std::endl;
    if (Buffer[Head]->CopyOf(sample) != SVL_OK) return false;

    return true;
}

svlSample* svlSampleQueu2::Pull(double timeout)
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

std::cerr << "svlSampleQueu2::Pull: " << std::fixed << ReadBuffer->GetTimestamp() << std::endl;
    return ReadBuffer;
}

svlSample* svlSampleQueu2::Peek()
{
    return ReadBuffer;
}
*/

