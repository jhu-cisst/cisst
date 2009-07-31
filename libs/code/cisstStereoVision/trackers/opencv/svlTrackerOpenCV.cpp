/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Ankur Kapoor & Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/trackers/svlTrackerOpenCV.h>
#include "trkOCVColor.h"

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


/*************************************/
/*** svlTrackerOpenCV class **********/
/*************************************/

svlTrackerOpenCV::svlTrackerOpenCV() : svlPointTrackerAlgoBase()
{
    TrackerObj = 0;
    TargetsAdded = false;

    WinRadius = 3;
    Vmin = 80;
	Vmax = 255; 
	Smin = 30;
    HDims = 48;
    MaxIter = 10;
    MaxError = 1.0;
}

svlTrackerOpenCV::~svlTrackerOpenCV()
{
    Release();
}

int svlTrackerOpenCV::SetParameters(int winrad, int vmin, int vmax, int smin, int hdims, int maxiter, double maxerror)
{
    // checkig parameters
    if (winrad < 1) winrad = 1;
    else if (winrad > 100) winrad = 100;
    if (vmin < 0) vmin = 0;
    else if (vmin > 255) vmin = 255;
    if (vmax < 0) vmax = 0;
    else if (vmax > 255) vmax = 255;
    if (smin < 0) smin = 0;
    else if (smin > 255) smin = 255;
    if (hdims < 8) hdims = 8;
    else if (hdims > 128) hdims = 128;
    if (maxiter < 1) maxiter = 1;
    else if (maxiter > 100) maxiter = 100;
    if (maxerror < 0.1) maxerror = 0.1;

    WinRadius = winrad;
    Vmin = vmin;
    Vmax = vmax;
    Smin = smin;
    HDims = hdims;
    MaxIter = maxiter;
    MaxError = maxerror;

    // if tracker exists, apply immediately
    if (Initialized) {
        trkOCVColor* tracker = reinterpret_cast<trkOCVColor*>(TrackerObj);
        tracker->Vmin = Vmin;
        tracker->Vmax = Vmax;
        tracker->Smin = Smin;
        tracker->HDims = HDims;
        tracker->MaxIter = MaxIter;
        tracker->MaxError = MaxError;
    }

    return 0;
}

int svlTrackerOpenCV::Initialize()
{
    if (Width < 1 || Height < 1) return -1;
    if (TargetCount < 1) return -2;

    Release();

    trkOCVColor* tracker = new trkOCVColor();
    if (tracker == 0) return -3;
    TrackerObj = tracker;

    // allocating resources
    CvSize size = cvSize(Width, Height);
    tracker->Image8U = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
    tracker->Hue  = cvCreateImage(size, IPL_DEPTH_8U, 1);
    tracker->Hsv  = cvCreateImage(size, IPL_DEPTH_8U, 3);
    tracker->Mask = cvCreateImage(size, IPL_DEPTH_8U, 1);

    tracker->Vmin = Vmin;
    tracker->Vmax = Vmax;
    tracker->Smin = Smin;
    tracker->HDims = HDims;
    tracker->MaxIter = MaxIter;
    tracker->MaxError = MaxError;

    Initialized = true;

    return 0;
}

void svlTrackerOpenCV::Release()
{
    Initialized = false;

    if (TrackerObj) {
        trkOCVColor* tracker = reinterpret_cast<trkOCVColor*>(TrackerObj);
        if (tracker->Image8U) cvReleaseImageHeader(&(tracker->Image8U));
        tracker->Image8U = 0;
        if (tracker->Hue) cvReleaseImage(&(tracker->Hue));
        tracker->Hue = 0;
        if (tracker->Hsv) cvReleaseImage(&(tracker->Hsv));
        tracker->Hsv = 0;
        if (tracker->Mask) cvReleaseImage(&(tracker->Mask));
        tracker->Mask = 0;
        TrackerObj = 0;
    }
}

int svlTrackerOpenCV::SetTargetCount(unsigned int CMN_UNUSED(targetcount))
{
    if (Initialized) return -1;
    if (svlPointTrackerAlgoBase::SetTargetCount(5) != 0) return -2;
    return 0;
}

int svlTrackerOpenCV::PreProcessImage(unsigned char* image)
{
    if (!Initialized) return -1;
    if (image == 0) return -2;

    trkOCVColor* tracker = reinterpret_cast<trkOCVColor*>(TrackerObj);
    tracker->Image8U->imageData = reinterpret_cast<char*>(image);
    tracker->PreProcessImage();

    return 0;
}

int svlTrackerOpenCV::Track(unsigned char* image)
{
    if (!Initialized) return -1;
    if (image == 0) return -2;

    trkOCVColor* tracker = reinterpret_cast<trkOCVColor*>(TrackerObj);
    tracker->Image8U->imageData = reinterpret_cast<char*>(image);

    if (!TargetsAdded) {
        // add targets
        for (unsigned int i = 0; i < TargetCount; i ++) {
            tracker->selection = cvRect(TargetParams[i].x - WinRadius,
                                        TargetParams[i].y - WinRadius,
                                        2 * WinRadius + 1,
                                        2 * WinRadius + 1);
            tracker->AddNewTarget(Width, Height);
        }
        TargetsAdded = true;
    }

    tracker->Track();

    return 0;
}

int svlTrackerOpenCV::GetTarget(unsigned int targetid, svlFilterPointTracker::TargetType* target)
{
    if (!Initialized) return -1;
    if (targetid >= TargetCount) return -2;

    trkOCVColor* tracker = reinterpret_cast<trkOCVColor*>(TrackerObj);
    TargetParams[targetid].x = static_cast<int>(tracker->TargetDataVec[targetid]->track_box.center.x);
    TargetParams[targetid].y = static_cast<int>(tracker->TargetDataVec[targetid]->track_box.center.y);

    return svlPointTrackerAlgoBase::GetTarget(targetid, target);
}

