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

#include <cisstStereoVision/svlTrackerMSBruteForce.h>

//#define __DEBUG_TRACKER


/*************************/
/*** Helper functions ****/
/*************************/

inline unsigned int sqrt_uint64(unsigned long long value)
{
	unsigned int a, g = 0;
	unsigned int bshft = 31;
	unsigned int b = 1 << 31;
    unsigned long long temp;
    union { unsigned int ui[2]; long long ll; } c;

    do {
        a = g + g + b;
        if (bshft) {
            c.ui[0] = a << bshft;
            c.ui[1] = a >> (32 - bshft);
            temp = c.ll;
        }
        else temp = a;

		if (value >= temp) {     
			g += b;
			value -= temp;
		}
		b >>= 1;
	} while (bshft --);

	return g;
}

inline unsigned int sqrt_uint32(unsigned int value)
{
    unsigned int a, g = 0;
    unsigned int bshft = 15;
    unsigned int b = 1 << bshft;

    do {
        a = (g + g + b) << bshft;
        if (value >= a) {
            g += b;
            value -= a;
        }
        b >>= 1;
    } while (bshft --);

    return g;
}


/*************************************/
/*** svlTrackerMSBruteForce class ****/
/*************************************/

svlTrackerMSBruteForce::svlTrackerMSBruteForce() :
    svlImageTracker(),
    TargetsAdded(false),
    OverwriteTemplates(false),
    TemplateUpdateEnabled(true),
    TemplateRadiusRequested(3),
    SearchRadiusRequested(6),
    Metric(svlNCC),
    Scale(1),
    TemplateUpdateWeight(0),
    ConfidenceThreshold(0),
    LowerScale(0),
    LowerScaleImage(0),
    PreviousImage(0)
{
}

svlTrackerMSBruteForce::~svlTrackerMSBruteForce()
{
    Release();
}

void svlTrackerMSBruteForce::SetErrorMetric(svlErrorMetric metric)
{
    if (Initialized) {
        if (LowerScale) LowerScale->SetErrorMetric(metric);
    }
    else {
        Metric = metric;
    }
}

void svlTrackerMSBruteForce::SetScales(unsigned int  scales)
{
    if (Initialized) {
        if (LowerScale) LowerScale->SetScales(scales - 1);
    }
    else {
        Scale = std::max(scales, 1u);
    }
}

void svlTrackerMSBruteForce::SetTemplateRadius(unsigned int radius)
{
    TemplateRadiusRequested = std::max(radius, 1u);

    if (Initialized && LowerScale) LowerScale->SetTemplateRadius(radius / 2);
}

void svlTrackerMSBruteForce::SetSearchRadius(unsigned int radius)
{
    SearchRadiusRequested = std::max(radius, 1u);

    if (Initialized && LowerScale) LowerScale->SetSearchRadius(radius / 2);
}

void svlTrackerMSBruteForce::SetOverwriteTemplates(bool enable)
{
    OverwriteTemplates = enable;
}

void svlTrackerMSBruteForce::SetTemplateUpdate(bool enable)
{
    TemplateUpdateEnabled = enable;
}

void svlTrackerMSBruteForce::SetTemplateUpdateWeight(double weight)
{
    int ival = static_cast<unsigned int>(weight * 255);
    if (ival < 0) ival = 0;
    else if (ival > 255) ival = 255;
    TemplateUpdateWeight = static_cast<unsigned char>(ival);

    if (Initialized && LowerScale) LowerScale->SetTemplateUpdateWeight(weight);
}

void svlTrackerMSBruteForce::SetConfidenceThreshold(double threshold)
{
    int ival = static_cast<unsigned int>(threshold * 255);
    if (ival < 0) ival = 0;
    else if (ival > 255) ival = 255;
    ConfidenceThreshold = static_cast<unsigned char>(ival);

    if (Initialized && LowerScale) LowerScale->SetConfidenceThreshold(threshold);
}

svlErrorMetric svlTrackerMSBruteForce::GetErrorMetric() const
{
    return Metric;
}

unsigned int svlTrackerMSBruteForce::GetScales() const
{
    return Scale;
}

unsigned int svlTrackerMSBruteForce::GetTemplateRadius() const
{
    return TemplateRadiusRequested;
}

unsigned int svlTrackerMSBruteForce::GetSearchRadius() const
{
    return SearchRadiusRequested;
}

bool svlTrackerMSBruteForce::GetOverwriteTemplates() const
{
    return OverwriteTemplates;
}

bool svlTrackerMSBruteForce::GetTemplateUpdate() const
{
    return TemplateUpdateEnabled;
}

double svlTrackerMSBruteForce::GetTemplateUpdateWeight() const
{
    return static_cast<double>(TemplateUpdateWeight) / 255.0;
}

double svlTrackerMSBruteForce::GetConfidenceThreshold() const
{
    return static_cast<double>(ConfidenceThreshold) / 255.0;
}

