/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#ifndef _svlStreamDefs_h
#define _svlStreamDefs_h

#include <cisstCommon/cmnLogger.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctFixedSizeMatrix.h>

#include <cisstStereoVision/svlConfig.h>

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

#if (CISST_SVL_HAS_OPENCV == ON)
    #if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_DARWIN)
        #include <cv.h>
        #include <highgui.h>
    #else
        #include <opencv/cv.h>
        #include <opencv/highgui.h>
    #endif
#else // CISST_HAS_OPENCV
    typedef void IplImage;
#endif // CISST_HAS_OPENCV

#define SVL_LEFT                    0
#define SVL_RIGHT                   1
#define SVL_OK                      0
#define SVL_FAIL                    -1
#define SVL_NO                      0
#define SVL_YES                     1


//////////////////////////////
// Stream type enumerations //
//////////////////////////////

enum svlStreamType
{
    svlTypeInvalid,           // Default in base class
    svlTypeStreamSource,      // Capture sources have an input connector of this type
    svlTypeStreamSink,        // Render filters may have an output connector of this type
    svlTypeImageRGB,          // Single RGB image
    svlTypeImageRGBA,         // Single RGBA image
    svlTypeImageRGBStereo,    // Dual RGB image
    svlTypeImageRGBAStereo,   // Dual RGBA image
    svlTypeImageMono8,        // Single Grayscale image (8bpp)
    svlTypeImageMono8Stereo,  // Dual Grayscale image (8bpp)
    svlTypeImageMono16,       // Single Grayscale image (16bpp)
    svlTypeImageMono16Stereo, // Dual Grayscale image (16bpp)
    svlTypeImageCustom,       // Custom, un-enumerated image format
    svlTypeDepthMap,          // Disparity map
    svlTypeRigidXform,        // 3D transformation
    svlTypePointCloud         // Vector of N dimensional points
};


////////////////////////////
// Image type definitions //
////////////////////////////

typedef vctDynamicMatrix<unsigned char> svlImageMono8;
typedef vctDynamicMatrixRef<unsigned char> svlImageMono8Ref;
typedef vctDynamicMatrix<unsigned short> svlImageMono16;
typedef vctDynamicMatrixRef<unsigned short> svlImageMono16Ref;
typedef vctDynamicMatrix<unsigned char> svlImageRGB;
typedef vctDynamicMatrixRef<unsigned char> svlImageRGBRef;
typedef vctDynamicMatrix<unsigned char> svlImageRGBA;
typedef vctDynamicMatrixRef<unsigned char> svlImageRGBARef;
typedef vctDynamicMatrix<float> svlDepthMap;
typedef vctDynamicMatrixRef<float> svlDepthMapRef;

typedef vctFixedSizeMatrix<double, 4, 4> svlRigidXform;
typedef vctDynamicMatrix<double> svlPointCloud;


////////////////////////////////////
// Type checking helper functions //
////////////////////////////////////

