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

#include <cisstStereoVision/svlImageProcessing.h>
#include "svlImageProcessingHelper.h"


/************************************/
/*** svlImageProcessing namespace ***/
/************************************/

int svlImageProcessing::Convolution(svlSampleImage* src_img, unsigned int src_videoch,
                                    svlSampleImage* dst_img, unsigned int dst_videoch,
                                    vctDynamicVector<double> kernel_horiz,
                                    vctDynamicVector<double> kernel_vert)
{
    if (!src_img || src_img->GetVideoChannels() <= src_videoch ||
        !dst_img || dst_img->GetVideoChannels() <= dst_videoch) return SVL_FAIL;

    const svlPixelType type = src_img->GetPixelType();
    const int width  = static_cast<int>(src_img->GetWidth(src_videoch));
    const int height = static_cast<int>(src_img->GetHeight(src_videoch));

    if (type != dst_img->GetPixelType() ||
        width  < 1 || width  != static_cast<int>(dst_img->GetWidth(dst_videoch)) ||
        height < 1 || height != static_cast<int>(dst_img->GetHeight(dst_videoch))) return SVL_FAIL;

    vctDynamicVector<int> fp_kernel_horiz, fp_kernel_vert;
    fp_kernel_horiz.SetSize(kernel_horiz.size());
    fp_kernel_vert.SetSize(kernel_vert.size());
    fp_kernel_horiz.Assign(kernel_horiz.Multiply(1024));
    fp_kernel_vert.Assign(kernel_vert.Multiply(1024));

    switch (type) {
        case svlPixelRGB:
            svlImageProcessingHelper::ConvolutionRGB(src_img->GetUCharPointer(src_videoch),
                                                     dst_img->GetUCharPointer(dst_videoch),
                                                     width, height,
                                                     fp_kernel_horiz, true);
            svlImageProcessingHelper::ConvolutionRGB(dst_img->GetUCharPointer(dst_videoch),
                                                     src_img->GetUCharPointer(src_videoch),
                                                     width, height,
                                                     fp_kernel_vert, false);
        break;

        case svlPixelRGBA:
            svlImageProcessingHelper::ConvolutionRGBA(src_img->GetUCharPointer(src_videoch),
                                                      dst_img->GetUCharPointer(dst_videoch),
                                                      width, height,
                                                      fp_kernel_horiz, true);
            svlImageProcessingHelper::ConvolutionRGBA(dst_img->GetUCharPointer(dst_videoch),
                                                      src_img->GetUCharPointer(src_videoch),
                                                      width, height,
                                                      fp_kernel_vert, false);
        break;

        case svlPixelMono8:
            svlImageProcessingHelper::ConvolutionMono8(src_img->GetUCharPointer(src_videoch),
                                                       dst_img->GetUCharPointer(dst_videoch),
                                                       width, height,
                                                       fp_kernel_horiz, true);
            svlImageProcessingHelper::ConvolutionMono8(dst_img->GetUCharPointer(dst_videoch),
                                                       src_img->GetUCharPointer(src_videoch),
                                                       width, height,
                                                       fp_kernel_vert, false);
        break;

        case svlPixelMono16:
            svlImageProcessingHelper::ConvolutionMono16(reinterpret_cast<unsigned short*>(src_img->GetUCharPointer(src_videoch)),
                                                        reinterpret_cast<unsigned short*>(dst_img->GetUCharPointer(dst_videoch)),
                                                        width, height,
                                                        fp_kernel_horiz, true);
            svlImageProcessingHelper::ConvolutionMono16(reinterpret_cast<unsigned short*>(dst_img->GetUCharPointer(dst_videoch)),
                                                        reinterpret_cast<unsigned short*>(src_img->GetUCharPointer(src_videoch)),
                                                        width, height,
                                                        fp_kernel_vert, false);
        break;

        case svlPixelMono32:
            svlImageProcessingHelper::ConvolutionMono32(reinterpret_cast<unsigned int*>(src_img->GetUCharPointer(src_videoch)),
                                                        reinterpret_cast<unsigned int*>(dst_img->GetUCharPointer(dst_videoch)),
                                                        width, height,
                                                        fp_kernel_horiz, true);
            svlImageProcessingHelper::ConvolutionMono32(reinterpret_cast<unsigned int*>(dst_img->GetUCharPointer(dst_videoch)),
                                                        reinterpret_cast<unsigned int*>(src_img->GetUCharPointer(src_videoch)),
                                                        width, height,
                                                        fp_kernel_vert, false);
        break;

        default:
            return SVL_FAIL;
    }

    memcpy(dst_img->GetUCharPointer(dst_videoch),
           src_img->GetUCharPointer(src_videoch),
           src_img->GetDataSize(src_videoch));

    return SVL_OK;
}

