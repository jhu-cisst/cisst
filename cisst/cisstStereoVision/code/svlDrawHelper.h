/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id$
 
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

#ifndef _svlDrawHelper_h
#define _svlDrawHelper_h

#include <cisstStereoVision/svlTypes.h>


class svlDrawInternals
{
protected:
    // Protected default constructor:
    //   Class can only be instantiated by derived class
    svlDrawInternals();

private:
    // Private copy constructor:
    //   No one else will be able to call it
    svlDrawInternals(const svlDrawInternals& internals);

public:
    // Virtual destructor:
    //   Will force the compiler to create the virtual function table
    virtual ~svlDrawInternals();
};


namespace svlDrawHelper
{
    //////////////////////
    // Triangle Drawing //
    //////////////////////

    class TriangleInternals : public svlDrawInternals
    {
    public:
        TriangleInternals();

        bool SetImage(svlSampleImage* image, unsigned int channel = 0);
        void Draw(int x1, int y1, int x2, int y2, int x3, int y3, svlRGB color);

    private:
        void SampleLine(vctDynamicVector<int>& samples, int x1, int y1, int x2, int y2);

    private:
        svlSampleImage* Image;
        unsigned int Channel;
        int Width;
        int Height;
        vctDynamicVector<int> LeftSamples;
        vctDynamicVector<int> RightSamples;
    };


    //////////////////////
    // Triangle Warping //
    //////////////////////

    class TriangleWarpInternals : public svlDrawInternals
    {
    public:
        TriangleWarpInternals();
        ~TriangleWarpInternals();

        bool SetInputImage(svlSampleImage* image, unsigned int channel = 0);
        bool SetOutputImage(svlSampleImage* image, unsigned int channel = 0);
        void Draw(int ix1, int iy1, int ix2, int iy2, int ix3, int iy3,
                  int ox1, int oy1, int ox2, int oy2, int ox3, int oy3,
                  unsigned int alpha = 256);

    private:
        int GetLinePixels(int* xs, int* ys, int x1, int y1, int x2, int y2);
        int GetLinePixels(int* idxs, int x1, int y1, int x2, int y2, const int w, const int h);
        void ResampleLine(int ix1, int iy1, int ix2, int iy2,
                          int ox1, int oy1, int ox2, int oy2);
        void ResampleLineAlpha(int ix1, int iy1, int ix2, int iy2,
                               int ox1, int oy1, int ox2, int oy2,
                               unsigned int alpha);

        void AllocateBuffers(const unsigned int size);
        void ReleaseBuffers();

    private:
        unsigned char* Input;
        int InWidth;
        int InHeight;

        unsigned char* Output;
        int OutWidth;
        int OutHeight;

        vctInt3 _ilen;
        vctInt3 _olen;

        int* _in_idxs;
        int* _out_idxs;
        int* _lm_x;
        int* _rm_x;
        int* _lm_id;
        int* _rm_id;
        int* _lm_pos;
        int* _rm_pos;

        vctFixedSizeVector<int*, 3> _ixs;
        vctFixedSizeVector<int*, 3> _iys;
        vctFixedSizeVector<int*, 3> _oxs;
        vctFixedSizeVector<int*, 3> _oys;
    };
};

#endif // _svlDrawHelper_h

