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

#ifndef _svlSampleCUDAImageCustom_h
#define _svlSampleCUDAImageCustom_h

#include <cisstStereoVision/svlTypeCheckers.h>
#include <cisstStereoVision/svlProcInfo.h>
#include <cisstStereoVision/svlSampleCUDAImage.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


extern "C" void* cisst_cudaMalloc(unsigned int size);
extern "C" bool cisst_cudaMemset(void* devPtr, unsigned char value, unsigned int size);
extern "C" bool cisst_cudaMemcpyHostToDevice(void* devDestPtr, const void* hostSrcPtr, unsigned int size);
extern "C" bool cisst_cudaMemcpyDeviceToDevice(void* devDestPtr, const void* devSrcPtr, unsigned int size);
extern "C" bool cisst_cudaMemcpyDeviceToHost(void* hostDestPtr, const void* devSrcPtr, unsigned int size);
extern "C" bool cisst_cudaFree(void* devPtr);


template <class _ValueType, unsigned int _DataChannels, unsigned int _VideoChannels>
class CISST_EXPORT svlSampleCUDAImageCustom : public svlSampleCUDAImage
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    typedef svlSampleCUDAImageCustom<_ValueType, _DataChannels, _VideoChannels> ThisType;

public:

    //////////////////
    // Constructors //
    //////////////////

    svlSampleCUDAImageCustom() :
        svlSampleCUDAImage()
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            ImageBuffer[vch] = 0;
            Width[vch] = 0;
            Height[vch] = 0;
        }
    }

    svlSampleCUDAImageCustom(const ThisType & other) :
        svlSampleCUDAImage(other)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            ImageBuffer[vch] = 0;
            Width[vch] = 0;
            Height[vch] = 0;
        }
        CopyOf(other);
    }


    ////////////////
    // Destructor //
    ////////////////

    ~svlSampleCUDAImageCustom()
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            cisst_cudaFree(ImageBuffer[vch]);
        }
    }

    ///////////////
    // Operators //
    ///////////////

    svlSampleCUDAImageCustom & operator= (const ThisType & other)
    {
        CopyOf(other);
        return *this;
    }


    //////////////////////////////
    // Inherited from svlSample //
    //////////////////////////////

    svlSample* GetNewInstance() const
    {
        return new ThisType;
    }

    svlStreamType GetType() const
    {
        if (IsTypeUInt8<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1) {
                if (_VideoChannels == 1) return svlTypeCUDAImageMono8;
                if (_VideoChannels == 2) return svlTypeCUDAImageMono8Stereo;
            }
            if (_DataChannels == 3) {
                if (_VideoChannels == 1) return svlTypeCUDAImageRGB;
                if (_VideoChannels == 2) return svlTypeCUDAImageRGBStereo;
            }
            if (_DataChannels == 4) {
                if (_VideoChannels == 1) return svlTypeCUDAImageRGBA;
                if (_VideoChannels == 2) return svlTypeCUDAImageRGBAStereo;
            }
        }
        if (IsTypeUInt16<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1) {
                if (_VideoChannels == 1) return svlTypeCUDAImageMono16;
                if (_VideoChannels == 2) return svlTypeCUDAImageMono16Stereo;
            }
        }
        if (IsTypeUInt32<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1) {
                if (_VideoChannels == 1) return svlTypeCUDAImageMono32;
                if (_VideoChannels == 2) return svlTypeCUDAImageMono32Stereo;
            }
        }
        if (IsTypeFloat<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 3 && _VideoChannels == 1) return svlTypeCUDAImage3DMap;
        }
        return svlTypeInvalid;
    }

    int SetSize(const svlSample* sample)
    {
        const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(sample);
        if (sampleimage) {
            unsigned int samplevideochannels = sampleimage->GetVideoChannels();
            for (unsigned int vch = 0; vch < _VideoChannels && vch < samplevideochannels; vch ++) {
                SetSize(vch, sampleimage->GetWidth(vch), sampleimage->GetHeight(vch));
            }
            return SVL_OK;
        }
        else {
            const svlSampleMatrix* samplematrix = dynamic_cast<const svlSampleMatrix*>(sample);
            if (samplematrix) {
                SetSize(samplematrix->GetCols(), samplematrix->GetRows());
                return SVL_OK;
            }
        }
        return SVL_FAIL;
    }

    int SetSize(const svlSample& sample)
    {
        const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(&sample);
        if (sampleimage) {
            unsigned int samplevideochannels = sampleimage->GetVideoChannels();
            for (unsigned int vch = 0; vch < _VideoChannels && vch < samplevideochannels; vch ++) {
                SetSize(vch, sampleimage->GetWidth(vch), sampleimage->GetHeight(vch));
            }
            return SVL_OK;
        }
        else {
            const svlSampleMatrix* samplematrix = dynamic_cast<const svlSampleMatrix*>(&sample);
            if (samplematrix) {
                SetSize(samplematrix->GetCols(), samplematrix->GetRows());
                return SVL_OK;
            }
        }
        return SVL_FAIL;
    }

    int CopyOf(const svlSample* sample)
    {
        if (!sample) return SVL_FAIL;
        const ThisType* cuda_image = dynamic_cast<const ThisType*>(sample);
        if (cuda_image) {
        // It is the same type of sample
            // Allocate memory on the device
            if (SetSize(cuda_image) != SVL_OK) return SVL_FAIL;
            // Perform DEVICE to DEVICE memory copy
            for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
                cisst_cudaMemcpyDeviceToDevice(GetCUDAPointer(vch), cuda_image->GetCUDAPointer(vch), GetDataSize(vch));
            }
        }
        else {
        // Try if it is a regular image sample of the same type
            const svlSampleImage* regular_image = dynamic_cast<const svlSampleImage*>(sample);
            if (!regular_image ||
                regular_image->GetVideoChannels() != GetVideoChannels() ||
                regular_image->GetPixelType() != GetPixelType()) return SVL_FAIL;
            // OK: same type sample
            // Allocate memory on the device
            if (SetSize(regular_image) != SVL_OK) return SVL_FAIL;
            // Perform HOST to DEVICE memory copy
            for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
                cisst_cudaMemcpyHostToDevice(GetCUDAPointer(vch), regular_image->GetUCharPointer(vch), GetDataSize(vch));
            }
        }
        SetTimestamp(sample->GetTimestamp());

        return SVL_OK;
    }

    int CopyOf(const svlSample& sample)
    {
        return CopyOf(&sample);
    }

    bool IsInitialized() const
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (Width[vch] < 1 || Height[vch] < 1) return false;
        }
        return true;
    }

    unsigned int GetDataSize() const
    {
        return GetDataSize(0);
    }

    ///////////////////////////////////
    // Inherited from svlSampleImage //
    ///////////////////////////////////

    void SetSize(const unsigned int width, const unsigned int height)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            SetSize(vch, width, height);
        }
    }

    void SetSize(const unsigned int videochannel, const unsigned int width, const unsigned int height)
    {
        if (videochannel < _VideoChannels) {
            if (Width[videochannel] == width && Height[videochannel] == height) return;

            if (ImageBuffer[videochannel]) {
                cisst_cudaFree(ImageBuffer[videochannel]);
                ImageBuffer[videochannel] = 0;
                Width[videochannel]  = 0;
                Height[videochannel] = 0;
            }

            if (width > 0 && height > 0) {
                ImageBuffer[videochannel] = cisst_cudaMalloc(width * GetBPP() * height);
                if (ImageBuffer[videochannel]) {
                    Width[videochannel]  = width;
                    Height[videochannel] = height;
                }
            }
        }
    }