int svlImageProcessing::Convolution(svlSampleImage* src_img, unsigned int src_videoch,
                                    svlSampleImage* dst_img, unsigned int dst_videoch,
                                    vctDynamicMatrix<double> kernel)
{
    if (!src_img || src_img->GetVideoChannels() <= src_videoch ||
        !dst_img || dst_img->GetVideoChannels() <= dst_videoch) return SVL_FAIL;

    const svlPixelType type = src_img->GetPixelType();
    const int width  = static_cast<int>(src_img->GetWidth(src_videoch));
    const int height = static_cast<int>(src_img->GetHeight(src_videoch));

    if (type != dst_img->GetPixelType() ||
        width  < 1 || width  != static_cast<int>(dst_img->GetWidth(dst_videoch)) ||
        height < 1 || height != static_cast<int>(dst_img->GetHeight(dst_videoch))) return SVL_FAIL;

    vctDynamicMatrix<int> fp_kernel;
    fp_kernel.SetSize(kernel.rows(), kernel.cols());
    fp_kernel.Assign(kernel.Multiply(1024));

    switch (type) {
        case svlPixelRGB:
            svlImageProcessingHelper::ConvolutionRGB(src_img->GetUCharPointer(src_videoch),
                                                     dst_img->GetUCharPointer(dst_videoch),
                                                     width, height,
                                                     fp_kernel);
        break;

        case svlPixelRGBA:
            svlImageProcessingHelper::ConvolutionRGBA(src_img->GetUCharPointer(src_videoch),
                                                      dst_img->GetUCharPointer(dst_videoch),
                                                      width, height,
                                                      fp_kernel);
        break;

        case svlPixelMono8:
            svlImageProcessingHelper::ConvolutionMono8(src_img->GetUCharPointer(src_videoch),
                                                       dst_img->GetUCharPointer(dst_videoch),
                                                       width, height,
                                                       fp_kernel);
        break;

        case svlPixelMono16:
            svlImageProcessingHelper::ConvolutionMono16(reinterpret_cast<unsigned short*>(src_img->GetUCharPointer(src_videoch)),
                                                        reinterpret_cast<unsigned short*>(dst_img->GetUCharPointer(dst_videoch)),
                                                        width, height,
                                                        fp_kernel);
        break;

        default:
            return SVL_FAIL;
    }

    return SVL_OK;
}

