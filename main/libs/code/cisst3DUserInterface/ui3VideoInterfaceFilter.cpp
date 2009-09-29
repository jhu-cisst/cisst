/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VideoInterfaceFilter.cpp 137 2009-03-11 18:51:15Z adeguet1 $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-06-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisst3DUserInterface/ui3VideoInterfaceFilter.h>
#include <cisst3DUserInterface/ui3BehaviorBase.h>


ui3VideoInterfaceFilter::ui3VideoInterfaceFilter(svlStreamType streamtype, int streamid, ui3BehaviorBase* behavior) :
    svlFilterBase(),
    StreamID(streamid),
    ParentBehavior(behavior)
{
    CMN_ASSERT(behavior);
    AddSupportedType(streamtype, streamtype);
}

ui3VideoInterfaceFilter::~ui3VideoInterfaceFilter()
{
}

int ui3VideoInterfaceFilter::Initialize(svlSample* inputdata)
{
    OutputData = inputdata;
    return SVL_OK;
}

int ui3VideoInterfaceFilter::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    // for now, ui3BehaviorBase::OnStreamSample remains single threaded for the
    // sake of simplicity but we can make it multithreaded later by simply
    // passing the procInfo to it
    _OnSingleThread(procInfo) {
        ParentBehavior->OnStreamSample(inputdata, StreamID);
    }
    return SVL_OK;
}

unsigned int ui3VideoInterfaceFilter::GetWidth(unsigned int videoch)
{
    if (!OutputData || !OutputData->IsImage()) return 0;
    return dynamic_cast<svlSampleImageBase*>(OutputData)->GetWidth(videoch);
}

unsigned int ui3VideoInterfaceFilter::GetHeight(unsigned int videoch)
{
    if (!OutputData || !OutputData->IsImage()) return 0;
    return dynamic_cast<svlSampleImageBase*>(OutputData)->GetHeight(videoch);
}
