/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterPointTracker.h>
#include <string.h>

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

using namespace std;

/******************************************/
/*** svlFilterPointTracker class **********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterPointTracker)

svlFilterPointTracker::svlFilterPointTracker() :
    svlFilterBase(),
    cmnGenericObject()
{
    AddSupportedType(svlTypeImageRGB, svlTypePointCloud);
    AddSupportedType(svlTypeImageRGBStereo, svlTypePointCloud);

    svlSamplePointCloud* points = new svlSamplePointCloud;

    OutputData = points;

    Tracker[SVL_LEFT] = Tracker[SVL_RIGHT] = 0;
    Target[SVL_LEFT] = Target[SVL_RIGHT] = 0;
    ForcedTarget[SVL_LEFT] = ForcedTarget[SVL_RIGHT] = 0;
    WALeft[SVL_LEFT] = WALeft[SVL_RIGHT] = 0;
    WARight[SVL_LEFT] = WARight[SVL_RIGHT] = 32000;
    WATop[SVL_LEFT] = WATop[SVL_RIGHT] = 0;
    WABottom[SVL_LEFT] = WABottom[SVL_RIGHT] = 32000;

    ResetFlag[0] = ResetFlag[1] = 0;
    FramesToSkip = 0;
    MovingAverageWeight = 0.0;
    Iterations = 1;
}

svlFilterPointTracker::~svlFilterPointTracker()
{
    ReleaseTargets(SVL_LEFT);
    ReleaseTargets(SVL_RIGHT);
    Release();

    if (OutputData) delete OutputData;
}

void svlFilterPointTracker::SetMovingAverageSmoothing(double weight)
{
    if (weight < 0.0) MovingAverageWeight = 0.0;
    else MovingAverageWeight = weight;
}

int svlFilterPointTracker::SetTrackerAlgo(svlPointTrackerAlgoBase* tracker, unsigned int targetcount, unsigned int videoch)
{
    if (GetInputType() == svlTypeImageRGB) videoch = SVL_LEFT;
    else if (videoch > 1) return SVL_FAIL;
    if (IsInitialized()) return SVL_ALREADY_INITIALIZED;
    if (tracker == 0) return SVL_FAIL;

    Tracker[videoch] = tracker;
    tracker->SetTargetCount(targetcount);
    AllocateTargets(targetcount, videoch);

    return SVL_OK;
}

int svlFilterPointTracker::SetWorkArea(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int videoch)
{
    if (GetInputType() == svlTypeImageRGB) videoch = SVL_LEFT;
    else if (videoch > 1) return SVL_FAIL;
    if (IsInitialized()) return SVL_ALREADY_INITIALIZED;

    WALeft[videoch] = left;
    WARight[videoch] = right;
    WATop[videoch] = top;
    WABottom[videoch] = bottom;

    return SVL_OK;
}

int svlFilterPointTracker::SetTarget(unsigned int targetid, int x, int y, unsigned int videoch)
{
    if (GetInputType() == svlTypeImageRGB) videoch = SVL_LEFT;
    else if (videoch > 1) return SVL_FAIL;
    if (Tracker[videoch] == 0) return SVL_FAIL;
    if (targetid >= Tracker[videoch]->GetTargetCount()) return SVL_FAIL;

    // these new positions will be applied at the next initialization or on tracker reset
    ForcedTarget[videoch][targetid].x = x;
    ForcedTarget[videoch][targetid].y = y;

    return SVL_FAIL;
}

void svlFilterPointTracker::SetIterations(unsigned int count)
{
    if (count < 1) count = 1;
    if (count > 100) count = 100;
    Iterations = count;
}

int svlFilterPointTracker::Initialize(svlSample* inputdata)
{
    unsigned int i, targetcount;
    svlSamplePointCloud* points = dynamic_cast<svlSamplePointCloud*>(OutputData);

    Release();

    if (Tracker[SVL_LEFT] == 0) return SVL_FAIL;

    if (GetInputType() == svlTypeImageRGB) {
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(inputdata);
        targetcount = Tracker[SVL_LEFT]->GetTargetCount();

        // Creating ouptut data structure
        points->SetSize(2, targetcount);
        points->points.SetAll(0.0);

        // Set image parameters in trackers
        Tracker[SVL_LEFT]->SetImageSize(img->GetWidth(), img->GetHeight());
        Tracker[SVL_LEFT]->SetWorkArea(WALeft[SVL_LEFT], WATop[SVL_LEFT], WARight[SVL_LEFT], WABottom[SVL_LEFT]);

        for (i = 0; i < targetcount; i ++) {
            // Initializing forced positions
            Target[SVL_LEFT][i] = ForcedTarget[SVL_LEFT][i];

            // Storing temporary results until tracking starts
            points->points.Element(0, i) = Target[SVL_LEFT][i].x;
            points->points.Element(1, i) = Target[SVL_LEFT][i].y;

            // Adding targets to trackers
            Tracker[SVL_LEFT]->SetTarget(i, Target[SVL_LEFT] + i);
        }

        // Initializing trackers
        Tracker[SVL_LEFT]->Initialize();

        FrameCount[SVL_LEFT] = -1;
    }
    else {
        if (Tracker[SVL_RIGHT] == 0) return SVL_FAIL;

        svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(inputdata);

        targetcount = Tracker[SVL_LEFT]->GetTargetCount();
        if (targetcount != Tracker[SVL_RIGHT]->GetTargetCount()) return SVL_FAIL;

        // Creating ouptut data structure
        points->SetSize(4, targetcount);
        points->points.SetAll(0.0);

        // Set image parameters in trackers
        Tracker[SVL_LEFT]->SetImageSize(stimg->GetWidth(SVL_LEFT), stimg->GetHeight(SVL_LEFT));
        Tracker[SVL_LEFT]->SetWorkArea(WALeft[SVL_LEFT], WATop[SVL_LEFT], WARight[SVL_LEFT], WABottom[SVL_LEFT]);
        Tracker[SVL_RIGHT]->SetImageSize(stimg->GetWidth(SVL_LEFT), stimg->GetHeight(SVL_LEFT));
        Tracker[SVL_RIGHT]->SetWorkArea(WALeft[SVL_RIGHT], WATop[SVL_RIGHT], WARight[SVL_RIGHT], WABottom[SVL_RIGHT]);

        for (i = 0; i < targetcount; i ++) {
            // Initializing forced positions
            Target[SVL_LEFT][i] = ForcedTarget[SVL_LEFT][i];
            Target[SVL_RIGHT][i] = ForcedTarget[SVL_RIGHT][i];

            // Storing temporary results until tracking starts
            points->points.Element(0, i) = Target[SVL_LEFT][i].x;
            points->points.Element(1, i) = Target[SVL_LEFT][i].y;
            points->points.Element(2, i) = Target[SVL_RIGHT][i].x;
            points->points.Element(3, i) = Target[SVL_RIGHT][i].y;

            // Adding targets to trackers
            Tracker[SVL_LEFT]->SetTarget(i, Target[SVL_LEFT] + i);
            Tracker[SVL_RIGHT]->SetTarget(i, Target[SVL_RIGHT] + i);
        }

        // Initializing trackers
        Tracker[SVL_LEFT]->Initialize();
        Tracker[SVL_RIGHT]->Initialize();

        FrameCount[SVL_LEFT] = FrameCount[SVL_RIGHT] = -1;
    }

    return SVL_OK;
}

int svlFilterPointTracker::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx, j;
    int err = SVL_OK;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        FrameCount[idx] ++;
        if (FrameCount[idx] < FramesToSkip) continue; // Skip frame

        if (ResetFlag[idx] != 0) {
            ResetToForcedPositions(idx);
            ResetFlag[idx] = 0;
        }

        // Process data
        Tracker[idx]->PreProcessImage(img->GetUCharPointer(idx));
        for (j = 0; j < Iterations; j ++) {
            Tracker[idx]->Track(img->GetUCharPointer(idx));
        }
        err = StoreResults(idx);
        if (err != SVL_OK) return err;
    }

    return SVL_OK;
}

int svlFilterPointTracker::Release()
{
    if (Tracker[SVL_LEFT]) Tracker[SVL_LEFT]->Release();
    if (Tracker[SVL_RIGHT]) Tracker[SVL_RIGHT]->Release();
    return SVL_OK;
}

int svlFilterPointTracker::AllocateTargets(unsigned int targetcount, unsigned int videoch)
{
    ReleaseTargets(videoch);

    if (targetcount < 1) return SVL_FAIL;

    Target[videoch] = new TargetType[targetcount];
    ForcedTarget[videoch] = new TargetType[targetcount];

    memset(Target[videoch], 0, sizeof(TargetType));
    memset(ForcedTarget[videoch], 0, sizeof(TargetType));

    return SVL_OK;
}

void svlFilterPointTracker::ReleaseTargets(unsigned int videoch)
{
    if (Target[videoch]) {
        delete [] Target[videoch];
        Target[videoch] = 0;
    }
    if (ForcedTarget[videoch]) {
        delete [] ForcedTarget[videoch];
        ForcedTarget[videoch] = 0;
    }
}

void svlFilterPointTracker::ResetToForcedPositions(unsigned int videoch)
{
    svlSamplePointCloud* points = dynamic_cast<svlSamplePointCloud*>(OutputData);
    unsigned int targetcount = Tracker[videoch]->GetTargetCount();
    Tracker[videoch]->Release();
    for (unsigned int i = 0; i < targetcount; i ++) {
        // Reinitializing forced positions
        Target[videoch][i] = ForcedTarget[videoch][i];
        // Storing temporary results until tracking restarts
        if (videoch == SVL_LEFT) {
            points->points.Element(0, SVL_LEFT) = Target[videoch][i].x;
            points->points.Element(1, SVL_LEFT) = Target[videoch][i].y;
        }
        else {
            points->points.Element(SVL_RIGHT, i) = Target[videoch][i].x;
            points->points.Element(SVL_RIGHT, i) = Target[videoch][i].y;
        }
        // Updating targets in trackers
        Tracker[videoch]->SetTarget(i, Target[videoch] + i);
    }
    Tracker[videoch]->Initialize();
}

int svlFilterPointTracker::StoreResults(unsigned int videoch)
{
    svlSamplePointCloud* points = dynamic_cast<svlSamplePointCloud*>(OutputData);
    TargetType target;
    const double weight = MovingAverageWeight;
    const double weightp1 = weight + 1.0;
    unsigned int i;
    unsigned int targetcount = Tracker[videoch]->GetTargetCount();

    if (videoch == SVL_LEFT) {
        for (i = 0; i < targetcount; i ++) {
            Tracker[videoch]->GetTarget(i, &target);
            points->points.Element(0, i) = (target.x + points->points.Element(0, i) * weight) / weightp1;
            points->points.Element(1, i) = (target.y + points->points.Element(1, i) * weight) / weightp1;
        }
    }
    else {
        for (i = 0; i < targetcount; i ++) {
            Tracker[videoch]->GetTarget(i, &target);
            points->points.Element(2, i) = (target.x + points->points.Element(2, i) * weight) / weightp1;
            points->points.Element(3, i) = (target.y + points->points.Element(3, i) * weight) / weightp1;
        }
    }

    return SVL_OK;
}


/*******************************************/
/*** svlPointTrackerAlgoBase class *********/
/*******************************************/

