/*
Author: Ankur Kapoor
Date  : 2007-02-15

SVL revision: Balazs Vagvolgyi
Date  : 2007-05-31

$Id: trkOCVColor.h,v 1.5 2008/10/29 19:34:55 vagvoba Exp $
*/

#ifndef _trkOCVColor_h
#define _trkOCVColor_h

#include <vector>

#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_DARWIN)
    #include <cv.h>
#else
    #include <opencv/cv.h>
#endif

#define NCOLOR_MARKS 5


class trkOCVColor
{
    typedef struct _trkOCVTarget {
        _trkOCVTarget(): backproject(0), hist(0) {}

        // data per target
        IplImage *backproject;
        CvHistogram *hist;
        CvRect track_window;
        CvBox2D track_box;
        CvConnectedComp track_comp;
    } trkOCVTarget;

public:
    trkOCVColor();
    void AddNewTarget(int width, int height);
    void PreProcessImage();
    void Track();

public:
    // data for individual targets
    std::vector<trkOCVTarget*> TargetDataVec;

    // images for end pt tracking
    IplImage *Image8U, *Hue, *Hsv, *Mask;

    CvRect selection;

    int Vmin, Vmax, Smin;
    int HDims;
    float Hranges[2], *HrangesPtr;
    int MaxIter;
    double MaxError;
};

#endif //_trkOCVColor_h
