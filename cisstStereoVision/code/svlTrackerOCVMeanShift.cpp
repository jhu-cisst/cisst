/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ankur Kapoor & Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlTrackerOCVMeanShift.h>


/*************************************/
/*** svlTrackerOCVMeanShift class ****/
/*************************************/

svlTrackerOCVMeanShift::svlTrackerOCVMeanShift() :
    svlImageTracker(),
    TargetsAdded(false),
    WinRadius(3),
    Vmin(80),
	Vmax(255),
	Smin(30),
    HistDims(48),
    MaxIter(10),
    MaxError(1.0),
    ocvHue(0),
    ocvHsv(0),
    ocvMask(0),
    ocvBackProj(0)
{
}

svlTrackerOCVMeanShift::~svlTrackerOCVMeanShift()
{
    Release();
}

void svlTrackerOCVMeanShift::SetParameters(int winrad, int vmin, int vmax, int smin, int hdims, int maxiter, double maxerror)
{
    if (winrad < 1) winrad = 1;
    else if (winrad > 100) winrad = 100;
    if (vmin < 0) vmin = 0;
    else if (vmin > 255) vmin = 255;
    if (vmax < 0) vmax = 0;
    else if (vmax > 255) vmax = 255;
    if (smin < 0) smin = 0;
    else if (smin > 255) smin = 255;
    if (hdims < 8) hdims = 8;
    else if (hdims > 128) hdims = 128;
    if (maxiter < 1) maxiter = 1;
    else if (maxiter > 100) maxiter = 100;
    if (maxerror < 0.1) maxerror = 0.1;

    WinRadius = winrad;
    Vmin      = vmin;
    Vmax      = vmax;
    Smin      = smin;
    HistDims  = hdims;
    MaxIter   = maxiter;
    MaxError  = maxerror;
}

int svlTrackerOCVMeanShift::SetTargetCount(unsigned int targetcount)
{
    ocvHist.SetSize(targetcount); ocvHist.SetAll(0);
    ocvRect.SetSize(targetcount);

    return svlImageTracker::SetTargetCount(targetcount);
}

int svlTrackerOCVMeanShift::SetTarget(unsigned int targetid, const svlTarget2D & target)
{
    if (targetid >= Targets.size()) return SVL_FAIL;

    ocvRect[targetid].x = target.pos.x - WinRadius;
    ocvRect[targetid].y = target.pos.y - WinRadius;
    ocvRect[targetid].width  = 2 * WinRadius + 1;
    ocvRect[targetid].height = 2 * WinRadius + 1;

    return svlImageTracker::SetTarget(targetid, target);
}

int svlTrackerOCVMeanShift::Initialize()
{
    if (Width < 1 || Height < 1 || Targets.size() < 1) return SVL_FAIL;

    Release();

    CvSize size;
    size.width  = Width;
    size.height = Height;
    ocvHue      = cvCreateImage(size, IPL_DEPTH_8U, 1);
    ocvHsv      = cvCreateImage(size, IPL_DEPTH_8U, 3);
    ocvMask     = cvCreateImage(size, IPL_DEPTH_8U, 1);
    ocvBackProj = cvCreateImage(size, IPL_DEPTH_8U, 1);

    Initialized = true;

    return SVL_OK;
}

int svlTrackerOCVMeanShift::PreProcessImage(svlSampleImage & image, unsigned int videoch)
{
    if (!Initialized) return SVL_FAIL;

    cvCvtColor(image.IplImageRef(videoch), ocvHsv, CV_BGR2HSV);
	cvInRangeS(ocvHsv,
               cvScalar(0, Smin, std::min(Vmin, Vmax), 0),
               cvScalar(180, 255, std::max(Vmin, Vmax), 0),
               ocvMask);
	cvSplit(ocvHsv, ocvHue, 0, 0, 0);

    return SVL_OK;
}

int svlTrackerOCVMeanShift::Track(svlSampleImage & CMN_UNUSED(image), unsigned int CMN_UNUSED(videoch))
{
    if (!Initialized) return SVL_FAIL;

    const unsigned int targetcount = static_cast<unsigned int>(Targets.size());
    unsigned int i;

    CvTermCriteria criteria;

    criteria.epsilon  = MaxError;
    criteria.max_iter = MaxIter;
    criteria.type     = CV_TERMCRIT_EPS | CV_TERMCRIT_ITER;

    if (!TargetsAdded) {
        float max_val;
        float ranges[2];
        float* pranges;

        ranges[0] = 0.0;
        ranges[1] = 180.0;
        pranges = &(ranges[0]);

        for (i = 0; i < targetcount; i ++) {

            max_val = 0.0f;
            ocvHist[i] = cvCreateHist(1, &HistDims, CV_HIST_ARRAY, &pranges, 1);

            cvSetImageROI(ocvHue,  ocvRect[i]);
            cvSetImageROI(ocvMask, ocvRect[i]);

            cvCalcHist(&(ocvHue), ocvHist[i], 0, ocvMask);
            cvGetMinMaxHistValue(ocvHist[i], 0, &max_val, 0, 0);
            cvConvertScale(ocvHist[i]->bins, ocvHist[i]->bins, max_val ? (255.0 / max_val) : 0.0, 0);

            cvResetImageROI(ocvHue);
            cvResetImageROI(ocvMask);
        }

        TargetsAdded = true;
    }

    for (i = 0; i < targetcount; i ++) {
        if (Targets[i].used) {

            cvCalcBackProject(&ocvHue, ocvBackProj, ocvHist[i]);
            cvAnd(ocvBackProj, ocvMask, ocvBackProj, 0);
            cvMeanShift(ocvBackProj, ocvRect[i], criteria, &ocvConnComp);
            ocvRect[i] = ocvConnComp.rect;

            Targets[i].pos.x = (ocvConnComp.rect.x + ocvConnComp.rect.width) / 2;
            Targets[i].pos.y = (ocvConnComp.rect.y + ocvConnComp.rect.height) / 2;
        }
    }

    return SVL_OK;
}

void svlTrackerOCVMeanShift::Release()
{
    Initialized = false;

    if (ocvHue) {
        cvReleaseImage(&(ocvHue));
        ocvHue = 0;
    }
    if (ocvHsv) {
        cvReleaseImage(&(ocvHsv));
        ocvHsv = 0;
    }
    if (ocvMask) {
        cvReleaseImage(&(ocvMask));
        ocvMask = 0;
    }
    if (ocvBackProj) {
        cvReleaseImage(&(ocvBackProj));
        ocvBackProj = 0;
    }
    for (unsigned int i = 0; i < ocvHist.size(); i ++) {
        if (ocvHist[i]) {
            cvReleaseHist(&(ocvHist[i]));
            ocvHist[i] = 0;
        }
    }
}

