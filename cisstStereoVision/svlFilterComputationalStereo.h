/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterComputationalStereo_h
#define _svlFilterComputationalStereo_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlCameraGeometry.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlComputationalStereoMethodBase
{
public:
    svlComputationalStereoMethodBase() {}
    virtual ~svlComputationalStereoMethodBase() {}

    virtual int Initialize() = 0;
    virtual int Process(svlSampleImage * images, int * depthmap) = 0;
    virtual void Free() = 0;
};

class CISST_EXPORT svlFilterComputationalStereo : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    enum StereoMethod {
        DynamicProgramming
    };

    svlFilterComputationalStereo();
    virtual ~svlFilterComputationalStereo();

    int  SetCameraGeometry(const svlCameraGeometry & geometry);
    void SetROI(const svlRect & rect);
    void SetROI(int left, int top, int right, int bottom);
    int  SetSubpixelPrecision(bool enabled);
    int  SetCrossCheck(bool enabled);
    void SetDisparityRange(unsigned int mindisparity, unsigned int maxdisparity);
    void SetBlockSize(unsigned int blocksize);
    void SetScalingFactor(unsigned int scalefactor);
    void SetQuickSearchRadius(unsigned int searchradius);
    void SetSmoothnessFactor(unsigned int smoothness);
    void SetTemporalFiltering(double tempfilt);
    void SetSpatialFiltering(unsigned int radius);

    bool         GetSubpixelPrecision();
    bool         GetCrossCheck();
    void         GetDisparityRange(unsigned int& mindisparity, unsigned int& maxdisparity);
    unsigned int GetBlockSize();
    unsigned int GetScalingFactor();
    unsigned int GetQuickSearchRadius();
    unsigned int GetSmoothnessFactor();
    double       GetTemporalFiltering();
    unsigned int GetSpatialFiltering();

private:
    // Work in progress...
    // Other methods not available yet.
    void SetMethod(StereoMethod method);
    StereoMethod GetMethod();

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleMatrixFloat* OutputMatrix;

    svlComputationalStereoMethodBase* StereoAlgorithm;
    svlComputationalStereoMethodBase* XCheckStereoAlgorithm;

    svlSampleImage* XCheckImage;
    vctDynamicMatrix<int> DisparityBuffer;
    vctDynamicMatrix<int> XCheckDisparityBuffer;

    vctDynamicMatrix<float> SpatialFilterBuffer;

    vctDynamicMatrix<float> UnitSurfaceVectors;
    vctDynamicVector<unsigned int> SurfaceImageMap;

    svlCameraGeometry Geometry;
    svlRect ROI;

    int    MinDisparity;
    int    MaxDisparity;
    int    ScaleFactor;
    int    BlockSize;
    int    NarrowedSearchRadius;
    int    Smoothness;
    double TemporalFilter;
    int    SpatialFilterRadius;
    bool   SubpixelPrecision;
    bool   XCheckEnabled;
    StereoMethod Method;

    template <class _paramType>
    void CreateXCheckImageMono(_paramType* source, _paramType* target, const unsigned int width, const unsigned int height);
    void CreateXCheckImageColor(unsigned char* source, unsigned char* target, const unsigned int width, const unsigned int height);

    void PerformXCheck();
    void ConvertDisparitiesToFloat(int* input, float* output, const int width, const int height);
    void ApplySpatialFilter(const int radius,
                            float* depthmap, float* tempbuffer,
                            const int mapwidth, const int mapheight, const int linestride);
};

// *******************************************************************
// CreateXCheckImageMono PRIVATE method
// arguments:
//           source         - input image
//           target         - output image
//           width          - width of images
//           height         - height of images
// function:
//    For single channel input images
//    Creates mirrored image for left-right check
// *******************************************************************
template <class _paramType>
void svlFilterComputationalStereo::CreateXCheckImageMono(_paramType* source, _paramType* target, const unsigned int width, const unsigned int height)
{
    unsigned int i, j;
    _paramType* dst = target + width * height - 1;
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            *dst = *source; source ++; dst --;
        }
    }
}

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterComputationalStereo)

#endif // _svlFilterComputationalStereo_h