#if CISST_SVL_HAS_OPENCV
    int SetSize(const IplImage* ipl_image, const unsigned int videochannel = 0)
#else // CISST_SVL_HAS_OPENCV
    int SetSize(const IplImage* CMN_UNUSED(ipl_image), const unsigned int CMN_UNUSED(videochannel) = 0)
#endif // CISST_SVL_HAS_OPENCV
    {
#if CISST_SVL_HAS_OPENCV
        if (ipl_image && videochannel >= _VideoChannels) {
            SetSize(videochannel, ipl_image->width, ipl_image->height);
            return SVL_OK;
        }
#endif // CISST_SVL_HAS_OPENCV
        return SVL_FAIL;
    }

    unsigned int GetVideoChannels() const
    {
        return _VideoChannels;
    }

    unsigned int GetDataChannels() const
    {
        return _DataChannels;
    }

    unsigned int GetBPP() const
    {
        return (sizeof(_ValueType) * _DataChannels);
    }

    unsigned int GetWidth(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return Width[videochannel];
        return 0;
    }

    unsigned int GetHeight(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return Height[videochannel];
        return 0;
    }

    unsigned int GetRowStride(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return Width[videochannel] * GetBPP();
        return 0;
    }

    unsigned int GetDataSize(const unsigned int videochannel) const
    {
        if (videochannel < _VideoChannels) {
            return (GetBPP() * GetWidth(videochannel) * GetHeight(videochannel));
        }
        return 0;
    }


    ///////////////////////////////////////
    // Inherited from svlSampleCUDAImage //
    ///////////////////////////////////////

    int CopyTo(svlSample* sample) const
    {
        if (!sample) return SVL_FAIL;
        ThisType* cuda_image = dynamic_cast<ThisType*>(sample);
        if (cuda_image) {
        // It is the same type of sample
            // Allocate memory on the device
            if (cuda_image->SetSize(this) != SVL_OK) return SVL_FAIL;
            // Perform DEVICE to DEVICE memory copy
            for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
                cisst_cudaMemcpyDeviceToDevice(cuda_image->GetCUDAPointer(vch), GetCUDAPointer(vch), GetDataSize(vch));
            }
        }
        else {
        // Try if it is a regular image sample of the same type
            svlSampleImage* regular_image = dynamic_cast<svlSampleImage*>(sample);
            if (!regular_image ||
                regular_image->GetVideoChannels() != GetVideoChannels() ||
                regular_image->GetPixelType() != GetPixelType()) return SVL_FAIL;
            // OK: same type sample
            // Allocate memory on the device
            if (regular_image->SetSize(this) != SVL_OK) return SVL_FAIL;
            // Perform DEVICE to HOST memory copy
            for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
                cisst_cudaMemcpyDeviceToHost(regular_image->GetUCharPointer(vch), GetCUDAPointer(vch), GetDataSize(vch));
            }
        }
        sample->SetTimestamp(GetTimestamp());

        return SVL_OK;
    }

    int CopyTo(svlSample& sample) const
    {
        return CopyTo(&sample);
    }

    void* GetCUDAPointer(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels) return ImageBuffer[videochannel];
        return 0;
    }

    const void* GetCUDAPointer(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return ImageBuffer[videochannel];
        return 0;
    }


private:
    void* ImageBuffer[_VideoChannels];
    unsigned int Width[_VideoChannels];
    unsigned int Height[_VideoChannels];
};

#endif // _svlSampleCUDAImageCustom_h

