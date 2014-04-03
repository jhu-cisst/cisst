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

#include <cisstStereoVision/svlSyncPoint.h>
#include <cisstStereoVision/svlDefinitions.h>


/*************************************/
/*** svlSyncPoint class **************/
/*************************************/

// *******************************************************************
// svlSyncPoint::svlSyncPoint Constructor
// arguments:
// *******************************************************************
svlSyncPoint::svlSyncPoint() :
    ThreadCount(2),
    LastChanged(-1)
{
    CheckedInCounter = ThreadCount;
    ReleaseEvent = new osaThreadSignal[ThreadCount];
}

// *******************************************************************
// svlSyncPoint::~svlSyncPoint Destructor
// *******************************************************************
svlSyncPoint::~svlSyncPoint()
{
    delete [] ReleaseEvent;
}

// *******************************************************************
// svlSyncPoint::Count method
// arguments:
//           count          - thread count, must be greater than 1
// function:
//    Sets or returns the number of threads to be synchronized.
//    This method is not thread safe.
// *******************************************************************
int svlSyncPoint::Count(unsigned int count)
{
    if (count < 2) return SVL_SYNC_ERROR;

    ThreadCount = count;
    CheckedInCounter = ThreadCount;
    LastChanged = -1;

    delete [] ReleaseEvent;
    ReleaseEvent = new osaThreadSignal[ThreadCount];

    return SVL_SYNC_OK;
}

unsigned int svlSyncPoint::Count()
{
    return ThreadCount;
}

// *******************************************************************
// svlSyncPoint::Sync method
// arguments:
//           id             - thread ID
// function:
//    Waits until all threads check in, then resumes the
//    execution on all threads.
// *******************************************************************
int svlSyncPoint::Sync(unsigned int _id)
{
    if (_id >= ThreadCount) return SVL_SYNC_ERROR;

    CS.Enter();
        CheckedInCounter --;
        LastChanged = static_cast<int>(_id);

    if (CheckedInCounter == 0 && LastChanged == static_cast<int>(_id)) {
        // It will happen only in exactly one time,
        // after all threads checked in.
        // We can reset states  in order to prepare
        // for the next synchronization cycle and
        // unlock all waiting threads.

        CheckedInCounter = ThreadCount;
        LastChanged = -1;

        for (unsigned int i = 0; i < ThreadCount; i ++) {
            if (i != _id) ReleaseEvent[i].Raise();
        }

        CS.Leave();
    }
    else {
        CS.Leave();

        // We have to wait until all other threads
        // check in.

        ReleaseEvent[_id].Wait();
    }

    return SVL_SYNC_OK;
}

// *******************************************************************
// svlSyncPoint::ReleaseAll method
// function:
//    Un-blocks (releases) all waiting threads.
// *******************************************************************
void svlSyncPoint::ReleaseAll()
{
    CS.Enter();
        for (unsigned int i = 0; i < ThreadCount; i ++) {
            ReleaseEvent[i].Raise();
        }
        
        CheckedInCounter = ThreadCount;
        LastChanged = -1;
    CS.Leave();
}

