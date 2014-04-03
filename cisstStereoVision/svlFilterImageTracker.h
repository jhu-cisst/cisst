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

#ifndef _svlFilterImageTracker_h
#define _svlFilterImageTracker_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlFilterImageCenterFinder.h>
#include <cisstStereoVision/svlDraw.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlImageTracker;


class CISST_EXPORT svlFilterImageTracker : public svlFilterBase, public svlFilterImageCenterFinderInterface
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageTracker();
    virtual ~svlFilterImageTracker();

    int SetTracker(svlImageTracker & tracker, unsigned int videoch = SVL_LEFT);

    void SetTargetTrajectorySmoothing(double weight);
    void SetFrameSkip(unsigned int skipcount);
    void SetIterations(unsigned int count);
    void ResetTargets();

    void SetRigidBodyTransformSmoothing(double weight);
    int SetRigidBody(bool enable);
    void SetRigidBodyConstraints(double angle_low, double angle_high, double scale_low, double scale_high);
    int SetRigidBodyTransform(const vct3x3 & transform, unsigned int videoch = SVL_LEFT);
    int GetRigidBodyTransform(vct3x3 & transform, unsigned int videoch = SVL_LEFT);
    vct3x3 CISST_DEPRECATED GetRigidBodyTransform(unsigned int videoch = SVL_LEFT);
    void GetRigidBodyError(vctDynamicVector<int> & errors);

    int SetROI(const svlRect & rect, unsigned int videoch = SVL_LEFT);
    int SetROI(int left, int top, int right, int bottom, unsigned int videoch = SVL_LEFT);
    int SetROI(const svlEllipse & ellipse, unsigned int videoch = SVL_LEFT);
    int GetROI(svlRect & rect, unsigned int videoch = SVL_LEFT) const;
    int GetROI(svlEllipse & ellipse, unsigned int videoch = SVL_LEFT) const;

    // Inherited from svlFilterImageCenterFinderInterface
    virtual int OnChangeCenterRect(const svlRect & rect, unsigned int videoch = SVL_LEFT);
    virtual int OnChangeCenterEllipse(const svlEllipse & ellipse, unsigned int videoch = SVL_LEFT);

    int SetMosaicSize(unsigned int width, unsigned int height);
    const svlSampleImage* GetMosaicImage() const;

protected:
    virtual int OnConnectInput(svlFilterInput &input, svlStreamType type);
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int OnStart(unsigned int procCount);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

    virtual void LinkChannelsVertically();
    virtual int ReconstructRigidBody();
    virtual void BackprojectRigidBody(int width, int height);
    virtual int ComputeHomography();
    virtual void BackprojectHomography();
    virtual void WarpImage(svlSampleImage* image, unsigned int videoch, int roi_margin, int threadid = -1);
    virtual int UpdateMosaicImage(unsigned int videoch, unsigned int width, unsigned int height);
    virtual void PushSamplesToAsyncOutputs(double timestamp);

private:
    svlSampleTargets OutputTargets;

    unsigned int VideoChannels;
    vctDynamicVector<svlImageTracker*> Trackers;
    vctDynamicVector<svlRect> ROIRect;
    vctDynamicVector<svlRect> ROIRectInternal;
    vctDynamicVector<svlEllipse> ROIEllipse;
    vctDynamicVector<svlEllipse> ROIEllipseInternal;
    vctDynamicVector<vctInt2> ROICenter;
    vctDynamicVector<svlQuad> WarpedROIRect;
    vctDynamicVector<svlEllipse> WarpedROIEllipse;

    bool RigidBody;
    bool RigidBodyInitialized;
    vctDynamicVector<double> RigidBodyAngle;
    vctDynamicVector<double> RigidBodyScale;
    vctDynamicVector<vct3x3> RigidBodyTransform;

    vctDynamicVector< vctFixedSizeVector<double, 9> > Homography;

    bool ResetFlag;
    svlSampleTargets InitialTargets;
    vctDynamicMatrix<svlTarget2D> Targets;

    int FrameCount;
    int FramesToSkip;
    double TargetTrajectorySmoothingWeight;
    double RigidBodyTransformSmoothingWeight;
    double RigidBodyAngleLow;
    double RigidBodyAngleHigh;
    double RigidBodyScaleLow;
    double RigidBodyScaleHigh;
    unsigned int Iterations;

    bool WarpingParallel;
    vctDynamicVector<double> WarpedRigidBodyAngle;
    vctDynamicVector<double> WarpedRigidBodyScale;
    svlSampleImage* WarpedImage;
    vctDynamicVector<svlDraw::Internals> WarpInternals;
    const unsigned int RigidBodyIterations;
    vctDynamicVector<int> RigidBodyError;

    svlSampleImage* Mosaic;
    vctDynamicVector< vctDynamicVector<unsigned short> > MosaicAccuBuffer;
    vctDynamicVector< vctDynamicVector<unsigned char> > MosaicAccuCount;
    unsigned int MosaicWidth;
    unsigned int MosaicHeight;

    // Work variables
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> T_ax;
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> T_ay;
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> T_proto_ax;
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> T_proto_ay;
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> T_cos_an;
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> T_sin_an;
    vctFixedSizeVector<double, SVL_MAX_CHANNELS> T_scale;
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
    virtual void SetROI(const svlQuad & quad);
    virtual void SetROI(const svlEllipse & ellipse);

    // Needs to be overridden if there is a need for margin within the ROI
    virtual int GetROIMargin();

    virtual int SetTargetCount(unsigned int targetcount);
    virtual int SetTarget(unsigned int targetid, const svlTarget2D & target);
    virtual int GetTarget(unsigned int targetid, svlTarget2D & target);

    virtual int Initialize();
    virtual void ResetTargets();
    virtual int PreProcessImage(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
	virtual int Track(svlSampleImage & image, unsigned int videoch = SVL_LEFT) = 0;
	virtual int Track(svlProcInfo* procInfo, svlSampleImage & image, unsigned int videoch = SVL_LEFT);
    virtual void Release();

protected:
    bool Initialized;
    unsigned int Width;
    unsigned int Height;
    svlQuad ROIRect;
    svlEllipse ROIEllipse;
    vctDynamicVector<svlTarget2D> Targets;
};

#endif // _svlFilterImageTracker_h