int svlImageProcessing::Crop(svlSampleImage* src_img, unsigned int src_videoch,
                             svlSampleImage* dst_img, unsigned int dst_videoch,
                             int left, int top)
{
    if (!src_img || !dst_img ||                               // source or destination is zero
        src_img->GetVideoChannels() <= src_videoch ||         // source has no such video channel
        dst_img->GetVideoChannels() <= dst_videoch ||         // destination has no such video channel
        src_img->GetPixelType() != dst_img->GetPixelType()) { // pixel type doesn't match
        return SVL_FAIL;
    }

    int i, wi, hi, wo, ho, xi, yi, xo, yo, copylen, linecount;
    unsigned char *in_data, *out_data;

    // Set background to black
    memset(dst_img->GetUCharPointer(dst_videoch), 0, dst_img->GetDataSize(dst_videoch));

    // Prepare for data copy
    wi = static_cast<int>(src_img->GetWidth(src_videoch) * src_img->GetBPP());
    hi = static_cast<int>(src_img->GetHeight(src_videoch));
    wo = static_cast<int>(dst_img->GetWidth(dst_videoch) * dst_img->GetBPP());
    ho = static_cast<int>(dst_img->GetHeight(dst_videoch));

    copylen = wo;
    linecount = ho;
    xi = left * src_img->GetBPP();
    yi = top;
    xo = yo = 0;

    // If cropping rectangle reaches out on the left
    if (xi < 0) {
        copylen += xi;
        xo -= xi;
        xi = 0;
    }
    // If cropping rectangle reaches out on the right
    if ((xi + copylen) > wi) {
        copylen += wi - (xi + copylen);
    }
    // If cropping rectangle is outside of the image boundaries
    if (copylen <= 0) return SVL_OK;

    // If cropping rectangle reaches out on the top
    if (yi < 0) {
        linecount += yi;
        yo -= yi;
        yi = 0;
    }
    // If cropping rectangle reaches out on the bottom
    if ((yi + linecount) > hi) {
        linecount += hi - (yi + linecount);
    }
    // If cropping rectangle is outside of the image boundaries
    if (linecount <= 0) return SVL_OK;

    in_data = src_img->GetUCharPointer(src_videoch) + (yi * wi) + xi;
    out_data = dst_img->GetUCharPointer(dst_videoch) + (yo * wo) + xo;

    for (i = 0; i < linecount; i ++) {
        memcpy(out_data, in_data, copylen);
        in_data += wi;
        out_data += wo;
    }

    return SVL_OK;
}


int svlImageProcessing::Resize(svlSampleImage* src_img, unsigned int src_videoch,
                               svlSampleImage* dst_img, unsigned int dst_videoch,
                               bool interpolation)
{
    // Please note if OpenCV is NOT enabled:
    //     This is slow because it may reallocate the work buffer every time it gets called.
    //     If possible, use the other implementation and provide a work buffer externally.
    // If OpenCV is enabled this warning does not apply.
    vctDynamicVector<unsigned char> internals;
    return Resize(src_img, src_videoch, dst_img, dst_videoch, interpolation, internals);
}

#if CISST_SVL_HAS_OPENCV
int svlImageProcessing::Resize(svlSampleImage* src_img, unsigned int src_videoch,
                               svlSampleImage* dst_img, unsigned int dst_videoch,
                               bool interpolation,
                               vctDynamicVector<unsigned char>& CMN_UNUSED(internals))
#else // CISST_SVL_HAS_OPENCV
int svlImageProcessing::Resize(svlSampleImage* src_img, unsigned int src_videoch,
                               svlSampleImage* dst_img, unsigned int dst_videoch,
                               bool interpolation,
                               vctDynamicVector<unsigned char>& internals)