int svlTrackerMSBruteForce::GetFeatureRef(unsigned int targetid, vctDynamicVectorRef<unsigned char> & feature)
{
    if (targetid >= Targets.size()) return SVL_FAIL;
    feature.SetRef(Targets[targetid].feature_data);
    return SVL_OK;
}

int svlTrackerMSBruteForce::SetTarget(unsigned int targetid, const svlTarget2D & target)
{
    if (targetid >= Targets.size()) return SVL_FAIL;

    if (LowerScale) {
        svlTarget2D ls_target;
        ls_target.used            = target.used;
        ls_target.conf            = target.conf;
        ls_target.pos.x           = (target.pos.x + 1) >> 1;
        ls_target.pos.y           = (target.pos.y + 1) >> 1;

        LowerScale->SetTarget(targetid, ls_target);
    }

    return svlImageTracker::SetTarget(targetid, target);
}

int svlTrackerMSBruteForce::Initialize()
{
    if (Width < 1 || Height < 1) return SVL_FAIL;

    Release();

    unsigned int i, templatesize;
    const unsigned int targetcount = static_cast<unsigned int>(Targets.size());

#ifdef __DEBUG_TRACKER
        std::stringstream __name;
        __name << "Scale " << Scale;
        ScaleName = __name.str();
#endif

    if (Scale > 1) {
        // creating lower scale
        LowerScale = new svlTrackerMSBruteForce();

        // setup lower scale
        LowerScale->SetErrorMetric(Metric);
        LowerScale->SetScales(Scale - 1);
        LowerScale->SetTemplateRadius((TemplateRadiusRequested + 1) / 2);
        LowerScale->SetSearchRadius((SearchRadiusRequested + 1) / 2);
        LowerScale->SetTemplateUpdateWeight(static_cast<double>(TemplateUpdateWeight) / 255.0);
        LowerScale->SetConfidenceThreshold(static_cast<double>(ConfidenceThreshold) / 255.0);
        LowerScale->SetImageSize(Width / 2, Height / 2);
        LowerScale->SetTargetCount(targetcount);

        // initialize
        LowerScale->Initialize();

        // create image for the lower scale
        LowerScaleImage = new svlSampleImageRGB;
        LowerScaleImage->SetSize(Width / 2, Height / 2);

        // modify current parameters for multiscale processing + add some margin
        TemplateRadius = std::max(TemplateRadiusRequested, 1u);
        SearchRadius = 2;
    }
    else {
        // coarsest scale so go by the original parameters
        TemplateRadius = std::max(TemplateRadiusRequested, 1u);
        SearchRadius = std::max(SearchRadiusRequested, 2u);
    }

    // create previous image buffer
    PreviousImage = new svlSampleImageRGB;
    PreviousImage->SetSize(Width, Height);

    templatesize = TemplateRadius * 2 + 1;
    templatesize *= templatesize * 3;

    // Allocate memory for the original image templates
    OrigTemplates.SetSize(targetcount);
    for (i = 0; i < targetcount; i ++) {
        OrigTemplates[i] = new unsigned char[templatesize];
        Targets[i].feature_quality = -1;
        Targets[i].feature_data.SetAll(0);
    }

    MatchMap.SetSize(SearchRadius * 2 + 1, SearchRadius * 2 + 1);

    TargetsAdded  = false;
    Initialized   = true;
    FrameCounter  = 0;
    ThreadCounter = 0;

    return SVL_OK;
}

void svlTrackerMSBruteForce::ResetTargets()
{
    const unsigned int targetcount = static_cast<unsigned int>(Targets.size());
    for (unsigned int i = 0; i < targetcount; i ++) {
        Targets[i].feature_quality = -1;
        Targets[i].feature_data.SetAll(0);
    }
    TargetsAdded  = false;
    FrameCounter  = 0;

    if (LowerScale) LowerScale->ResetTargets();
}

int svlTrackerMSBruteForce::PreProcessImage(svlSampleImage & image, unsigned int videoch)
{
    if (!Initialized) return SVL_FAIL;

    // pre-processing image
    if (LowerScale) {
        // shirinking image for the lower scales recursively
        ShrinkImage(image.GetUCharPointer(videoch), LowerScaleImage->GetUCharPointer());
        LowerScale->SetROI(ROI.left / 2, ROI.top / 2, ROI.right / 2, ROI.bottom / 2);
        LowerScale->PreProcessImage(*LowerScaleImage);
    }

    return SVL_OK;
}

