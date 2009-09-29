/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlTrackerCisst.cpp 791 2009-09-01 19:24:56Z bvagvol1 $

  Author(s):  Maneesh Dewan & Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/trackers/svlTrackerCisst.h>
#include <cisstStereoVision/svlConverters.h>
#include "trkCisstBase.h"
#include "trkCisstNCC.h"
#include "trkCisstWSSD.h"

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
/*** svlTrackerCisst class ***********/
/*************************************/

svlTrackerCisst::svlTrackerCisst() : svlPointTrackerAlgoBase()
{
    TrackerCisst = 0;
    TargetsAdded = false;

    Image16 = 0;
    TemplateRadius = 3;
    WindowRadius = 6;
    TrackerType = trkCisstTypeWSSD;
    TrackerParam = 0;
}

svlTrackerCisst::~svlTrackerCisst()
{
    Release();
}

int svlTrackerCisst::SetParameters(trkCisstType type, unsigned int templateradius, unsigned int windowradius, void* param)
{
    if (Initialized) return -1;
    if (templateradius < 1) return -2;
    if (windowradius < 1) return -3;
    TrackerType = type;
    TemplateRadius = templateradius;
    WindowRadius = windowradius;
    TrackerParam = param;
    return 0;
}

int svlTrackerCisst::Initialize()
{
    if (Width < 1 || Height < 1) return -1;
    if (TargetCount < 1) return -2;

    Release();

    trkCisstBase* tracker = 0;

    if (TrackerType == trkCisstTypeNCC) {
        tracker = new trkCisstNCC();
    }
    else if (TrackerType == trkCisstTypeWSSD) {
        if (TrackerParam) tracker = new trkCisstWSSD(*(reinterpret_cast<trkCisstWSSDModel*>(TrackerParam)));
        else tracker = new trkCisstWSSD();
    }

    if (tracker == 0) return -3;
    TrackerCisst = tracker;

    // allocating resources
    Image16 = new unsigned short[Width * Height];

    Initialized = true;

    return 0;
}

void svlTrackerCisst::Release()
{
    Initialized = false;

    if (TrackerCisst) {
        trkCisstBase* tracker = reinterpret_cast<trkCisstBase*>(TrackerCisst);
        delete tracker;
        TrackerCisst = 0;
    }

    // releasing resources
    if (Image16) {
        delete [] Image16;
        Image16 = 0;
    }
}

int svlTrackerCisst::SetTargetCount(unsigned int CMN_UNUSED(targetcount))
{
    if (Initialized) return -1;
    // hard wired to 1 until I figure out how to add more targets
    if (svlPointTrackerAlgoBase::SetTargetCount(1) != 0) return -2;
    return 0;
}

int svlTrackerCisst::PreProcessImage(unsigned char* image)
{
    if (!Initialized) return -1;
    if (image == 0) return -2;

    // pre-processing image

    return 0;
}

int svlTrackerCisst::Track(unsigned char* image)
{
    if (!Initialized) return -1;
    if (image == 0) return -2;

    trkCisstBase* tracker = reinterpret_cast<trkCisstBase*>(TrackerCisst);
    unsigned int templatesize, windowsize;

    // the Cisst trackers work on 16bpp grayscale images only
    svlConverter::RGB24toGray16(image, Image16, Width * Height);

    if (!TargetsAdded) {
    // tracker and target initialization

        // set template size
        templatesize = 2 * TemplateRadius + 1;
        tracker->setTemplateSize(templatesize, templatesize);
        tracker->setTemplateCenter(TemplateRadius, TemplateRadius);

        // set search window size
        windowsize = 2 * WindowRadius + 1;
        tracker->setWindowSize(windowsize, windowsize);
        tracker->setWindowCenter(WindowRadius, WindowRadius);

        // add target
        tracker->pushTemplate(Image16 + Width * (TargetParams[0].y - TemplateRadius) + (TargetParams[0].x - TemplateRadius),
                              Height, Width);
        tracker->pushSelectedTemplate(0);

        // set initial position
        tracker->setWindowPosition(TargetParams[0].y - WindowRadius, TargetParams[0].x - WindowRadius);
        tracker->setInitPosition(TargetParams[0].y, TargetParams[0].x);

        // initialize tracker
        tracker->initializeTrack();

        // set source image size
        tracker->setCurrentimageSize(Height, Width);
        tracker->setImageSize(Height, Width);

        TargetsAdded = true;
    }

    // tracking
    tracker->copyCurrentImage(Image16, Height, Width); 
    tracker->updateTrack();

    return 0;
}

int svlTrackerCisst::GetTarget(unsigned int targetid, svlFilterPointTracker::TargetType* target)
{
    if (!Initialized) return -1;
    if (targetid >= TargetCount) return -2;

    // retrieve tracking results
    trkCisstBase* tracker = reinterpret_cast<trkCisstBase*>(TrackerCisst);
    TargetParams[targetid].x = static_cast<int>(tracker->getOutputPosX());
    TargetParams[targetid].y = static_cast<int>(tracker->getOutputPosY());
    // winsize is undefined

    return svlPointTrackerAlgoBase::GetTarget(targetid, target);
}

