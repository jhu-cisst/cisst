/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlTrackerCisst.h 618 2009-07-31 16:39:42Z bvagvol1 $

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

#ifndef _svlTrackerCisst_h
#define _svlTrackerCisst_h

#include <cisstStereoVision/svlFilterPointTracker.h>


enum trkCisstType
{
    trkCisstTypeNCC,         // Normalized cross correlation
    trkCisstTypeWSSD         // Weighted sum of squared differences
};


class svlTrackerCisst : public svlPointTrackerAlgoBase
{
public:
    svlTrackerCisst();
    ~svlTrackerCisst();

    int SetParameters(trkCisstType type, unsigned int templateradius, unsigned int windowradius, void* param = 0);

protected:
    void* TrackerCisst;
    bool TargetsAdded;

    int Initialize();
    void Release();

    int SetTargetCount(unsigned int targetcount);
    int GetTarget(unsigned int targetid, svlFilterPointTracker::TargetType* target);

    int PreProcessImage(unsigned char* image);
	int Track(unsigned char* image);

private:
    unsigned short* Image16;

    unsigned int TemplateRadius;
    unsigned int WindowRadius;
    trkCisstType TrackerType;
    void* TrackerParam;
};

#endif //_svlTrackerCisst_h