int svlTrackerMSBruteForce::Track(svlSampleImage & image, unsigned int videoch)
{
    if (!Initialized) return SVL_FAIL;

    const int leftborder   = ROI.left   + TemplateRadius;
    const int topborder    = ROI.top    + TemplateRadius;
    const int rightborder  = ROI.right  - TemplateRadius;
    const int bottomborder = ROI.bottom - TemplateRadius;
    const unsigned int targetcount = static_cast<unsigned int>(Targets.size());
    const unsigned int scalem1 = Scale - 1;
    unsigned int templatesize = TemplateRadius * 2 + 1;
    templatesize *= templatesize * 3;

    int xpre, ypre, x, y;
    svlTarget2D target, *ptgt;
    unsigned char conf, *p_img;
    unsigned int i;


    if (FrameCounter > 0) p_img = PreviousImage->GetUCharPointer();
    else p_img = image.GetUCharPointer(videoch);

    for (i = 0, ptgt = Targets.Pointer(); i < targetcount; i ++, ptgt ++) {
        if (!ptgt->used) {
            ptgt->visible = false;
            continue;
        }

        // Determine target visibility
        x = ptgt->pos.x;
        y = ptgt->pos.y;
        if (x < leftborder || x >= rightborder || y < topborder  || y >= bottomborder) {
            ptgt->visible = false;
            ptgt->conf    = 0;
            // Skip target if not visible
            continue;
        }
        ptgt->visible = true;

        if (TemplateUpdateEnabled) {
            // Check if this scale already has a template
            // Acquire target templates if necessary
            if (ptgt->feature_quality == -1) {
                ptgt->feature_data.SetSize(templatesize);

                // Update this scale's template with the
                // new position estimated by the tracker
                // filter
                CopyTemplate(p_img,
                             OrigTemplates[i],
                             ptgt->pos.x - TemplateRadius,
                             ptgt->pos.y - TemplateRadius);
                CopyTemplate(p_img,
                             ptgt->feature_data.Pointer(),
                             ptgt->pos.x - TemplateRadius,
                             ptgt->pos.y - TemplateRadius);

                ptgt->conf            = 255;
                ptgt->feature_quality = 256;
            }
            else if (OverwriteTemplates) {
                // Overwrite template based on updated position
                UpdateTemplate(p_img,
                               OrigTemplates[i],
                               ptgt->feature_data.Pointer(),
                               ptgt->pos.x - TemplateRadius,
                               ptgt->pos.y - TemplateRadius);
            }
        }
    }


    if (LowerScale) {

        // Call lower scales recursively
        LowerScale->SetROI(ROI.left / 2, ROI.top / 2, ROI.right / 2, ROI.bottom / 2);
        LowerScale->Track(*LowerScaleImage);

        // Scale up the tracking results from the
        // lower scale and use that as new position
        for (i = 0, ptgt = Targets.Pointer(); i < targetcount; i ++, ptgt ++) {
            if (!ptgt->used) continue;

            // Scale up lower scale results only if this target has been initialized earlier
            if (ptgt->feature_quality >= 0) {

                LowerScale->GetTarget(i, target);

                // Scale up only valid lower scale results
                if (target.feature_quality >= 0 && target.visible) {
                    ptgt->conf    = target.conf;
                    ptgt->pos.x   = target.pos.x * 2 + 1;
                    ptgt->pos.y   = target.pos.y * 2 + 1;
                }
                else {
                    ptgt->visible = false;
                    ptgt->conf    = 0;
                }
            }
        }
    }


    // Track targets
    for (i = 0, ptgt = Targets.Pointer(); i < targetcount; i ++, ptgt ++) {

        // Skip non-visible targets
        if (!ptgt->visible) continue;

        // Skip templates with poor quality
        if (ptgt->feature_quality < ConfidenceThreshold) {
            ptgt->visible = false;
            ptgt->conf    = 0;
            continue;
        }

        // template matching + updating coordinates
        xpre = ptgt->pos.x;
        ypre = ptgt->pos.y;

        if (Scale == 1) {
            if (Metric == svlSAD) {
                MatchTemplateSAD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, ptgt->conf, false);
            }
            else if (Metric == svlSSD) {
                MatchTemplateSSD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, ptgt->conf, false);
            }
            else if (Metric == svlNCC) {
                MatchTemplateNCC(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, ptgt->conf, true);
            }
            else return SVL_FAIL;
        }
        else {
            if (Metric == svlSAD) {
                MatchTemplateSAD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, conf, false);
            }
            else if (Metric == svlSSD) {
                MatchTemplateSSD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, conf, false);
            }
            else if (Metric == svlNCC) {
                MatchTemplateNCC(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, conf, true);
            }
            else return SVL_FAIL;

            if (ptgt->conf < ConfidenceThreshold) ptgt->conf = 0;
            ptgt->conf = (static_cast<int>(ptgt->conf) * scalem1 + conf) / Scale;
        }

        if (ptgt->feature_quality == 256) {
            // Determine the quality of the feature when it was first used
            ptgt->feature_quality = ptgt->conf;
        }

        ptgt->pos.x = x + xpre;
        ptgt->pos.y = y + ypre;

