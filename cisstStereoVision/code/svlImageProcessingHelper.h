/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlImageProcessingHelper_h
#define _svlImageProcessingHelper_h

#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <string>

#if CISST_SVL_HAS_CISSTNETLIB
    #include <cisstNumerical/nmrNetlib.h>
#endif // CISST_SVL_HAS_CISSTNETLIB


// Always include last!
#include <cisstStereoVision/svlExport.h>


class svlImageProcessingInternals
{
protected:
    // Protected default constructor:
    //   Class can only be instantiated by derived class
    svlImageProcessingInternals();

private:
    // Private copy constructor:
    //   No one else will be able to call it
    svlImageProcessingInternals(const svlImageProcessingInternals& internals);

public:
    // Virtual destructor:
    //   Will force the compiler to create the virtual function table
    virtual ~svlImageProcessingInternals();
};


namespace svlImageProcessingHelper
{
#if CISST_SVL_HAS_CISSTNETLIB

    ///////////////////////////////////////////////
    // Additional numerical routines from LAPACK //
    ///////////////////////////////////////////////

    extern "C" {
        void dgeev_( char* JOBVL,
                    char* JOBVR,
                    CISSTNETLIB_INTEGER* N,
                    CISSTNETLIB_DOUBLE* A,
                    CISSTNETLIB_INTEGER* LDA,
                    CISSTNETLIB_DOUBLE* WR,
                    CISSTNETLIB_DOUBLE* WI,
                    CISSTNETLIB_DOUBLE* VL,
                    CISSTNETLIB_INTEGER* LDVL,
                    CISSTNETLIB_DOUBLE* VR,
                    CISSTNETLIB_INTEGER* LDVR,
                    CISSTNETLIB_DOUBLE* WORK,
                    CISSTNETLIB_INTEGER* LWORK,
                    CISSTNETLIB_INTEGER* INFO );
    }

    template <vct::size_type _n>
    CISSTNETLIB_INTEGER nmrEigenVectors(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _n, _n, VCT_COL_MAJOR> & A, 
                                        vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> & WR,
                                        vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> & WI,
                                        vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _n, _n, VCT_COL_MAJOR> & VL,
                                        vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _n, _n, VCT_COL_MAJOR> & VR)
    {
        vctFixedSizeVector<CISSTNETLIB_DOUBLE, 4 * _n> WORK;
        CISSTNETLIB_INTEGER LWORK = 4 * _n;

        char jobl = 'V';
        char jobr = 'V';

        CISSTNETLIB_INTEGER N = _n;
        CISSTNETLIB_INTEGER INFO;

        dgeev_(&jobl,
               &jobr,
               &N,
               A.Pointer(),
               &N,
               WR.Pointer(),
               WI.Pointer(),
               VL.Pointer(),
               &N,
               VR.Pointer(),
               &N,
               WORK.Pointer(),
               &LWORK,
               &INFO);

        return INFO;
    }

#endif // CISST_SVL_HAS_CISSTNETLIB

    /////////////////
    // Convolution //
    /////////////////

    void CISST_EXPORT ConvolutionRGB(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal, bool absres);
    void CISST_EXPORT ConvolutionRGBA(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal, bool absres);
    void CISST_EXPORT ConvolutionMono8(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal, bool absres);
    void CISST_EXPORT ConvolutionMono16(unsigned short* input, unsigned short* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal, bool absres);
    void CISST_EXPORT ConvolutionMono32(unsigned int* input, unsigned int* output, const int width, const int height, vctDynamicVector<int> & kernel, bool horizontal, bool absres);

    void CISST_EXPORT ConvolutionRGB(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicMatrix<int> & kernel, bool absres);
    void CISST_EXPORT ConvolutionRGBA(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicMatrix<int> & kernel, bool absres);
    void CISST_EXPORT ConvolutionMono8(unsigned char* input, unsigned char* output, const int width, const int height, vctDynamicMatrix<int> & kernel, bool absres);
    void CISST_EXPORT ConvolutionMono16(unsigned short* input, unsigned short* output, const int width, const int height, vctDynamicMatrix<int> & kernel, bool absres);
    void CISST_EXPORT ConvolutionMono32(unsigned int* input, unsigned int* output, const int width, const int height, vctDynamicMatrix<int> & kernel, bool absres);

