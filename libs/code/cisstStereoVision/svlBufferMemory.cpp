/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstStereoVision/svlBufferMemory.h>


/*********************************/
/*** svlBufferMemory class *******/
/*********************************/

svlBufferMemory::svlBufferMemory()
{
}

svlBufferMemory::svlBufferMemory(unsigned int size) :
    Next(1),
    Latest(0),
    Locked(2)
{
    Buffer.SetSize(3, size);
}

unsigned int svlBufferMemory::GetSize()
{
    return Buffer.cols();
}

unsigned char* svlBufferMemory::GetPushBuffer()
{
    return Buffer.Row(Next).Pointer();
}

unsigned char* svlBufferMemory::GetPushBuffer(unsigned int& size)
{
    size = Buffer.cols();
    return Buffer.Row(Next).Pointer();
}

void svlBufferMemory::Push()
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

    NewFrameEvent.Raise();
}

bool svlBufferMemory::Push(unsigned char* buffer, unsigned int size)
{
    unsigned int datasize = Buffer.cols();
    if (buffer == 0 || size < datasize) return false;

    bool ret = true;

    // Copy image to buffer
    memcpy(Buffer.Row(Next).Pointer(), buffer, datasize);

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

    NewFrameEvent.Raise();

    return ret;
}

unsigned char* svlBufferMemory::Pull(bool waitfornew, double timeout)
{
    if (!waitfornew) return Buffer.Row(Latest).Pointer();

    if (!NewFrameEvent.Wait(timeout)) return 0;

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

    return Buffer.Row(Locked).Pointer();
}