#endif // CISST_SVL_HAS_OPENCV
{
    if (!src_img || !dst_img ||                               // source or destination is zero
        src_img->GetVideoChannels() <= src_videoch ||         // source has no such video channel
        dst_img->GetVideoChannels() <= dst_videoch ||         // destination has no such video channel
        src_img->GetPixelType() != dst_img->GetPixelType() || // image type mismatch
        (src_img->GetBPP() != 1 &&                            // pixel type is not Mono8
         src_img->GetBPP() != 3)) {                           // pixel type is not RGB
        return SVL_FAIL;
    }

    const unsigned int src_width  = src_img->GetWidth(src_videoch);
    const unsigned int src_height = src_img->GetHeight(src_videoch);
    const unsigned int dst_width  = dst_img->GetWidth(dst_videoch);
    const unsigned int dst_height = dst_img->GetHeight(dst_videoch);
    const bool weq = (src_width  == dst_width);
    const bool heq = (src_height == dst_height);

    if (weq && heq) {
        memcpy(dst_img->GetUCharPointer(dst_videoch), src_img->GetUCharPointer(src_videoch), src_img->GetDataSize(src_videoch));
        return SVL_OK;
    }
    else if (!interpolation && weq && src_height == (dst_height << 1)) {
        // Special case: decimate by 2 vertically

        const unsigned int stride  = src_width * 3;
        const unsigned int stride2 = stride << 1;
        unsigned char *src_buf = src_img->GetUCharPointer(src_videoch);
        unsigned char *dst_buf = dst_img->GetUCharPointer(dst_videoch);

        for (unsigned int j = 0; j < dst_height; j ++) {
            memcpy(dst_buf, src_buf, stride);
            dst_buf += stride;
            src_buf += stride2;
        }
        return SVL_OK;
    }

#if CISST_SVL_HAS_OPENCV

    if (interpolation) cvResize(src_img->IplImageRef(src_videoch), dst_img->IplImageRef(dst_videoch), CV_INTER_LINEAR);
    else cvResize(src_img->IplImageRef(src_videoch), dst_img->IplImageRef(dst_videoch), CV_INTER_NN);

#else // CISST_SVL_HAS_OPENCV

    if (src_img->GetBPP() == 3) { // RGB
        if (interpolation) {
            if (weq) {
                svlImageProcessingHelper::ResampleAndInterpolateVRGB24(src_img->GetUCharPointer(src_videoch),
                                                                       src_height,
                                                                       dst_img->GetUCharPointer(dst_videoch),
                                                                       dst_height,
                                                                       dst_width);
            }
            else if (heq) {
                svlImageProcessingHelper::ResampleAndInterpolateHRGB24(src_img->GetUCharPointer(src_videoch),
                                                                       src_width,
                                                                       dst_img->GetUCharPointer(dst_videoch),
                                                                       dst_width,
                                                                       src_height);
            }
            else {
                // Reallocate internal work buffer if needed
                const unsigned int internals_size = dst_width * src_height * 3;
                if (internals.size() < internals_size) internals.SetSize(internals_size);

                svlImageProcessingHelper::ResampleAndInterpolateHRGB24(src_img->GetUCharPointer(src_videoch),
                                                                       src_width,
                                                                       internals.Pointer(),
                                                                       dst_width,
                                                                       src_height);
                svlImageProcessingHelper::ResampleAndInterpolateVRGB24(internals.Pointer(),
                                                                       src_height,
                                                                       dst_img->GetUCharPointer(dst_videoch),
                                                                       dst_height,
                                                                       dst_width);
            }
        }
        else {
            svlImageProcessingHelper::ResampleRGB24(src_img->GetUCharPointer(src_videoch),
                                                    src_width,
                                                    src_height,
                                                    dst_img->GetUCharPointer(dst_videoch),
                                                    dst_width,
                                                    dst_height);
        }
    }
    else { // Mono8
        if (interpolation) {
            if (weq) {
                svlImageProcessingHelper::ResampleAndInterpolateVMono8(src_img->GetUCharPointer(src_videoch),
                                                                       src_height,
                                                                       dst_img->GetUCharPointer(dst_videoch),
                                                                       dst_height,
                                                                       dst_width);
            }
            else if (heq) {
                svlImageProcessingHelper::ResampleAndInterpolateHMono8(src_img->GetUCharPointer(src_videoch),
                                                                       src_width,
                                                                       dst_img->GetUCharPointer(dst_videoch),
                                                                       dst_width,
                                                                       src_height);
            }
            else {
                // Reallocate internal work buffer if needed
                const unsigned int internals_size = dst_width * src_height;
                if (internals.size() < internals_size) internals.SetSize(internals_size);

                svlImageProcessingHelper::ResampleAndInterpolateHMono8(src_img->GetUCharPointer(src_videoch),
                                                                       src_width,
                                                                       internals.Pointer(),
                                                                       dst_width,
                                                                       src_height);
                svlImageProcessingHelper::ResampleAndInterpolateVMono8(internals.Pointer(),
                                                                       src_height,
                                                                       dst_img->GetUCharPointer(dst_videoch),
                                                                       dst_height,
                                                                       dst_width);
            }
        }
        else {
            svlImageProcessingHelper::ResampleMono8(src_img->GetUCharPointer(src_videoch),
                                                    src_width,
                                                    src_height,
                                                    dst_img->GetUCharPointer(dst_videoch),
                                                    dst_width,
                                                    dst_height);
        }
    }

#endif // CISST_SVL_HAS_OPENCV

    return SVL_OK;
}