    //////////////////
    // Unsharp Mask //
    //////////////////

    void CISST_EXPORT UnsharpMaskBlurRGB(const unsigned char* img_in, unsigned char* img_out, const int width, const int height, int radius);
    void CISST_EXPORT UnsharpMaskSharpenRGB(const unsigned char* img_in, const unsigned char* img_mask, unsigned char* img_out, const int width, const int height, const int amount, const int threshold);

    //////////////
    // Resizing //
    //////////////

    void CISST_EXPORT ResampleMono8(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                       unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight);
    void CISST_EXPORT ResampleAndInterpolateHMono8(unsigned char* src, const unsigned int srcwidth,
                                      unsigned char* dst, const unsigned int dstwidth,
                                      const unsigned int height);
    void CISST_EXPORT ResampleAndInterpolateVMono8(unsigned char* src, const unsigned int srcheight,
                                      unsigned char* dst, const unsigned int dstheight,
                                      const unsigned int width);
    void CISST_EXPORT ResampleRGB24(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                       unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight);
    void CISST_EXPORT ResampleAndInterpolateHRGB24(unsigned char* src, const unsigned int srcwidth,
                                      unsigned char* dst, const unsigned int dstwidth,
                                      const unsigned int height);
    void CISST_EXPORT ResampleAndInterpolateVRGB24(unsigned char* src, const unsigned int srcheight,
                                      unsigned char* dst, const unsigned int dstheight,
                                      const unsigned int width);

    ///////////////////
    // Deinterlacing //
    ///////////////////

    void CISST_EXPORT DeinterlaceBlending(unsigned char* buffer, const unsigned int width, const unsigned int height);
    void CISST_EXPORT DeinterlaceDiscarding(unsigned char* buffer, const unsigned int width, const unsigned int height);
    void CISST_EXPORT DeinterlaceAdaptiveBlending(unsigned char* buffer, const unsigned int width, const unsigned int height);
    void CISST_EXPORT DeinterlaceAdaptiveDiscarding(unsigned char* buffer, const unsigned int width, const unsigned int height);

    ///////////////////
    // Rectification //
    ///////////////////

    class CISST_EXPORT RectificationInternals : public svlImageProcessingInternals
    {
    public:
        RectificationInternals();
        virtual ~RectificationInternals();

        bool Generate(unsigned int width, unsigned int height,const svlSampleCameraGeometry & geometry, unsigned int cam_id = SVL_LEFT);
        bool Load(const std::string &filepath, int exponentlen = 3);
        bool SetFromCameraCalibration(unsigned int height,unsigned int width,vct3x3 R,vct2 f, vct2 c, vctFixedSizeVector<double,7> k, double alpha, unsigned int videoch=0);
        void TransposeLUTArray2(unsigned int* index, unsigned int size, unsigned int width, unsigned int height);

        unsigned int Width;
        unsigned int Height;
        unsigned int* idxDest;
        int idxDestSize;
        unsigned int* idxSrc1;
        int idxSrc1Size;
        unsigned int* idxSrc2;
        int idxSrc2Size;
        unsigned int* idxSrc3;
        int idxSrc3Size;
        unsigned int* idxSrc4;
        int idxSrc4Size;
        unsigned char* blendSrc1;
        int blendSrc1Size;
        unsigned char* blendSrc2;
        int blendSrc2Size;
        unsigned char* blendSrc3;
        int blendSrc3Size;
        unsigned char* blendSrc4;
        int blendSrc4Size;

    protected:
        int LoadLine(std::ifstream &file, double* dblbuf, char* chbuf, unsigned int size, int explen);
        void TransposeLUTArray(unsigned int* index, unsigned int size, unsigned int width, unsigned int height);
        void Release();
    };

    //////////////
    // Exposure //
    //////////////

