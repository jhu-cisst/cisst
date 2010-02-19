/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlConverters.h 319 2009-05-06 17:40:46Z bvagvol1 $
  
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

#include <cisstStereoVision/svlStreamDefs.h>

namespace svlConverter
{
    void ConvertImage(svlSampleImageBase* inimage, svlSampleImageBase* outimage, int param = 0, unsigned int threads = 1, unsigned int threadid = 0);

    void Gray8toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    void Gray8toRGBA32(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    void Gray8toGray16(unsigned char* input, unsigned short* output, const unsigned int pixelcount);
    void Gray16toRGB24(unsigned short* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    void Gray16toRGBA32(unsigned short* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    void Gray16toGray8(unsigned short* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    void Gray32toRGB24(unsigned int* input, unsigned char* output, const unsigned int pixelcount, const unsigned int shiftdown = 0);
    void int32toRGB24(int* input, unsigned char* output, const unsigned int pixelcount, const int maxinputvalue = -1);
    void int32toRGBA32(int* input, unsigned char* output, const unsigned int pixelcount, const int maxinputvalue = -1);
    void int32toGray8(int* input, unsigned char* output, const unsigned int pixelcount, const int maxinputvalue = -1);
    void float32toRGB24(float* input, unsigned char* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    void float32toRGBA32(float* input, unsigned char* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    void float32toGray8(float* input, unsigned char* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    void float32toGray16(float* input, unsigned short* output, const unsigned int pixelcount, const float scalingratio = 1.0f, const int elementstride = 1);
    void RGB16toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toRGBA32(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    void RGB24toGray8(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    void RGB24toGray16(unsigned char* input, unsigned short* output, const unsigned int pixelcount, bool accurate = false, bool bgr = false);
    void RGB24toYUV444(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toYUV444P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void BGR24toYUV422(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toYUV422(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toYUV422P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toHSV24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toHSV24P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toHSL24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGB24toHSL24P(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void RGBA32toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount);
    void YUV444toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void YUV444PtoRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void YUV422toRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
    void YUV422PtoRGB24(unsigned char* input, unsigned char* output, const unsigned int pixelcount, bool ch1 = true, bool ch2 = true, bool ch3 = true);
}

#endif // _svlConverters_h

