/*
Author: Ankur Kapoor
Date  : 2007-02-15

SVL revision: Balazs Vagvolgyi
Date  : 2007-05-31

$Id$
*/

#include "trkOCVColor.h"


trkOCVColor::trkOCVColor(): TargetDataVec(0), Image8U(0), Hue(0), Hsv(0), Mask(0)
{
    Vmin = 80;
	Vmax = 255; 
	Smin = 30;

    HDims = 48;

	Hranges[0] = 0.0;
	Hranges[1] = 180.0;
	HrangesPtr = &(Hranges[0]);

    MaxIter = 10;
    MaxError = 1.0;
}

void trkOCVColor::AddNewTarget(int width, int height)
{
    trkOCVTarget *target = new trkOCVTarget();
    if (target) {
        CvSize size = cvSize(width, height);
        target->backproject = cvCreateImage( size, IPL_DEPTH_8U, 1 );
        target->hist = cvCreateHist( 1, &HDims, CV_HIST_ARRAY, &HrangesPtr, 1 );

        float max_val = 0.f;
        cvSetImageROI( Hue, selection );
        cvSetImageROI( Mask, selection );
        cvCalcHist( &(Hue), target->hist, 0, Mask );
        cvGetMinMaxHistValue( target->hist, 0, &max_val, 0, 0 );
        cvConvertScale( target->hist->bins, target->hist->bins, max_val ? 255. / max_val : 0., 0 );
        cvResetImageROI( Hue );
        cvResetImageROI( Mask );
        target->track_window = selection;

        TargetDataVec.push_back(target);
    }
}

void trkOCVColor::PreProcessImage()
{
	cvCvtColor(Image8U, Hsv, CV_BGR2HSV);
	cvInRangeS(Hsv,
               cvScalar(0, Smin, std::min(Vmin, Vmax), 0),
               cvScalar(180, 255, std::max(Vmin, Vmax), 0),
               Mask);
	cvSplit(Hsv, Hue, 0, 0, 0);
}

void trkOCVColor::Track()
{
    for (unsigned int i = 0; i < TargetDataVec.size(); i++) {
        trkOCVTarget *target = TargetDataVec[i];

        // required on GCC 4.0.0 to avoid warning:
        //   'dereferencing type-punned pointer will break strict-aliasing rules'
        IplImage** hue = &Hue;

        // if tracking
        // 1. compute back projection
        // 2. use cam shift
        // 3. windows for next iteration := output of cam shift

        cvCalcBackProject(hue, target->backproject, target->hist);
        cvAnd(target->backproject, Mask, target->backproject, 0);
        cvCamShift(target->backproject, target->track_window,
                   cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, MaxIter, MaxError),
                   &(target->track_comp), &(target->track_box));
        target->track_window = target->track_comp.rect;
        target->track_box.angle = -target->track_box.angle;
    }
}
