/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

    void SetTrajectorySmoothing(double smoothing);
    void SetThreshold(unsigned char thresholdlevel);
    void SetMassRatio(unsigned int ratio);
    void SetMask(bool enablemask);
    void SetHorizontalLink(bool enable);
    void SetVerticalLink(bool enable);

    double GetTrajectorySmoothing() const;
    unsigned char GetThreshold() const;
    unsigned int GetMassRatio() const;
    bool GetMask() const;
    bool GetHorizontalLink() const;
    bool GetVerticalLink() const;

    void AddReceiver(svlFilterImageCenterFinderInterface* receiver);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    double Smoothing;
    unsigned int ThresholdLevel;
    unsigned int MassRatio;
    bool MaskEnabled;
    bool LinkHorizontally;
    bool LinkVertically;
    vctDynamicVector<svlFilterImageCenterFinderInterface*> Receivers;

    vctDynamicVector<int> CenterX;
    vctDynamicVector<int> CenterY;
    vctDynamicVector<int> RadiusX;
    vctDynamicVector<int> RadiusY;
    vctDynamicVector<int> CenterXInternal;
    vctDynamicVector<int> CenterYInternal;
    vctDynamicVector<int> RadiusXInternal;
    vctDynamicVector<int> RadiusYInternal;

    vctDynamicVector< vctDynamicVector<unsigned int> > ProjectionV;
    vctDynamicVector< vctDynamicVector<unsigned int> > ProjectionH;
};

class CISST_EXPORT svlFilterImageCenterFinderInterface
{
public:
    virtual int SetCenter(int x, int y, int rx, int ry, unsigned int videoch = SVL_LEFT) = 0;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageCenterFinder)

#endif // _svlFilterImageCenterFinder_h

