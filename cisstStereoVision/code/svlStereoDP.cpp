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

#include "svlStereoDP.h"
#include <math.h>


/******************************************/
/*** svlStereoDP class ********************/
/******************************************/

// *******************************************************************
// svlStereoDP::svlStereoDP constructor
// arguments:
//           width                  - width of input and output images
//           height                 - height of input and output images
//           roi                    - region of interest (where computation will be performed)
//           ppoffset               - horizontal principal point difference (from stereo calibration)
//           max disparity          - maximum disparity
//           scale                  - image scale-down factor: 1/2^scale
//           searchrad              - search radius around t-1 frame
//           smoothness             - smoothness weight in DP optimization (the higher the smoother)
//           tempfilt               - temporal filtering (0 - off)
//           disparityinterpolation - disparity interpolation on/off
// *******************************************************************
svlStereoDP::svlStereoDP(int width, int height,
                         const svlRect & roi,
                         int mindisparity, int maxdisparity,
                         int ppoffset,
                         int scale,
                         int blocksize,
                         int searchrad,
                         int smoothness,
                         double tempfilt,
                         bool disparityinterpolation) :
    svlComputationalStereoMethodBase()
{
    // ScoreTruncationLevel has been determined
    // experimentally in the Scharstein-Szelinski paper.
    ScoreTruncationLevel = 255;

    // DiscontinuityThreshold is the threshold level
    // above which we assume that there is discontinuity
    // between neighbors.
    //   (Neighborhood is defined as 1 pixel distance on
    //    the original, high resolution image. In case of
    //    downscaled processing, this number is automatically
    //    compensated with the scaling factor.)
    if (scale == 0) DiscontinuityThreshold = 2.0;
    else DiscontinuityThreshold = 1.0;

    // MaxDisparityDifference defines what the maximal
    // difference may be between two neighboring disparities.
    // Increasing the number might make optimization more accurate
    // around discontinuities but slows down processing.
    //   (Neighborhood is defined as 1 pixel distance on
    //    the original, high resolution image. In case of
    //    downscaled processing, this number is automatically
    //    compensated with the scaling factor.)
    MaxDisparityDifference = 3;

    // Initializing filter parameters
    ScaleFactor = scale;
    InputWidth = width;
    InputHeight = height;
    SurfaceWidth = width >> scale;
    SurfaceHeight = height >> scale;
    ScaleWidth = width;
    ScaleHeight = height >> scale;
    ValidAreaLeft = roi.left >> scale;
    ValidAreaRight = (roi.right + (1 << scale)) >> scale;
    ValidAreaTop = roi.top >> scale;
    ValidAreaBottom = (roi.bottom + (1 << scale)) >> scale;
    Smoothness = smoothness >> scale;
    PrincipalPointOffset = ppoffset;
    MinDisparity = mindisparity;
    MaxDisparity = maxdisparity;
    DisparityRange = maxdisparity - mindisparity;
    BlockSize = blocksize;
    NarrowedSearchRadius = searchrad;
    TemporalFilter = tempfilt;
    DisparityInterpolation = disparityinterpolation;

    // Zeroing pointers
    LeftImage = 0;
    RightImage = 0;
    RightLineBuffer = 0;
    DisparityMap = 0;
    DisparityMapTemp = 0;
    DisparityCost = 0;
    DisparityGraph = 0;

    // Compensating with the scale factor.
    DiscontinuityThreshold *= (1 << scale);
    MaxDisparityDifference <<= scale;
}

// *******************************************************************
// svlStereoDP::~svlStereoDP destructor
// arguments:
// *******************************************************************
svlStereoDP::~svlStereoDP()
{
    Free();
}

