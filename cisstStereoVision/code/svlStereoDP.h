/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlStereoDP_h
#define _svlStereoDP_h

#include <cisstStereoVision/svlFilterComputationalStereo.h>


class svlStereoDP : public svlComputationalStereoMethodBase
{
public:
    svlStereoDP(int width, int height,
                const svlRect & roi,
                int mindisparity, int maxdisparity,
                int ppoffset,
                int scale,
                int blocksize,
                int searchrad,
                int smoothness,
                double tempfilt,
                bool disparityinterpolation);
    virtual ~svlStereoDP();

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

    svlRGB *LeftImage;
    svlRGB *RightImage;
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

    void CreateScale(svlRGB* src_img, svlRGB* dest_img);
    void DisparityOptimization();
    void FilterDisparityMap();
    void RenderDisparityMap(int *disparitymap);
};

#endif // _svlStereoDP_h

