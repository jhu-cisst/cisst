/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlTypes.h>


/********************************/
/*** svlSampleCUDAImage class ***/
/********************************/

svlSampleCUDAImage::svlSampleCUDAImage() :
    svlSampleImage()
{
}

svlSampleCUDAImage::svlSampleCUDAImage(const svlSampleCUDAImage & other) :
    svlSampleImage(other)
{
}

svlSampleCUDAImage::~svlSampleCUDAImage()
{
}

unsigned char* svlSampleCUDAImage::GetUCharPointer()
{
    CMN_LOG_CLASS_INIT_ERROR << "GetUCharPointer: not allowed to return pointer to GPU memory" << std::endl;
    return 0;
}

const unsigned char* svlSampleCUDAImage::GetUCharPointer() const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetUCharPointer: not allowed to return pointer to GPU memory" << std::endl;
    return 0;
}

void svlSampleCUDAImage::SerializeRaw(std::ostream & CMN_UNUSED(outputStream)) const
{
    CMN_LOG_CLASS_INIT_ERROR << "SerializeRaw: serialization from GPU memory is not supported" << std::endl;
}

void svlSampleCUDAImage::DeSerializeRaw(std::istream & CMN_UNUSED(inputStream))
{
    CMN_LOG_CLASS_INIT_ERROR << "DeSerializeRaw: de-serialization to GPU memory is not supported" << std::endl;
}


///////////////////////////////////
// Inherited from svlSampleImage //
///////////////////////////////////

IplImage* svlSampleCUDAImage::IplImageRef(const unsigned int CMN_UNUSED(videochannel)) const
{
    CMN_LOG_CLASS_INIT_ERROR << "IplImageRef: OpenCV image reference not supported for GPU memory" << std::endl;
    return 0;
}

unsigned char* svlSampleCUDAImage::GetUCharPointer(const unsigned int CMN_UNUSED(videochannel))
{
    CMN_LOG_CLASS_INIT_ERROR << "GetUCharPointer: not allowed to return pointer to GPU memory" << std::endl;
    return 0;
}

const unsigned char* svlSampleCUDAImage::GetUCharPointer(const unsigned int CMN_UNUSED(videochannel)) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetUCharPointer: not allowed to return pointer to GPU memory" << std::endl;
    return 0;
}

unsigned char* svlSampleCUDAImage::GetUCharPointer(const unsigned int CMN_UNUSED(videochannel), const unsigned int CMN_UNUSED(x), const unsigned int CMN_UNUSED(y))
{
    CMN_LOG_CLASS_INIT_ERROR << "GetUCharPointer: not allowed to return pointer to GPU memory" << std::endl;
    return 0;
}

const unsigned char* svlSampleCUDAImage::GetUCharPointer(const unsigned int CMN_UNUSED(videochannel), const unsigned int CMN_UNUSED(x), const unsigned int CMN_UNUSED(y)) const
{
    CMN_LOG_CLASS_INIT_ERROR << "GetUCharPointer: not allowed to return pointer to GPU memory" << std::endl;
    return 0;
}

int svlSampleCUDAImage::CopyOf(const IplImage* CMN_UNUSED(ipl_image), const unsigned int CMN_UNUSED(videochannel))
{
    CMN_LOG_CLASS_INIT_ERROR << "CopyOf: not allowed to copy OpenCV image into GPU memory" << std::endl;
    return SVL_FAIL;
}

svlSampleImage* svlSampleCUDAImage::GetSubImage(const unsigned int CMN_UNUSED(top), const unsigned int CMN_UNUSED(height), const unsigned int CMN_UNUSED(videochannel))
{
    CMN_LOG_CLASS_INIT_ERROR << "GetSubImage: not allowed to split image in GPU memory" << std::endl;
    return 0;
}

svlSampleImage* svlSampleCUDAImage::GetSubImage(svlProcInfo* CMN_UNUSED(procInfo), const unsigned int CMN_UNUSED(videochannel))
{
    CMN_LOG_CLASS_INIT_ERROR << "GetSubImage: not allowed to split image in GPU memory" << std::endl;
    return 0;
}

