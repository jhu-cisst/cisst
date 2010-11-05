/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#include "svlImageProcessingHelper.h"
#include "cisstCommon/cmnPortability.h"
#include <fstream>


/*****************************************/
/*** svlImageProcessingInternals class ***/
/*****************************************/

svlImageProcessingInternals::svlImageProcessingInternals()
{
    // Called only by derived class
}

svlImageProcessingInternals::svlImageProcessingInternals(const svlImageProcessingInternals& CMN_UNUSED(internals))
{
    // Never called
}

svlImageProcessingInternals::~svlImageProcessingInternals()
{
    // NOP
}


/******************************************/
/*** svlImageProcessingHelper namespace ***/
/******************************************/

void svlImageProcessingHelper::ConvolutionRGB(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal)
{
    if (!input || !output || kernel.size() < 1) return;

    const int kernel_size = kernel.size();
    const int kernel_rad = kernel_size / 2;
    const int rowstride = width * 3;
    int sum1, sum2, sum3;
    unsigned char *input2;
    int *kernelptr;
    int i, j, k_val;

    if (horizontal) {

        int k, k_from, k_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum1 = sum2 = sum3 = 0;

                kernelptr = kernel.Pointer();

                k = kernel_size;
                k_from = i - kernel_rad;
                k_to   = k_from + kernel_size;
                if (k_from < 0) {
                    k += k_from;
                    kernelptr -= k_from;
                    k_from = 0;
                }
                if (k_to > width) {
                    k -= k_to - width;
                    k_to = width;
                }

                input2 = input + j * rowstride + k_from * 3;

                while (k) {

                    k_val = *kernelptr; kernelptr ++;
                    sum1 += (*input2) * k_val; input2 ++; 
                    sum2 += (*input2) * k_val; input2 ++;
                    sum3 += (*input2) * k_val; input2 ++;

                    k --;
                }

                sum1 >>= 10; sum2 >>= 10; sum3 >>= 10;

                if (sum1 < 0) sum1 = 0; else if (sum1 > 255) sum1 = 255;
                *output = static_cast<unsigned char>(sum1); output ++;
                if (sum2 < 0) sum2 = 0; else if (sum2 > 255) sum2 = 255;
                *output = static_cast<unsigned char>(sum2); output ++;
                if (sum3 < 0) sum3 = 0; else if (sum3 > 255) sum3 = 255;
                *output = static_cast<unsigned char>(sum3); output ++;
            }
        }

    }
    else {

        const int rowstride_short = rowstride - 3;
        int l, l_from, l_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum1 = sum2 = sum3 = 0;

                kernelptr = kernel.Pointer();

                l = kernel_size;
                l_from = j - kernel_rad;
                l_to   = l_from + kernel_size;
                if (l_from < 0) {
                    l += l_from;
                    kernelptr -= l_from;
                    l_from = 0;
                }
                if (l_to > height) {
                    l -= l_to - height;
                    l_to = height;
                }

                input2 = input + l_from * rowstride + i * 3;

                while (l) {

                    k_val = *kernelptr; kernelptr ++;
                    sum1 += (*input2) * k_val; input2 ++; 
                    sum2 += (*input2) * k_val; input2 ++;
                    sum3 += (*input2) * k_val; input2 ++;

                    input2 += rowstride_short;

                    l --;
                }

                sum1 >>= 10; sum2 >>= 10; sum3 >>= 10;

                if (sum1 < 0) sum1 = 0; else if (sum1 > 255) sum1 = 255;
                *output = static_cast<unsigned char>(sum1); output ++;
                if (sum2 < 0) sum2 = 0; else if (sum2 > 255) sum2 = 255;
                *output = static_cast<unsigned char>(sum2); output ++;
                if (sum3 < 0) sum3 = 0; else if (sum3 > 255) sum3 = 255;
                *output = static_cast<unsigned char>(sum3); output ++;
            }
        }

    }
}

void svlImageProcessingHelper::ConvolutionRGBA(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal)
{
    if (!input || !output || kernel.size() < 1) return;

    const int kernel_size = kernel.size();
    const int kernel_rad = kernel_size / 2;
    const int rowstride = width * 4;
    int sum1, sum2, sum3, sum4;
    unsigned char *input2;
    int *kernelptr;
    int i, j, k_val;

    if (horizontal) {

        int k, k_from, k_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum1 = sum2 = sum3 = sum4 = 0;

                kernelptr = kernel.Pointer();

                k = kernel_size;
                k_from = i - kernel_rad;
                k_to   = k_from + kernel_size;
                if (k_from < 0) {
                    k += k_from;
                    kernelptr -= k_from;
                    k_from = 0;
                }
                if (k_to > width) {
                    k -= k_to - width;
                    k_to = width;
                }

                input2 = input + j * rowstride + k_from * 4;

                while (k) {

                    k_val = *kernelptr; kernelptr ++;
                    sum1 += (*input2) * k_val; input2 ++; 
                    sum2 += (*input2) * k_val; input2 ++;
                    sum3 += (*input2) * k_val; input2 ++;
                    sum4 += (*input2) * k_val; input2 ++;

                    k --;
                }

                sum1 >>= 10; sum2 >>= 10; sum3 >>= 10; sum4 >>= 10;

                if (sum1 < 0) sum1 = 0; else if (sum1 > 255) sum1 = 255;
                *output = static_cast<unsigned char>(sum1); output ++;
                if (sum2 < 0) sum2 = 0; else if (sum2 > 255) sum2 = 255;
                *output = static_cast<unsigned char>(sum2); output ++;
                if (sum3 < 0) sum3 = 0; else if (sum3 > 255) sum3 = 255;
                *output = static_cast<unsigned char>(sum3); output ++;
                if (sum4 < 0) sum4 = 0; else if (sum4 > 255) sum4 = 255;
                *output = static_cast<unsigned char>(sum4); output ++;
            }
        }

    }
    else {

        const int rowstride_short = rowstride - 4;
        int l, l_from, l_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum1 = sum2 = sum3 = sum4 = 0;

                kernelptr = kernel.Pointer();

                l = kernel_size;
                l_from = j - kernel_rad;
                l_to   = l_from + kernel_size;
                if (l_from < 0) {
                    l += l_from;
                    kernelptr -= l_from;
                    l_from = 0;
                }
                if (l_to > height) {
                    l -= l_to - height;
                    l_to = height;
                }

                input2 = input + l_from * rowstride + i * 4;

                while (l) {

                    k_val = *kernelptr; kernelptr ++;
                    sum1 += (*input2) * k_val; input2 ++; 
                    sum2 += (*input2) * k_val; input2 ++;
                    sum3 += (*input2) * k_val; input2 ++;
                    sum4 += (*input2) * k_val; input2 ++;

                    input2 += rowstride_short;

                    l --;
                }

                sum1 >>= 10; sum2 >>= 10; sum3 >>= 10; sum4 >>= 10;

                if (sum1 < 0) sum1 = 0; else if (sum1 > 255) sum1 = 255;
                *output = static_cast<unsigned char>(sum1); output ++;
                if (sum2 < 0) sum2 = 0; else if (sum2 > 255) sum2 = 255;
                *output = static_cast<unsigned char>(sum2); output ++;
                if (sum3 < 0) sum3 = 0; else if (sum3 > 255) sum3 = 255;
                *output = static_cast<unsigned char>(sum3); output ++;
                if (sum4 < 0) sum4 = 0; else if (sum4 > 255) sum4 = 255;
                *output = static_cast<unsigned char>(sum4); output ++;
            }
        }

    }
}

