/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlTypes.h 1212 2010-02-19 01:47:33Z bvagvol1 $
  
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

#if CISST_SVL_HAS_OPENCV
  #if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_DARWIN)
    #include <cv.h>
    #include <highgui.h>
  #else
    #include <opencv/cv.h>
    #include <opencv/highgui.h>
  #endif
#else
// to compile the same API
typedef void IplImage;
#endif // CISST_SVL_HAS_OPENCV

#include <cisstStereoVision/svlSample.h>
#include <cisstStereoVision/svlSampleImage.h>
#include <cisstStereoVision/svlSampleImageCustom.h>
#include <cisstStereoVision/svlSampleImageTypes.h>
#include <cisstStereoVision/svlSampleMatrix.h>
#include <cisstStereoVision/svlSampleMatrixCustom.h>
#include <cisstStereoVision/svlSampleMatrixTypes.h>
#include <cisstStereoVision/svlSampleTransform3D.h>
#include <cisstStereoVision/svlSampleTargets.h>
#include <cisstStereoVision/svlSampleText.h>

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

struct CISST_EXPORT svlRect
{
    svlRect();
    svlRect(int left, int top, int right, int bottom);
    void Assign(const svlRect & rect);
    void Assign(int left, int top, int right, int bottom);
    void Normalize();
    void Trim(const int minx, const int maxx, const int miny, const int maxy);

    int left;
    int top;
    int right;
    int bottom;
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
};


/////////////////////////////////
// Image structure definitions //
/////////////////////////////////

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

#pragma pack()

#endif // _svlTypes_h

