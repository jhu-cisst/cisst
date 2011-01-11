/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstStereoVision/svlFilterImageTracker.h>
#include <cisstStereoVision/svlSeries.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlTrackerMSBruteForce : public svlImageTracker
{
public:
    svlTrackerMSBruteForce();
    virtual ~svlTrackerMSBruteForce();

    virtual void SetParameters(svlErrorMetric metric,
                               unsigned int  templateradius,
                               unsigned int  windowradius,
                               unsigned int  scales,
                               unsigned char tmplupdweight,
                               double        trajfilter);

    virtual int SetTarget(unsigned int targetid, const svlTarget2D & target);
    virtual int Initialize();
    virtual void ResetTargets();
    virtual int PreProcessImage(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
	virtual int Track(svlSampleImage & image, unsigned int videoch = SVL_LEFT);
    virtual void Release();

protected:
    std::string ScaleName;

    bool TargetsAdded;
    unsigned int TemplateRadiusRequested;
    unsigned int WindowRadiusRequested;
    unsigned int TemplateRadius;
    unsigned int WindowRadius;
    vctDynamicVector<unsigned char*> Templates;
    vctDynamicVector<unsigned char*> OrigTemplates;
    vctDynamicVector<unsigned char> OrigTemplateConf;
    vctDynamicMatrix<int> MatchMap;

    svlErrorMetric Metric;
    unsigned int Scale;
    unsigned char OrigTmpltWeight;
    svlTrackerMSBruteForce* LowerScale;
    svlSampleImageRGB* LowerScaleImage;

    unsigned int TrajectoryModelOrder;
    double TrajectoryFilter;
    double TrajectoryFilterInv;

    virtual void CopyTemplate(unsigned char* img, unsigned char* tmp, unsigned int left, unsigned int top);
    virtual void UpdateTemplate(unsigned char* img, unsigned char* origtmp, unsigned char* tmp, unsigned int left, unsigned int top);
    virtual void MatchTemplateSAD(unsigned char* img, unsigned char* tmp, int x, int y);
    virtual void MatchTemplateSSD(unsigned char* img, unsigned char* tmp, int x, int y);
    virtual void MatchTemplateNCC(unsigned char* img, unsigned char* tmp, int x, int y);
    virtual void GetBestMatch(int &x, int &y, unsigned char &conf, bool higherbetter);
    virtual void ShrinkImage(unsigned char* src, unsigned char* dst);
};

#endif //_svlTrackerMSBruteForce_h

