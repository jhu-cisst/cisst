/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi, Min Yang Jung
  Created on: 2008 

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _osaCriticalSection_h
#define _osaCriticalSection_h

#include <cisstCommon/cmnPortability.h>

#if (CISST_OS == CISST_WINDOWS)
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0400
    #endif // _WIN32_WINNT
    #include <windows.h>
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    #include <cisstOSAbstraction/osaMutex.h>
#endif

// Always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaCriticalSection
{
public:
    osaCriticalSection();
    ~osaCriticalSection();

    bool TryEnter();
    void Enter();
    void Leave();

private:
#if (CISST_OS == CISST_WINDOWS)
    CRITICAL_SECTION csHandle;
#endif
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    osaMutex Mutex;
#endif
};

#endif // _osaCriticalSection_h
