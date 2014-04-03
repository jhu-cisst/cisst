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
#include <cisstStereoVision/svlTypeCheckers.h>


/*****************************/
/*** svlSampleMatrix class ***/
/*****************************/

svlSampleMatrix::svlSampleMatrix() :
    svlSample()
{
}

svlSampleMatrix::svlSampleMatrix(const svlSampleMatrix & other) :
    svlSample(other)
{
}

svlSampleMatrix::~svlSampleMatrix()
{
}

template <class _TypeIn>
int svlSampleMatrix::ImportData(_TypeIn *input, const unsigned int size)
{
    switch (GetType()) {
        case svlTypeMatrixInt8:   return ConvertData<_TypeIn, char>(input, size);
        case svlTypeMatrixInt16:  return ConvertData<_TypeIn, short>(input, size);
        case svlTypeMatrixInt32:  return ConvertData<_TypeIn, int>(input, size);
        case svlTypeMatrixInt64:  return ConvertData<_TypeIn, long long int>(input, size);
        case svlTypeMatrixUInt8:  return ConvertData<_TypeIn, unsigned char>(input, size);
        case svlTypeMatrixUInt16: return ConvertData<_TypeIn, unsigned short>(input, size);
        case svlTypeMatrixUInt32: return ConvertData<_TypeIn, unsigned int>(input, size);
        case svlTypeMatrixUInt64: return ConvertData<_TypeIn, unsigned long long int>(input, size);
        case svlTypeMatrixFloat:  return ConvertData<_TypeIn, float>(input, size);
        case svlTypeMatrixDouble: return ConvertData<_TypeIn, double>(input, size);

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
        case svlTypeImage3DMap:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBAStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
        break;
    }
    return SVL_FAIL;
}

