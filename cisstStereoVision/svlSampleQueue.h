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

#ifndef _svlSampleQueue_h
#define _svlSampleQueue_h

#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstStereoVision/svlTypes.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSampleQueue
{
public:
    svlSampleQueue(svlStreamType type, unsigned int size);
    ~svlSampleQueue();

    bool Push(const svlSample* sample);
    svlSample* Pull(double timeout = 5.0);

    svlStreamType GetType();
    unsigned int GetLength();
    unsigned int GetUsage();
    double GetUsageRatio();
    unsigned int GetDroppedSampleCount();
    svlSample* Peek();

private:
    svlStreamType Type;
    unsigned int Size;
    unsigned int DroppedSamples;
    std::list<svlSample*> UnusedItems;
    std::list<svlSample*> BufferedItems;
    svlSample* PullItem;

    osaCriticalSection CS;
    osaThreadSignal NewSampleEvent;
};

/*
class CISST_EXPORT svlSampleQueu2
{
public:
    svlSampleQueu2(svlStreamType type, int length);
    ~svlSampleQueu2();

    svlStreamType GetType();
    int GetLength();
    int GetUsage();
    double GetUsageRatio();
    int GetDroppedSampleCount();

    bool PreAllocate(const svlSample* sample);

    bool Push(const svlSample* sample);
    svlSample* Pull(double timeout = 5.0);
    svlSample* Peek();

private:
    svlSampleQueu2();

    svlStreamType Type;
    const int Length;
    int Tail;
    int Head;
    int BufferUsage;
    int DroppedSamples;
    svlSample* ReadBuffer;
    vctDynamicVector<svlSample*> Buffer;
    osaCriticalSection CS;
    osaThreadSignal NewSampleEvent;
};
*/

#endif // _svlSampleQueue_h

