/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlBufferImage.h>
#include <cisstStereoVision/svlConverters.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <string.h> // for memcpy

/*********************************/
/*** svlBufferImage class ********/
/*********************************/

svlBufferImage::svlBufferImage(unsigned int width, unsigned int height)
{
    Buffer[0].SetSize(height, width * 3);
    Buffer[1].SetSize(height, width * 3);
    Buffer[2].SetSize(height, width * 3);

#if CISST_SVL_HAS_OPENCV
    OCVImage[0] = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
    OCVImage[1] = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
    OCVImage[2] = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
    cvSetData(OCVImage[0], Buffer[0].Pointer(), width * 3);
    cvSetData(OCVImage[1], Buffer[1].Pointer(), width * 3);
    cvSetData(OCVImage[2], Buffer[2].Pointer(), width * 3);
#endif // CISST_SVL_HAS_OPENCV

    Latest = 0;
    Next = 1;
    Locked = 2;
    InitializationCounter = 60;
}

svlBufferImage::~svlBufferImage()
{
#if CISST_SVL_HAS_OPENCV
    cvReleaseImageHeader(&(OCVImage[0]));
    cvReleaseImageHeader(&(OCVImage[1]));
    cvReleaseImageHeader(&(OCVImage[2]));
#endif // CISST_SVL_HAS_OPENCV
}

unsigned int svlBufferImage::GetWidth()
{
    return static_cast<unsigned int>(Buffer[0].width() / 3);
}

unsigned int svlBufferImage::GetHeight()
{
    return static_cast<unsigned int>(Buffer[0].height());
}

unsigned int svlBufferImage::GetDataSize()
{
    return static_cast<unsigned int>(Buffer[0].width() * Buffer[0].height());
}

unsigned char* svlBufferImage::GetPushBuffer()
{
    return Buffer[Next].Pointer();
}

unsigned char* svlBufferImage::GetPushBuffer(unsigned int& size)
{
    size = GetDataSize();
    return Buffer[Next].Pointer();
}

void svlBufferImage::Push()
{
    // Atomic exchange of values
#if (CISST_OS == CISST_WINDOWS)
    Next = InterlockedExchange(&Latest, Next);
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    CS.Enter();
        int ti = Next;
        Next = Latest;
        Latest = ti;
    CS.Leave();
#endif

    NewFrameEvent.Raise();
}

bool svlBufferImage::Push(const unsigned char* buffer, unsigned int size, bool topdown)
{
    unsigned int datasize = static_cast<unsigned int>(Buffer[0].width() * Buffer[0].height());
    if (buffer == 0 || size < datasize) return false;

    bool ret = true;

    // Copy image to buffer
    if (topdown) ret = TopDownCopy(Buffer[Next].Pointer(), buffer);
    else memcpy(Buffer[Next].Pointer(), buffer, datasize);

    // Atomic exchange of values
#if (CISST_OS == CISST_WINDOWS)
    Next = InterlockedExchange(&Latest, Next);
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    CS.Enter();
        int ti = Next;
        Next = Latest;
        Latest = ti;
    CS.Leave();
#endif

    NewFrameEvent.Raise();

    return ret;
}

#if CISST_SVL_HAS_OPENCV
bool svlBufferImage::PushIplImage(IplImage* image)
{
    const bool is_4_channel_input_padded = true; // Must be a bug in OpenCV 2.2
    const int channels = image->widthStep / image->width;

    if (channels == 3) { // No conversion needed
        return Push(reinterpret_cast<unsigned char*>(image->imageData), GetDataSize(), image->origin != IPL_ORIGIN_TL);
    }
    else { // Conversion needed
        const int pixelcount = image->width * image->height;
        const int datasize = pixelcount * 3;

        if (OCVConvBuffer.size() < static_cast<unsigned int>(datasize)) OCVConvBuffer.SetSize(datasize);

        switch (channels) {
            case 1:
                svlConverter::Gray8toRGB24(reinterpret_cast<unsigned char*>(image->imageData), OCVConvBuffer.Pointer(), pixelcount);
            break;

            case 2:
                svlConverter::RGB16toRGB24(reinterpret_cast<unsigned char*>(image->imageData), OCVConvBuffer.Pointer(), pixelcount);
            break;

            case 4:
                if (is_4_channel_input_padded) {
                    const int stride_in  = image->width * 4;
                    const int stride_out = image->width * 3;
                    const int height = image->height;
                    unsigned char* input  = reinterpret_cast<unsigned char*>(image->imageData);
                    unsigned char* output = OCVConvBuffer.Pointer();
                    for (int i = 0; i < height; i ++) {
                        memcpy(output, input, stride_out);
                        input  += stride_in;
                        output += stride_out;
                    }
                }
                else {
                    svlConverter::RGBA32toRGB24(reinterpret_cast<unsigned char*>(image->imageData), OCVConvBuffer.Pointer(), pixelcount);
                }
            break;

            default:
            return false;
        }
        return Push(OCVConvBuffer.Pointer(), GetDataSize(), image->origin != IPL_ORIGIN_TL);
    }
}
#endif

svlImageRGB* svlBufferImage::Pull(bool waitfornew, double timeout)
{
    if (!waitfornew) return &(Buffer[Latest]);

    if (!NewFrameEvent.Wait(0.0)) {
        if (InitializationCounter > 0) {
            // It might take some time to start up the stream in the beginning.
            // Let the method return with a blank image while waiting for
            // the first frame to arrive.
            InitializationCounter --;
            osaSleep(0.033);
        }
        else {
            if (!NewFrameEvent.Wait(timeout)) return 0;
        }
    }
    else {
        // Frames returned, thus we will not tolerate any more interuption
        InitializationCounter = 0;
    }

    // Atomic exchange of values
#if (CISST_OS == CISST_WINDOWS)
    Locked = InterlockedExchange(&Latest, Locked);
#endif

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    CS.Enter();
        int ti = Locked;
        Locked = Latest;
        Latest = ti;
    CS.Leave();
#endif

    return &(Buffer[Locked]);
}

#if CISST_SVL_HAS_OPENCV
IplImage* svlBufferImage::PullIplImage(bool waitfornew, double timeout)
{
    if (Pull(waitfornew, timeout) == 0) return 0;
    return OCVImage[Locked];
}
#endif // CISST_SVL_HAS_OPENCV

bool svlBufferImage::TopDownCopy(unsigned char *targetbuffer, const unsigned char *sourcebuffer)
{
    if (targetbuffer == 0 ||
        sourcebuffer == 0) return false;

    const unsigned int linesize = static_cast<unsigned int>(Buffer[0].width());
    const unsigned int height = static_cast<unsigned int>(Buffer[0].height());
    unsigned char *tptr1 = targetbuffer;
    unsigned char *tptr2 = const_cast<unsigned char*>(sourcebuffer) + linesize * (height - 1);

    for (unsigned int i = 0; i < height; i ++) {
        memcpy(tptr1, tptr2, linesize);
        tptr1 += linesize;
        tptr2 -= linesize;
    }

    return true;
}

