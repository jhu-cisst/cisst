/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlToolTrackerAlgorithmBase;


class CISST_EXPORT svlFilterToolTracker : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

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
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleTargets OutputTargets;
    
    svlToolTrackerAlgorithmBase* Algorithm;
    TargetType Targets[2];
    svlSampleImage* WarpedImage;
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
    svlFilterToolTracker::TargetType Targets[2];

    virtual int Initialize();
    virtual int Process(svlProcInfo* procInfo, svlSampleImage* input) = 0;
    virtual void Release();
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterToolTracker)

#endif // _svlFilterToolTracker_h