int svlImageProcessing::Deinterlace(svlSampleImage* image, unsigned int videoch, svlImageProcessing::DI_Algorithm algorithm)
{
    if (!image || image->GetVideoChannels() <= videoch || image->GetBPP() != 3) return SVL_FAIL;

    switch (algorithm) {
        case DI_None:
            // NOP
        break;

        case DI_Blending:
            svlImageProcessingHelper::DeinterlaceBlending(image->GetUCharPointer(videoch),
                                                          static_cast<int>(image->GetWidth(videoch)),
                                                          static_cast<int>(image->GetHeight(videoch)));
        break;

        case DI_Discarding:
            svlImageProcessingHelper::DeinterlaceDiscarding(image->GetUCharPointer(videoch),
                                                            static_cast<int>(image->GetWidth(videoch)),
                                                            static_cast<int>(image->GetHeight(videoch)));
        break;

        case DI_AdaptiveBlending:
            svlImageProcessingHelper::DeinterlaceAdaptiveBlending(image->GetUCharPointer(videoch),
                                                                  static_cast<int>(image->GetWidth(videoch)),
                                                                  static_cast<int>(image->GetHeight(videoch)));
        break;

        case DI_AdaptiveDiscarding:
            svlImageProcessingHelper::DeinterlaceAdaptiveDiscarding(image->GetUCharPointer(videoch),
                                                                    static_cast<int>(image->GetWidth(videoch)),
                                                                    static_cast<int>(image->GetHeight(videoch)));
        break;
    }

    return SVL_OK;
}


int svlImageProcessing::DisparityMapToSurface(svlSampleMatrixFloat* disparity_map,
                                              svlSampleImage3DMap* mesh_3d,
                                              svlCameraGeometry& camera_geometry,
                                              svlRect& roi)
{
    if (!disparity_map || !mesh_3d) return SVL_FAIL;

    const int disp_width = disparity_map->GetCols();
    const int disp_height = disparity_map->GetRows();
    const int mesh_width = mesh_3d->GetWidth();
    const int mesh_height = mesh_3d->GetHeight();
    if (disp_width != mesh_width || disp_height != mesh_height) return SVL_FAIL;

    const svlCameraGeometry::Intrinsics* intrinsicsL = camera_geometry.GetIntrinsicsPtr(SVL_LEFT);
    const svlCameraGeometry::Intrinsics* intrinsicsR = camera_geometry.GetIntrinsicsPtr(SVL_RIGHT);
    const svlCameraGeometry::Extrinsics* extrinsicsL = camera_geometry.GetExtrinsicsPtr(SVL_LEFT);
    const svlCameraGeometry::Extrinsics* extrinsicsR = camera_geometry.GetExtrinsicsPtr(SVL_RIGHT);
    if (!intrinsicsL || !intrinsicsR || !extrinsicsL || !extrinsicsR) return SVL_FAIL;
    if (camera_geometry.IsCameraPairRectified(SVL_LEFT, SVL_RIGHT) != SVL_YES) return SVL_FAIL;

    const float bl           = static_cast<float>(extrinsicsL->T.X() - extrinsicsR->T.X());
	const float rightcamposx = static_cast<float>(extrinsicsR->T.X());
    const float fl           = static_cast<float>(intrinsicsR->fc[0]);
    const float ppx          = static_cast<float>(intrinsicsR->cc[0]);
    const float ppy          = static_cast<float>(intrinsicsR->cc[1]);
	const float disp_corr    = static_cast<float>(intrinsicsL->cc[0]) - ppx;

    int l, t, r, b;
    l = roi.left;   if (l < 0) l = 0;
    t = roi.top;    if (t < 0) t = 0;
    r = roi.right;  if (r >= disp_width) r = disp_width - 1;
    b = roi.bottom; if (b >= disp_height) b = disp_height - 1;

    const unsigned int vertstride = mesh_width - r + l - 1;
    const unsigned int vertstride3 = vertstride * 3;
    float *disparities = disparity_map->GetPointer();
    float *vectors = mesh_3d->GetPointer();
    float fi, fj, disp, ratio;
    int i, j;

    disparities += t * mesh_width + l;
    vectors += (t * mesh_width + l) * 3;

    for (j = t; j <= b; j ++) {
        fj = static_cast<float>(j);

        for (i = l; i <= r; i ++) {
            fi = static_cast<float>(i);

            disp = (*disparities) - disp_corr; disparities ++;
            if (disp < 0.01f) disp = 0.01f;
            ratio = bl / disp;

            // Disparity map corresponds to right camera
            *vectors = (fi - ppx) * ratio - rightcamposx; vectors ++; // X
            *vectors = (fj - ppy) * ratio;                vectors ++; // Y
            *vectors = fl         * ratio;                vectors ++; // Z
        }

        disparities += vertstride;
        vectors += vertstride3;
    }

    return SVL_OK;
}


