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

#ifndef _svlImageProcessing_h
#define _svlImageProcessing_h

#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlCameraGeometry.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlImageProcessingInternals;


namespace svlImageProcessing
{
    class CISST_EXPORT Internals
    {
        public:
            Internals();
            ~Internals();
            svlImageProcessingInternals* Get();
            void Set(svlImageProcessingInternals* ib);
            void Release();
        private:
            svlImageProcessingInternals* Ptr;
    };


    enum DI_Algorithm
    {
        DI_None,
        DI_Blending,
        DI_Discarding,
        DI_AdaptiveBlending,
        DI_AdaptiveDiscarding
    };


    int CISST_EXPORT Convolution(svlSampleImage* src_img,
                                 unsigned int src_videoch,
                                 svlSampleImage* dst_img,
                                 unsigned int dst_videoch,
                                 vctDynamicVector<double> kernel_horiz,
                                 vctDynamicVector<double> kernel_vert,
                                 bool absres = false);

    int CISST_EXPORT Convolution(svlSampleImage* src_img,
                                 unsigned int src_videoch,
                                 svlSampleImage* dst_img,
                                 unsigned int dst_videoch,
                                 vctDynamicMatrix<double> kernel,
                                 bool absres = false);

    int CISST_EXPORT UnsharpMask(const svlSampleImage* src_img,
                                 unsigned int src_videoch,
                                 svlSampleImage* dst_img,
                                 unsigned int dst_videoch,
                                 int radius,
                                 double amount,
                                 int threshold = 0);

    int CISST_EXPORT Crop(svlSampleImage* src_img,
                          unsigned int src_videoch,
                          svlSampleImage* dst_img,
                          unsigned int dst_videoch,
                          int left,
                          int top);

    int CISST_EXPORT Resize(svlSampleImage* src_img,
                            unsigned int src_videoch,
                            svlSampleImage* dst_img,
                            unsigned int dst_videoch,
                            bool interpolation);

    int CISST_EXPORT Resize(svlSampleImage* src_img,
                            unsigned int src_videoch,
                            svlSampleImage* dst_img,
                            unsigned int dst_videoch,
                            bool interpolation,
                            vctDynamicVector<unsigned char>& internals);

    int CISST_EXPORT Deinterlace(svlSampleImage* image,
                                 unsigned int videoch,
                                 DI_Algorithm algorithm);

    int CISST_EXPORT DisparityMapToSurface(svlSampleMatrixFloat* disparity_map,
                                           svlSampleImage3DMap* mesh_3d,
                                           svlCameraGeometry& camera_geometry,
                                           svlRect& roi);

    int CISST_EXPORT Rectify(svlSampleImage* src_img,
                             unsigned int src_videoch,
                             svlSampleImage* dst_img,
                             unsigned int dst_videoch,
                             const std::string& table_filename,
                             bool interpolation);

    int CISST_EXPORT Rectify(svlSampleImage* src_img,
                             unsigned int src_videoch,
                             svlSampleImage* dst_img,
                             unsigned int dst_videoch,
                             const std::string& table_filename,
                             bool interpolation,
                             Internals& internals);

    int CISST_EXPORT Rectify(svlSampleImage* src_img,
                             unsigned int src_videoch,
                             svlSampleImage* dst_img,
                             unsigned int dst_videoch,
                             bool interpolation,
                             Internals& internals);

    int CISST_EXPORT SetExposure(svlSampleImage* image,
                                 unsigned int videoch,
                                 double brightness,
                                 double contrast,
                                 double gamma);

    int CISST_EXPORT SetExposure(svlSampleImage* image,
                                 unsigned int videoch,
                                 double brightness,
                                 double contrast,
                                 double gamma,
                                 Internals& internals);

    int CISST_EXPORT Dilate(svlSampleImage* src_img,
                            unsigned int src_videoch,
                            svlSampleImage* dst_img,
                            unsigned int dst_videoch,
                            unsigned int radius);

    int CISST_EXPORT Erode(svlSampleImage* src_img,
                           unsigned int src_videoch,
                           svlSampleImage* dst_img,
                           unsigned int dst_videoch,
                           unsigned int radius);

    int CISST_EXPORT Blend(svlSampleImage* src1_img,
                           unsigned int src1_videoch,
                           svlSampleImage* src2_img,
                           unsigned int src2_videoch,
                           svlSampleImage* mask_img,
                           unsigned int mask_videoch,
                           svlSampleImage* dst_img,
                           unsigned int dst_videoch);

    int CISST_EXPORT SwapColorChannels(svlSampleImage* src_img,
                                       unsigned int src_videoch,
                                       svlSampleImage* dst_img,
                                       unsigned int dst_videoch);

    unsigned int CISST_EXPORT LabelBlobs(const svlSampleImageMono8* image,
                                         svlSampleImageMono32* labels,
                                         Internals& internals);
    unsigned int CISST_EXPORT LabelBlobs(const svlSampleImageMono8Stereo* image,
                                         svlSampleImageMono32Stereo* labels,
                                         const unsigned int videoch,
                                         Internals& internals);
    int CISST_EXPORT GetBlobsFromLabels(const svlSampleImageMono8* image,
                                        const svlSampleImageMono32* labels,
                                        svlSampleBlobs* blobs,
                                        Internals& internals,
                                        unsigned int min_area = 0,
                                        unsigned int max_area = 0,
                                        double min_compactness = 0.0,
                                        double max_compactness = 0.0);
    int CISST_EXPORT GetBlobsFromLabels(const svlSampleImageMono8Stereo* image,
                                        const svlSampleImageMono32Stereo* labels,
                                        svlSampleBlobs* blobs,
                                        const unsigned int videoch,
                                        Internals& internals,
                                        unsigned int min_area = 0,
                                        unsigned int max_area = 0,
                                        double min_compactness = 0.0,
                                        double max_compactness = 0.0);

    int CISST_EXPORT FitEllipse(vctDynamicVector<vctInt2> & points,
                                svlEllipse & ellipse,
                                Internals& internals);

    int CISST_EXPORT FitEllipse(vctDynamicVectorRef<vctInt2> & points,
                                svlEllipse & ellipse,
                                Internals& internals);

    int CISST_EXPORT FitEllipse(vctDynamicVector<int> & xs,
                                vctDynamicVector<int> & ys,
                                svlEllipse & ellipse,
                                Internals& internals);

    int CISST_EXPORT FitEllipse(vctDynamicVectorRef<int> & xs,
                                vctDynamicVectorRef<int> & ys,
                                svlEllipse & ellipse,
                                Internals& internals);
};

#endif // _svlImageProcessing_h