void svlImageProcessingHelper::ConvolutionMono8(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal)
{
    if (!input || !output || kernel.size() < 1) return;

    const int kernel_size = kernel.size();
    const int kernel_rad = kernel_size / 2;
    unsigned char *input2;
    int *kernelptr;
    int i, j, k_val, sum;

    if (horizontal) {

        int k, k_from, k_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum = 0;

                kernelptr = kernel.Pointer();

                k = kernel_size;
                k_from = i - kernel_rad;
                k_to   = k_from + kernel_size;
                if (k_from < 0) {
                    k += k_from;
                    kernelptr -= k_from;
                    k_from = 0;
                }
                if (k_to > width) {
                    k -= k_to - width;
                    k_to = width;
                }

                input2 = input + j * width + k_from;

                while (k) {

                    k_val = *kernelptr; kernelptr ++;
                    sum += (*input2) * k_val; input2 ++; 

                    k --;
                }

                sum >>= 10;

                if (sum < 0) sum = 0; else if (sum > 255) sum = 255;
                *output = static_cast<unsigned char>(sum); output ++;
            }
        }

    }
    else {

        const int rowstride_short = width - 1;
        int l, l_from, l_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum = 0;
                
                kernelptr = kernel.Pointer();
                
                l = kernel_size;
                l_from = j - kernel_rad;
                l_to   = l_from + kernel_size;
                if (l_from < 0) {
                    l += l_from;
                    kernelptr -= l_from;
                    l_from = 0;
                }
                if (l_to > height) {
                    l -= l_to - height;
                    l_to = height;
                }
                
                input2 = input + l_from * width + i;
                
                while (l) {
                    
                    k_val = *kernelptr; kernelptr ++;
                    sum += (*input2) * k_val; input2 ++; 
                    
                    input2 += rowstride_short;
                    
                    l --;
                }
                
                sum >>= 10;
                
                if (sum < 0) sum = 0; else if (sum > 255) sum = 255;
                *output = static_cast<unsigned char>(sum); output ++;
            }
        }
        
    }
}

void svlImageProcessingHelper::ConvolutionMono16(unsigned short* input, unsigned short* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal)
{
    if (!input || !output || kernel.size() < 1) return;

    const int kernel_size = kernel.size();
    const int kernel_rad = kernel_size / 2;
    unsigned short *input2;
    int *kernelptr;
    int i, j, k_val, sum;

    if (horizontal) {

        int k, k_from, k_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum = 0;

                kernelptr = kernel.Pointer();

                k = kernel_size;
                k_from = i - kernel_rad;
                k_to   = k_from + kernel_size;
                if (k_from < 0) {
                    k += k_from;
                    kernelptr -= k_from;
                    k_from = 0;
                }
                if (k_to > width) {
                    k -= k_to - width;
                    k_to = width;
                }

                input2 = input + j * width + k_from;

                while (k) {

                    k_val = *kernelptr; kernelptr ++;
                    sum += (*input2) * k_val; input2 ++; 

                    k --;
                }

                sum >>= 10;

                if (sum < 0) sum = 0; else if (sum > 65535) sum = 65535;
                *output = static_cast<unsigned short>(sum); output ++;
            }
        }

    }
    else {

        const int rowstride_short = width - 1;
        int l, l_from, l_to;

        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {

                sum = 0;

                kernelptr = kernel.Pointer();

                l = kernel_size;
                l_from = j - kernel_rad;
                l_to   = l_from + kernel_size;
                if (l_from < 0) {
                    l += l_from;
                    kernelptr -= l_from;
                    l_from = 0;
                }
                if (l_to > height) {
                    l -= l_to - height;
                    l_to = height;
                }

                input2 = input + l_from * width + i;

                while (l) {

                    k_val = *kernelptr; kernelptr ++;
                    sum += (*input2) * k_val; input2 ++; 

                    input2 += rowstride_short;

                    l --;
                }

                sum >>= 10;

                if (sum < 0) sum = 0; else if (sum > 65535) sum = 65535;
                *output = static_cast<unsigned short>(sum); output ++;
            }
        }

    }
}

