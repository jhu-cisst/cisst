/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstStereoVision/svlTrackerCisst.h>
#include <cisstStereoVision/svlConverters.h>
#include "trackers/cisst/svlTrackerCisstBase.h"
#include "trackers/cisst/svlTrackerCisstNCC.h"
#include "trackers/cisst/svlTrackerCisstWSSD.h"


/*************************************/
/*** svlTrackerCisst class ***********/
/*************************************/

svlTrackerCisst::svlTrackerCisst() :
    svlImageTracker(),
    TargetsAdded(false),
    TrackerCisst(0),
    Image16(0),
    TemplateRadius(3),
    WindowRadius(6),
    Metric(svlWSSD),
    TrackerParam(0)
{
}

svlTrackerCisst::~svlTrackerCisst()
{
    Release();
}

int svlTrackerCisst::SetParameters(svlErrorMetric metric,
                                   unsigned int templateradius,
                                   unsigned int windowradius,
                                   void* param)
{
    if (Initialized) return SVL_FAIL;
    if (templateradius < 1) templateradius = 1;
    if (windowradius < 1) templateradius = 1;

    Metric = metric;
    TemplateRadius = templateradius;
    WindowRadius = windowradius;
    TrackerParam = param;

    return SVL_OK;
}

int svlTrackerCisst::SetTargetCount(unsigned int targetcount)
{
    if (Initialized) return SVL_FAIL;

    // hard wired to 1 until I figure out how to add more targets
    targetcount = 1;

    return svlImageTracker::SetTargetCount(targetcount);
}

int svlTrackerCisst::GetTarget(unsigned int targetid, svlTarget2D & target)
{
    if (!Initialized || targetid > 0) return SVL_FAIL;

    svlTrackerCisstBase* tracker = reinterpret_cast<svlTrackerCisstBase*>(TrackerCisst);
    target.pos.x = static_cast<int>(tracker->getOutputPosX());
    target.pos.y = static_cast<int>(tracker->getOutputPosY());

    return svlImageTracker::GetTarget(targetid, target);
}

int svlTrackerCisst::Initialize()
{
    if (Width < 1 || Height < 1 || Targets.size() < 1) return SVL_FAIL;

    Release();

    svlTrackerCisstBase* tracker = 0;

    if (Metric == svlNCC) {
        tracker = new svlTrackerCisstNCC();
    }
    else if (Metric == svlWSSD) {
        if (TrackerParam) tracker = new svlTrackerCisstWSSD(*(reinterpret_cast<svlTrackerCisstWSSD::Model*>(TrackerParam)));
        else tracker = new svlTrackerCisstWSSD();
    }
    else return SVL_FAIL;

    TrackerCisst = tracker;

    // allocating resources
    Image16 = new unsigned short[Width * Height];

    Initialized = true;

    return SVL_OK;
}

int svlTrackerCisst::Track(svlSampleImage & image, unsigned int videoch)
{
    if (!Initialized) return SVL_FAIL;

    svlTrackerCisstBase* tracker = reinterpret_cast<svlTrackerCisstBase*>(TrackerCisst);
    unsigned int templatesize, windowsize;

    // the Cisst trackers work on 16bpp grayscale images only
    svlConverter::RGB24toGray16(image.GetUCharPointer(videoch), Image16, Width * Height);

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
        tracker->pushTemplate(Image16 +
                              Width * (Targets[0].pos.y - TemplateRadius) +
                              (Targets[0].pos.x - TemplateRadius),
                              Height,
                              Width);
        tracker->pushSelectedTemplate(0);

        // set initial position
        tracker->setWindowPosition(Targets[0].pos.y - WindowRadius, Targets[0].pos.x - WindowRadius);
        tracker->setInitPosition(Targets[0].pos.y, Targets[0].pos.x);

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

    return SVL_OK;
}

void svlTrackerCisst::Release()
{
    Initialized = false;

    if (TrackerCisst) {
        svlTrackerCisstBase* tracker = reinterpret_cast<svlTrackerCisstBase*>(TrackerCisst);
        delete tracker;
        TrackerCisst = 0;
    }
    if (Image16) {
        delete [] Image16;
        Image16 = 0;
    }
}