// *******************************************************************
// svlStereoDP::Initialize method
// arguments:
// function:
//    To be called once before starting processing.
//    Allocates memory and computes look-up tables
// *******************************************************************
int svlStereoDP::Initialize()
{
    Free();

    // Allocate buffers
    DisparityCost = new unsigned short[DisparityRange * ST_DP_TEMP_BUFF_SIZE];
    DisparityGraph = new unsigned short[DisparityRange * SurfaceWidth * SurfaceHeight];
    DisparityMap = new unsigned short[SurfaceWidth * SurfaceHeight];
    DisparityMapTemp = new unsigned short[SurfaceWidth * SurfaceHeight];
    LeftImage = new svlRGB[ScaleWidth * ScaleHeight];
    RightImage = new svlRGB[ScaleWidth * ScaleHeight];
    RightLineBuffer = new int[BlockSize * 3];

    memset(DisparityCost, 0, DisparityRange * ST_DP_TEMP_BUFF_SIZE * sizeof(unsigned short));
    memset(DisparityGraph, 0, DisparityRange * SurfaceWidth * SurfaceHeight * sizeof(unsigned short));
    memset(DisparityMap, 0, SurfaceWidth * SurfaceHeight * sizeof(unsigned short));
    memset(DisparityMapTemp, 0, SurfaceWidth * SurfaceHeight * sizeof(unsigned short));
    memset(LeftImage, 0, ScaleWidth * ScaleHeight * sizeof(svlRGB));
    memset(RightImage, 0, ScaleWidth * ScaleHeight * sizeof(svlRGB));
    memset(RightLineBuffer, 0, BlockSize * 3 * sizeof(int));

    // building look up tables for optimization
    int i, j, diff, absdiff;
    // Distance function
    for (j = 0; j < 256; j ++) {
        for (i = 0; i < 256; i ++) {
            if (i < j) absdiff = j - i;
            else absdiff = i - j;
            if (absdiff > static_cast<int>(DiscontinuityThreshold)) absdiff = static_cast<int>(DiscontinuityThreshold);
            diff = absdiff * absdiff * Smoothness / 4;
            DispDiffLUT[j][i] = diff;
        }
    }

    FrameCounter = 0;

    return 0;
}

// *******************************************************************
// svlStereoDP::Process method
// arguments:
//           images         - input image pair (non-padded, 3 color channels)
//           disparitymap   - output image pointer (non-padded, int32)
// function:
//    To be called once for each frame.
//    Computes disparity map from the input image pair
// *******************************************************************
int svlStereoDP::Process(svlSampleImage *images, int *disparitymap)
{
    if (images->GetVideoChannels() != 2 ||      // stereo ?
        images->GetBPP() != 3 ||                // 24 bits per pixel ?
        images->GetDataChannels() != 3)         // 3 color channels ?
        return -1;

    // Creating scales of the stereo input images
    CreateScale(reinterpret_cast<svlRGB*>(images->GetUCharPointer(SVL_LEFT)), LeftImage);
    CreateScale(reinterpret_cast<svlRGB*>(images->GetUCharPointer(SVL_RIGHT)), RightImage);

    // Running optimization
    DisparityOptimization();

    // Filtering result
    FilterDisparityMap();

    // Rendering output
    RenderDisparityMap(disparitymap);

    FrameCounter ++;

    return 0;
}

// *******************************************************************
// svlStereoDP::Free method
// arguments:
// function:
//    To be called after finishing processing. Destructor calls it too, just in case.
//    Releases all resources allocated in the Initialize function
// *******************************************************************
void svlStereoDP::Free()
{
    if (DisparityCost) {
        delete [] DisparityCost;
        DisparityCost = 0;
    }
    if (DisparityGraph) {
        delete [] DisparityGraph;
        DisparityGraph = 0;
    }
    if (DisparityMap) {
        delete [] DisparityMap;
        DisparityMap = 0;
    }
    if (DisparityMapTemp) {
        delete [] DisparityMapTemp;
        DisparityMapTemp = 0;
    }
    if (LeftImage) {
        delete [] LeftImage;
        LeftImage = 0;
    }
    if (RightImage) {
        delete [] RightImage;
        RightImage = 0;
    }
    if (RightLineBuffer) {
        delete [] RightLineBuffer;
        RightLineBuffer = 0;
    }
}

