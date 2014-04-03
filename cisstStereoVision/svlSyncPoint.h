/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlSyncPoint_h
#define _svlSyncPoint_h

#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstOSAbstraction/osaCriticalSection.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSyncPoint
{
public:
    svlSyncPoint();
    virtual ~svlSyncPoint();

    int Count(unsigned int count);
    unsigned int Count();
    int Sync(unsigned int _id);
    void ReleaseAll();

private:
    unsigned int ThreadCount;
    int LastChanged;
    unsigned int CheckedInCounter;
    osaThreadSignal* ReleaseEvent;
    osaCriticalSection CS;
};

#endif // _svlSyncPoint_h