    class CISST_EXPORT ExposureInternals : public svlImageProcessingInternals
    {
    public:
        ExposureInternals();

        void SetBrightness(double brightness);
        double GetBrightness();
        void SetContrast(double contrast);
        double GetContrast();
        void SetGamma(double gamma);
        double GetGamma();

        void CalculateCurve();

        vctDynamicVector<unsigned char> Curve;

    private:
        double Brightness;
        double Contrast;
        double Gamma;
        bool Modified;
    };

    //////////////////
    // BlobDetector //
    //////////////////

    class CISST_EXPORT BlobDetectorInternals : public svlImageProcessingInternals
    {
    public:
        BlobDetectorInternals();

        void SetFilterArea();
        void SetFilterCompactness();

        unsigned int CalculateLabels(const svlSampleImageMono8* image,
                                     svlSampleImageMono32* labels);
        unsigned int CalculateLabels(const svlSampleImageMono8Stereo* image,
                                     svlSampleImageMono32Stereo* labels,
                                     const unsigned int videoch);
        bool GetBlobs(const svlSampleImageMono8* image,
                      const svlSampleImageMono32* labels,
                      svlSampleBlobs* blobs,
                      unsigned int min_area,
                      unsigned int max_area,
                      double min_compactness,
                      double max_compactness);
        bool GetBlobs(const svlSampleImageMono8Stereo* image,
                      const svlSampleImageMono32Stereo* labels,
                      svlSampleBlobs* blobs,
                      const unsigned int videoch,
                      unsigned int min_area,
                      unsigned int max_area,
                      double min_compactness,
                      double max_compactness);

    protected:
        unsigned int  BlobCount;
        vctDynamicMatrix<int> ContourBuffer;

        unsigned int CalculateLabelsInternal(svlSampleImage* image,
                                             svlSampleImage* labels,
                                             const unsigned int videoch);
        bool GetBlobsInternal(svlSampleImage* image,
                              svlSampleImage* labels,
                              svlSampleBlobs* blobs,
                              const unsigned int videoch,
                              const unsigned int min_area,
                              const unsigned int max_area,
                              const double min_compactness,
                              const double max_compactness);
    };

    ///////////////////
    // EllipseFitter //
    ///////////////////

#if CISST_SVL_HAS_CISSTNETLIB

    class CISST_EXPORT EllipseFitterInternals : public svlImageProcessingInternals
    {
    public:
        EllipseFitterInternals();

        bool FitEllipse(vctDynamicVectorRef<int> & xs, vctDynamicVectorRef<int> & ys, svlEllipse & ellipse);

    private:
        vctDynamicVector<double> Xs;
        vctDynamicVector<double> Ys;
        vctDynamicVector<double> XX;
        vctDynamicVector<double> XY;
        vctDynamicVector<double> YY;
        vctDynamicMatrix<double> D;
        vctDynamicMatrix<double> S;
        vctDynamicMatrix<double> C;
        vctDynamicMatrix<double> A;

        vctDynamicVectorRef<double> r_Xs;
        vctDynamicVectorRef<double> r_Ys;
        vctDynamicVectorRef<double> r_XX;
        vctDynamicVectorRef<double> r_XY;
        vctDynamicVectorRef<double> r_YY;
        vctDynamicMatrixRef<double> r_D;

        bool InvertMatrix(const vct3x3 & matrix, vct3x3 & inverse);
    };

#elif CISST_SVL_HAS_OPENCV || CISST_SVL_HAS_OPENCV2

    class CISST_EXPORT EllipseFitterInternals : public svlImageProcessingInternals
    {
    public:
        EllipseFitterInternals();
        ~EllipseFitterInternals();

        bool FitEllipse(vctDynamicVectorRef<int> & xs, vctDynamicVectorRef<int> & ys, svlEllipse & ellipse);

    private:
        unsigned int BufferSize;
        CvPoint2D32f* PointBuffer;
    };

#endif // CISST_SVL_HAS_CISSTNETLIB

};

#endif // _svlImageProcessingHelper_h