void svlImageProcessingHelper::ConvolutionRGB(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicMatrix<int> & kernel)
{
    if (!input || !output || kernel.rows() < 1 || kernel.rows() != kernel.cols()) return;

    const int kernel_size = kernel.cols();
    const int kernel_rad = kernel_size / 2;
    const int rowstride = width * 3;
    const int rowstride_short = rowstride - kernel_size * 3;
    int i, j, k, k_, l, l_from, l_to, k_from, k_to, k_val, kernelstride;
    int sum1, sum2, sum3;
    unsigned char *input2;
    int *kernelptr;

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {

            sum1 = sum2 = sum3 = 0;

            kernelstride = 0;
            kernelptr = kernel.Pointer();

            l = kernel_size;
            l_from = j - kernel_rad;
            l_to   = l_from + kernel_size;
            if (l_from < 0) {
                l += l_from;
                kernelptr -= l_from * kernel_size;

                l_from = 0;
            }
            if (l_to > height) {
                l -= l_to - height;
                l_to = height;
            }

            k = kernel_size;
            k_from = i - kernel_rad;
            k_to   = k_from + kernel_size;
            if (k_from < 0) {
                k += k_from;
                kernelptr -= k_from;
                kernelstride -= k_from;

                k_from = 0;
            }
            if (k_to > width) {
                k_ = k_to - width;
                k -= k_;
                kernelstride += k_;

                k_to = width;
            }

            input2 = input + l_from * rowstride + k_from * 3;

            while (l) {

                k_ = k;
                while (k_) {

                    k_val = *kernelptr; kernelptr ++;
                    sum1 += (*input2) * k_val; input2 ++; 
                    sum2 += (*input2) * k_val; input2 ++;
                    sum3 += (*input2) * k_val; input2 ++;
                    
                    k_ --;
                }

                input2 += rowstride_short + kernelstride * 3;
                kernelptr += kernelstride;

                l --;
            }

            sum1 >>= 10; sum2 >>= 10; sum3 >>= 10;

            if (sum1 < 0) sum1 = 0; else if (sum1 > 255) sum1 = 255;
            *output = static_cast<unsigned char>(sum1); output ++;
            if (sum2 < 0) sum2 = 0; else if (sum2 > 255) sum2 = 255;
            *output = static_cast<unsigned char>(sum2); output ++;
            if (sum3 < 0) sum3 = 0; else if (sum3 > 255) sum3 = 255;
            *output = static_cast<unsigned char>(sum3); output ++;
        }
    }
}

void svlImageProcessingHelper::ConvolutionRGBA(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicMatrix<int> & kernel)
{
    if (!input || !output || kernel.rows() < 1 || kernel.rows() != kernel.cols()) return;

    const int kernel_size = kernel.cols();
    const int kernel_rad = kernel_size / 2;
    const int rowstride = width * 4;
    const int rowstride_short = rowstride - kernel_size * 4;
    int i, j, k, k_, l, l_from, l_to, k_from, k_to, k_val, kernelstride;
    int sum1, sum2, sum3, sum4;
    unsigned char *input2;
    int *kernelptr;

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {

            sum1 = sum2 = sum3 = sum4 = 0;

            kernelstride = 0;
            kernelptr = kernel.Pointer();

            l = kernel_size;
            l_from = j - kernel_rad;
            l_to   = l_from + kernel_size;
            if (l_from < 0) {
                l += l_from;
                kernelptr -= l_from * kernel_size;

                l_from = 0;
            }
            if (l_to > height) {
                l -= l_to - height;
                l_to = height;
            }

            k = kernel_size;
            k_from = i - kernel_rad;
            k_to   = k_from + kernel_size;
            if (k_from < 0) {
                k += k_from;
                kernelptr -= k_from;
                kernelstride -= k_from;

                k_from = 0;
            }
            if (k_to > width) {
                k_ = k_to - width;
                k -= k_;
                kernelstride += k_;

                k_to = width;
            }

            input2 = input + l_from * rowstride + k_from * 4;

            while (l) {

                k_ = k;
                while (k_) {

                    k_val = *kernelptr; kernelptr ++;
                    sum1 += (*input2) * k_val; input2 ++; 
                    sum2 += (*input2) * k_val; input2 ++;
                    sum3 += (*input2) * k_val; input2 ++;
                    sum4 += (*input2) * k_val; input2 ++;

                    k_ --;
                }

                input2 += rowstride_short + kernelstride * 4;
                kernelptr += kernelstride;

                l --;
            }

            sum1 >>= 10; sum2 >>= 10; sum3 >>= 10; sum4 >>= 10;

            if (sum1 < 0) sum1 = 0; else if (sum1 > 255) sum1 = 255;
            *output = static_cast<unsigned char>(sum1); output ++;
            if (sum2 < 0) sum2 = 0; else if (sum2 > 255) sum2 = 255;
            *output = static_cast<unsigned char>(sum2); output ++;
            if (sum3 < 0) sum3 = 0; else if (sum3 > 255) sum3 = 255;
            *output = static_cast<unsigned char>(sum3); output ++;
            if (sum4 < 0) sum4 = 0; else if (sum4 > 255) sum4 = 255;
            *output = static_cast<unsigned char>(sum4); output ++;
        }
    }
}

