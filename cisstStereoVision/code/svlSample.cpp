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

#include <cisstStereoVision/svlTypes.h>


/***********************/
/*** svlSample class ***/
/***********************/

svlSample::svlSample() :
    mtsGenericObject(),
    EncoderParameter(-1)
{
}

svlSample::svlSample(const svlSample & other) :
    mtsGenericObject(other)
{
    SetTimestamp(other.Timestamp);
    SetEncoder(other.Encoder, other.EncoderParameter);
}

svlSample::~svlSample()
{
}

svlSample & svlSample::operator= (const svlSample & other)
{
    SetTimestamp(other.Timestamp);
    SetEncoder(other.Encoder, other.EncoderParameter);
    return *this;
}

bool svlSample::IsInitialized() const
{
    return false;
}

void svlSample::SetTimestamp(double ts)
{
    Timestamp = ts;
}

double svlSample::GetTimestamp() const
{
    return Timestamp;
}

svlSample* svlSample::GetNewFromType(svlStreamType type)
{
    switch (type) {
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:            return 0;
        case svlTypeImageRGB:              return new svlSampleImageRGB;
        case svlTypeImageRGBA:             return new svlSampleImageRGBA;
        case svlTypeImageRGBStereo:        return new svlSampleImageRGBStereo;
        case svlTypeImageRGBAStereo:       return new svlSampleImageRGBAStereo;
        case svlTypeImageMono8:            return new svlSampleImageMono8;
        case svlTypeImageMono8Stereo:      return new svlSampleImageMono8Stereo;
        case svlTypeImageMono16:           return new svlSampleImageMono16;
        case svlTypeImageMono16Stereo:     return new svlSampleImageMono16Stereo;
        case svlTypeImageMono32:           return new svlSampleImageMono32;
        case svlTypeImageMono32Stereo:     return new svlSampleImageMono32Stereo;
        case svlTypeImage3DMap:            return new svlSampleImage3DMap;
#if CISST_SVL_HAS_CUDA
        case svlTypeCUDAImageRGB:          return new svlSampleCUDAImageRGB;
        case svlTypeCUDAImageRGBA:         return new svlSampleCUDAImageRGBA;
        case svlTypeCUDAImageRGBStereo:    return new svlSampleCUDAImageRGBStereo;
        case svlTypeCUDAImageRGBAStereo:   return new svlSampleCUDAImageRGBAStereo;
        case svlTypeCUDAImageMono8:        return new svlSampleCUDAImageMono8;
        case svlTypeCUDAImageMono8Stereo:  return new svlSampleCUDAImageMono8Stereo;
        case svlTypeCUDAImageMono16:       return new svlSampleCUDAImageMono16;
        case svlTypeCUDAImageMono16Stereo: return new svlSampleCUDAImageMono16Stereo;
        case svlTypeCUDAImageMono32:       return new svlSampleCUDAImageMono32;
        case svlTypeCUDAImageMono32Stereo: return new svlSampleCUDAImageMono32Stereo;
        case svlTypeCUDAImage3DMap:        return new svlSampleCUDAImage3DMap;
#else // CISST_SVL_HAS_CUDA
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeCUDAImageRGBAStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:        return 0;
#endif // CISST_SVL_HAS_CUDA
        case svlTypeMatrixInt8:            return new svlSampleMatrixInt8;
        case svlTypeMatrixInt16:           return new svlSampleMatrixInt16;
        case svlTypeMatrixInt32:           return new svlSampleMatrixInt32;
        case svlTypeMatrixInt64:           return new svlSampleMatrixInt64;
        case svlTypeMatrixUInt8:           return new svlSampleMatrixUInt8;
        case svlTypeMatrixUInt16:          return new svlSampleMatrixUInt16;
        case svlTypeMatrixUInt32:          return new svlSampleMatrixUInt32;
        case svlTypeMatrixUInt64:          return new svlSampleMatrixUInt64;
        case svlTypeMatrixFloat:           return new svlSampleMatrixFloat;
        case svlTypeMatrixDouble:          return new svlSampleMatrixDouble;
        case svlTypeTransform3D:           return new svlSampleTransform3D;
        case svlTypeTargets:               return new svlSampleTargets;
        case svlTypeText:                  return new svlSampleText;
        case svlTypeCameraGeometry:        return new svlSampleCameraGeometry;
        case svlTypeBlobs:                 return new svlSampleBlobs;
    }
    return 0;
}

void svlSample::SetEncoder(const std::string & codec, const int parameter)
{
    Encoder = codec;
    EncoderParameter = parameter;
}

void svlSample::GetEncoder(std::string & codec, int & parameter) const
{
    codec = Encoder;
    parameter = EncoderParameter;
}

