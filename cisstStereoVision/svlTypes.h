/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2006

  (C) Copyright 2006-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlTypes_h
#define _svlTypes_h

#include <cisstCommon/cmnLogger.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctTransformationTypes.h>
#include <cisstStereoVision/svlDefinitions.h>
#include <cisstStereoVision/svlConfig.h>

#ifdef _MSC_VER
    // Disable warning C4996
    #pragma warning(disable : 4996)
#endif // _MSC_VER

#if (CISST_COMPILER == CISST_CLANG)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunused-parameter"
    #pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif // (CISST_COMPILER == CISST_CLANG)

#if CISST_SVL_HAS_OPENCV2
    #include <opencv2/core/core_c.h>
    #include <opencv2/core/core.hpp>
    #include <opencv2/core/wimage.hpp>
//    #include <opencv2/eigen.hpp>
    #include <opencv2/core/internal.hpp>
    #include <opencv2/imgproc/imgproc_c.h>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <opencv2/features2d/features2d.hpp>
    #include <opencv2/flann/flann.hpp>
    #include <opencv2/calib3d/calib3d.hpp>
    #include <opencv2/objdetect/objdetect.hpp>
    #include <opencv2/legacy/compat.hpp>
    #include <opencv2/legacy/legacy.hpp>
    #include <opencv2/legacy/blobtrack.hpp>
    #include <opencv2/contrib/contrib.hpp>
    #include <opencv2/highgui/highgui_c.h>
    #include <opencv2/highgui/highgui.hpp>
    #include <opencv2/ml/ml.hpp>
    #include <opencv2/video/tracking.hpp>
    #include <opencv2/video/background_segm.hpp>
    #include <opencv2/gpu/gpu.hpp>
#else // CISST_SVL_HAS_OPENCV2
    // to compile the same API
    typedef void IplImage;
    namespace cv {
        typedef std::string Mat;
    }
#endif // CISST_SVL_HAS_OPENCV2

#if (CISST_COMPILER == CISST_CLANG)
    #pragma clang diagnostic pop
#endif // (CISST_COMPILER == CISST_CLANG)

#ifdef _MSC_VER
    // Resume C4996 warnings
    #pragma warning(default : 4996)
#endif // _MSC_VER

#pragma pack(1)

/////////////////////////////////
// Image structure definitions //
/////////////////////////////////

// Always include last!
#include <cisstStereoVision/svlExport.h>

struct CISST_EXPORT svlBMPFileHeader
{
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
};

struct CISST_EXPORT svlDIBHeader
{
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
};

struct CISST_EXPORT svlRGB
{
    svlRGB();
    svlRGB(unsigned char r, unsigned char g, unsigned char b);
    void Assign(const svlRGB & color);
    void Assign(unsigned char r, unsigned char g, unsigned char b);

    unsigned char b;
    unsigned char g;
    unsigned char r;
};

