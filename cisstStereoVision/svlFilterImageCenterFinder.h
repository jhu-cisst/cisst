/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterImageCenterFinder_h
#define _svlFilterImageCenterFinder_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlDraw.h>
#include <cisstStereoVision/svlImageProcessing.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlFilterImageCenterFinderInterface;

class CISST_EXPORT svlFilterImageCenterFinder : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageCenterFinder();

    int GetCenter(int &x, int &y, unsigned int videoch = SVL_LEFT) const;
    int GetRadius(int &x, int &y, unsigned int videoch = SVL_LEFT) const;
    int GetEllipse(svlEllipse &ellipse, unsigned int videoch = SVL_LEFT) const;

    void SetTrajectorySmoothing(double smoothing);
    void SetThreshold(unsigned char thresholdlevel);
    void SetMinRadius(unsigned int radius);
    void SetMassRatio(unsigned int ratio);
    void SetHorizontalLink(bool enable);
    void SetVerticalLink(bool enable);

    double GetTrajectorySmoothing() const;
    unsigned char GetThreshold() const;
    unsigned int GetMinRadius() const;
    unsigned int GetMassRatio() const;
    bool GetHorizontalLink() const;
    bool GetVerticalLink() const;

    int  SetEnableEllipseFitting(bool enable);
    int  SetEnableEllipseFittingDrawEllipse(bool enable);
    int  SetEnableEllipseMask(bool enable);
    void SetEllipseMaskTransition(int start, int end);
    void SetEllipseMargin(int margin);

    bool GetEnableEllipseFitting() const;
    bool GetEnableEllipseFittingDrawEllipse() const;
    bool GetEnableEllipseMask() const;
    void GetEllipseMaskTransition(int & start, int & end) const;
    int  GetEllipseMargin();

    svlSampleImage* GetEllipseMask();

    void AddReceiver(svlFilterImageCenterFinderInterface* receiver);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    bool FindEllipse(svlSampleImage* image, unsigned int videoch, const int cx, const int cy, svlEllipse & ellipse);
    void CreateTransitionImage();
    void UpdateMaskImage(unsigned int videoch, svlEllipse & ellipse);
    unsigned int sqrt_uint32(unsigned int value);

    double       Smoothing;
    unsigned int ThresholdLevel;
    unsigned int MinRadius;
    unsigned int MassRatio;
    bool         LinkHorizontally;
    bool         LinkVertically;
    bool         EllipseFittingEnabled;
    bool         EllipseFittingDrawEllipse;
    int          EllipseFittingSlices;
    int          EllipseFittingMode;
    int          EllipseFittingEdgeThreshold;
    int          EllipseFittingErrorThreshold;
    bool         EllipseMaskEnabled;
    int          EllipseMaskSlices;
    int          EllipseMaskTransitionStart;
    int          EllipseMaskTransitionEnd;
    int          EllipseMargin;

    vctDynamicVector<svlFilterImageCenterFinderInterface*> Receivers;

    vctDynamicVector<int> CenterX;
    vctDynamicVector<int> CenterY;
    vctDynamicVector<int> RadiusX;
    vctDynamicVector<int> RadiusY;
    vctDynamicVector<int> CenterXInternal;
    vctDynamicVector<int> CenterYInternal;
    vctDynamicVector<int> RadiusXInternal;
    vctDynamicVector<int> RadiusYInternal;
    vctDynamicVector<svlEllipse> Ellipse;

    vctDynamicVector< vctDynamicVector<unsigned int> > ProjectionV;
    vctDynamicVector< vctDynamicVector<unsigned int> > ProjectionH;

    // Ellipse fitting
    vctDynamicVector< vctDynamicMatrix<int> > RadialProfiles;
    vctDynamicVector< vctDynamicVector<int> > TempProfile;
    vctDynamicVector< vctDynamicVector<int> > StepX;
    vctDynamicVector< vctDynamicVector<int> > StepY;
    vctDynamicVector< vctDynamicVector<int> > EdgeX;
    vctDynamicVector< vctDynamicVector<int> > EdgeY;
    vctDynamicVector< vctDynamicVector<int> > EdgeRad;
    vctDynamicVector< vctDynamicVector<int> > EdgeRadSmooth;
    vctDynamicVector<svlImageProcessing::Internals> EllipseFittingInternals;

    // Mask generation
    svlSampleImage* MaskImage;
    svlSampleImageMono8* TransitionImage;
    vctDynamicVector<svlDraw::Internals> TriangleInternals;
    vctDynamicVector<svlDraw::Internals> WarpInternals;
    vctDynamicVector< vctDynamicVector<vctInt2> > EllipseSamplesIn;
    vctDynamicVector< vctDynamicVector<vctInt2> > EllipseSamplesOut;
};

class CISST_EXPORT svlFilterImageCenterFinderInterface
{
public:
    virtual int OnChangeCenter(int x, int y, unsigned int videoch = SVL_LEFT);
    virtual int OnChangeCenterRect(const svlRect & rect, unsigned int videoch = SVL_LEFT);
    virtual int OnChangeCenterEllipse(const svlEllipse & ellipse, unsigned int videoch = SVL_LEFT);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageCenterFinder)

#endif // _svlFilterImageCenterFinder_h

