/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFilterImageTracker.h 1236 2010-02-26 20:38:21Z adeguet1 $
  
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

#ifndef _svlFilterImageTracker_h
#define _svlFilterImageTracker_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlImageTracker;


class CISST_EXPORT svlFilterImageTracker : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageTracker();
    virtual ~svlFilterImageTracker();

    int SetTracker(svlImageTracker & tracker, unsigned int videoch = SVL_LEFT);

    void SetMovingAverageSmoothing(double weight);
    void SetFrameSkip(unsigned int skipcount);
    void SetIterations(unsigned int count);
    void ResetTargets();

    int SetRigidBody(bool enable);
    void SetRigidBodyConstraints(double angle_low, double angle_high, double scale_low, double scale_high);

    int SetROI(const svlRect & rect, unsigned int videoch = SVL_LEFT);
    int SetROI(int left, int top, int right, int bottom, unsigned int videoch = SVL_LEFT);
    int GetROI(svlRect & rect, unsigned int videoch = SVL_LEFT) const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

    virtual void ReconstructRigidBody();

private:
    svlSampleTargets OutputTargets;

    unsigned int VideoChannels;
    vctDynamicVector<svlImageTracker*> Trackers;
    vctDynamicVector<svlRect> ROI;

    bool RigidBody;
    vctDynamicVector<double> RigidBodyAngle;
    vctDynamicVector<double> RigidBodyScale;

    bool ResetFlag;
    svlSampleTargets InitialTargets;
    vctDynamicMatrix<svlTarget2D> Targets;

    int FrameCount;
    int FramesToSkip;
    double MovingAverageWeight;
    double RigidBodyAngleLow;
    double RigidBodyAngleHigh;
    double RigidBodyScaleLow;
    double RigidBodyScaleHigh;
    unsigned int Iterations;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageTracker)


class svlImageTracker
{
public:
	svlImageTracker();

    bool IsInitialized();

    virtual int SetImageSize(unsigned int width, unsigned int height);
    virtual void SetROI(const svlRect & rect);
    virtual void SetROI(int left, int top, int right, int bottom);
    virtual int SetTargetCount(unsigned int targetcount);
    virtual int SetTarget(unsigned int targetid, const svlTarget2D & target);
    virtual int GetTarget(unsigned int targetid, svlTarget2D & target);

    virtual int Initialize();
    virtual void ResetTargets();
    virtual int PreProcessImage(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
	virtual int Track(svlSampleImage & image, unsigned int videoch = SVL_LEFT) = 0;
    virtual void Release();

protected:
    bool Initialized;
    unsigned int Width;
    unsigned int Height;
    svlRect ROI;
    vctDynamicVector<svlTarget2D> Targets;
};

#endif // _svlFilterImageTracker_h

