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

#ifndef _svlTrackerOCVMeanShift_h
#define _svlTrackerOCVMeanShift_h

#include <cisstStereoVision/svlFilterImageTracker.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlTrackerOCVMeanShift : public svlImageTracker
{
public:
    svlTrackerOCVMeanShift();
    virtual ~svlTrackerOCVMeanShift();

    virtual void SetParameters(int winrad, int vmin, int vmax, int smin, int hdims, int maxiter, double maxerror);

    virtual int SetTargetCount(unsigned int targetcount);
    virtual int SetTarget(unsigned int targetid, const svlTarget2D & target);
    virtual int Initialize();
    virtual int PreProcessImage(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
	virtual int Track(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
    virtual void Release();

protected:
    bool TargetsAdded;
    int WinRadius;
    int Vmin;
    int Vmax;
    int Smin;
    int HistDims;
    int MaxIter;
    double MaxError;

    vctDynamicVector<CvHistogram*> ocvHist;
    vctDynamicVector<CvRect> ocvRect;
    CvConnectedComp ocvConnComp;
    CvBox2D ocvBox;
    IplImage *ocvHue;
    IplImage *ocvHsv;
    IplImage *ocvMask;
    IplImage *ocvBackProj;
};

#endif //_svlTrackerOCVMeanShift_h