void svlImageProcessingHelper::ConvolutionMono8(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicMatrix<int> & kernel)
{
    if (!input || !output || kernel.rows() < 1 || kernel.rows() != kernel.cols()) return;

    const int kernel_size = kernel.cols();
    const int kernel_rad = kernel_size / 2;
    const int rowstride_short = width - kernel_size;
    int i, j, k, k_, l, l_from, l_to, k_from, k_to, k_val, kernelstride, sum;
    unsigned char *input2;
    int *kernelptr;

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {

            sum = 0;

            kernelstride = 0;
            kernelptr = kernel.Pointer();

            l = kernel_size;
            l_from = j - kernel_rad;
            l_to   = l_from + kernel_size;
            if (l_from < 0) {
                l += l_from;
                kernelptr -= l_from * kernel_size;

                l_from = 0;
            }
            if (l_to > height) {
                l -= l_to - height;
                l_to = height;
            }

            k = kernel_size;
            k_from = i - kernel_rad;
            k_to   = k_from + kernel_size;
            if (k_from < 0) {
                k += k_from;
                kernelptr -= k_from;
                kernelstride -= k_from;

                k_from = 0;
            }
            if (k_to > width) {
                k_ = k_to - width;
                k -= k_;
                kernelstride += k_;

                k_to = width;
            }

            input2 = input + l_from * width + k_from;

            while (l) {

                k_ = k;
                while (k_) {

                    k_val = *kernelptr; kernelptr ++;
                    sum += (*input2) * k_val; input2 ++;

                    k_ --;
                }

                input2 += rowstride_short + kernelstride;
                kernelptr += kernelstride;

                l --;
            }

            sum >>= 10;

            if (sum < 0) sum = 0; else if (sum > 255) sum = 255;
            *output = static_cast<unsigned char>(sum); output ++;
        }
    }
}

void svlImageProcessingHelper::ConvolutionMono16(unsigned short* input, unsigned short* output, const int width, const int height, vctDynamicMatrix<int> & kernel)
{
    if (!input || !output || kernel.rows() < 1 || kernel.rows() != kernel.cols()) return;

    const int kernel_size = kernel.cols();
    const int kernel_rad = kernel_size / 2;
    const int rowstride_short = width - kernel_size;
    int i, j, k, k_, l, l_from, l_to, k_from, k_to, k_val, kernelstride, sum;
    unsigned short *input2;
    int *kernelptr;

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {

            sum = 0;

            kernelstride = 0;
            kernelptr = kernel.Pointer();

            l = kernel_size;
            l_from = j - kernel_rad;
            l_to   = l_from + kernel_size;
            if (l_from < 0) {
                l += l_from;
                kernelptr -= l_from * kernel_size;

                l_from = 0;
            }
            if (l_to > height) {
                l -= l_to - height;
                l_to = height;
            }

            k = kernel_size;
            k_from = i - kernel_rad;
            k_to   = k_from + kernel_size;
            if (k_from < 0) {
                k += k_from;
                kernelptr -= k_from;
                kernelstride -= k_from;

                k_from = 0;
            }
            if (k_to > width) {
                k_ = k_to - width;
                k -= k_;
                kernelstride += k_;

                k_to = width;
            }

            input2 = input + l_from * width + k_from;

            while (l) {

                k_ = k;
                while (k_) {

                    k_val = *kernelptr; kernelptr ++;
                    sum += (*input2) * k_val; input2 ++;

                    k_ --;
                }

                input2 += rowstride_short + kernelstride;
                kernelptr += kernelstride;

                l --;
            }

            sum >>= 10;

            if (sum < 0) sum = 0; else if (sum > 65535) sum = 65535;
            *output = static_cast<unsigned short>(sum); output ++;
        }
    }
}

void svlImageProcessingHelper::ResampleMono8(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                                             unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight)
{
    unsigned int i, j;
    unsigned int x1, y1, x2, y2;
    unsigned char *psrc, *plsrc, *pdst;
    
    // vertical sampling loop
    plsrc = src;
    pdst = dst;
    y1 = 0;
    y2 = dstheight;
    for (j = 0; j < dstheight; j ++) {
        
        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = dstwidth;
        for (i = 0; i < dstwidth; i ++) {
            *pdst = *psrc;
            pdst ++;
            
            x1 += srcwidth;
            while (x1 >= x2) {
                x2 += dstwidth;
                psrc ++;
            }
        }
        
        y1 += srcheight;
        while (y1 >= y2) {
            y2 += dstheight;
            plsrc += srcwidth;
        }
    }
}

void svlImageProcessingHelper::ResampleAndInterpolateHMono8(unsigned char* src, const unsigned int srcwidth,
                                                            unsigned char* dst, const unsigned int dstwidth,
                                                            const unsigned int height)
{
    unsigned int i, j;
    unsigned int x1, x2;
    int wx1, wx2;
    unsigned char *psrc, *plsrc, *pdst;
    unsigned char prev_col, this_col;

    // eliminating division by using integral powers of 2
    const unsigned int fast_dstwidth = 256;   // 2^8
    const unsigned int fast_srcwidth = fast_dstwidth * srcwidth / dstwidth;
    
    plsrc = src;
    pdst = dst;
    for (j = 0; j < height; j ++) {
        
        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = 128;
        prev_col = this_col = *psrc;
        wx1 = 0;
        wx2 = fast_dstwidth;
        
        for (i = 0; i < dstwidth; i ++) {
            *pdst = (wx1 * prev_col + wx2 * this_col) >> 8;
            pdst ++;
            
            x1 += fast_srcwidth;
            while (x1 > x2) {
                x2 += fast_dstwidth;
                prev_col = this_col; this_col = *psrc; psrc ++;
            }
            
            wx1 = x2 - x1;
            wx2 = fast_dstwidth - wx1;
        }
        plsrc += srcwidth;
    }
}

void svlImageProcessingHelper::ResampleAndInterpolateVMono8(unsigned char* src, const unsigned int srcheight,
                                                            unsigned char* dst, const unsigned int dstheight,
                                                            const unsigned int width)
{
    unsigned int i, j;
    unsigned int y1, y2;
    int wy1, wy2;
    unsigned char *psrc, *pcsrc, *pdst, *pcdst;
    unsigned char prev_col, this_col;
    
    // eliminating division by using integral powers of 2
    const unsigned int fast_dstheight = 256;   // 2^8
    const unsigned int fast_srcheight = fast_dstheight * srcheight / dstheight;
    
    pcsrc = src;
    pcdst = dst;
    for (j = 0; j < width; j ++) {
        
        // vertical sampling loop
        psrc = pcsrc;
        pdst = pcdst;
        y1 = 0;
        y2 = 128;
        prev_col = this_col = *psrc;
        wy1 = 0;
        wy2 = fast_dstheight;
        
        for (i = 0; i < dstheight; i ++) {
            *pdst = (wy1 * prev_col + wy2 * this_col) >> 8;
            pdst += width;
            
            y1 += fast_srcheight;
            while (y1 > y2) {
                y2 += fast_dstheight;
                prev_col = this_col; this_col = *psrc; psrc += width;
            }
            
            wy1 = y2 - y1;
            wy2 = fast_dstheight - wy1;
        }
        pcsrc ++;
        pcdst ++;
    }
}

