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

#include <cisstStereoVision/trackers/svlTrackerMSBruteForce.h>
#include <string.h>

svlTrackerMSBruteForce::svlTrackerMSBruteForce() : svlPointTrackerAlgoBase()
{
    TargetsAdded = false;
    TemplateRadiusRequested = 3;
    WindowRadiusRequested = 6;
    Metric = trkMSBFMetricSAD;
    Scale = 1;
    OrigTmpltWeight = 255;
    OrigTemplates = 0;
    Templates = 0;
    LowerScale = 0;
    LowerScaleImage = 0;
    TrajectoryModelOrder = 5;
    TrajectoryFilter = 0.0;
    TrajectoryFilterInv = 1.0 - TrajectoryFilter;
    SeriesX = SeriesY = 0;
}

svlTrackerMSBruteForce::~svlTrackerMSBruteForce()
{
    Release();
}

int svlTrackerMSBruteForce::SetParameters(trkMSBFMetric metric,
                                          unsigned int templateradius, unsigned int windowradius,
                                          unsigned int scales, unsigned char tmplupdweight,
                                          unsigned int trajmodelorder, double trajfilter)
{
    if (Initialized) return -1;
    if (templateradius < 1) return -2;
    if (windowradius < 1) return -3;
    if (scales < 1) scales = 1;
    else if (scales > 5) scales = 5;
    Metric = metric;
    TemplateRadiusRequested = templateradius;
    WindowRadiusRequested = windowradius;
    Scale = scales;
    OrigTmpltWeight = 255 - tmplupdweight;
    TrajectoryModelOrder = trajmodelorder;
    TrajectoryFilter = trajfilter;
    TrajectoryFilterInv = 1.0 - TrajectoryFilter;
    return 0;
}

int svlTrackerMSBruteForce::Initialize()
{
    if (Width < 1 || Height < 1) return -1;
    if (TargetCount < 1) return -2;

    Release();

    unsigned int i, templatesize;

    if (Scale > 1) {
        // creating lower scale
        LowerScale = new svlTrackerMSBruteForce();
        // half the image size, scale decremented recursively
        LowerScale->SetParameters(Metric,
                                  TemplateRadiusRequested / 2, WindowRadiusRequested / 2,
                                  Scale - 1, 255 - OrigTmpltWeight,
                                  TrajectoryModelOrder, TrajectoryFilter);
        // same target count
        LowerScale->SetTargetCount(TargetCount);
        // half the image size
        LowerScale->SetImageSize(Width / 2, Height / 2);
        // half the work area
        LowerScale->SetWorkArea(Left / 2, Top / 2, Right / 2, Bottom / 2);
        // adding targets
        svlFilterPointTracker::TargetType params;
        for (i = 0; i < TargetCount; i ++) {
            params.x = TargetParams[i].x / 2;
            params.y = TargetParams[i].y / 2;
            LowerScale->SetTarget(i, &params);
        }
        // initialize
        LowerScale->Initialize();

        // create image buffer for the lower scale
        LowerScaleImage = new unsigned char[(Width / 2) * (Height / 2) * 3];

        // modify current parameters for multiscale processing + add some margin
        TemplateRadius = 2;
        WindowRadius = 2;
    }
    else {
        // coarsest scale so go by the original parameters
        TemplateRadius = TemplateRadiusRequested;
        WindowRadius = WindowRadiusRequested;
    }

    templatesize = TemplateRadius * 2 + 1;
    templatesize *= templatesize * 3;

    OrigTemplates = new unsigned char*[TargetCount];
    Templates = new unsigned char*[TargetCount];
    for (i = 0; i < TargetCount; i ++) {
        OrigTemplates[i] = new unsigned char[templatesize];
        Templates[i] = new unsigned char[templatesize];
    }

    if (Scale == 1) {
        SeriesX = new svlSeries*[TargetCount];
        SeriesY = new svlSeries*[TargetCount];
        for (i = 0; i < TargetCount; i ++) {
            SeriesX[i] = new svlSeries();
            SeriesX[i]->Setup(TrajectoryModelOrder, 0.0);
            SeriesY[i] = new svlSeries();
            SeriesY[i]->Setup(TrajectoryModelOrder, 0.0);
        }
    }

    TargetsAdded = false;
    Initialized = true;

    return 0;
}