struct CISST_EXPORT svlRGBA
{
    svlRGBA();
    svlRGBA(const svlRGB & rgb, unsigned char a);
    svlRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void Assign(const svlRGBA & color);
    void Assign(const svlRGB & rgb, unsigned char a);
    void Assign(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

struct CISST_EXPORT svlBlob
{
    svlBlob();
    void Assign(const svlBlob & blob);

    unsigned int ID;
    bool         used;
    int          left;
    int          right;
    int          top;
    int          bottom;
    int          center_x;
    int          center_y;
    unsigned int area;
    unsigned int circumference;
    unsigned int label;
};

#pragma pack()


#include <cisstStereoVision/svlSample.h>
#include <cisstStereoVision/svlSampleImage.h>
#include <cisstStereoVision/svlSampleImageCustom.h>
#include <cisstStereoVision/svlSampleImageTypes.h>
#if CISST_SVL_HAS_CUDA
#include <cisstStereoVision/svlSampleCUDAImage.h>
#include <cisstStereoVision/svlSampleCUDAImageCustom.h>
#include <cisstStereoVision/svlSampleCUDAImageTypes.h>
#endif // CISST_SVL_HAS_CUDA
#include <cisstStereoVision/svlSampleMatrix.h>
#include <cisstStereoVision/svlSampleMatrixCustom.h>
#include <cisstStereoVision/svlSampleMatrixTypes.h>
#include <cisstStereoVision/svlSampleTransform3D.h>
#include <cisstStereoVision/svlSampleTargets.h>
#include <cisstStereoVision/svlSampleText.h>
#include <cisstStereoVision/svlSampleCameraGeometry.h>
#include <cisstStereoVision/svlSampleBlobs.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


////////////////////
// Image typedefs //
////////////////////

typedef vctDynamicMatrix<unsigned char> svlImageMono8;
typedef vctDynamicMatrix<unsigned short> svlImageMono16;
typedef vctDynamicMatrix<float> svlImageMonoFloat;
typedef vctDynamicMatrix<unsigned char> svlImageRGB;
typedef vctDynamicMatrix<unsigned char> svlImageRGBA;


#pragma pack(1)

//////////////////////////////////////////////
// Miscellaneous structure type definitions //
//////////////////////////////////////////////

struct svlRect;

struct CISST_EXPORT svlShape
{
    virtual ~svlShape() {}
    virtual svlShape* Clone() = 0;
    virtual bool IsWithin(int x, int y) const = 0;
    virtual void GetBoundingRect(svlRect & rect) const = 0;
};

struct CISST_EXPORT svlRect : public svlShape
{
    svlRect();
    svlRect(int left, int top, int right, int bottom);

    virtual svlShape* Clone();
    virtual bool IsWithin(int x, int y) const;
    virtual void GetBoundingRect(svlRect & rect) const;

    void Assign(const svlRect & rect);
    void Assign(int left, int top, int right, int bottom);
    void Normalize();
    void Trim(const int minx, const int maxx, const int miny, const int maxy);

    int left;
    int top;
    int right;
    int bottom;
};

struct CISST_EXPORT svlTriangle : public svlShape
{
    svlTriangle();
    svlTriangle(int x1, int y1, int x2, int y2, int x3, int y3);

    virtual svlShape* Clone();
    virtual bool IsWithin(int x, int y) const;
    virtual void GetBoundingRect(svlRect & rect) const;

    void Assign(const svlTriangle & triangle);
    void Assign(int x1, int y1, int x2, int y2, int x3, int y3);

    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
};

struct CISST_EXPORT svlQuad : public svlShape
{
    svlQuad();
    svlQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    svlQuad(const svlRect & rect);

    virtual svlShape* Clone();
    virtual bool IsWithin(int x, int y) const;
    virtual void GetBoundingRect(svlRect & rect) const;

    void Assign(const svlQuad & quad);
    void Assign(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    void Assign(const svlRect & rect);

    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;
};

struct CISST_EXPORT svlEllipse : public svlShape
{
    svlEllipse();
    svlEllipse(int cx, int cy, int rx, int ry, double angle);

    virtual svlShape* Clone();
    virtual bool IsWithin(int x, int y) const;
    virtual void GetBoundingRect(svlRect & rect) const;

    void Assign(const svlEllipse & ellipse);
    void Assign(int cx, int cy, int rx, int ry, double angle);

    int cx;
    int cy;
    int rx;
    int ry;
    double angle;
};

struct CISST_EXPORT svlPoint2D
{
    svlPoint2D();
    svlPoint2D(int x, int y);
    void Assign(const svlPoint2D & point);
    void Assign(int x, int y);

    int x;
    int y;
};

struct CISST_EXPORT svlTarget2D
{
    svlTarget2D();
    svlTarget2D(bool used, bool visible, unsigned char conf, int x, int y);
    svlTarget2D(bool used, bool visible, unsigned char conf, svlPoint2D & pos);
    svlTarget2D(int x, int y);
    svlTarget2D(svlPoint2D & pos);
    void Assign(const svlTarget2D & target);
    void Assign(bool used, bool visible, unsigned char conf, int x, int y);
    void Assign(bool used, bool visible, unsigned char conf, svlPoint2D & pos);
    void Assign(int x, int y);
    void Assign(svlPoint2D & pos);

    bool          used;
    bool          visible;
    unsigned char conf;
    svlPoint2D    pos;
    int           feature_quality;
    vctDynamicVector<unsigned char> feature_data;
    vctDynamicVector<unsigned char> image_data;
};

#pragma pack()

#endif // _svlTypes_h