int svlImageProcessing::Rectify(svlSampleImage* src_img, unsigned int src_videoch,
                                svlSampleImage* dst_img, unsigned int dst_videoch,
                                const std::string& table_filename,
                                bool interpolation)
{
    Internals internals;
    return Rectify(src_img, src_videoch, dst_img, dst_videoch, table_filename, interpolation, internals);
}

int svlImageProcessing::Rectify(svlSampleImage* src_img, unsigned int src_videoch,
                                svlSampleImage* dst_img, unsigned int dst_videoch,
                                const std::string& table_filename,
                                bool interpolation,
                                svlImageProcessing::Internals& internals)
{
    if (!src_img || !dst_img ||                             // source or destination is zero
        src_img->GetVideoChannels() <= src_videoch ||       // source has no such video channel
        dst_img->GetVideoChannels() <= dst_videoch ||       // destination has no such video channel
        src_img->GetBPP() != 3 || dst_img->GetBPP() != 3) { // pixel type is not RGB
        return SVL_FAIL;
    }

    svlImageProcessingHelper::RectificationInternals* table = dynamic_cast<svlImageProcessingHelper::RectificationInternals*>(internals.Get());
    if (table == 0) {
        table = new svlImageProcessingHelper::RectificationInternals;

        // Load rectification LUT
        if (!table->Load(table_filename, 3)) return SVL_FAIL;

        internals.Set(table);
    }

    if (table->Width != src_img->GetWidth(src_videoch) ||
        table->Height != src_img->GetHeight(src_videoch)) return SVL_FAIL;

    dst_img->SetSize(dst_videoch, src_img->GetWidth(src_videoch), src_img->GetHeight(src_videoch));

    unsigned char* srcimg = src_img->GetUCharPointer(src_videoch);
    unsigned char* destimg = dst_img->GetUCharPointer(dst_videoch);

    unsigned char *srcbld1, *srcbld2, *srcbld3, *srcbld4;
    unsigned int *destidx, *srcidx1, *srcidx2, *srcidx3, *srcidx4;
    unsigned char *destr, *destg, *destb;
    unsigned char *srcr, *srcg, *srcb;
    unsigned int destofs, srcofs;
    unsigned int resr, resg, resb;
    unsigned int blnd;

    const unsigned int destlen = table->idxDestSize;

    if (interpolation) {
        destidx = table->idxDest;
        srcidx1 = table->idxSrc1;
        srcidx2 = table->idxSrc2;
        srcidx3 = table->idxSrc3;
        srcidx4 = table->idxSrc4;
        srcbld1 = table->blendSrc1;
        srcbld2 = table->blendSrc2;
        srcbld3 = table->blendSrc3;
        srcbld4 = table->blendSrc4;
    
        for (unsigned int i = 0; i < destlen; i ++) {

            // interpolation - 1st source pixel and weight
            srcofs = *srcidx1;
            srcr = srcimg + srcofs;
            srcg = srcr + 1;
            srcb = srcg + 1;

            blnd = *srcbld1;
            resr = blnd * (*srcr);
            resg = blnd * (*srcg);
            resb = blnd * (*srcb);

            // interpolation - 2nd source pixel and weight
            srcofs = *srcidx2;
            srcr = srcimg + srcofs;
            srcg = srcr + 1;
            srcb = srcg + 1;

            blnd = *srcbld2;
            resr += blnd * (*srcr);
            resg += blnd * (*srcg);
            resb += blnd * (*srcb);

            // interpolation - 3rd source pixel and weight
            srcofs = *srcidx3;
            srcr = srcimg + srcofs;
            srcg = srcr + 1;
            srcb = srcg + 1;

            blnd = *srcbld3;
            resr += blnd * (*srcr);
            resg += blnd * (*srcg);
            resb += blnd * (*srcb);

            // interpolation - 4th source pixel and weight
            srcofs = *srcidx4;
            srcr = srcimg + srcofs;
            srcg = srcr + 1;
            srcb = srcg + 1;

            blnd = *srcbld4;
            resr += blnd * (*srcr);
            resg += blnd * (*srcg);
            resb += blnd * (*srcb);

            // destination pixel
            destofs = *destidx;
            destr = destimg + destofs;
            destg = destr + 1;
            destb = destg + 1;

            *destr = static_cast<unsigned char>(resr >> 8);
            *destg = static_cast<unsigned char>(resg >> 8);
            *destb = static_cast<unsigned char>(resb >> 8);

            destidx ++;
            srcidx1 ++;
            srcidx2 ++;
            srcidx3 ++;
            srcidx4 ++;
            srcbld1 ++;
            srcbld2 ++;
            srcbld3 ++;
            srcbld4 ++;
        }
    }
    else {
        destidx = table->idxDest;
        srcidx1 = table->idxSrc1;
        svlRGB *prgb1, *prgb2;

        for (unsigned int i = 0; i < destlen; i ++) {
            
            // sampling - 1st source pixel
            srcofs = *srcidx1;
            prgb1 = reinterpret_cast<svlRGB*>(srcimg + srcofs);

            // copying value
            destofs = *destidx;
            prgb2 = reinterpret_cast<svlRGB*>(destimg + destofs);

            *prgb2 = *prgb1;

            destidx ++;
            srcidx1 ++;
        }
    }

    return SVL_OK;
}

