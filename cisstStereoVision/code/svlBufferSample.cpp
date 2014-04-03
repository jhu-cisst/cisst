/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlBufferSample.h>


/*********************************/
/*** svlBufferSample class *******/
/*********************************/

svlBufferSample::svlBufferSample(svlStreamType type)
{
    Buffer[0] = svlSample::GetNewFromType(type);
    Buffer[1] = svlSample::GetNewFromType(type);
    Buffer[2] = svlSample::GetNewFromType(type);

    Latest = 0;
    Next = 1;
    Locked = 2;
}

svlBufferSample::svlBufferSample(const svlSample &sample)
{
    Buffer[0] = svlSample::GetNewFromType(sample.GetType());
    Buffer[1] = svlSample::GetNewFromType(sample.GetType());
    Buffer[2] = svlSample::GetNewFromType(sample.GetType());

    Buffer[0]->SetSize(sample);
    Buffer[1]->SetSize(sample);
    Buffer[2]->SetSize(sample);

    Latest = 0;
    Next = 1;
    Locked = 2;
}

svlBufferSample::~svlBufferSample()
{
    delete Buffer[0];
    delete Buffer[1];
    delete Buffer[2];
}

svlStreamType svlBufferSample::GetType() const
{
    return Buffer[0]->GetType();
}

int svlBufferSample::Push(const svlSample* sample)
{
    int ret = Buffer[Next]->CopyOf(sample);

    // Atomic exchange of values
#if (CISST_OS == CISST_WINDOWS)
    Next = InterlockedExchange(&Latest, Next);
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    CS.Enter();
        int ti = Next;
        Next = Latest;
        Latest = ti;
    CS.Leave();
#endif

    NewSampleEvent.Raise();

    return ret;
}

svlSample* svlBufferSample::Pull(bool waitfornew, double timeout)
{
    if (!waitfornew) return Buffer[Latest];

    if (!NewSampleEvent.Wait(timeout)) return 0;

    // Atomic exchange of values
#if (CISST_OS == CISST_WINDOWS)
    Locked = InterlockedExchange(&Latest, Locked);
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    CS.Enter();
        int ti = Locked;
        Locked = Latest;
        Latest = ti;
    CS.Leave();
#endif

    return Buffer[Locked];
}

svlSample* svlBufferSample::GetPushBuffer()
{
    return Buffer[Next];
}

void svlBufferSample::Push()
{
    // Atomic exchange of values
#if (CISST_OS == CISST_WINDOWS)
    Next = InterlockedExchange(&Latest, Next);
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    CS.Enter();
        int ti = Next;
        Next = Latest;
        Latest = ti;
    CS.Leave();
#endif

    NewSampleEvent.Raise();
}