template <class __ValueType>
static bool IsTypeFloat(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeFloat<float>(float CMN_UNUSED(val)) { return true; }

template <class __ValueType>
static bool IsTypeUChar(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeUChar<unsigned char>(unsigned char CMN_UNUSED(val)) { return true; }

template <class __ValueType>
static bool IsTypeUWord(__ValueType CMN_UNUSED(val)) { return false; }
template <>
inline bool IsTypeUWord<unsigned short>(unsigned short CMN_UNUSED(val)) { return true; }


///////////////////////////////////////
// Stream data structure definitions //
///////////////////////////////////////

class CISST_EXPORT svlSample
{
public:
    svlSample();
    virtual ~svlSample();
    virtual svlSample* GetNewInstance() = 0;
    virtual svlStreamType GetType() = 0;
    virtual bool IsImage();
    virtual bool IsInitialized();
    void SetTimestamp(double ts);
    double GetTimestamp();
    void SetModified(bool modified);
    bool IsModified();
    static svlSample* GetNewFromType(svlStreamType type);

private:
    double Timestamp; // [seconds]
    bool ModifiedFlag;
};


class CISST_EXPORT svlSampleImageBase : public svlSample
{
public:
    svlSampleImageBase();
    virtual ~svlSampleImageBase();
    virtual svlSample* GetNewInstance() = 0;
    virtual IplImage* IplImageRef(const unsigned int videochannel = 0) = 0;
    bool IsImage();
    virtual svlStreamType GetType() = 0;
    virtual bool IsInitialized() = 0;
    virtual void SetSize(const unsigned int width, const unsigned int height) = 0;
    virtual void SetSize(const unsigned int videochannel, const unsigned int width, const unsigned int height) = 0;
    virtual void SetSize(svlSampleImageBase &sample) = 0;
    virtual unsigned int GetVideoChannels() = 0;
    virtual unsigned int GetDataChannels() = 0;
    virtual unsigned int GetBPP() = 0;
    virtual unsigned int GetWidth(const unsigned int videochannel = 0) = 0;
    virtual unsigned int GetHeight(const unsigned int videochannel = 0) = 0;
    virtual unsigned int GetDataSize(const unsigned int videochannel = 0) = 0;
    virtual unsigned char* GetUCharPointer(const unsigned int videochannel = 0) = 0;
    virtual unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y) = 0;
};


template <class _ValueType, unsigned int _DataChannels, unsigned int _VideoChannels>
class CISST_EXPORT svlSampleImageCustom : public svlSampleImageBase
{
public:
    svlSampleImageCustom() :
        svlSampleImageBase(),
        OwnData(true)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            Image[vch] = new vctDynamicMatrix<_ValueType>;
            ImageRef[vch].SetRef(*(Image[vch]));
#if (CISST_SVL_HAS_OPENCV == ON)
            int ocvdepth = GetOCVDepth();
            if (ocvdepth >= 0) OCVImageHeader[vch] = cvCreateImageHeader(cvSize(0, 0), ocvdepth, _DataChannels);
            else OCVImageHeader[vch] = 0;
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    svlSampleImageCustom(bool owndata) :
        svlSampleImageBase(),
        OwnData(owndata)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (OwnData) {
                Image[vch] = new vctDynamicMatrix<_ValueType>;
                ImageRef[vch].SetRef(*(Image[vch]));
            }
            else Image[vch] = 0;
#if (CISST_SVL_HAS_OPENCV == ON)
            int ocvdepth = GetOCVDepth();
            if (ocvdepth >= 0) OCVImageHeader[vch] = cvCreateImageHeader(cvSize(0, 0), ocvdepth, _DataChannels);
            else OCVImageHeader[vch] = 0;
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    ~svlSampleImageCustom()
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (OwnData) delete Image[vch];
#if (CISST_SVL_HAS_OPENCV == ON)
            if (OCVImageHeader[vch]) cvReleaseImageHeader(&(OCVImageHeader[vch]));
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    svlSample* GetNewInstance()
    {
        return new svlSampleImageCustom<_ValueType, _DataChannels, _VideoChannels>;
    }

    bool SetData(vctDynamicMatrix<_ValueType>* pointer, unsigned int videochannel = 0)
    {
        if (!OwnData && videochannel < _VideoChannels && pointer) {
            Image[videochannel] = pointer;
            ImageRef[videochannel].SetRef(*pointer);
#if (CISST_SVL_HAS_OPENCV == ON)
            if (OCVImageHeader[videochannel]) {
                cvInitImageHeader(OCVImageHeader[videochannel],
                                  cvSize(GetWidth(videochannel), GetHeight(videochannel)),
                                  GetOCVDepth(),
                                  _DataChannels);
                cvSetData(OCVImageHeader[videochannel],
                          GetPointer(videochannel),
                          GetWidth(videochannel) * _DataChannels);
            }
#endif // CISST_SVL_HAS_OPENCV
            return true;
        }
        return false;
    }

    vctDynamicMatrixRef<_ValueType> MatrixRef(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels) return ImageRef[videochannel];
        else return InvalidRef;
    }


#if (CISST_SVL_HAS_OPENCV == ON)
    IplImage* IplImageRef(const unsigned int videochannel = 0)
#else // CISST_SVL_HAS_OPENCV
    IplImage* IplImageRef(const unsigned int CMN_UNUSED(videochannel) = 0)
#endif // CISST_SVL_HAS_OPENCV
    {
#if (CISST_SVL_HAS_OPENCV == ON)
        if (videochannel < _VideoChannels) return OCVImageHeader[videochannel];
        else return 0;
#else // CISST_SVL_HAS_OPENCV
        CMN_LOG_INIT_WARNING << "Class svlSampleImageCustom: IplImageRef() called while OpenCV is disabled" << std::endl;
        return 0;
#endif // CISST_SVL_HAS_OPENCV
    }

    bool IsImage() { return true; }
    unsigned int GetVideoChannels() { return _VideoChannels; }
    unsigned int GetDataChannels() { return _DataChannels; }
    unsigned int GetBPP() { return (sizeof(_ValueType) * _DataChannels); }

    svlStreamType GetType()
    {
        if (IsTypeUChar<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1) {
                if (_VideoChannels == 1) return svlTypeImageMono8;
                if (_VideoChannels == 2) return svlTypeImageMono8Stereo;
            }
            if (_DataChannels == 3) {
                if (_VideoChannels == 1) return svlTypeImageRGB;
                if (_VideoChannels == 2) return svlTypeImageRGBStereo;
            }
            if (_DataChannels == 4) {
                if (_VideoChannels == 1) return svlTypeImageRGBA;
                if (_VideoChannels == 2) return svlTypeImageRGBAStereo;
            }
        }
        if (IsTypeUWord<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1) {
                if (_VideoChannels == 1) return svlTypeImageMono16;
                if (_VideoChannels == 2) return svlTypeImageMono16Stereo;
            }
        }
        if (IsTypeFloat<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1 && _VideoChannels == 1) return svlTypeDepthMap;
        }
        return svlTypeImageCustom;
    }

    bool IsInitialized()
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (Image[vch] == 0 ||
                Image[vch]->width() < _DataChannels ||
                Image[vch]->height() < 1) return false;
        }
        return true;
    }

    void SetSize(const unsigned int width, const unsigned int height)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            SetSize(vch, width, height);
        }
    }

    void SetSize(const unsigned int videochannel, const unsigned int width, const unsigned int height)
    {
        if (videochannel < _VideoChannels && Image[videochannel]) {
            Image[videochannel]->SetSize(height,  width * _DataChannels);
            ImageRef[videochannel].SetRef(*(Image[videochannel]));
#if (CISST_SVL_HAS_OPENCV == ON)
            if (OCVImageHeader[videochannel]) {
                cvInitImageHeader(OCVImageHeader[videochannel],
                                  cvSize(width, height),
                                  GetOCVDepth(),
                                  _DataChannels);
                cvSetData(OCVImageHeader[videochannel],
                          GetPointer(videochannel),
                          width * GetBPP());
            }
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    void SetSize(svlSampleImageBase &sample)
    {
        unsigned int samplevideochannels = sample.GetVideoChannels();
        for (unsigned int vch = 0; vch < _VideoChannels && vch < samplevideochannels; vch ++) {
            SetSize(vch, sample.GetWidth(vch), sample.GetHeight(vch));
        }
    }

    unsigned int GetWidth(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels && Image[videochannel]) return (Image[videochannel]->width() / _DataChannels);
        return 0;
    }

    unsigned int GetHeight(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels && Image[videochannel]) return Image[videochannel]->height();
        return 0;
    }

    unsigned int GetDataSize(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels && Image[videochannel]) {
            return (GetBPP() * GetWidth(videochannel) * GetHeight(videochannel));
        }
        return 0;
    }

    unsigned char* GetUCharPointer(const unsigned int videochannel = 0)
    {
        return reinterpret_cast<unsigned char*>(GetPointer(videochannel));
    }

    unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y)
    {
        return reinterpret_cast<unsigned char*>(GetPointer(videochannel, x, y));
    }

    _ValueType* GetPointer(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels && Image[videochannel]) return Image[videochannel]->Pointer();
        return 0;
    }

    _ValueType* GetPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y)
    {
        if (videochannel < _VideoChannels && Image[videochannel]) {
            return Image[videochannel]->Pointer(y, x * _DataChannels);
        }
        return 0;
    }