int svlImageProcessing::Rectify(svlSampleImage* src_img, unsigned int src_videoch,
                                svlSampleImage* dst_img, unsigned int dst_videoch,
                                bool interpolation,
                                svlImageProcessing::Internals& internals)
{
    return Rectify(src_img, src_videoch, dst_img, dst_videoch, "", interpolation, internals);
}


int svlImageProcessing::SetExposure(svlSampleImage* image, unsigned int videoch, double brightness, double contrast, double gamma)
{
    svlImageProcessing::Internals internals;
    return SetExposure(image, videoch, brightness, contrast, gamma, internals);
}

int svlImageProcessing::SetExposure(svlSampleImage* image, unsigned int videoch, double brightness, double contrast, double gamma, svlImageProcessing::Internals& internals)
{
    if (!image || image->GetVideoChannels() <= videoch ||
        image->GetBPP() != image->GetDataChannels()) return SVL_FAIL;

    svlImageProcessingHelper::ExposureInternals* exposure = dynamic_cast<svlImageProcessingHelper::ExposureInternals*>(internals.Get());
    if (!exposure) {
        exposure = new svlImageProcessingHelper::ExposureInternals;
        internals.Set(exposure);
    }

    // Will not do anything if parameters have not changed
    exposure->SetBrightness(brightness);
    exposure->SetContrast(contrast);
    exposure->SetGamma(gamma);
    exposure->CalculateCurve();

    const unsigned int datachannels = image->GetDataChannels();
    const unsigned int toskip = (image->GetAlphaChannel() >= 0) ? 1 : 0;
    unsigned int pixelcount = image->GetWidth(videoch) * image->GetHeight(videoch);
    vctFixedSizeVectorRef<unsigned char, 255, 1> curve(exposure->Curve);

    unsigned char* ptr = image->GetUCharPointer(videoch);
    unsigned int i;

    while (pixelcount) {

        i = datachannels;
        while (i) {
            *ptr = curve[*ptr]; ptr ++;
            i --;
        }
        ptr += toskip;

        pixelcount --;
    }

    return SVL_OK;
}