void svlTrackerMSBruteForce::Release()
{
    unsigned int i;
    if (Templates) {
        for (i = 0; i < TargetCount; i ++) delete [] Templates[i];
        delete [] Templates;
        Templates = 0;
    }
    if (OrigTemplates) {
        for (i = 0; i < TargetCount; i ++) delete [] OrigTemplates[i];
        delete [] OrigTemplates;
        OrigTemplates = 0;
    }
    if (LowerScale) {
        // deletes all the lower scales recursively
        delete LowerScale;
        LowerScale = 0;
    }
    if (LowerScaleImage) {
        delete [] LowerScaleImage;
        LowerScaleImage = 0;
    }
    if (SeriesX) {
        for (i = 0; i < TargetCount; i ++) delete SeriesX[i];
        delete [] SeriesX;
        SeriesX = 0;
    }
    if (SeriesY) {
        for (i = 0; i < TargetCount; i ++) delete SeriesY[i];
        delete [] SeriesY;
        SeriesY = 0;
    }
    Initialized = false;
}

int svlTrackerMSBruteForce::PreProcessImage(unsigned char* image)
{
    if (!Initialized) return -1;
    if (image == 0) return -2;

    // pre-processing image
    if (Scale > 1) {
        // shirinking image for the lower scales recursively
        ShrinkImage(image, LowerScaleImage);
        LowerScale->PreProcessImage(LowerScaleImage);
    }

    return 0;
}

int svlTrackerMSBruteForce::Track(unsigned char* image)
{
    if (!Initialized) return -1;
    if (image == 0) return -2;

    unsigned int i;

    if (Scale > 1) {
        // track on lower scales recursively
        LowerScale->Track(LowerScaleImage);

        if (TargetsAdded) {
            // scale back results
            svlFilterPointTracker::TargetType target;
            for (i = 0; i < TargetCount; i ++) {
                LowerScale->GetTarget(i, &target);
                TargetParams[i].x = target.x * 2 + 1;
                TargetParams[i].y = target.y * 2 + 1;
            }
        }
    }

    if (TargetsAdded) {
        // tracking
        for (i = 0; i < TargetCount; i ++) {
            // template matching + updating coordinates
            if (Scale == 1) {
                if (Metric == trkMSBFMetricSAD) {
                    MatchTemplateSAD(image,
                                     Templates[i],
                                     static_cast<int>(SeriesX[i]->Prediction()), static_cast<int>(SeriesY[i]->Prediction()),
                                     TargetParams[i].x, TargetParams[i].y);
                }
                else {
                    MatchTemplateSSD(image,
                                     Templates[i],
                                     static_cast<int>(SeriesX[i]->Prediction()), static_cast<int>(SeriesY[i]->Prediction()),
                                     TargetParams[i].x, TargetParams[i].y);
                }

                // trajectory filtering
                TargetParams[i].x = static_cast<int>(TrajectoryFilter * SeriesX[i]->Prediction() +
                                                     TrajectoryFilterInv * TargetParams[i].x);
                TargetParams[i].y = static_cast<int>(TrajectoryFilter * SeriesY[i]->Prediction() +
                                                     TrajectoryFilterInv * TargetParams[i].y);
                // checking filtered results
                if (TargetParams[i].x < static_cast<int>(TemplateRadius)) TargetParams[i].x = TemplateRadius;
                if (TargetParams[i].x >= static_cast<int>(Width - TemplateRadius)) TargetParams[i].x = Width - TemplateRadius - 1;
                if (TargetParams[i].y < static_cast<int>(TemplateRadius)) TargetParams[i].y = TemplateRadius;
                if (TargetParams[i].y >= static_cast<int>(Height - TemplateRadius)) TargetParams[i].y = Height - TemplateRadius - 1;

                // adding new samples to time series
                SeriesX[i]->Push(static_cast<double>(TargetParams[i].x));
                SeriesY[i]->Push(static_cast<double>(TargetParams[i].y));
            }
            else {
                if (Metric == trkMSBFMetricSAD) {
                    MatchTemplateSAD(image,
                                     Templates[i],
                                     TargetParams[i].x, TargetParams[i].y,
                                     TargetParams[i].x, TargetParams[i].y);
                }
                else {
                    MatchTemplateSSD(image,
                                     Templates[i],
                                     TargetParams[i].x, TargetParams[i].y,
                                     TargetParams[i].x, TargetParams[i].y);
                }
            }

            // update templates with new tracking results
            UpdateTemplate(image,
                           OrigTemplates[i],
                           Templates[i],
                           TargetParams[i].x - TemplateRadius, TargetParams[i].y - TemplateRadius);
        }
    }
    else {
        // target initialization
        for (i = 0; i < TargetCount; i ++) {
            if (Scale == 1) {
                // initializing time series
                SeriesX[i]->Reset();
                SeriesY[i]->Reset();
                SeriesX[i]->Push(static_cast<double>(TargetParams[i].x));
                SeriesY[i]->Push(static_cast<double>(TargetParams[i].y));
            }

            // store original templates
            CopyTemplate(image,
                         OrigTemplates[i],
                         TargetParams[i].x - TemplateRadius, TargetParams[i].y - TemplateRadius);
            CopyTemplate(image,
                         Templates[i],
                         TargetParams[i].x - TemplateRadius, TargetParams[i].y - TemplateRadius);
        }
        TargetsAdded = true;
    }

    return 0;
}

