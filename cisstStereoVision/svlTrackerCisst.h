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

#ifndef _svlTrackerCisst_h
#define _svlTrackerCisst_h

#include <cisstStereoVision/svlFilterImageTracker.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlTrackerCisst : public svlImageTracker
{
public:
    svlTrackerCisst();
    virtual ~svlTrackerCisst();

    virtual int SetParameters(svlErrorMetric metric,
                              unsigned int templateradius,
                              unsigned int windowradius,
                              void* param = 0);

    virtual int SetTargetCount(unsigned int targetcount);
    virtual int GetTarget(unsigned int targetid, svlTarget2D & target);

    virtual int Initialize();
	virtual int Track(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
    virtual void Release();

protected:
    bool TargetsAdded;

    void* TrackerCisst;
    unsigned short* Image16;
    unsigned int TemplateRadius;
    unsigned int WindowRadius;
    svlErrorMetric Metric;
    void* TrackerParam;
};

#endif //_svlTrackerCisst_h