int svlImageProcessing::Dilate(svlSampleImage* src_img, unsigned int src_videoch, svlSampleImage* dst_img, unsigned int dst_videoch, unsigned int radius)
{
    if (!src_img || src_img->GetVideoChannels() <= src_videoch ||
        !dst_img || dst_img->GetVideoChannels() <= dst_videoch) return SVL_FAIL;

    const int width  = static_cast<int>(src_img->GetWidth(src_videoch));
    const int height = static_cast<int>(src_img->GetHeight(src_videoch));

    if (src_img->GetPixelType() != svlPixelMono8 ||
        dst_img->GetPixelType() != svlPixelMono8 ||
        width  < 1 || width  != static_cast<int>(dst_img->GetWidth(dst_videoch)) ||
        height < 1 || height != static_cast<int>(dst_img->GetHeight(dst_videoch))) return SVL_FAIL;

    unsigned char *input  = src_img->GetUCharPointer(src_videoch);
    unsigned char *output = dst_img->GetUCharPointer(src_videoch);

    if (radius < 1) {
        memcpy(output, input, width * height);
        return SVL_OK;
    }

    // TO DO: Add support for larger radius
    radius = 1;

    unsigned char *input_u = input - width;
    unsigned char *input_d = input + width;
    unsigned char *input_l = input - 1;
    unsigned char *input_r = input + 1;

    const int widthm1  = width  - 1;
    const int heightm1 = height - 1;
    int i, j;

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            if (*input ||
                (j > 0        && *input_u) ||
                (j < heightm1 && *input_d) ||
                (i > 0        && *input_l) ||
                (i < widthm1  && *input_r)) {
                *output = 255;
            }
            else {
                *output = 0;
            }
            input_u ++;
            input_d ++;
            input_l ++;
            input_r ++;
            input   ++;
            output  ++;
        }
    }

    return SVL_OK;
}

int svlImageProcessing::Erode(svlSampleImage* src_img, unsigned int src_videoch, svlSampleImage* dst_img, unsigned int dst_videoch, unsigned int radius)
{
    if (!src_img || src_img->GetVideoChannels() <= src_videoch ||
        !dst_img || dst_img->GetVideoChannels() <= dst_videoch) return SVL_FAIL;

    const int width  = static_cast<int>(src_img->GetWidth(src_videoch));
    const int height = static_cast<int>(src_img->GetHeight(src_videoch));

    if (src_img->GetPixelType() != svlPixelMono8 ||
        dst_img->GetPixelType() != svlPixelMono8 ||
        width  < 1 || width  != static_cast<int>(dst_img->GetWidth(dst_videoch)) ||
        height < 1 || height != static_cast<int>(dst_img->GetHeight(dst_videoch))) return SVL_FAIL;

    unsigned char *input  = src_img->GetUCharPointer(src_videoch);
    unsigned char *output = dst_img->GetUCharPointer(src_videoch);

    if (radius < 1) {
        memcpy(output, input, width * height);
        return SVL_OK;
    }

    // TO DO: Add support for larger radius
    radius = 1;

    unsigned char *input_u = input - width;
    unsigned char *input_d = input + width;
    unsigned char *input_l = input - 1;
    unsigned char *input_r = input + 1;

    const int widthm1  = width  - 1;
    const int heightm1 = height - 1;
    int i, j;

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            if (*input == 0 ||
                j == 0 || j == heightm1 ||
                i == 0 || i == widthm1  ||
                (j > 0 && *input_u == 0) || (j < heightm1 && *input_d == 0) ||
                (i > 0 && *input_l == 0) || (i < widthm1  && *input_r == 0)) {
                *output = 0;
            }
            else {
                *output = 255;
            }
            input_u ++;
            input_d ++;
            input_l ++;
            input_r ++;
            input   ++;
            output  ++;
        }
    }

    return SVL_OK;
}


/*******************************************/
/*** svlImageProcessing::Internals class ***/
/*******************************************/

svlImageProcessing::Internals::Internals() :
    Ptr(0)
{
}

svlImageProcessing::Internals::~Internals()
{
    Release();
}

svlImageProcessingInternals* svlImageProcessing::Internals::Get()
{
    return Ptr;
}

void svlImageProcessing::Internals::Set(svlImageProcessingInternals* ib)
{
    Release();
    Ptr = ib;
}

void svlImageProcessing::Internals::Release()
{
    if (Ptr) delete Ptr;
    Ptr = 0;
}

