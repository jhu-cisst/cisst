/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterToolTracker_h
#define _svlFilterToolTracker_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class svlToolTrackerAlgorithmBase;

class CISST_EXPORT svlFilterToolTracker : public svlFilterBase
{
public:
    typedef struct _TargetType {
        vct2 tooltipos;
        double orientation;
        double scale;
    } TargetType;

    svlFilterToolTracker();
    virtual ~svlFilterToolTracker();

    int SetAlgorithm(svlToolTrackerAlgorithmBase* algorithm);
    int SetInitialTarget(TargetType & target, unsigned int videoch = SVL_LEFT);

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    svlToolTrackerAlgorithmBase* Algorithm;
    TargetType Target[2];
    svlSampleImageBase* WarpedImage;
};

class CISST_EXPORT svlToolTrackerAlgorithmBase
{
friend class svlFilterToolTracker;

public:
	svlToolTrackerAlgorithmBase();
	virtual ~svlToolTrackerAlgorithmBase();

private:
    void SetInput(unsigned int width, unsigned int height, unsigned int videochannels);
    void SetInitialTarget(svlFilterToolTracker::TargetType & target, unsigned int videoch);
    void GetTarget(svlFilterToolTracker::TargetType & target, unsigned int videoch);

protected:
    bool Initialized;
    unsigned int Width;
    unsigned int Height;
    unsigned int VideoChannels;
    svlFilterToolTracker::TargetType Target[2];

    virtual int Initialize();
    virtual int Process(svlFilterBase::ProcInfo* procInfo, svlSampleImageBase* input) = 0;
    virtual void Release();
};


#endif // _svlFilterToolTracker_h

