/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlTrackerMSBruteForce.h 618 2009-07-31 16:39:42Z bvagvol1 $

  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlTrackerMSBruteForce_h
#define _svlTrackerMSBruteForce_h

#include <cisstStereoVision/svlFilterPointTracker.h>
#include <cisstStereoVision/svlSeries.h>


enum trkMSBFMetric
{
    trkMSBFMetricSAD,
    trkMSBFMetricSSD
};

class svlTrackerMSBruteForce : public svlPointTrackerAlgoBase
{
public:
    svlTrackerMSBruteForce();
    ~svlTrackerMSBruteForce();

    int SetParameters(trkMSBFMetric metric,
                      unsigned int templateradius, unsigned int windowradius,
                      unsigned int scales, unsigned char tmplupdweight,
                      unsigned int trajmodelorder, double trajfilter);

protected:
    int Initialize();
    void Release();

    int PreProcessImage(unsigned char* image);
	int Track(unsigned char* image);

private:
    bool TargetsAdded;
    unsigned int TemplateRadiusRequested;
    unsigned int WindowRadiusRequested;
    unsigned int TemplateRadius;
    unsigned int WindowRadius;
    unsigned char** Templates;
    unsigned char** OrigTemplates;

    trkMSBFMetric Metric;
    unsigned int Scale;
    unsigned char OrigTmpltWeight;
    svlTrackerMSBruteForce* LowerScale;
    unsigned char* LowerScaleImage;

    unsigned int TrajectoryModelOrder;
    double TrajectoryFilter;
    double TrajectoryFilterInv;
    svlSeries **SeriesX, **SeriesY;

    void CopyTemplate(unsigned char* img, unsigned char* tmp, unsigned int left, unsigned int top);
    void UpdateTemplate(unsigned char* img, unsigned char* origtmp, unsigned char* tmp, unsigned int left, unsigned int top);
    void MatchTemplateSAD(unsigned char* img, unsigned char* tmp, int scx, int scy, int &x, int &y);
    void MatchTemplateSSD(unsigned char* img, unsigned char* tmp, int scx, int scy, int &x, int &y);
    void ShrinkImage(unsigned char* src, unsigned char* dst);
};

#endif //_svlTrackerMSBruteForce_h