private:
    bool OwnData;
    vctDynamicMatrix<_ValueType>* Image[_VideoChannels];
    vctDynamicMatrixRef<_ValueType> ImageRef[_VideoChannels];
    vctDynamicMatrixRef<_ValueType> InvalidRef;

#if (CISST_SVL_HAS_OPENCV == ON)
    IplImage* OCVImageHeader[_VideoChannels];

    int GetOCVDepth()
    {
        if (IsTypeUChar<_ValueType>(static_cast<_ValueType>(0))) return IPL_DEPTH_8U;
        if (IsTypeUWord<_ValueType>(static_cast<_ValueType>(0))) return IPL_DEPTH_16U;
        if (IsTypeFloat<_ValueType>(static_cast<_ValueType>(0))) return IPL_DEPTH_32F;
        return -1;
    }
#endif // CISST_SVL_HAS_OPENCV
};


typedef svlSampleImageCustom<unsigned char,  1, 1>   svlSampleImageMono8;
typedef svlSampleImageCustom<unsigned char,  1, 2>   svlSampleImageMono8Stereo;
typedef svlSampleImageCustom<unsigned short, 1, 1>   svlSampleImageMono16;
typedef svlSampleImageCustom<unsigned short, 1, 2>   svlSampleImageMono16Stereo;
typedef svlSampleImageCustom<unsigned char,  3, 1>   svlSampleImageRGB;
typedef svlSampleImageCustom<unsigned char,  4, 1>   svlSampleImageRGBA;
typedef svlSampleImageCustom<unsigned char,  3, 2>   svlSampleImageRGBStereo;
typedef svlSampleImageCustom<unsigned char,  4, 2>   svlSampleImageRGBAStereo;
typedef svlSampleImageCustom<float,          1, 1>   svlSampleDepthMap;


