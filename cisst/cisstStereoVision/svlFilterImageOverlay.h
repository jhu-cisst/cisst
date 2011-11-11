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

#ifndef _svlFilterImageOverlay_h
#define _svlFilterImageOverlay_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlOverlayObjects.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <map>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterImageOverlay : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef svlFilterImageOverlay ThisType;
    typedef struct _ImageTransform {
	    int ID;
        vct3x3 frame;
    } ImageTransform;

protected:
    typedef std::map<svlFilterInput*, svlSample*> _SampleCacheMap;
    typedef std::map<int, vct3x3> _TransformCacheMap;

public:
    svlFilterImageOverlay();
    ~svlFilterImageOverlay();

    int AddInputImage(const std::string &name);
    int AddInputMatrix(const std::string &name);
    int AddInputTargets(const std::string &name);
    int AddInputBlobs(const std::string &name);
    int AddInputText(const std::string &name);
    void AddOverlay(svlOverlay & overlay);
    int AddQueuedItems();

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

protected:
    virtual void CreateInterfaces();
    virtual void SetTransform(const ThisType::ImageTransform & transform);
    virtual void SetTransforms(const vctDynamicVector<ThisType::ImageTransform> & transforms);

private:
    svlOverlay* FirstOverlay;
    svlOverlay* LastOverlay;
    _SampleCacheMap SampleCache;
    _TransformCacheMap TransformCache;

    osaCriticalSection CS;
    osaCriticalSection TransformCS;

    unsigned int ImageInputsToAddUsed;
    unsigned int MatrixInputsToAddUsed;
    unsigned int TargetInputsToAddUsed;
    unsigned int BlobInputsToAddUsed;
    unsigned int TextInputsToAddUsed;
    unsigned int OverlaysToAddUsed;
    vctDynamicVector<std::string> ImageInputsToAdd;
    vctDynamicVector<std::string> MatrixInputsToAdd;
    vctDynamicVector<std::string> TargetInputsToAdd;
    vctDynamicVector<std::string> BlobInputsToAdd;
    vctDynamicVector<std::string> TextInputsToAdd;
    vctDynamicVector<svlOverlay*> OverlaysToAdd;

    bool IsInputAlreadyQueued(const std::string &name);
    void AddQueuedItemsInternal();
};

typedef mtsGenericObjectProxy<svlFilterImageOverlay::ImageTransform> svlFilterImageOverlay_ImageTransform;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageOverlay_ImageTransform);
typedef mtsGenericObjectProxy<vctDynamicVector<svlFilterImageOverlay::ImageTransform> > svlFilterImageOverlay_ImageTransformVector;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageOverlay_ImageTransformVector);

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageOverlay)

CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterImageOverlay::ImageTransform & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const vctDynamicVector<svlFilterImageOverlay::ImageTransform> & objref);

#endif // _svlFilterImageOverlay_h

