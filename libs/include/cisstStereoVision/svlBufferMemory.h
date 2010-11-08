/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlBufferMemory.h 1236 2010-02-26 20:38:21Z adeguet1 $
  
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

#ifndef _svlBufferMemory_h
#define _svlBufferMemory_h

#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstOSAbstraction/osaCriticalSection.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlBufferMemory
{
public:
    svlBufferMemory(unsigned int size);

    unsigned int GetSize();

    unsigned char* GetPushBuffer();
    unsigned char* GetPushBuffer(unsigned int& size);
    void Push();
    bool Push(unsigned char* buffer, unsigned int size);

    unsigned char* Pull(bool waitfornew, double timeout = 5.0);

private:
    svlBufferMemory();

#if (CISST_OS == CISST_WINDOWS)
    LONG Next, Latest, Locked;
#else
    unsigned int Next, Latest, Locked;
#endif
    osaThreadSignal NewFrameEvent;
    vctDynamicMatrix<unsigned char> Buffer;

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    osaCriticalSection CS;
#endif
};

#endif // _svlBufferMemory_h