// *******************************************************************
// svlStereoDP::CreateScale PRIVATE method
// arguments:
//           src_img        - input image pointer (non-padded, RGB24)
//           dest_img       - output image pointer (non-padded, RGB24)
// function:
//    Scales down the input image vertically with the factor of 1/2^ScaleFactor
//    Horizontal size preserved for maximal depth resolution
// *******************************************************************
void svlStereoDP::CreateScale(svlRGB* src_img, svlRGB* dest_img)
{
    int i, j, l;
    unsigned int val_r, val_g, val_b;
    unsigned char *src, *tsrc, *dest;

    const int magfact = 1 << ScaleFactor;
    const int srclinestep = InputWidth * 3 - 2;
    const int srcblockstep_y = (magfact - 1) * InputWidth * 3;

    src = reinterpret_cast<unsigned char*>(src_img);
    dest = reinterpret_cast<unsigned char*>(dest_img);

    for (j = 0; j < ScaleHeight; j ++) {
        for (i = 0; i < ScaleWidth; i ++) {

            tsrc = src;
            src += 3;

            val_r = 0;
            val_g = 0;
            val_b = 0;

            for (l = 0; l < magfact; l ++) {
                val_r += *tsrc;
                tsrc ++;
                val_g += *tsrc;
                tsrc ++;
                val_b += *tsrc;
                tsrc += srclinestep;
            }

            *dest = val_r >> ScaleFactor;
            dest ++;
            *dest = val_g >> ScaleFactor;
            dest ++;
            *dest = val_b >> ScaleFactor;
            dest ++;
        }

        src += srcblockstep_y;
    }
}

