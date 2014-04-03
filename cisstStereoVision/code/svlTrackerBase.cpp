/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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
/*
#include "svlTrackerBase.h"
#include <stdlib.h>
#include <string.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

svlTrackerBase::svlTrackerBase()
{
    Width = 0;
    Height = 0;
    Left = 0;
    Top = 0;
    Right = 0;
    Bottom = 0;
    TargetCount = 0;
    TargetParams = 0;
    TargetParamsSize = 0;
    TrackerParams = 0;
    TrackerParamsSize = 0;
}

svlTrackerBase::~svlTrackerBase()
{
    ReallocateTargetParams(0);
    SetupTracker(0, 0);
}

void svlTrackerBase::SetImageSize(unsigned int width, unsigned int height)
{
    Width = width;
    Height = height;
}

void svlTrackerBase::SetWorkArea(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
{
    if (left < 0) left = 0;
    if (left >= Width) left = max(0, Width - 1);
    if (top < 0) top = 0;
    if (top >= Height) top = max(0, Height - 1);
    if (right < 0) right = 0;
    if (right >= Width) right = max(0, Width - 1);
    if (bottom < 0) bottom = 0;
    if (bottom >= Height) bottom = max(0, Height - 1);
    Left = min(left, right);
    Top = min(top, bottom);
    Right = max(left, right);
    Bottom = max(top, bottom);
}

int svlTrackerBase::SetupTracker(void *trackerparams, unsigned int paramssize)
{
    if (TrackerParams) free(TrackerParams);
    if (trackerparams != 0 && paramssize > 0) {
        TrackerParams = calloc(paramssize, 1);
        TrackerParamsSize = paramssize;
        memcpy(TrackerParams, trackerparams, TrackerParamsSize);
    }
    else {
        TrackerParams = 0;
        TrackerParamsSize = 0;
    }
    return 0;
}

int svlTrackerBase::SetTargetCount(unsigned int targetcount)
{
    ReallocateTargetParams(targetcount);
    TargetCount = targetcount;
    return 0;
}

int svlTrackerBase::SetTarget(unsigned int targetid, void *targetparams, unsigned int paramssize)
{
    if (targetid >= TargetCount) return -1;
    if (paramssize != TargetParamsSize) return -2;
    if (targetparams == 0) return -3;
    memcpy(TargetParams[targetid], targetparams, TargetParamsSize);
    return 0;
}

int svlTrackerBase::PreProcessImage(unsigned char* image)
{
    // derived classes may override this method
    // for image pre-processing
    return 0;
}

int svlTrackerBase::Track(unsigned char* image)
{
    // derived classes shall override this method
    return 0;
}

int svlTrackerBase::GetTarget(unsigned int targetid, void *targetparams, unsigned int paramssize)
{
    if (targetid >= TargetCount) return -1;
    if (paramssize != TargetParamsSize) return -2;
    if (targetparams == 0) return -3;
    memcpy(targetparams, TargetParams[targetid], TargetParamsSize);
    return 0;
}

void svlTrackerBase::ReallocateTargetParams(unsigned int targetcount)
{
    if (TargetParams) {
        for (unsigned int i = 0; i < TargetCount; i ++) {
            if (TargetParams[i]) free(TargetParams[i]);
        }
        free(TargetParams);
    }
    if (targetcount != 0 && TargetParamsSize > 0) {
        TargetParams = (void**)malloc(targetcount * sizeof(void*));
        // every target is initialized to zero after creation
        for (unsigned int i = 0; i < TargetCount; i ++) TargetParams[i] = calloc(TargetParamsSize, 1);
    }
    else {
        TargetParams = 0;
    }
}
*/
