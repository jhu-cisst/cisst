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

#ifndef _svlConverters_h
#define _svlConverters_h

#include <cisstStereoVision/svlTypes.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


namespace svlConverter
{
    CISST_EXPORT int ConvertSample(const svlSample* inimage, svlSample* outimage,
                                   unsigned int threads = 1, unsigned int threadid = 0);
    CISST_EXPORT int ConvertImage(const svlSampleImage* inimage, svlSampleImage* outimage,
                                  int param = 0, unsigned int threads = 1, unsigned int threadid = 0);
    CISST_EXPORT int ConvertImage(const svlSampleImage* inimage, const unsigned int inchannel,
                                  svlSampleImage* outimage, const unsigned int outchannel,
                                  int param = 0, unsigned int threads = 1, unsigned int threadid = 0);

    CISST_EXPORT void Gray8toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    CISST_EXPORT void Gray8toRGBA32(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    CISST_EXPORT void Gray8toGray16(unsigned char* input, unsigned short* output, const unsigned int pixelcount);
    CISST_EXPORT void Gray8toGray32(unsigned char* input, unsigned int* output, const unsigned int pixelcount);
    CISST_EXPORT void Gray16toRGB24(unsigned short* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    CISST_EXPORT void Gray16toRGBA32(unsigned short* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    CISST_EXPORT void Gray16toGray8(unsigned short* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    CISST_EXPORT void Gray16toGray32(unsigned short* input, unsigned int* output, const unsigned int pixelcount);
    CISST_EXPORT void Gray32toRGB24(unsigned int* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    CISST_EXPORT void Gray32toRGBA32(unsigned int* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    CISST_EXPORT void Gray32toGray8(unsigned int* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    CISST_EXPORT void Gray32toGray16(unsigned int* input, unsigned short* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    CISST_EXPORT void int32toRGB24(int* input, unsigned char* output, const unsigned int pixelcount, const int maxinputvalue = -1);
    CISST_EXPORT void int32toRGBA32(int* input, unsigned char* output, const unsigned int pixelcount, const int maxinputvalue = -1);
    CISST_EXPORT void int32toGray8(int* input, unsigned char* output, const unsigned int pixelcount, const int maxinputvalue = -1);
    CISST_EXPORT void int32toGray16(int* input, unsigned short* output, const unsigned int pixelcount, const int maxinputvalue = -1);
    CISST_EXPORT void int32toGray32(int* input, unsigned int* output, const unsigned int pixelcount);
    CISST_EXPORT void float32toRGB24(float* input, unsigned char* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    CISST_EXPORT void float32toRGBA32(float* input, unsigned char* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    CISST_EXPORT void float32toGray8(float* input, unsigned char* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    CISST_EXPORT void float32toGray16(float* input, unsigned short* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    CISST_EXPORT void float32toGray32(float* input, unsigned int* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    CISST_EXPORT void RGB16toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toRGBA32(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    CISST_EXPORT void RGB24toGray8(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    CISST_EXPORT void RGB24toGray16(unsigned char* input, unsigned short* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    CISST_EXPORT void RGB24toGray32(unsigned char* input, unsigned int* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    CISST_EXPORT void RGB24toYUV444(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toYUV444P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void BGR24toYUV422(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toYUV422(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toYUV422P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toHSV24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toHSV24P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toHSL24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGB24toHSL24P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void RGBA32toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    CISST_EXPORT void RGBA32toGray8(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    CISST_EXPORT void RGBA32toGray16(unsigned char* input, unsigned short* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    CISST_EXPORT void RGBA32toGray32(unsigned char* input, unsigned int* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    CISST_EXPORT void YUV444toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void YUV444PtoRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void YUV422toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void YUV422PtoRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void NV21toRGB24(unsigned char* input, unsigned char* output, const unsigned int width, const unsigned int height);
    CISST_EXPORT void UYVYtoRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    CISST_EXPORT void HSV24toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
}

#endif // _svlConverters_h