void svlTrackerMSBruteForce::CopyTemplate(unsigned char* img, unsigned char* tmp, unsigned int left, unsigned int top)
{
    const unsigned int imstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmpwidth = tmpheight * 3;
    unsigned char *input = img + imstride * top + left * 3;

    // copy data
    for (unsigned int j = 0; j < tmpheight; j ++) {
        memcpy(tmp, input, tmpwidth);
        input += imstride;
        tmp += tmpwidth;
    }
}

void svlTrackerMSBruteForce::UpdateTemplate(unsigned char* img, unsigned char* origtmp, unsigned char* tmp, unsigned int left, unsigned int top)
{
    if (OrigTmpltWeight == 255) {
        unsigned int tmplsize = TemplateRadius * 2 + 1;
        tmplsize *= tmplsize * 3;
        memcpy(tmp, origtmp, tmplsize);
        return;
    }

    if (OrigTmpltWeight == 0) {
        CopyTemplate(img, tmp, left, top);
        return;
    }

    const unsigned int imstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmpwidth = tmpheight * 3;
    const unsigned int endstride = imstride - tmpwidth;
    const unsigned int origweight = OrigTmpltWeight;
    const unsigned int newweight = 255 - origweight;
    unsigned char *input = img + imstride * top + left * 3;
    unsigned int i, j;

    // update template
    for (j = 0; j < tmpheight; j ++) {
        for (i = 0; i < tmpwidth; i ++) {
            *tmp = static_cast<unsigned char>((origweight * (*origtmp) + newweight * (*input)) >> 8);
            input ++;
            origtmp ++;
            tmp ++;
        }
        input += endstride;
    }
}

void svlTrackerMSBruteForce::MatchTemplateSAD(unsigned char* img, unsigned char* tmp, int scx, int scy, int &x, int &y)
{
    const unsigned int imstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmpwidth = tmpheight * 3;
    const unsigned int endstride = imstride - tmpwidth;
    const unsigned int winsize = WindowRadius * 2 + 1;
    const unsigned int imorigstride = imstride - winsize * 3;
    unsigned char *imorig, *image, *ttmp;
    unsigned int i, j;
    int k, l, sum, ival, minval, minx, miny;
    int hfrom, hto, vfrom, vto;

    if (scx < static_cast<int>(TemplateRadius)) scx = TemplateRadius;
    if (scx >= static_cast<int>(Width - TemplateRadius)) scx = Width - TemplateRadius - 1;
    if (scy < static_cast<int>(TemplateRadius)) scy = TemplateRadius;
    if (scy >= static_cast<int>(Height - TemplateRadius)) scy = Height - TemplateRadius - 1;

    hfrom = scx - TemplateRadius - WindowRadius;
    if (hfrom < 0) hfrom = 0;
    hto = scx - TemplateRadius + WindowRadius;
    if (hto > static_cast<int>(Width - tmpheight)) hto = Width - tmpheight;
    vfrom = scy - TemplateRadius - WindowRadius;
    if (vfrom < 0) vfrom = 0;
    vto = scy - TemplateRadius + WindowRadius;
    if (vto > static_cast<int>(Height - tmpheight)) vto = Height - tmpheight;

    imorig = img + imstride * vfrom + hfrom * 3;

    minx = scx - TemplateRadius;
    miny = scy - TemplateRadius;
    minval = 0x7FFFFFFF;

    for (l = vfrom; l <= vto; l ++) {
        for (k = hfrom; k <= hto; k ++) {
            image = imorig;
            ttmp = tmp;

            // match in current position
            sum = 0;
            for (j = 0; j < tmpheight; j ++) {
                for (i = 0; i < tmpwidth; i ++) {
                    ival = (static_cast<int>(*image) - *ttmp);
                    if (ival < 0) sum -= ival;
                    else sum += ival;
                    image ++;
                    ttmp ++;
                }
                image += endstride;
            }

            if (sum < minval) {
                minval = sum;
                minx = k; miny = l;
            }

            imorig += 3;
        }
        imorig += imorigstride;
    }

    x = minx + TemplateRadius;
    y = miny + TemplateRadius;
}