void svlImageProcessingHelper::ResampleRGB24(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                                             unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight)
{
    unsigned int i, j;
    unsigned int x1, y1, x2, y2;
    unsigned char *psrc, *plsrc, *pdst;
    const unsigned int srcstride = srcwidth * 3;
    
    // vertical sampling loop
    plsrc = src;
    pdst = dst;
    y1 = 0;
    y2 = dstheight;
    for (j = 0; j < dstheight; j ++) {
        
        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = dstwidth;
        for (i = 0; i < dstwidth; i ++) {
            *pdst = psrc[0]; pdst ++;
            *pdst = psrc[1]; pdst ++;
            *pdst = psrc[2]; pdst ++;
            
            x1 += srcwidth;
            while (x1 >= x2) {
                x2 += dstwidth;
                psrc += 3;
            }
        }
        
        y1 += srcheight;
        while (y1 >= y2) {
            y2 += dstheight;
            plsrc += srcstride;
        }
    }
}

void svlImageProcessingHelper::ResampleAndInterpolateHRGB24(unsigned char* src, const unsigned int srcwidth,
                                                            unsigned char* dst, const unsigned int dstwidth,
                                                            const unsigned int height)
{
    unsigned int i, j;
    unsigned int x1, x2;
    int wx1, wx2;
    unsigned char *psrc, *plsrc, *pdst;
    unsigned char prev_r, prev_g, prev_b, this_r, this_g, this_b;
    const unsigned int srcstride = srcwidth * 3;
    
    // eliminating division by using integral powers of 2
    const unsigned int fast_dstwidth = 256;   // 2^8
    const unsigned int fast_srcwidth = fast_dstwidth * srcwidth / dstwidth;
    
    plsrc = src;
    pdst = dst;
    for (j = 0; j < height; j ++) {
        
        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = 128;
        prev_r = this_r = psrc[0];
        prev_g = this_g = psrc[1];
        prev_b = this_b = psrc[2];
        wx1 = 0;
        wx2 = fast_dstwidth;
        
        for (i = 0; i < dstwidth; i ++) {
            *pdst = (wx1 * prev_r + wx2 * this_r) >> 8;
            pdst ++;
            *pdst = (wx1 * prev_g + wx2 * this_g) >> 8;
            pdst ++;
            *pdst = (wx1 * prev_b + wx2 * this_b) >> 8;
            pdst ++;
            
            x1 += fast_srcwidth;
            while (x1 > x2) {
                x2 += fast_dstwidth;
                prev_r = this_r; this_r = *psrc; psrc ++;
                prev_g = this_g; this_g = *psrc; psrc ++;
                prev_b = this_b; this_b = *psrc; psrc ++;
            }
            
            wx1 = x2 - x1;
            wx2 = fast_dstwidth - wx1;
        }
        plsrc += srcstride;
    }
}

void svlImageProcessingHelper::ResampleAndInterpolateVRGB24(unsigned char* src, const unsigned int srcheight,
                                                            unsigned char* dst, const unsigned int dstheight,
                                                            const unsigned int width)
{
    unsigned int i, j;
    unsigned int y1, y2;
    int wy1, wy2;
    unsigned char *psrc, *pcsrc, *pdst, *pcdst;
    unsigned char prev_r, prev_g, prev_b, this_r, this_g, this_b;
    const unsigned int stride = width * 3 - 2;
    
    // eliminating division by using integral powers of 2
    const unsigned int fast_dstheight = 256;   // 2^8
    const unsigned int fast_srcheight = fast_dstheight * srcheight / dstheight;
    
    pcsrc = src;
    pcdst = dst;
    for (j = 0; j < width; j ++) {
        
        // vertical sampling loop
        psrc = pcsrc;
        pdst = pcdst;
        y1 = 0;
        y2 = 128;
        prev_r = this_r = psrc[0];
        prev_g = this_g = psrc[1];
        prev_b = this_b = psrc[2];
        wy1 = 0;
        wy2 = fast_dstheight;
        
        for (i = 0; i < dstheight; i ++) {
            *pdst = (wy1 * prev_r + wy2 * this_r) >> 8;
            pdst ++;
            *pdst = (wy1 * prev_g + wy2 * this_g) >> 8;
            pdst ++;
            *pdst = (wy1 * prev_b + wy2 * this_b) >> 8;
            pdst += stride;
            
            y1 += fast_srcheight;
            while (y1 > y2) {
                y2 += fast_dstheight;
                prev_r = this_r; this_r = *psrc; psrc ++;
                prev_g = this_g; this_g = *psrc; psrc ++;
                prev_b = this_b; this_b = *psrc; psrc += stride;
            }
            
            wy1 = y2 - y1;
            wy2 = fast_dstheight - wy1;
        }
        pcsrc += 3;
        pcdst += 3;
    }
}

void svlImageProcessingHelper::DeinterlaceBlending(unsigned char* buffer, const unsigned int width, const unsigned int height)
{
    unsigned int i, j;
    int ar, ag, ab;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    const int colstride = width * 3;
    
    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;
    
    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;
    
    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            ar = (*r0 + *r1) >> 1; ag = (*g0 + *g1) >> 1; ab = (*b0 + *b1) >> 1;
            *r0 = ar; *g0 = ag; *b0 = ab;
            *r1 = ar; *g1 = ag; *b1 = ab;
            
            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
    }
}