// *******************************************************************
// svlStereoDP::DisparityOptimization PRIVATE method
// arguments:
// function:
//    2D Dynamic Programming in a single step
//    Carried out on the scaled down input images
//    It performs a full search on the first frame, then a narrowed
//    search on the following frames. Search area size defined in the
//    constructor as an argument.
//    Image buffer overflow is not checked! Make sure the valid image
//    area is set properly and the disparity search range is within the
//    valid area borders when calling the constructor.
// *******************************************************************
void svlStereoDP::DisparityOptimization()
{
    const int rowstride = SurfaceWidth;
    const int inputrowstride = ScaleWidth;
    const int disparitystride = SurfaceWidth * SurfaceHeight;
    const int bsbytes = BlockSize * 3;
    const int halfbsbytes = (bsbytes - 3) >> 1;
    unsigned char *left, *right;
    unsigned short *dispgraph, *dispcost1, *dispcost2;
    int *iptr1, *iptr2, *tmpintbuff;

    bool processed;
    int i, j, d, l, d1, d2;
    int cost, min_cost, min_cost_pos, disp, disp2, score;
    int min_prev_cost, min_next_cost;
    int cost_array[1024];
    int from1, to1, from2, to2, diff, error;
    int pb_min_cost_pos, pb_h21, pb_p1, pb_p2, weight1, weight2, pb_t1, pb_t2;
    int pdispmin1, pdispmin2, pdispmax1, pdispmax2;
    int offset, ijoffset, inputoffset;
    int pos = 1;


    ///////////////////////////////////////////////////////
    // Dynamic programming
    //   Cost:
    //     C[j+1] = C[j] + D[j,j+1] + Score[j+1]

    // initializing the upper left corner of the graph
    PrevLineDispMin[ValidAreaTop] = 0;
    PrevLineDispMax[ValidAreaTop] = DisparityRange;
    offset = ValidAreaTop * rowstride + ValidAreaLeft;
    dispcost1 = DisparityCost + ValidAreaTop; // DisparityCost(0, ValidAreaTop, ValidAreaLeft)
    dispgraph = DisparityGraph + offset; // DisparityGraph(0, ValidAreaTop, ValidAreaLeft)
    for (i = 0; i < DisparityRange; i ++) {
        *dispcost1 = 0;
        *dispgraph = 0;
        dispcost1 += ST_DP_TEMP_BUFF_SIZE;
        dispgraph += disparitystride;
    }

    // "recursive" walkthrough to process the whole graph
    // processing a single diagonal line in each step
    do {
        processed = false;

        i = ValidAreaLeft;
        j = ValidAreaTop + pos;
        if (j >= ValidAreaBottom) {
            i = ValidAreaLeft + j - ValidAreaBottom + 1;
            j = ValidAreaBottom - 1;
        }
        pos ++;

        // processing next diagonal line
        while (i < ValidAreaRight && j >= ValidAreaTop) {

            ijoffset = j * rowstride + i;

        /////////////////////////////////////////////
        // processing single node at position (i, j)

            if (0) {//FrameCounter > 0) {
            // if not the first frame:
            //    perform narrowed search

                pdispmin1 = PrevLineDispMin[j];
                pdispmin2 = PrevLineDispMin[j - 1];
                pdispmax1 = PrevLineDispMax[j];
                pdispmax2 = PrevLineDispMax[j - 1];

                // compute range for narrowed search
                disp = DisparityMap[ijoffset];
                if (DisparityInterpolation) disp >>= 2;
                from1 = disp - NarrowedSearchRadius;
                if (from1 < 0) from1 = 0;
                to1 = disp + NarrowedSearchRadius;
                if (to1 > DisparityRange) to1 = DisparityRange;

                // compute ScoreCache (costs from previous diagonal)
                inputoffset = j * inputrowstride + (i << ScaleFactor);
                right = reinterpret_cast<unsigned char*>(RightImage + inputoffset);
                left = reinterpret_cast<unsigned char*>(LeftImage + inputoffset + from1 + MinDisparity + PrincipalPointOffset);

                if (BlockSize > 1) {
                    right -= halfbsbytes;
                    tmpintbuff = RightLineBuffer;
                    for (d = 0; d < bsbytes; d ++) {
                        *tmpintbuff = *right;
                        tmpintbuff ++; right ++;
                    }

                    for (d = from1; d < to1; d ++) {
                        left -= halfbsbytes;

                        // computing error
                        tmpintbuff = RightLineBuffer;
                        error = 0;
                        for (d2 = 0; d2 < bsbytes; d2 ++) {
                            diff = *tmpintbuff - *left;
                            left ++; tmpintbuff ++;
                            if (diff < 0) error -= diff;
                            else error += diff;
                        }
                        // scoring
                        ScoreCache[d] = error / BlockSize;

                        left -= halfbsbytes;
                    }
                }
                else {
                    RightLineBuffer[0] = right[0];
                    RightLineBuffer[1] = right[1];
                    RightLineBuffer[2] = right[2];

                    for (d = from1; d < to1; d ++) {
                        // computing error
                        // R
                        diff = RightLineBuffer[0] - *left;
                        left ++;
                        if (diff < 0) error = -diff;
                        else error = diff;
                        // G
                        diff = RightLineBuffer[1] - *left;
                        left ++;
                        if (diff < 0) error -= diff;
                        else error += diff;
                        // B
                        diff = RightLineBuffer[2] - *left;
                        left ++;
                        if (diff < 0) error -= diff;
                        else error += diff;
                        // scoring
                        ScoreCache[d] = error;
                    }
                }

                // compute range for PrevCostCache
                from2 = from1 - MaxDisparityDifference;
                if (from2 < 0) from2 = 0;
                to2 = to1 + MaxDisparityDifference;
                if (to2 > DisparityRange) to2 = DisparityRange;

                // compute PrevCostCache
                dispcost1 = DisparityCost + from2 * ST_DP_TEMP_BUFF_SIZE + j; // DisparityCost(from2, j, i - 1)
                dispcost2 = dispcost1 - 1; // DisparityCost(from2, j - 1, i)
                if (i > ValidAreaLeft) {
                    if (j > ValidAreaTop) {
                        for (d = from2; d < to2; d ++) {
                            if (d < pdispmin1 || d >= pdispmax1) d1 = BIG_I32_VAL;
                            else d1 = *dispcost1;
                            if (d < pdispmin2 || d >= pdispmax2) d2 = BIG_I32_VAL;
                            else d2 = *dispcost2;
                            PrevCostCache[d] = (d1 + d2 + 1) >> 1;
                            dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                            dispcost2 += ST_DP_TEMP_BUFF_SIZE;
                        }
                    }
                    else {
                        for (d = from2; d < to2; d ++) {
                            if (d < pdispmin1 || d >= pdispmax1) d1 = BIG_I32_VAL;
                            else d1 = *dispcost1;
                            PrevCostCache[d] = d1;
                            dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                        }
                    }
                }
                else {
                    for (d = from2; d < to2; d ++) {
                        if (d < pdispmin2 || d >= pdispmax2) d2 = BIG_I32_VAL;
                        else d2 = *dispcost2;
                        PrevCostCache[d] = d2;
                        dispcost2 += ST_DP_TEMP_BUFF_SIZE;
                    }
                }

                ////////////////////////////////////////////////
                // computing cost for new nodes

                // only for the lower right corner:
                if ((i == ValidAreaRight - 1) && (j == ValidAreaBottom - 1)) {
                    // below narrowed search range: maximal cost
                    dispcost1 = DisparityCost + j; // DisparityCost(0, j, i)
                    for (d = 0; d < from1; d ++) {
                        *dispcost1 = MAX_UI16_VAL;
                        dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                    }

                    // above narrowed search range: maximal cost
                    dispcost1 = DisparityCost + to1 * ST_DP_TEMP_BUFF_SIZE + j; // DisparityCost(to1, j, i)
                    for (d = to1; d < DisparityRange; d ++) {
                        *dispcost1 = MAX_UI16_VAL;
                        dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                    }
                }

                offset = from1 * disparitystride + ijoffset;
                dispcost1 = DisparityCost + from1 * ST_DP_TEMP_BUFF_SIZE + j; // DisparityCost(from1, j, i)
                dispgraph = DisparityGraph + offset; // DisparityCost(from1, j, i)
                for (d = from1; d < to1; d ++) {

                    // compute neighborhood range
                    from2 = d - MaxDisparityDifference;
                    if (from2 < 0) from2 = 0;
                    to2 = d + MaxDisparityDifference;
                    if (to2 > DisparityRange) to2 = DisparityRange;

                    score = ScoreCache[d];
                    if (score > ScoreTruncationLevel) score = ScoreTruncationLevel;
                    iptr1 = PrevCostCache + from2;
                    iptr2 = DispDiffLUT[d] + from2;

                    min_cost = MAX_I32_VAL;
					min_cost_pos = 0;
                    for (l = from2; l < to2; l ++) {

                        cost_array[l] = cost = *iptr1 + *iptr2 + score;

                        iptr1 ++;
                        iptr2 ++;

                        if (cost < min_cost) {
                            min_cost = cost;
                            min_cost_pos = l;
                        }
                    }

                    *dispcost1 = min_cost;

                    if (!DisparityInterpolation) {
                        *dispgraph = static_cast<unsigned short>(min_cost_pos);
                    }
                    else {
                        if (min_cost_pos == from2) min_prev_cost = min_cost + 10000;
                        else min_prev_cost = cost_array[min_cost_pos - 1];
                        if (min_cost_pos == (to2 - 1)) min_next_cost = min_cost + 10000;
                        else min_next_cost = cost_array[min_cost_pos + 1];

                        // parabole fitting
                        pb_min_cost_pos = ((min_cost_pos - 1) << 2);
                        pb_h21 = (min_cost - min_prev_cost) << 1;
                        pb_p1 = min_next_cost - min_prev_cost - pb_h21;
                        if (pb_p1 > 0) {
                            pb_p2 = (pb_p1 * (pb_min_cost_pos << 1)) - (pb_h21 << 3);
                            pb_t1 = (pb_p2 / pb_p1) >> 1;
                            pb_t2 = pb_min_cost_pos + 8;
                            if (pb_t1 < pb_min_cost_pos) pb_t1 = pb_min_cost_pos;
                            else if (pb_t1 > pb_t2) pb_t1 = pb_t2;
                            *dispgraph = static_cast<unsigned short>(pb_t1);
                        }
                        else {
                            *dispgraph = static_cast<unsigned short>(pb_min_cost_pos + 4);
                        }
                    }

                    dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                    dispgraph += disparitystride;
                }

                // store local disparity range
                PrevLineDispMin[j] = from1;
                PrevLineDispMax[j] = to1;
            }
            else {
            // if the first frame:
            //    perform full search

                // compute ScoreCache (costs from previous diagonal)
                inputoffset = j * inputrowstride + (i << ScaleFactor);
                right = reinterpret_cast<unsigned char*>(RightImage + inputoffset);
                left = reinterpret_cast<unsigned char*>(LeftImage + inputoffset + MinDisparity + PrincipalPointOffset);

                if (BlockSize > 1) {
                    right -= halfbsbytes;
                    tmpintbuff = RightLineBuffer;
                    for (d = 0; d < bsbytes; d ++) {
                        *tmpintbuff = *right;
                        tmpintbuff ++; right ++;
                    }

                    for (d = 0; d < DisparityRange; d ++) {
                        left -= halfbsbytes;

                        // computing error
                        tmpintbuff = RightLineBuffer;
                        error = 0;
                        for (d2 = 0; d2 < bsbytes; d2 ++) {
                            diff = *tmpintbuff - *left;
                            left ++; tmpintbuff ++;
                            if (diff < 0) error -= diff;
                            else error += diff;
                        }
                        // scoring
                        ScoreCache[d] = error / BlockSize;

                        left -= halfbsbytes;
                    }
                }
                else {
                    RightLineBuffer[0] = right[0];
                    RightLineBuffer[1] = right[1];
                    RightLineBuffer[2] = right[2];

                    for (d = 0; d < DisparityRange; d ++) {
                        // computing error
                        // R
                        diff = RightLineBuffer[0] - *left;
                        left ++;
                        if (diff < 0) error = -diff;
                        else error = diff;
                        // G
                        diff = RightLineBuffer[1] - *left;
                        left ++;
                        if (diff < 0) error -= diff;
                        else error += diff;
                        // B
                        diff = RightLineBuffer[2] - *left;
                        left ++;
                        if (diff < 0) error -= diff;
                        else error += diff;
                        // scoring
                        ScoreCache[d] = error;
                    }
                }

                // compute PrevCostCache
                dispcost1 = DisparityCost + j; // DisparityCost(0, j, i - 1)
                dispcost2 = dispcost1 - 1 ; // DisparityCost(0, j - 1, i)
                if (i > ValidAreaLeft) {
                    if (j > ValidAreaTop) {
                        for (d = 0; d < DisparityRange; d ++) {
                            PrevCostCache[d] = (*dispcost1 + *dispcost2 + 1) >> 1;
                            dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                            dispcost2 += ST_DP_TEMP_BUFF_SIZE;
                        }
                    }
                    else {
                        for (d = 0; d < DisparityRange; d ++) {
                            PrevCostCache[d] = *dispcost1;
                            dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                        }
                    }
                }
                else {
                    for (d = 0; d < DisparityRange; d ++) {
                        PrevCostCache[d] = *dispcost2;
                        dispcost2 += ST_DP_TEMP_BUFF_SIZE;
                    }
                }

                ////////////////////////////////////////////////
                // computing cost for new nodes

                dispcost1 = DisparityCost + j; // DisparityCost(0, j, i)
                dispgraph = DisparityGraph + ijoffset; // DisparityCost(0, j, i)
                for (d = 0; d < DisparityRange; d ++) {

                    // compute neighborhood range
                    from2 = d - MaxDisparityDifference;
                    if (from2 < 0) from2 = 0;
                    to2 = d + MaxDisparityDifference;
                    if (to2 > DisparityRange) to2 = DisparityRange;

                    score = ScoreCache[d];
                    if (score > ScoreTruncationLevel) score = ScoreTruncationLevel;
                    iptr1 = PrevCostCache + from2;
                    iptr2 = DispDiffLUT[d] + from2;

                    min_cost = MAX_I32_VAL;
					min_cost_pos = 0;
                    for (l = from2; l < to2; l ++) {

                        cost_array[l] = cost = *iptr1 + *iptr2 + score;

                        iptr1 ++;
                        iptr2 ++;

                        if (cost < min_cost) {
                            min_cost = cost;
                            min_cost_pos = l;
                        }
                    }

                    *dispcost1 = min_cost;

                    if (!DisparityInterpolation) {
                        *dispgraph = static_cast<unsigned short>(min_cost_pos);
                    }
                    else {
                        if (min_cost_pos == from2) min_prev_cost = min_cost + 10000;
                        else min_prev_cost = cost_array[min_cost_pos - 1];
                        if (min_cost_pos == (to2 - 1)) min_next_cost = min_cost + 10000;
                        else min_next_cost = cost_array[min_cost_pos + 1];

                        // parabole fitting
                        pb_min_cost_pos = ((min_cost_pos - 1) << 2);
                        pb_h21 = (min_cost - min_prev_cost) << 1;
                        pb_p1 = min_next_cost - min_prev_cost - pb_h21;
                        if (pb_p1 > 0) {
                            pb_p2 = (pb_p1 * (pb_min_cost_pos << 1)) - (pb_h21 << 3);
                            pb_t1 = (pb_p2 / pb_p1) >> 1;
                            pb_t2 = pb_min_cost_pos + 8;
                            if (pb_t1 < pb_min_cost_pos) pb_t1 = pb_min_cost_pos;
                            else if (pb_t1 > pb_t2) pb_t1 = pb_t2;
                            *dispgraph = static_cast<unsigned short>(pb_t1);
                        }
                        else {
                            *dispgraph = static_cast<unsigned short>(pb_min_cost_pos + 4);
                        }
                    }

                    dispcost1 += ST_DP_TEMP_BUFF_SIZE;
                    dispgraph += disparitystride;
                }
            }
        // Processing nodes at position (i, j)
        ////////////////////////////////////////

            i ++;
            j --;

            processed = true;
        }

    } while (processed);

    // Dynamic programming
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////
    // Going back along the lowest cost path (surface) to get the final disparity map

    pos -= 1;

    // starting from the lower right corner:
    i = ValidAreaRight - 1;
    j = ValidAreaBottom - 1;

    ijoffset = j * rowstride + i;

    //   finding the lowest cost
    min_cost = MAX_I32_VAL;
	min_cost_pos = 0;
    dispcost1 = DisparityCost + j; // DisparityCost(0, j, i)
    for (d = 0; d < DisparityRange; d ++) {
        cost = *dispcost1;
        if (cost < min_cost) {
            min_cost = cost;
            min_cost_pos = d;
        }
        dispcost1 += ST_DP_TEMP_BUFF_SIZE;
    }
    if (!DisparityInterpolation) DisparityMap[ijoffset] = min_cost_pos;
    else DisparityMap[ijoffset] = min_cost_pos << 2;

    // "recursive" walkback to process the whole graph
    do {
        i = ValidAreaLeft;
        j = ValidAreaTop + pos;
        if (j >= ValidAreaBottom) {
            i = ValidAreaLeft + j - ValidAreaBottom + 1;
            j = ValidAreaBottom - 1;
        }
        pos --;

        // processing next diagonal line
        while (i < ValidAreaRight && j >= ValidAreaTop) {

            ijoffset = j * rowstride + i;

        ////////////////////////////////////////
        // processing graph at position (i, j)

            if (!DisparityInterpolation) {
                disp = DisparityMap[ijoffset];
                if (disp >= DisparityRange) disp = DisparityRange - 1;
                offset = disp * disparitystride + ijoffset;
                disp = DisparityGraph[offset]; // DisparityGraph(DisparityMap(j, i), j, i)
            }
            else {
                disp = DisparityMap[ijoffset];
                weight2 = disp % 4;
                weight1 = 4 - weight2;
                disp >>= 2;
                if (disp < (DisparityRange - 1)) {
                    offset = disp * disparitystride + ijoffset;
                    disp = DisparityGraph[offset];
                    offset += disparitystride;
                    disp2 = DisparityGraph[offset];
                    disp = (disp * weight1 + disp2 * weight2) >> 2;
                }
                else {
                    offset = (DisparityRange - 1) * disparitystride + ijoffset;
                    disp = DisparityGraph[offset];
                }
            }

            // left neighbor
            if (i > ValidAreaLeft) {
                offset = ijoffset - 1;
                if (j < ValidAreaBottom - 1) {
                    DisparityMap[offset] = (DisparityMap[offset] + disp + 1) >> 1;
                }
                else {
                    DisparityMap[offset] = disp;
                }
            }
            // top neighbor
            if (j > ValidAreaTop) {
                offset = ijoffset - rowstride;
                DisparityMap[offset] = disp;
            }

        // Processing graph at position (i, j)
        ////////////////////////////////////////

            i ++;
            j --;
        }

    } while (pos > 0);

    // Going back along the lowest cost path (surface) to get the final disparity map
    ///////////////////////////////////////////////////////////////////////////////////
}