#ifdef __DEBUG_TRACKER
        cvNamedWindow(ScaleName.c_str(), CV_WINDOW_AUTOSIZE); 
        cvShowImage(ScaleName.c_str(), image.IplImageRef(videoch));
        cvWaitKey(1);
#endif
        if (TemplateUpdateEnabled && !OverwriteTemplates) {
            // Update template temporarily based on updated position
            UpdateTemplate(image.GetUCharPointer(videoch),
                           OrigTemplates[i],
                           ptgt->feature_data.Pointer(),
                           ptgt->pos.x - TemplateRadius,
                           ptgt->pos.y - TemplateRadius);
       }
    }

    FrameCounter ++;

    memcpy(PreviousImage->GetUCharPointer(), image.GetUCharPointer(videoch), PreviousImage->GetDataSize());

    return SVL_OK;
}

int svlTrackerMSBruteForce::Track(svlProcInfo* procInfo, svlSampleImage & image, unsigned int videoch)
{
    if (!Initialized) return SVL_FAIL;

    const int leftborder   = ROI.left   + TemplateRadius;
    const int topborder    = ROI.top    + TemplateRadius;
    const int rightborder  = ROI.right  - TemplateRadius;
    const int bottomborder = ROI.bottom - TemplateRadius;
    const unsigned int targetcount = static_cast<unsigned int>(Targets.size());
    const unsigned int target_from = procInfo->ID;
    const unsigned int target_step = procInfo->count;
    const unsigned int scalem1 = Scale - 1;
    unsigned int templatesize = TemplateRadius * 2 + 1;
    templatesize *= templatesize * 3;

    svlTarget2D target, *ptgt;
    int xpre, ypre, x, y;
    unsigned char conf, *p_img;
    unsigned int i;


    if (FrameCounter > 0) p_img = PreviousImage->GetUCharPointer();
    else p_img = image.GetUCharPointer(videoch);

    for (i = target_from, ptgt = Targets.Pointer() + target_from;
         i < targetcount;
         i += target_step, ptgt += target_step) {

        if (!ptgt->used) {
            ptgt->visible = false;
            continue;
        }

        // Determine target visibility
        x = ptgt->pos.x;
        y = ptgt->pos.y;
        if (x < leftborder || x >= rightborder || y < topborder  || y >= bottomborder) {
            ptgt->visible = false;
            ptgt->conf    = 0;
            // Skip target if not visible
            continue;
        }
        ptgt->visible = true;

        if (TemplateUpdateEnabled) {
            // Check if this scale already has a template
            // Acquire target templates if necessary
            if (ptgt->feature_quality == -1) {
                ptgt->feature_data.SetSize(templatesize);

                // Update this scale's template with the
                // new position estimated by the tracker
                // filter
                CopyTemplate(p_img,
                             OrigTemplates[i],
                             ptgt->pos.x - TemplateRadius,
                             ptgt->pos.y - TemplateRadius);
                CopyTemplate(p_img,
                             ptgt->feature_data.Pointer(),
                             ptgt->pos.x - TemplateRadius,
                             ptgt->pos.y - TemplateRadius);

                ptgt->conf            = 255;
                ptgt->feature_quality = 256;
            }
            else if (OverwriteTemplates) {
                // Overwrite template based on updated position
                UpdateTemplate(p_img,
                               OrigTemplates[i],
                               ptgt->feature_data.Pointer(),
                               ptgt->pos.x - TemplateRadius,
                               ptgt->pos.y - TemplateRadius);
                memcpy(OrigTemplates[i], ptgt->feature_data.Pointer(), ptgt->feature_data.size());
            }
        }
    }


    if (LowerScale) {

        // Call lower scales recursively
        LowerScale->SetROI(ROI.left / 2, ROI.top / 2, ROI.right / 2, ROI.bottom / 2);
        LowerScale->Track(procInfo, *LowerScaleImage);

        // Scale up the tracking results from the
        // lower scale and use that as new position
        for (i = target_from, ptgt = Targets.Pointer() + target_from;
             i < targetcount;
             i += target_step, ptgt += target_step) {

            if (!ptgt->used) continue;

            // Scale up lower scale results only if this target has been initialized earlier
            if (ptgt->feature_quality >= 0) {

                LowerScale->GetTarget(i, target);

                // Scale up only valid lower scale results
                if (target.feature_quality >= 0 && target.visible) {
                    ptgt->conf    = target.conf;
                    ptgt->pos.x   = target.pos.x * 2 + 1;
                    ptgt->pos.y   = target.pos.y * 2 + 1;
                }
                else {
                    ptgt->visible = false;
                    ptgt->conf    = 0;
                }
            }
        }
    }


    // Track targets
    for (i = target_from, ptgt = Targets.Pointer() + target_from;
         i < targetcount;
         i += target_step, ptgt += target_step) {

        // Skip non-visible targets
        if (!ptgt->visible) continue;

        // Skip templates with poor quality
        if (ptgt->feature_quality < ConfidenceThreshold) {
            ptgt->visible = false;
            ptgt->conf    = 0;
            continue;
        }

        // template matching + updating coordinates
        xpre = ptgt->pos.x;
        ypre = ptgt->pos.y;

        if (Scale == 1) {
            if (Metric == svlSAD) {
                MatchTemplateSAD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, ptgt->conf, false);
            }
            else if (Metric == svlSSD) {
                MatchTemplateSSD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, ptgt->conf, false);
            }
            else if (Metric == svlNCC) {
                MatchTemplateNCC(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, ptgt->conf, true);
            }
            else return SVL_FAIL;
        }
        else {
            if (Metric == svlSAD) {
                MatchTemplateSAD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, conf, false);
            }
            else if (Metric == svlSSD) {
                MatchTemplateSSD(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, conf, false);
            }
            else if (Metric == svlNCC) {
                MatchTemplateNCC(image.GetUCharPointer(videoch), ptgt->feature_data.Pointer(), xpre, ypre);
                GetBestMatch(x, y, conf, true);
            }
            else return SVL_FAIL;

            if (ptgt->conf < ConfidenceThreshold) ptgt->conf = 0;
            ptgt->conf = (static_cast<int>(ptgt->conf) * scalem1 + conf) / Scale;
        }

        if (ptgt->feature_quality == 256) {
            // Determine the quality of the feature when it was first used
            ptgt->feature_quality = ptgt->conf;
        }

        ptgt->pos.x = x + xpre;
        ptgt->pos.y = y + ypre;