void svlImageProcessingHelper::DeinterlaceDiscarding(unsigned char* buffer, const unsigned int width, const unsigned int height)
{
    unsigned int i, j;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    const int colstride = width * 3;
    
    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;
    
    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;
    
    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            *r1 = *r0; *g1 = *g0; *b1 = *b0;
            
            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
    }
}

void svlImageProcessingHelper::DeinterlaceAdaptiveBlending(unsigned char* buffer, const unsigned int width, const unsigned int height)
{
    unsigned int i, j;
    int ar, ag, ab;
    unsigned int diff, diffinv;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    unsigned char *r2, *g2, *b2;
    const int colstride = width * 3;
    
    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;
    
    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;
    
    r2 = r1 + colstride;
    g2 = r2 + 1;
    b2 = g2 + 1;
    
    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            ar = (*r0 + *r2) >> 1; ag = (*g0 + *g2) >> 1; ab = (*b0 + *b2) >> 1;
            
            ar -= *r1; if (ar < 0) ar = -ar;
            ag -= *g1; if (ag < 0) ag = -ag;
            ab -= *b1; if (ab < 0) ab = -ab;
            diff = (ar + ag + ab) << 2;
            if (diff > 765) diff = 765;
            diffinv = 765 - diff;
            *r1 = (diff * ar + diffinv * (*r1)) / 765;
            *g1 = (diff * ag + diffinv * (*g1)) / 765;
            *b1 = (diff * ab + diffinv * (*b1)) / 765;
            
            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
            r2 += 3; g2 += 3; b2 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
        r2 += colstride; g2 += colstride; b2 += colstride;
    }
}

void svlImageProcessingHelper::DeinterlaceAdaptiveDiscarding(unsigned char* buffer, const unsigned int width, const unsigned int height)
{
    unsigned int i, j;
    int ar, ag, ab;
    unsigned int diff, diffinv;
    unsigned char *r0, *g0, *b0;
    unsigned char *r1, *g1, *b1;
    unsigned char *r2, *g2, *b2;
    const int colstride = width * 3;
    
    r0 = buffer;
    g0 = r0 + 1;
    b0 = g0 + 1;
    
    r1 = r0 + colstride;
    g1 = r1 + 1;
    b1 = g1 + 1;
    
    r2 = r1 + colstride;
    g2 = r2 + 1;
    b2 = g2 + 1;
    
    for (j = 0; j < height; j += 2) {
        for (i = 0; i < width; i ++) {
            ar = (*r0 + *r2) >> 1; ag = (*g0 + *g2) >> 1; ab = (*b0 + *b2) >> 1;
            
            ar -= *r1; if (ar < 0) ar = -ar;
            ag -= *g1; if (ag < 0) ag = -ag;
            ab -= *b1; if (ab < 0) ab = -ab;
            diff = (ar + ag + ab) << 1;
            if (diff > 765) diff = 765;
            diffinv = 765 - diff;
            *r1 = (diff * ar + diffinv * (*r1)) / 765;
            *g1 = (diff * ag + diffinv * (*g1)) / 765;
            *b1 = (diff * ab + diffinv * (*b1)) / 765;

            r0 += 3; g0 += 3; b0 += 3;
            r1 += 3; g1 += 3; b1 += 3;
            r2 += 3; g2 += 3; b2 += 3;
        }
        r0 += colstride; g0 += colstride; b0 += colstride;
        r1 += colstride; g1 += colstride; b1 += colstride;
        r2 += colstride; g2 += colstride; b2 += colstride;
    }
}


/**************************************************************/
/*** svlImageProcessingHelper::RectificationInternals class ***/
/**************************************************************/

svlImageProcessingHelper::RectificationInternals::RectificationInternals() :
    svlImageProcessingInternals(),
    Width(0),
    Height(0),
    idxDest(0),
    idxDestSize(0),
    idxSrc1(0),
    idxSrc1Size(0),
    idxSrc2(0),
    idxSrc2Size(0),
    idxSrc3(0),
    idxSrc3Size(0),
    idxSrc4(0),
    idxSrc4Size(0),
    blendSrc1(0),
    blendSrc1Size(0),
    blendSrc2(0),
    blendSrc2Size(0),
    blendSrc3(0),
    blendSrc3Size(0),
    blendSrc4(0),
    blendSrc4Size(0)
{
}

svlImageProcessingHelper::RectificationInternals::~RectificationInternals()
{
    Release();
}