// *******************************************************************
// svlStereoDPMono::FilterDisparityMap PRIVATE method
// arguments:
// function:
//    Performs temporal filtering if enabled
// *******************************************************************
void svlStereoDP::FilterDisparityMap()
{
    if (fabs(TemporalFilter) < 0.01) return;

    if (FrameCounter > 0) {
        int i, j;
        unsigned short *dmap = DisparityMap;
        unsigned short *tdmap = DisparityMapTemp;
        const int tfilt = static_cast<int>(TemporalFilter * 256);
        const int dvdr = 256 + tfilt;

        for (j = 0; j < SurfaceHeight; j ++) {
            for (i = 0; i < SurfaceWidth; i ++) {
                *tdmap = static_cast<unsigned short>((( static_cast<int>(*tdmap) * tfilt) + (*dmap << 8)) / dvdr);
                dmap ++; tdmap ++;
            }
        }
        // swapping disparity buffers
        unsigned short *tbuff;
        tbuff = DisparityMap;
        DisparityMap = DisparityMapTemp;
        DisparityMapTemp = tbuff;
    }
    else {
        memcpy(DisparityMapTemp, DisparityMap, SurfaceWidth * SurfaceHeight * sizeof(unsigned short));
    }
}

// *******************************************************************
// svlStereoDP::RenderDisparityMap PRIVATE method
// arguments:
//           disparitymap   - output image pointer (non-padded, signed int32)
// function:
//    Stretches the scaled-down disparity map to full scale
// *******************************************************************
void svlStereoDP::RenderDisparityMap(int *disparitymap)
{
    const int scale = 1 << ScaleFactor;
    const int nextrowstride = InputWidth - scale;
    const int blockrowstride = InputWidth * (scale - 1);

    int *output;
    unsigned short *dmap = DisparityMap;
    int i, j, k, l, val, dispoffset;

    if (DisparityInterpolation) dispoffset = (MinDisparity + PrincipalPointOffset) << 2;
    else dispoffset = MinDisparity + PrincipalPointOffset;

    for (j = 0; j < SurfaceHeight; j ++) {
        for (i = 0; i < SurfaceWidth; i ++) {
            val = *dmap + dispoffset;
            output = disparitymap;
            for (l = 0; l < scale; l ++) {
                for (k = 0; k < scale; k ++) {
                    *output = val;
                    output ++;
                }
                output += nextrowstride;
            }
            dmap ++;
            disparitymap += scale;
        }
        disparitymap += blockrowstride;
    }
}

