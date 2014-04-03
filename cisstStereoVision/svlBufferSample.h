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

#ifndef _svlBufferSample_h
#define _svlBufferSample_h

#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstStereoVision/svlTypes.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif 

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlBufferSample
{
public:
    svlBufferSample(svlStreamType type);
    //creates a new buffer based on an existing sample prototype (including size)
    svlBufferSample(const svlSample &sample);
    ~svlBufferSample();

    svlStreamType GetType() const;

    int Push(const svlSample* sample);
    svlSample* GetPushBuffer();
    void Push();

    svlSample* Pull(bool waitfornew, double timeout = 5.0);

private:
    svlBufferSample();

#if (CISST_OS == CISST_WINDOWS)
    LONG Next, Latest, Locked;
#else
    unsigned int Next, Latest, Locked;
#endif
    osaThreadSignal NewSampleEvent;
    svlSample* Buffer[3];

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    osaCriticalSection CS;
#endif
};

#endif // _svlBufferSample_h