class CISST_EXPORT svlSampleRigidXform : public svlSample
{
public:
    svlSampleRigidXform();
    svlSample* GetNewInstance();
    svlStreamType GetType();
    bool IsInitialized();
    unsigned char* GetUCharPointer();
    double* GetPointer();
    unsigned int GetDataSize();

    svlRigidXform frame4x4;
};


class CISST_EXPORT svlSamplePointCloud : public svlSample
{
public:
    svlSamplePointCloud();
    svlSample* GetNewInstance();
    svlStreamType GetType();
    bool IsInitialized();
    void SetSize(unsigned int dimensions, unsigned int size);
    void SetSize(svlSamplePointCloud& sample);
    unsigned int GetDimensions();
    unsigned int GetSize();
    unsigned char* GetUCharPointer();
    double* GetPointer();
    unsigned int GetDataSize();

    svlPointCloud points;
};


//////////////////////////////////////////////
// Miscellaneous structure type definitions //
//////////////////////////////////////////////

typedef struct _svlRect {
    int left;
    int top;
    int right;
    int bottom;
    _svlRect() {}
    _svlRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
    void Assign(int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }
} svlRect;


/////////////////////////////////
// Image structure definitions //
/////////////////////////////////

#pragma pack(1)

typedef struct _svlImageProperties {
    svlStreamType DataType;
    unsigned int DataSize;
    unsigned int Width;
    unsigned int Height;
    bool Padding;
} svlImageProperties;

typedef struct _svlBMPFileHeader {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} svlBMPFileHeader;

typedef struct _svlDIBHeader {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} svlDIBHeader;

typedef struct _svlRGB {
    unsigned char B;
    unsigned char G;
    unsigned char R;
} svlRGB;

typedef struct _svlRGBA {
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char A;
} svlRGBA;

#pragma pack()

#endif // _svlStreamDefs_h