void svlTrackerMSBruteForce::MatchTemplateSSD(unsigned char* img, unsigned char* tmp, int scx, int scy, int &x, int &y)
{
    const unsigned int imstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmpwidth = tmpheight * 3;
    const unsigned int endstride = imstride - tmpwidth;
    const unsigned int winsize = WindowRadius * 2 + 1;
    const unsigned int imorigstride = imstride - winsize * 3;
    unsigned char *imorig, *image, *ttmp;
    unsigned int i, j, sum, minval;
    int k, l, ival, minx, miny;
    int hfrom, hto, vfrom, vto;

    if (scx < static_cast<int>(TemplateRadius)) scx = TemplateRadius;
    if (scx >= static_cast<int>(Width - TemplateRadius)) scx = Width - TemplateRadius - 1;
    if (scy < static_cast<int>(TemplateRadius)) scy = TemplateRadius;
    if (scy >= static_cast<int>(Height - TemplateRadius)) scy = Height - TemplateRadius - 1;

    hfrom = scx - TemplateRadius - WindowRadius;
    if (hfrom < 0) hfrom = 0;
    hto = scx - TemplateRadius + WindowRadius;
    if (hto > static_cast<int>(Width - tmpheight)) hto = Width - tmpheight;
    vfrom = scy - TemplateRadius - WindowRadius;
    if (vfrom < 0) vfrom = 0;
    vto = scy - TemplateRadius + WindowRadius;
    if (vto > static_cast<int>(Height - tmpheight)) vto = Height - tmpheight;

    imorig = img + imstride * vfrom + hfrom * 3;

    minx = scx - TemplateRadius;
    miny = scy - TemplateRadius;
    minval = 0xFFFFFFFF;

    for (l = vfrom; l <= vto; l ++) {
        for (k = hfrom; k <= hto; k ++) {
            image = imorig;
            ttmp = tmp;

            // match in current position
            sum = 0;
            for (j = 0; j < tmpheight; j ++) {
                for (i = 0; i < tmpwidth; i ++) {
                    ival = (static_cast<int>(*image) - *ttmp);
                    sum += ival * ival;
                    image ++;
                    ttmp ++;
                }
                image += endstride;
            }

            if (sum < minval) {
                minval = sum;
                minx = k; miny = l;
            }

            imorig += 3;
        }
        imorig += imorigstride;
    }

    x = minx + TemplateRadius;
    y = miny + TemplateRadius;
}

void svlTrackerMSBruteForce::ShrinkImage(unsigned char* src, unsigned char* dst)
{
    const unsigned int smw = Width / 2;
    const unsigned int smh = Height / 2;
    const unsigned int lgstride = Width * 3;
    const unsigned int lgstride2 = lgstride * 2;

    unsigned char *srcln1, *srcln2, *src1, *src2;
    unsigned int i, j, r, g, b;

    srcln1 = src;
    srcln2 = src + lgstride;

    // update template
    for (j = 0; j < smh; j ++) {
        src1 = srcln1;
        src2 = srcln2;
        for (i = 0; i < smw; i ++) {
            r = *src1 + *src2;
            src1 ++; src2 ++;
            g = *src1 + *src2;
            src1 ++; src2 ++;
            b = *src1 + *src2;
            src1 ++; src2 ++;

            r += *src1 + *src2;
            src1 ++; src2 ++;
            g += *src1 + *src2;
            src1 ++; src2 ++;
            b += *src1 + *src2;
            src1 ++; src2 ++;

            *dst = r >> 2;
            dst ++;
            *dst = g >> 2;
            dst ++;
            *dst = b >> 2;
            dst ++;
        }
        srcln1 += lgstride2;
        srcln2 += lgstride2;
    }
}

