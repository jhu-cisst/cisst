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

#ifndef _svlStereoDPMono_h
#define _svlStereoDPMono_h

#include <cisstStereoVision/svlFilterComputationalStereo.h>


class svlStereoDPMono : public svlComputationalStereoMethodBase
{
public:
    svlStereoDPMono(int width, int height,
                    const svlRect & roi,
                    int mindisparity, int maxdisparity,
                    int ppoffset,
                    int scale,
                    int blocksize,
                    int searchrad,
                    int smoothness,
                    double tempfilt,
                    bool disparityinterpolation);
    virtual ~svlStereoDPMono();

    void SetInterpolation(bool enable) { DisparityInterpolation = enable; }
    bool GetInterpolation() { return DisparityInterpolation; }

    virtual int Initialize();
    virtual int Process(svlSampleImage *images, int *disparitymap);
    virtual void Free();

private:
    //////////////////////////
    // Variables

    int InputWidth;
    int InputHeight;
    int SurfaceWidth;
    int SurfaceHeight;
    int ScaleWidth;
    int ScaleHeight;
    int ValidAreaLeft;
    int ValidAreaRight;
    int ValidAreaTop;
    int ValidAreaBottom;

    // constants: no need to be able to set them externally
    int MaxDisparityDifference;
    int ScoreTruncationLevel;
    double DiscontinuityThreshold;

    int PrincipalPointOffset;
    int MinDisparity;
    int MaxDisparity;
    int DisparityRange;
    int ScaleFactor;
    int NarrowedSearchRadius;
    int BlockSize;
    int Smoothness;
    double TemporalFilter;
    bool DisparityInterpolation;
    unsigned int FrameCounter;

    int *LeftImage;
    int *RightImage;
    int *RightLineBuffer;
    unsigned short *DisparityMap;
    unsigned short *DisparityGraph;
    unsigned short *DisparityCost;

    unsigned short *DisparityMapTemp;

    int DispDiffLUT[256][256];
    int ScoreCache[ST_DP_TEMP_BUFF_SIZE];
    int PrevCostCache[ST_DP_TEMP_BUFF_SIZE];
    unsigned short PrevLineDispMin[ST_DP_TEMP_BUFF_SIZE];
    unsigned short PrevLineDispMax[ST_DP_TEMP_BUFF_SIZE];

    //////////////////////////
    // Functions

    void DisparityOptimization();
    void FilterDisparityMap();
    void RenderDisparityMap(int *disparitymap);

    template <class _paramType>
    void CreateScale(_paramType *src_img, int *dest_img);
};

// *******************************************************************
// CreateScale PRIVATE method
// arguments:
//           src_img        - input image pointer
//           dest_img       - output image pointer
// function:
//    Scales down the input image vertically with the factor of 1/2^ScaleFactor
//    Horizontal size preserved for maximal depth resolution
// *******************************************************************
template <class _paramType>
void svlStereoDPMono::CreateScale(_paramType *src_img, int *dest_img)
{
    int i, j, l, val;
    const int magfact = 1 << ScaleFactor;
    const int srclinestep = InputWidth * sizeof(_paramType);
    const int srcblockstep_y = (magfact - 1) * InputWidth * sizeof(_paramType);
    _paramType *tsrc = 0;

    for (j = 0; j < ScaleHeight; j ++) {
        for (i = 0; i < ScaleWidth; i ++) {

            tsrc = src_img;
            src_img ++;

            val = 0;

            for (l = 0; l < magfact; l ++) {
                val += static_cast<int>(*tsrc);
                tsrc += srclinestep;
            }

            *dest_img = val >> ScaleFactor;
            dest_img ++;
        }

        src_img += srcblockstep_y;
    }
}

#endif // _svlStereoDPMono_h

