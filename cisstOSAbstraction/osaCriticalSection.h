/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Min Yang Jung
  Created on: 2008

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
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

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
#include <cisstOSAbstraction/osaMutex.h>
#endif


// Always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaCriticalSection
{
    friend class osaMutexTest;

#if (CISST_OS == CISST_WINDOWS)
    // for Windows only, hide critical section handle in internals
    enum {INTERNALS_SIZE = 64};
    char Internals[INTERNALS_SIZE];
#endif
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    enum {INTERNALS_SIZE = 0};
    osaMutex Mutex;
#endif

    /*! Return the size of the actual object used by the OS.  This is
      used for testing only. */
    static unsigned int SizeOfInternals(void);

 public:
    osaCriticalSection();
    ~osaCriticalSection();

    bool TryEnter(void);
    void Enter(void);
    void Leave(void);
};

#endif // _osaCriticalSection_h