int svlSampleMatrix::ImportMatrix(const svlSampleMatrix* matrix)
{
    if (!matrix) return SVL_FAIL;

    const unsigned int cols = matrix->GetCols();
    const unsigned int rows = matrix->GetRows();
    SetSize(cols, rows);

    svlSampleMatrix* nc_matrix = const_cast<svlSampleMatrix*>(matrix);
    const unsigned int size = cols * rows;

    switch (matrix->GetType()) {
        case svlTypeMatrixInt8:   return ImportData(reinterpret_cast<char*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixInt16:  return ImportData(reinterpret_cast<short*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixInt32:  return ImportData(reinterpret_cast<int*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixInt64:  return ImportData(reinterpret_cast<long long int*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixUInt8:  return ImportData(reinterpret_cast<unsigned char*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixUInt16: return ImportData(reinterpret_cast<unsigned short*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixUInt32: return ImportData(reinterpret_cast<unsigned int*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixUInt64: return ImportData(reinterpret_cast<unsigned long long int*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixFloat:  return ImportData(reinterpret_cast<float*>(nc_matrix->GetUCharPointer()), size); break;
        case svlTypeMatrixDouble: return ImportData(reinterpret_cast<double*>(nc_matrix->GetUCharPointer()), size); break;

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
        case svlTypeImage3DMap:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBAStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
        break;
    }

    return SVL_FAIL;
}

int svlSampleMatrix::ImportImage(const svlSampleImage* image)
{
    if (!image) return SVL_FAIL;

    svlSampleImage* nc_image = const_cast<svlSampleImage*>(image);
    const unsigned int width = image->GetWidth(0);
    const unsigned int height = image->GetHeight(0);
    const unsigned int size = width * height;

    switch (image->GetType()) {
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
            SetSize(width * 3, height);
            return ImportData(reinterpret_cast<unsigned char*>(nc_image->GetUCharPointer(0)), size * 3);
        break;

        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
            SetSize(width * 4, height);
            return ImportData(reinterpret_cast<unsigned char*>(nc_image->GetUCharPointer(0)), size * 4);
        break;

        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
            SetSize(width, height);
            return ImportData(reinterpret_cast<unsigned char*>(nc_image->GetUCharPointer(0)), size);
        break;

        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
            SetSize(width, height);
            return ImportData(reinterpret_cast<unsigned short*>(nc_image->GetUCharPointer(0)), size);
        break;

        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
            SetSize(width, height);
            return ImportData(reinterpret_cast<unsigned int*>(nc_image->GetUCharPointer(0)), size);
        break;

        case svlTypeImage3DMap:

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeMatrixInt8:
        case svlTypeMatrixInt16:
        case svlTypeMatrixInt32:
        case svlTypeMatrixInt64:
        case svlTypeMatrixUInt8:
        case svlTypeMatrixUInt16:
        case svlTypeMatrixUInt32:
        case svlTypeMatrixUInt64:
        case svlTypeMatrixFloat:
        case svlTypeMatrixDouble:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBAStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
        break;
    }

    return SVL_FAIL;
}

int svlSampleMatrix::ImportSample(const svlSample* sample)
{
    const svlSampleMatrix* matrix = dynamic_cast<const svlSampleMatrix*>(sample);
    if (matrix) return ImportMatrix(matrix);

    const svlSampleImage* image = dynamic_cast<const svlSampleImage*>(sample);
    if (image) return ImportImage(image);

    return SVL_FAIL;
}

template <class _TypeIn, class _TypeOut>
int svlSampleMatrix::ConvertData(_TypeIn *input, const unsigned int size)
{
    if (!input || size == 0) return SVL_FAIL;
    
    unsigned int i = GetCols() * GetRows();
    if (size < i) i = size;
    
    bool checklow = true, checkhi = true;
    _TypeOut* output = reinterpret_cast<_TypeOut*>(GetUCharPointer());
    long long int inmin, outmin, inmax, outmax;
    _TypeIn    inval;
    
    if (GetMaxValue<_TypeOut>(outmax) == false ||
        (GetMaxValue<_TypeIn>(inmax) == true &&
         inmax <= outmax)) checkhi = false;
    if (GetMinValue<_TypeOut>(outmin) == false ||
        (GetMinValue<_TypeIn>(inmin) == true &&
         inmin >= outmin)) checklow = false;
    
    if (checkhi) {
        _TypeIn  intype_max  = static_cast<_TypeIn >(outmax);
        _TypeOut outtype_max = static_cast<_TypeOut>(outmax);
        
        if (checklow) {
            _TypeIn  intype_min  = static_cast<_TypeIn >(outmin);
            _TypeOut outtype_min = static_cast<_TypeOut>(outmin);
            
            while (i) {
                inval = *input;
                if (inval > intype_max) *output = outtype_max;
                else if (inval < intype_min) *output = outtype_min;
                else *output = static_cast<_TypeOut>(inval);
                output ++; input ++; i --;
            }
        }
        else {
            while (i) {
                inval = *input;
                if (inval > intype_max) *output = outtype_max;
                else *output = static_cast<_TypeOut>(inval);
                output ++; input ++; i --;
            }
        }
    }
    else {
        if (checklow) {
            _TypeIn  intype_min  = static_cast<_TypeIn >(outmin);
            _TypeOut outtype_min = static_cast<_TypeOut>(outmin);
            
            while (i) {
                inval = *input;
                if (inval < intype_min) *output = outtype_min;
                else *output = static_cast<_TypeOut>(inval);
                output ++; input ++; i --;
            }
        }
        else {
            while (i) {
                *output = static_cast<_TypeOut>(*input);
                output ++; input ++; i --;
            }
        }
    }
    
    return SVL_OK;
}


// Explicit instantiation of used types
template int svlSampleMatrix::ImportData<char>(char *input, const unsigned int size);
template int svlSampleMatrix::ImportData<short>(short *input, const unsigned int size);
template int svlSampleMatrix::ImportData<int>(int *input, const unsigned int size);
template int svlSampleMatrix::ImportData<long long int>(long long int *input, const unsigned int size);
template int svlSampleMatrix::ImportData<unsigned char>(unsigned char *input, const unsigned int size);
template int svlSampleMatrix::ImportData<unsigned short>(unsigned short *input, const unsigned int size);
template int svlSampleMatrix::ImportData<unsigned int>(unsigned int *input, const unsigned int size);
template int svlSampleMatrix::ImportData<unsigned long long int>(unsigned long long int *input, const unsigned int size);
template int svlSampleMatrix::ImportData<float>(float *input, const unsigned int size);
template int svlSampleMatrix::ImportData<double>(double *input, const unsigned int size);