bool svlImageProcessingHelper::RectificationInternals::Load(const std::string &filepath, int explen)
{
    Release();

    std::ifstream file(filepath.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!file.is_open()) return false;

    double dbl;
    const unsigned int maxwidth = 1920;
    const unsigned int maxheight = 1200;
    const unsigned int size = maxwidth * maxheight;
    double* dblbuf = new double[size];
    char* chbuf    = new char[(16 * size) + 1];
    int valcnt, i;

    // lutpos:
    //          1 - width, height
    //          2 - destination index lut
    //          3 - source index lut 1
    //          4 - source index lut 2
    //          5 - source index lut 3
    //          6 - source index lut 4
    //          7 - source blending lut 1
    //          8 - source blending lut 2
    //          9 - source blending lut 3
    //         10 - source blending lut 4
    int lutpos = 0;

    while (lutpos < 10) {
        switch (lutpos) {
            case 0:
                if (LoadLine(file, &dbl, chbuf, 1, explen) < 1) goto labError;
                Height = static_cast<int>(dbl);
                if (LoadLine(file, &dbl, chbuf, 1, explen) < 1) goto labError;
                Width = static_cast<int>(dbl);
                
                if (Width > maxwidth || Height > maxheight) goto labError;
                
                lutpos ++;
                break;
                
            case 1:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                idxDestSize = valcnt;
                idxDest = new unsigned int[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    idxDest[i] = static_cast<unsigned int>(dblbuf[i] + 0.5);
                }
                
                lutpos ++;
                break;
                
            case 2:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                idxSrc1Size = valcnt;
                idxSrc1 = new unsigned int[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    idxSrc1[i] = static_cast<unsigned int>(dblbuf[i] + 0.5);
                }
                
                lutpos ++;
                break;
                
            case 3:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                idxSrc2Size = valcnt;
                idxSrc2 = new unsigned int[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    idxSrc2[i] = static_cast<unsigned int>(dblbuf[i] + 0.5);
                }
                
                lutpos ++;
                break;
                
            case 4:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                idxSrc3Size = valcnt;
                idxSrc3 = new unsigned int[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    idxSrc3[i] = static_cast<unsigned int>(dblbuf[i] + 0.5);
                }
                
                lutpos ++;
                break;
                
            case 5:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                idxSrc4Size = valcnt;
                idxSrc4 = new unsigned int[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    idxSrc4[i] = static_cast<unsigned int>(dblbuf[i] + 0.5);
                }
                
                lutpos ++;
                break;
                
            case 6:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                blendSrc1Size = valcnt;
                blendSrc1 = new unsigned char[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    blendSrc1[i] = static_cast<unsigned char>(dblbuf[i] * 256);
                }
                
                lutpos ++;
                break;
                
            case 7:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                blendSrc2Size = valcnt;
                blendSrc2 = new unsigned char[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    blendSrc2[i] = static_cast<unsigned char>(dblbuf[i] * 256);
                }
                
                lutpos ++;
                break;
                
            case 8:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                blendSrc3Size = valcnt;
                blendSrc3 = new unsigned char[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    blendSrc3[i] = static_cast<unsigned char>(dblbuf[i] * 256);
                }
                
                lutpos ++;
                break;
                
            case 9:
                valcnt = LoadLine(file, dblbuf, chbuf, size, explen);
                if (valcnt < 1) goto labError;
                blendSrc4Size = valcnt;
                blendSrc4 = new unsigned char[valcnt];
                for (i = 0; i < valcnt; i ++) {
                    blendSrc4[i] = static_cast<unsigned char>(dblbuf[i] * 256);
                }
                
                lutpos ++;
                break;
        }
    }

    file.close();

    valcnt = idxDestSize;
    if (idxSrc1Size != valcnt ||
        idxSrc2Size != valcnt ||
        idxSrc3Size != valcnt ||
        idxSrc4Size != valcnt ||
        blendSrc1Size != valcnt ||
        blendSrc2Size != valcnt ||
        blendSrc3Size != valcnt ||
        blendSrc4Size != valcnt) goto labError;

    TransposeLUTArray(idxDest, valcnt, Width, Height);
    TransposeLUTArray(idxSrc1, valcnt, Width, Height);
    TransposeLUTArray(idxSrc2, valcnt, Width, Height);
    TransposeLUTArray(idxSrc3, valcnt, Width, Height);
    TransposeLUTArray(idxSrc4, valcnt, Width, Height);

    for (i = 0; i < valcnt; i ++) {
        idxDest[i] *= 3;
        idxSrc1[i] *= 3;
        idxSrc2[i] *= 3;
        idxSrc3[i] *= 3;
        idxSrc4[i] *= 3;
    }

    if (dblbuf) delete [] dblbuf;
    if (chbuf) delete [] chbuf;

    return true;

labError:
    if (file.is_open()) file.close();

    Release();

    if (dblbuf) delete [] dblbuf;
    if (chbuf) delete [] chbuf;

    return false;
}

int svlImageProcessingHelper::RectificationInternals::LoadLine(std::ifstream &file, double* dblbuf, char* chbuf, unsigned int size, int explen)
{
    unsigned int bufsize = (16 * size) + 1; // max text line length
    unsigned int datalen; // actual data size
    unsigned int bufpos = 0;
    unsigned int filepos;
    char* tbuf;
    char ch;
    int ival;
    int counter = 0;
    int linebreak = 0;
    double dbl;
    double negexp[] = {1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001, 0.000000001};
	double negexpTens[] = {1.0, 0.0000000001, 0.00000000001, 0.000000000001, 0.000000000001, 0.000000000001, 0.000000000001, 0.000000000001, 0.000000000001, 0.0000000000001, 0.0000000000001};
    double posexp[] = {1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0, 1000000000.0};
    
    filepos = file.tellg();
    file.read(chbuf, bufsize);
    datalen = file.gcount();
    file.clear();
    
    if (explen == 3) {
        while (datalen > 16 ) {
            bufpos += 2;
            
            tbuf = chbuf + bufpos;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // ?.xxxxxxxes00x
            dbl = ch;
            
            tbuf += 2;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.?xxxxxxes00x
            dbl += 0.1 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.x?xxxxxes00x
            dbl += 0.01 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xx?xxxxes00x
            dbl += 0.001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxx?xxxes00x
            dbl += 0.0001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxxx?xxes00x
            dbl += 0.00001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxxxx?xes00x
            dbl += 0.000001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxxxxx?es00x
            dbl += 0.0000001 * ch;
            
            tbuf += 2;
            
            // x.xxxxxxxe?00x
            if (*tbuf == '-') {
                tbuf += 3;
                ival = *tbuf - 48; if (ival < 0 || ival > 9) break;
                
                // x.xxxxxxxe-00?
                dbl *= negexp[ival];
            }
            else {
                tbuf += 3;
                ival = *tbuf - 48; if (ival < 0 || ival > 9) break;
                
                // x.xxxxxxxe+00?
                dbl *= posexp[ival];
            }
            
            dblbuf[counter] = dbl;
            bufpos += 14;
            counter ++;
            datalen -= 16;
            
            // if end of line, seek back
            ch = chbuf[bufpos];
            if (ch == 0x0a) { // LF
                file.seekg(filepos + bufpos + 1);
                linebreak = 1;
                break;
            }
            else if (ch == 0x0d) { // CRLF
                file.seekg(filepos + bufpos + 2);
                linebreak = 1;
                break;
            }
        }
    }
    else if (explen == 2) {
        while (datalen > 16 ) {
            bufpos += 3;
            
            tbuf = chbuf + bufpos;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // ?.xxxxxxxes0x
            dbl = ch;
            
            tbuf += 2;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.?xxxxxxes0x
            dbl += 0.1 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.x?xxxxxes0x
            dbl += 0.01 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xx?xxxxes0x
            dbl += 0.001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxx?xxxes0x
            dbl += 0.0001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxxx?xxes0x
            dbl += 0.00001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxxxx?xes0x
            dbl += 0.000001 * ch;
            
            tbuf ++;
            ch = *tbuf - 48; if (ch < 0 || ch > 9) break;
            
            // x.xxxxxx?es0x
            dbl += 0.0000001 * ch;
            
            tbuf += 2;
            
            // x.xxxxxxxe?0x
            if (*tbuf == '-') {
                tbuf ++;
                ival = *tbuf - 48; if (ival < 0 || ival > 9) break;
                
                // x.xxxxxxxe-?x
                dbl *= negexpTens[ival];
                
				tbuf ++;
                ival = *tbuf - 48; if (ival < 0 || ival > 9) break;
                
                // x.xxxxxxxe-x?
                dbl *= negexp[ival];
            }
            else {
                tbuf += 2;
                ival = *tbuf - 48; if (ival < 0 || ival > 9) break;
                
                // x.xxxxxxxe+0?
                dbl *= posexp[ival];
            }
            
            dblbuf[counter] = dbl;
            bufpos += 13;
            counter ++;
            datalen -= 16;
            
            // if end of line, seek back
            ch = chbuf[bufpos];
            if (ch == 0x0a) { // LF
                file.seekg(filepos + bufpos + 1);
                linebreak = 1;
                break;
            }
            else if (ch == 0x0d) { // CRLF
                file.seekg(filepos + bufpos + 2);
                linebreak = 1;
                break;
            }
        }
    }

    if (linebreak == 1) return counter;
    return 0;
}

