/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstOSAbstraction/osaCriticalSection.h>


/*************************************/
/*** osaCriticalSection class ********/
/*************************************/

osaCriticalSection::osaCriticalSection()
{
#if (CISST_OS == CISST_WINDOWS)
    ::InitializeCriticalSection(&csHandle);
#endif
}

osaCriticalSection::~osaCriticalSection()
{
#if (CISST_OS == CISST_WINDOWS)
    ::DeleteCriticalSection(&csHandle);
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
    ::EnterCriticalSection(&csHandle);
#endif
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    Mutex.Lock();
#endif
}

void osaCriticalSection::Leave()
{
#if (CISST_OS == CISST_WINDOWS)
    ::LeaveCriticalSection(&csHandle);
#endif
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    Mutex.Unlock();
#endif
}