#ifdef __DEBUG_TRACKER
        cvNamedWindow(ScaleName.c_str(), CV_WINDOW_AUTOSIZE); 
        cvShowImage(ScaleName.c_str(), image.IplImageRef(videoch));
        cvWaitKey(1);
#endif

        if (TemplateUpdateEnabled && !OverwriteTemplates) {
            // Update template temporarily based on updated position
            UpdateTemplate(image.GetUCharPointer(videoch),
                           OrigTemplates[i],
                           ptgt->feature_data.Pointer(),
                           ptgt->pos.x - TemplateRadius,
                           ptgt->pos.y - TemplateRadius);
        }
    }

    ThreadCounter ++;
    if (ThreadCounter == procInfo->count) {
        // The last thread to finish stores the current image for later  use
        memcpy(PreviousImage->GetUCharPointer(), image.GetUCharPointer(videoch), PreviousImage->GetDataSize());

        ThreadCounter = 0;
        FrameCounter ++;
    }

    return SVL_OK;
}

void svlTrackerMSBruteForce::Release()
{
    unsigned int i;

    for (i = 0; i < OrigTemplates.size(); i ++) delete [] OrigTemplates[i];

    OrigTemplates.SetSize(0);

    if (LowerScale) {
        // deletes all the lower scales recursively
        delete LowerScale;
        LowerScale = 0;
    }
    if (LowerScaleImage) {
        delete LowerScaleImage;
        LowerScaleImage = 0;
    }
    if (PreviousImage) {
        delete PreviousImage;
        PreviousImage = 0;
    }

    Initialized = false;
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
    if (TemplateUpdateWeight == 0) {
        unsigned int tmplsize = TemplateRadius * 2 + 1;
        tmplsize *= tmplsize * 3;
        memcpy(tmp, origtmp, tmplsize);
        return;
    }

    if (TemplateUpdateWeight == 255) {
        CopyTemplate(img, tmp, left, top);
        return;
    }

    const unsigned int imstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmpwidth = tmpheight * 3;
    const unsigned int endstride = imstride - tmpwidth;
    const unsigned int origweight = 255 - TemplateUpdateWeight;
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

void svlTrackerMSBruteForce::MatchTemplateSAD(unsigned char* img, unsigned char* tmp, int x, int y)
{
    const unsigned int imgstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmppixcount = tmpheight * tmpheight;
    const unsigned int tmpstride = imgstride - tmpheight * 3;
    const unsigned int winsize = SearchRadius * 2 + 1;
    const unsigned int imgwinstride = imgstride - winsize * 3;
    const int imgwidth = static_cast<int>(Width);
    const int imgheight = static_cast<int>(Height);

    int k, l, sum, ival, hfrom, vfrom;
    int* map = MatchMap.Pointer();
    unsigned char *timg, *ttmp;
    unsigned int i, j, v, h;

    if (x < static_cast<int>(TemplateRadius)) x = TemplateRadius;
    else if (x >= static_cast<int>(Width - TemplateRadius)) x = Width - TemplateRadius - 1;
    if (y < static_cast<int>(TemplateRadius)) y = TemplateRadius;
    else if (y >= static_cast<int>(Height - TemplateRadius)) y = Height - TemplateRadius - 1;

    hfrom = x - TemplateRadius - SearchRadius;
    vfrom = y - TemplateRadius - SearchRadius;

    k = vfrom * imgstride + hfrom * 3;
    if (k > 0) img += k;
    else img -= -k;
    
    for (v = 0, l = vfrom; v < winsize; v ++, l ++) {
        if (l >= 0 && l < imgheight) {

            for (h = 0, k = hfrom; h < winsize; h ++, k ++) {
                if (k >= 0 && k < imgwidth) {

                    // match in current position
                    timg = img; ttmp = tmp;
                    sum = 0;
                    for (j = 0; j < tmpheight; j ++) {
                        for (i = 0; i < tmpheight; i ++) {
                            ival = (static_cast<int>(*timg) - *ttmp); timg ++; ttmp ++;
                            ival < 0 ? sum -= ival : sum += ival;
                            ival = (static_cast<int>(*timg) - *ttmp); timg ++; ttmp ++;
                            ival < 0 ? sum -= ival : sum += ival;
                            ival = (static_cast<int>(*timg) - *ttmp); timg ++; ttmp ++;
                            ival < 0 ? sum -= ival : sum += ival;
                        }
                        timg += tmpstride;
                    }
                    sum /= tmppixcount;

                    *map = sum + 1; map ++;

                    #ifdef __DEBUG_TRACKER
                        int __res = sum / 30;
                        if (__res > 255) __res = 255;
                        img[0] = img[1] = img[2] = __res;
                    #endif

                }
                else {

                    *map = 0; map ++;

                }

                img += 3;
            }
            img += imgwinstride;

        }
        else {

            memset(map, 0, winsize * sizeof(int)); map += winsize;
            img += imgstride;

        }
    }
}

void svlTrackerMSBruteForce::MatchTemplateSSD(unsigned char* img, unsigned char* tmp, int x, int y)
{
    const unsigned int imgstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmppixcount = tmpheight * tmpheight;
    const unsigned int tmpstride = imgstride - tmpheight * 3;
    const unsigned int winsize = SearchRadius * 2 + 1;
    const unsigned int imgwinstride = imgstride - winsize * 3;
    const int imgwidth = static_cast<int>(Width);
    const int imgheight = static_cast<int>(Height);

    int k, l, sum, ival, hfrom, vfrom;
    int* map = MatchMap.Pointer();
    unsigned char *timg, *ttmp;
    unsigned int i, j, v, h;

    if (x < static_cast<int>(TemplateRadius)) x = TemplateRadius;
    else if (x >= static_cast<int>(Width - TemplateRadius)) x = Width - TemplateRadius - 1;
    if (y < static_cast<int>(TemplateRadius)) y = TemplateRadius;
    else if (y >= static_cast<int>(Height - TemplateRadius)) y = Height - TemplateRadius - 1;

    hfrom = x - TemplateRadius - SearchRadius;
    vfrom = y - TemplateRadius - SearchRadius;

    k = vfrom * imgstride + hfrom * 3;
    if (k > 0) img += k;
    else img -= -k;
    
    for (v = 0, l = vfrom; v < winsize; v ++, l ++) {
        if (l >= 0 && l < imgheight) {

            for (h = 0, k = hfrom; h < winsize; h ++, k ++) {
                if (k >= 0 && k < imgwidth) {

                    // match in current position
                    timg = img; ttmp = tmp;
                    sum = 0;
                    for (j = 0; j < tmpheight; j ++) {
                        for (i = 0; i < tmpheight; i ++) {
                            ival = (static_cast<int>(*timg) - *ttmp); timg ++; ttmp ++;
                            sum += ival * ival;
                            ival = (static_cast<int>(*timg) - *ttmp); timg ++; ttmp ++;
                            sum += ival * ival;
                            ival = (static_cast<int>(*timg) - *ttmp); timg ++; ttmp ++;
                            sum += ival * ival;
                        }
                        timg += tmpstride;
                    }
                    sum /= tmppixcount;

                    *map = sum + 1; map ++;

                    #ifdef __DEBUG_TRACKER
                        int __res = sqrt_uint32(sum / 3) * 10;
                        if (__res > 255) __res = 255;
                        img[0] = img[1] = img[2] = __res;
                    #endif
                    
                }
                else {

                    *map = 0; map ++;

                }

                img += 3;
            }
            img += imgwinstride;

        }
        else {

            memset(map, 0, winsize * sizeof(int)); map += winsize;
            img += imgstride;

        }
    }
}

void svlTrackerMSBruteForce::MatchTemplateNCC(unsigned char* img, unsigned char* tmp, int x, int y)
{
    const unsigned int imgstride = Width * 3;
    const unsigned int tmpheight = TemplateRadius * 2 + 1;
    const unsigned int tmpwidth = tmpheight * 3;
    const unsigned int winsize = SearchRadius * 2 + 1;
    const unsigned int imgwinstride = imgstride - winsize * 3;
    const int imgwidth_m1 = static_cast<int>(Width) - 1;
    const int imgheight_m1 = static_cast<int>(Height) - 1;
    const int tmpheight_m1 = tmpheight - 1;

    int i, j, k, l, sum, hfrom, vfrom;
    int tmpxfrom, tmpxto, tmpyfrom, tmpyto;
    int tmpstride, tmprowcount, tmpcolcount, tmppixcount;
    int xoffs, yoffs, ioffs;
    int mi1, mi2, mi3, mt1, mt2, mt3;
    int di1, di2, di3, dt1, dt2, dt3;
    int di, dt, cr1, cr2, cr3;
    int* map = MatchMap.Pointer();
    unsigned char *timg, *ttmp;
    unsigned int v, h;

    hfrom = x - TemplateRadius - SearchRadius;
    vfrom = y - TemplateRadius - SearchRadius;

    k = vfrom * imgstride + hfrom * 3;
    if (k > 0) img += k;
    else img -= -k;

    tmpxfrom = tmpyfrom = 0;
    tmpxto = tmpyto = tmpheight;
    tmppixcount = tmpheight * tmpheight;

    // Compute template means 
    ttmp = tmp; mt1 = mt2 = mt3 = 0;
    for (j = tmpyfrom; j < tmpyto; j ++) {
        for (i = tmpxfrom; i < tmpxto; i ++) {
            mt1 += *ttmp; ttmp ++;
            mt2 += *ttmp; ttmp ++;
            mt3 += *ttmp; ttmp ++;
        }
    }
    mt1 /= tmppixcount; mt2 /= tmppixcount; mt3 /= tmppixcount;

    // Compute template standard deviations
    ttmp = tmp; dt1 = dt2 = dt3 = 0;
    for (j = tmpyfrom; j < tmpyto; j ++) {
        for (i = tmpxfrom; i < tmpxto; i ++) {
            dt = static_cast<int>(*ttmp) - mt1; dt1 += dt * dt; ttmp ++;
            dt = static_cast<int>(*ttmp) - mt2; dt2 += dt * dt; ttmp ++;
            dt = static_cast<int>(*ttmp) - mt3; dt3 += dt * dt; ttmp ++;
        }
    }
    dt1 = sqrt_uint32(dt1); dt2 = sqrt_uint32(dt2); dt3 = sqrt_uint32(dt3);
    if (dt1 == 0) dt1 = 1; if (dt2 == 0) dt2 = 1; if (dt3 == 0) dt3 = 1;

    for (v = 0, l = vfrom; v < winsize; v ++, l ++) {

        tmprowcount = 0;

        tmpyfrom = l;
        if (tmpyfrom <= imgheight_m1) {
            yoffs = 0;
            if (tmpyfrom < 0) {
                tmpyfrom = 0;
                yoffs = -tmpyfrom;
            }
            tmpyto = l + tmpheight_m1;
            if (tmpyto >= 0) {
                if (tmpyto > imgheight_m1) {
                    tmpyto = imgheight_m1;
                }
                tmprowcount = tmpyto - tmpyfrom + 1;
            }
        }

        if (tmprowcount > 0) {

            for (h = 0, k = hfrom; h < winsize; h ++, k ++) {

                tmpcolcount = 0;

                tmpxfrom = l;
                if (tmpxfrom <= imgwidth_m1) {
                    xoffs = 0;
                    if (tmpxfrom < 0) {
                        tmpxfrom = 0;
                        xoffs = -tmpxfrom;
                    }
                    tmpxto = l + tmpheight_m1;
                    if (tmpxto >= 0) {
                        if (tmpxto > imgwidth_m1) {
                            tmpxto = imgwidth_m1;
                        }
                        tmpcolcount = tmpxto - tmpxfrom + 1;
                    }
                }

                if (tmpcolcount > 0) {

                    tmpstride = imgstride - tmpcolcount * 3;
                    tmppixcount = tmprowcount * tmpcolcount;

                    xoffs *= 3;
                    ioffs = yoffs * imgstride + xoffs;

                    // Compute image means
                    timg = img + ioffs;
                    mi1 = mi2 = mi3 = 0;
                    for (j = tmpyfrom; j <= tmpyto; j ++) {
                        for (i = tmpxfrom; i <= tmpxto; i ++) {
                            mi1 += *timg; timg ++;
                            mi2 += *timg; timg ++;
                            mi3 += *timg; timg ++;
                        }
                        timg += tmpstride;
                    }
                    mi1 /= tmppixcount; mi2 /= tmppixcount; mi3 /= tmppixcount;

                    // Compute image standard deviations and correlations
                    timg = img + ioffs;
                    ttmp = tmp + yoffs * tmpwidth + xoffs;
                    cr1 = cr2 = cr3 = 0;
                    di1 = di2 = di3 = 0;
                    for (j = tmpyfrom; j <= tmpyto; j ++) {
                        for (i = tmpxfrom; i <= tmpxto; i ++) {
                            di = static_cast<int>(*timg) - mi1; di1 += di * di; timg ++;
                            dt = static_cast<int>(*ttmp) - mt1;                 ttmp ++;
                            cr1 += di * dt;
                            di = static_cast<int>(*timg) - mi2; di2 += di * di; timg ++;
                            dt = static_cast<int>(*ttmp) - mt2;                 ttmp ++;
                            cr2 += di * dt;
                            di = static_cast<int>(*timg) - mi3; di3 += di * di; timg ++;
                            dt = static_cast<int>(*ttmp) - mt3;                 ttmp ++;
                            cr3 += di * dt;
                        }
                        timg += tmpstride;
                    }
                    di1 = sqrt_uint32(di1); di2 = sqrt_uint32(di2); di3 = sqrt_uint32(di3);

                    if (di1 != 0) sum  = (cr1 << 8) / (di1 * dt1); else sum  = (cr1 << 8);
                    if (di2 != 0) sum += (cr2 << 8) / (di2 * dt2); else sum += (cr2 << 8);
                    if (di3 != 0) sum += (cr3 << 8) / (di3 * dt3); else sum += (cr3 << 8);

                    *map = sum + 1; map ++;

                    #ifdef __DEBUG_TRACKER
                        int __res = sum / 10 + 128;
                        if (__res > 255) __res = 255;
                        img[0] = img[1] = img[2] = __res;
                    #endif
                    
                }
                else {

                    *map = 0; map ++;

                }

                img += 3;
            }
            img += imgwinstride;

        }
        else {

            memset(map, 0, winsize * sizeof(int)); map += winsize;
            img += imgstride;

        }
    }
}

void svlTrackerMSBruteForce::GetBestMatch(int &x, int &y, unsigned char &conf, bool higherbetter)
{
    const int size = SearchRadius * 2 + 1;
    const int size2 = size * size;
    int i, j, t, avrg, best, best_x = 0, best_y = 0;
    int* map = MatchMap.Pointer();

    // Compute average match and best match
    avrg = 0;
    if (higherbetter) {
        best = 0x80000000;
        for (j = 0; j < size; j ++) {
            for (i = 0; i < size; i ++) {
                t = *map; map ++;

                if (t > best) {
                    best = t;
                    best_x = i;
                    best_y = j;
                }

                avrg += t;
            }
        }
    }
    else {
        best = 0x7FFFFFFF;
        for (j = 0; j < size; j ++) {
            for (i = 0; i < size; i ++) {
                t = *map; map ++;

                if (t < best) {
                    best = t;
                    best_x = i;
                    best_y = j;
                }

                avrg += t;
            }
        }
    }
    avrg /= size2;
    x = best_x - SearchRadius;
    y = best_y - SearchRadius;

    if (Metric == svlSAD) avrg <<= 6;
    else if (Metric == svlSSD) best <<= 3;
    else if (Metric == svlNCC) best <<= 6;

    if (higherbetter) {
        if (avrg > 0) best = best / avrg;
        else best = 0;
        if (best < 0) best = 0;
        else if (best > 255) best = 255;
        conf = static_cast<unsigned char>(best);
    }
    else {
        if (best > 0) best = (avrg << 8) / best;
        if (best < 0) best = 0;
        else if (best > 255) best = 255;
        conf = static_cast<unsigned char>(best);
    }
}

void svlTrackerMSBruteForce::ShrinkImage(unsigned char* src, unsigned char* dst)
{
    const int smwidth  = Width >> 1;
    const int smheight = Height >> 1;

    // make sure ROI values are even and in range
    int wr = SearchRadius;
    int smleft   = (ROI.left - wr) / 2;
    int smtop    = (ROI.top - wr)  / 2;
    int smright  = ((ROI.right + wr)  + 1) / 2;
    int smbottom = ((ROI.bottom + wr) + 1) / 2;
    if (smleft < 0) smleft = 0;
    if (smtop  < 0) smtop = 0;
    if (smright >= smwidth) smright = smwidth - 1;
    if (smbottom >= smheight) smbottom = smheight - 1;

    const int lgleft   = smleft << 1;
    const int lgtop    = smtop  << 1;
    const unsigned int lgstride = Width * 3;
    const unsigned int lgstride2 = lgstride << 1;
    const unsigned int smw = smright - smleft;
    const unsigned int smh = smbottom - smtop;
    const unsigned int smstride = lgstride >> 1;
    const unsigned int smstride2 = smstride - smw * 3;

    unsigned char *srcln1, *srcln2, *src1, *src2;
    unsigned int i, j, r, g, b;

    src += lgtop * lgstride + lgleft * 3;
    dst += smtop * smstride + smleft * 3;

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
        dst    += smstride2;
    }
}