void svlImageProcessingHelper::RectificationInternals::TransposeLUTArray(unsigned int* index, unsigned int size, unsigned int width, unsigned int height)
{
    unsigned int i, x, y, val;

    for (i = 0; i < size; i ++) {
        val = index[i] - 1;
        x = val / height;
        y = val % height;
        index[i] = y * width + x;
    }
}

void svlImageProcessingHelper::RectificationInternals::Release()
{
    if (idxDest) delete [] idxDest;
    if (idxSrc1) delete [] idxSrc1;
    if (idxSrc2) delete [] idxSrc2;
    if (idxSrc3) delete [] idxSrc3;
    if (idxSrc4) delete [] idxSrc4;
    if (blendSrc1) delete [] blendSrc1;
    if (blendSrc2) delete [] blendSrc2;
    if (blendSrc3) delete [] blendSrc3;
    if (blendSrc4) delete [] blendSrc4;

    Width = 0;
    Height = 0;
    idxDest = 0;
    idxDestSize = 0;
    idxSrc1 = 0;
    idxSrc1Size = 0;
    idxSrc2 = 0;
    idxSrc2Size = 0;
    idxSrc3 = 0;
    idxSrc3Size = 0;
    idxSrc4 = 0;
    idxSrc4Size = 0;
    blendSrc1 = 0;
    blendSrc1Size = 0;
    blendSrc2 = 0;
    blendSrc2Size = 0;
    blendSrc3 = 0;
    blendSrc3Size = 0;
    blendSrc4 = 0;
    blendSrc4Size = 0;
}


/*********************************************************/
/*** svlImageProcessingHelper::ExposureInternals class ***/
/*********************************************************/

svlImageProcessingHelper::ExposureInternals::ExposureInternals() :
    svlImageProcessingInternals(),
    Brightness(0.0),
    Contrast(0.0),
    Gamma(0.0),
    Modified(true)
{
    Curve.SetAll(0);
}

void svlImageProcessingHelper::ExposureInternals::SetBrightness(double brightness)
{
    if (brightness > 100.0) brightness = 100.0;
    else  if (brightness < -100.0) brightness = -100.0;

    if (Brightness != brightness) {
        Brightness = brightness;
        Modified = true;
    }
}

double svlImageProcessingHelper::ExposureInternals::GetBrightness()
{
    return Brightness;
}

void svlImageProcessingHelper::ExposureInternals::SetContrast(double contrast)
{
    if (contrast > 100.0) contrast = 100.0;
    else  if (contrast < -100.0) contrast = -100.0;

    if (Contrast != contrast) {
        Contrast = contrast;
        Modified = true;
    }
}

double svlImageProcessingHelper::ExposureInternals::GetContrast()
{
    return Contrast;
}

void svlImageProcessingHelper::ExposureInternals::SetGamma(double gamma)
{
    if (gamma > 100.0) gamma = 100.0;
    else  if (gamma < -100.0) gamma = -100.0;

    if (Gamma != gamma) {
        Gamma = gamma;
        Modified = true;
    }
}

double svlImageProcessingHelper::ExposureInternals::GetGamma()
{
    return Gamma;
}

void svlImageProcessingHelper::ExposureInternals::CalculateCurve()
{
    if (!Modified) return;

    const double cntrst = pow(10.0, Contrast / 100.0);
    const double gmma = pow(10.0, -Gamma / 100.0);
    const double scale100 = 2.55;
    double dbval;
    int result;

    for (unsigned int i = 0; i < 256; i ++) {
        dbval = (Brightness * scale100 + cntrst * i) / 255.0;
        result = static_cast<int>(pow(dbval, gmma) * 255.0 + 0.5);

        if (result < 0) result = 0;
        else if (result > 255) result = 255;
        Curve[i] = result;
    }

    Modified = false;
}

