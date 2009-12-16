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

svlSampleBuffer::svlSampleBuffer(svlStreamType type) :
    Type(type)
{
    for (unsigned int i = 0; i < 3; i ++) {
        Buffer[i] = svlSample::GetNewFromType(type);
    }

    Latest = 0;
    Next = 1;
    Locked = 2;
}

svlSampleBuffer::~svlSampleBuffer()
{
    for (unsigned int i = 0; i < 3; i ++) {
        delete Buffer[i];
    }
}

svlStreamType svlSampleBuffer::GetType()
{
    return Type;
}

bool svlSampleBuffer::Push(const svlSample & sample)
{
    if (Buffer[Next]->CopyOf(sample) != SVL_OK) return false;

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

    return true;
}

svlSample* svlSampleBuffer::Pull(bool waitfornew, double timeout)
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

