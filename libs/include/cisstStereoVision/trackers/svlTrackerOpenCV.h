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

#ifndef _svlTrackerOpenCV_h
#define _svlTrackerOpenCV_h

#include <cisstStereoVision/svlFilterPointTracker.h>


class CISST_EXPORT svlTrackerOpenCV : public svlPointTrackerAlgoBase
{
public:
    svlTrackerOpenCV();
    ~svlTrackerOpenCV();

    int SetParameters(int winrad, int vmin, int vmax, int smin, int hdims, int maxiter, double maxerror);

protected:
    void* TrackerObj;
    bool TargetsAdded;

    int Initialize();
    void Release();

    int SetTargetCount(unsigned int targetcount);
    int GetTarget(unsigned int targetid, svlFilterPointTracker::TargetType* target);

    int PreProcessImage(unsigned char* image);
	int Track(unsigned char* image);

private:
    int WinRadius;
    int Vmin, Vmax, Smin, HDims, MaxIter;
    double MaxError;
};

#endif //_svlTrackerOpenCV_h

