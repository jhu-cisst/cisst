/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2008

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstCommon/cmnAssert.h>

#if (CISST_OS == CISST_WINDOWS)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif // _WIN32_WINNT

#include <windows.h>

struct osaCriticalSectionInternals
{
    CRITICAL_SECTION CriticalSectionHandle;
};

#define INTERNALS(A) (reinterpret_cast<osaCriticalSectionInternals*>(Internals)->A)

#endif



osaCriticalSection::osaCriticalSection()
{
#if (CISST_OS == CISST_WINDOWS)
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    ::InitializeCriticalSection(&INTERNALS(CriticalSectionHandle));
#endif
}

osaCriticalSection::~osaCriticalSection()
{
#if (CISST_OS == CISST_WINDOWS)
    ::DeleteCriticalSection(&INTERNALS(CriticalSectionHandle));
#endif
}


unsigned int osaCriticalSection::SizeOfInternals(void)
{
#if (CISST_OS == CISST_WINDOWS)
    return sizeof(osaCriticalSectionInternals);
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    return 0;
#endif
}


/*
  bool osaCriticalSection::TryEnter()
  {
  #if (CISST_OS == CISST_WINDOWS)
  if (::TryEnterCriticalSection(&csHandle) != 0) return true;
  #endif
  #if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
  if (Mutex.TryLock(0) == osaMutex::SUCCESS) return true;
  #endif
  return false;
  }
*/

void osaCriticalSection::Enter()
{
#if (CISST_OS == CISST_WINDOWS)
    ::EnterCriticalSection(&INTERNALS(CriticalSectionHandle));
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    Mutex.Lock();
#endif
}

void osaCriticalSection::Leave()
{
#if (CISST_OS == CISST_WINDOWS)
    ::LeaveCriticalSection(&INTERNALS(CriticalSectionHandle));
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    Mutex.Unlock();
#endif
}