svlPointTrackerAlgoBase::svlPointTrackerAlgoBase()
{
    Initialized = false;
    Width = 0;
    Height = 0;
    Left = 0;
    Top = 0;
    Right = 0;
    Bottom = 0;
    TargetCount = 0;
    TargetParams = 0;
}

svlPointTrackerAlgoBase::~svlPointTrackerAlgoBase()
{
    ReallocateTargetParams(0);
}

void svlPointTrackerAlgoBase::SetImageSize(unsigned int width, unsigned int height)
{
    Width = width;
    Height = height;
}

void svlPointTrackerAlgoBase::SetWorkArea(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
{
    if (left >= Width) left = std::max(0u, Width - 1);
    if (top >= Height) top = std::max(0u, Height - 1);
    if (right >= Width) right = std::max(0u, Width - 1);
    if (bottom >= Height) bottom = std::max(0u, Height - 1);
    Left = min(left, right);
    Top = min(top, bottom);
    Right = max(left, right);
    Bottom = max(top, bottom);
}

int svlPointTrackerAlgoBase::SetTargetCount(unsigned int targetcount)
{
    ReallocateTargetParams(targetcount);
    TargetCount = targetcount;
    return 0;
}

int svlPointTrackerAlgoBase::SetTarget(unsigned int targetid, svlFilterPointTracker::TargetType* target)
{
    if (targetid >= TargetCount) return -2;
    if (target == 0) return -3;
    TargetParams[targetid] = *target;
    return 0;
}

int svlPointTrackerAlgoBase::Initialize()
{
    // derived classes may override this method
    // to allocate resources before tracking
    return -1;
}

void svlPointTrackerAlgoBase::Release()
{
    // derived classes may override this method
    // to release resources allocated in the Initialize method
}

int svlPointTrackerAlgoBase::PreProcessImage(unsigned char* CMN_UNUSED(image))
{
    // derived classes may override this method
    // for image pre-processing
    return 0;
}

int svlPointTrackerAlgoBase::Track(unsigned char* CMN_UNUSED(image))
{
    // derived classes shall override this method
    return 0;
}

int svlPointTrackerAlgoBase::GetTarget(unsigned int targetid, svlFilterPointTracker::TargetType* target)
{
    if (targetid >= TargetCount) return -1;
    if (target == 0) return -2;
    *target =  TargetParams[targetid];
    return 0;
}

void svlPointTrackerAlgoBase::ReallocateTargetParams(unsigned int targetcount)
{
    if (TargetParams) {
        delete [] TargetParams;
        TargetParams = 0;
    }
    if (targetcount > 0) {
        TargetParams = new svlFilterPointTracker::TargetType[targetcount];
        memset(TargetParams, 0, sizeof(svlFilterPointTracker::TargetType));
    }
}

