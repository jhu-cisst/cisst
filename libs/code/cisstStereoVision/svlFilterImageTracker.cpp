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

#include <cisstStereoVision/svlFilterImageTracker.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlTrackerMSBruteForce.h>

#include <math.h>

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

#define __PI    3.1415926535898
#define __2PI   6.2831853071795


/******************************************/
/*** svlFilterImageTracker class **********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageTracker)

svlFilterImageTracker::svlFilterImageTracker() :
    svlFilterBase(),
    VideoChannels(0),
    RigidBody(false),
    ResetFlag(false),
    FramesToSkip(0),
    MovingAverageWeight(0.0),
    RigidBodyAngleLow(- __PI),
    RigidBodyAngleHigh(__PI),
    RigidBodyScaleLow(0.01),
    RigidBodyScaleHigh(100.0),
    Iterations(1),
    WarpedImage(0),
    Mosaic(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddInput("targets", false);
    AddInputType("targets", svlTypeTargets);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    AddOutput("targets", false);
    SetOutputType("targets", svlTypeTargets);

    AddOutput("warpedimage", false);
    SetOutputType("warpedimage", svlTypeImageRGB);

    AddOutput("mosaicimage", false);
    SetOutputType("mosaicimage", svlTypeImageRGB);

    Trackers.SetSize(SVL_MAX_CHANNELS);
    Trackers.SetAll(0);

    ROI.SetSize(SVL_MAX_CHANNELS);
    ROI.SetAll(svlRect(0, 0, 4096, 4096));
    ROICenter.SetSize(SVL_MAX_CHANNELS);
}

svlFilterImageTracker::~svlFilterImageTracker()
{
    Release();
}

int svlFilterImageTracker::SetTracker(svlImageTracker & tracker, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;
    if (IsInitialized()) return SVL_FAIL;

    Trackers[videoch] = &tracker;

    return SVL_OK;
}

void svlFilterImageTracker::SetMovingAverageSmoothing(double weight)
{
    if (weight < 0.0) MovingAverageWeight = 0.0;
    else MovingAverageWeight = weight;
}

void svlFilterImageTracker::SetFrameSkip(unsigned int skipcount)
{
    FramesToSkip = static_cast<int>(skipcount);
}

void svlFilterImageTracker::SetIterations(unsigned int count)
{
    if (count < 1) count = 1;
    if (count > 100) count = 100;
    Iterations = count;
}

void svlFilterImageTracker::ResetTargets()
{
    ResetFlag = true;
}

int svlFilterImageTracker::SetRigidBody(bool enable)
{
    if (IsInitialized()) return SVL_FAIL;
    RigidBody = enable;
    return SVL_OK;
}

void svlFilterImageTracker::SetRigidBodyConstraints(double angle_low, double angle_high, double scale_low, double scale_high)
{
    RigidBodyAngleLow = angle_low;
    RigidBodyAngleHigh = angle_high;
    RigidBodyScaleLow = scale_low;
    RigidBodyScaleHigh = scale_high;
}

int svlFilterImageTracker::SetROI(const svlRect & rect, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    ROI[videoch] = rect;

    return SVL_OK;
}

int svlFilterImageTracker::SetROI(int left, int top, int right, int bottom, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    ROI[videoch].left   = left;
    ROI[videoch].top    = top;
    ROI[videoch].right  = right;
    ROI[videoch].bottom = bottom;

    return SVL_OK;
}

int svlFilterImageTracker::GetROI(svlRect & rect, unsigned int videoch) const
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    rect = ROI[videoch];

    return SVL_OK;
}

int svlFilterImageTracker::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    VideoChannels = dynamic_cast<svlSampleImage*>(syncInput)->GetVideoChannels();

    // Initializing frame-skip counter
    FrameCount = 0;

    if (RigidBody) {
        RigidBodyAngle.SetSize(VideoChannels);
        RigidBodyAngle.SetAll(0.0);
        RigidBodyScale.SetSize(VideoChannels);
        RigidBodyScale.SetAll(1.0);

        WarpedRigidBodyAngle.SetSize(VideoChannels);
        WarpedRigidBodyAngle.SetAll(0.0);
        WarpedRigidBodyScale.SetSize(VideoChannels);
        WarpedRigidBodyScale.SetAll(1.0);
        WarpInternals.SetSize(VideoChannels * 2);

        WarpedImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
        if (!WarpedImage) return SVL_FAIL;
        WarpedImage->SetSize(syncInput);
        WarpedImage->SetTimestamp(syncInput->GetTimestamp());
        GetOutput("warpedimage")->SetupSample(WarpedImage);

        Mosaic = new svlSampleImageRGB;
        if (!Mosaic) return SVL_FAIL;
        Mosaic->SetSize(1600, 1600);
        Mosaic->SetTimestamp(syncInput->GetTimestamp());
        GetOutput("mosaicimage")->SetupSample(Mosaic);
    }

    syncOutput = syncInput;
    GetOutput("targets")->SetupSample(&OutputTargets);

    return SVL_OK;
}

int svlFilterImageTracker::OnStart(unsigned int procCount)
{
    if (RigidBody && procCount >= WarpInternals.size()) WarpingParallel = true;
    else WarpingParallel = false;

    return SVL_OK;
}

int svlFilterImageTracker::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfDisabled();

    // Skipping frames (if requested)
    if (FrameCount < FramesToSkip) {
        _SynchronizeThreads(procInfo);
        _OnSingleThread(procInfo) FrameCount ++;
        return SVL_OK;
    }

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    svlTarget2D *target_buffer;
    vctDynamicVectorRef<int> flag;
    vctDynamicVectorRef<int> confidence;
    vctDynamicMatrixRef<int> position;

    const int weight = static_cast<int>(1000.0 * MovingAverageWeight);
    const int weightsum = weight + 1000;
    unsigned int vch, i, targetcount;

    // Resetting positions (if requested)
    _OnSingleThread(procInfo)
    {
        svlSampleTargets* target_input = dynamic_cast<svlSampleTargets*>(GetInput("targets")->PullSample(true, 0.0));
        if (target_input || ResetFlag) {

            if (target_input) {
                // Re-allocate work target buffer
                targetcount = target_input->GetMaxTargets();
                Targets.SetSize(VideoChannels, targetcount);
            }

            for (vch = 0; vch < VideoChannels; vch ++) {

                if (!Trackers[vch]) continue;

                if (!Trackers[vch]->IsInitialized()) {
                    Trackers[vch]->SetImageSize(img->GetWidth(vch), img->GetHeight(vch));
                    Trackers[vch]->SetTargetCount(target_input->GetMaxTargets());
                    Trackers[vch]->Initialize();
                }
                else {
                    Trackers[vch]->ResetTargets();
                }

                // Copy the input targets into the current output buffer
                if (target_input) InitialTargets.CopyOf(*target_input);
                OutputTargets.CopyOf(InitialTargets);

                flag.SetRef(targetcount, OutputTargets.GetFlagPointer());
                confidence.SetRef(targetcount, OutputTargets.GetConfidencePointer(vch));
                position.SetRef(2, targetcount, OutputTargets.GetPositionPointer(vch));

                target_buffer = Targets.Pointer(vch, 0);

                for (i = 0; i < targetcount; i ++) {

                    // Storing temporary results until tracking starts
                    target_buffer->used            = flag.Element(i) ? true : false;
                    target_buffer->visible         = false;
                    target_buffer->conf            = confidence.Element(i);
                    target_buffer->pos.x           = position.Element(0, i);
                    target_buffer->pos.y           = position.Element(1, i);
                    target_buffer->feature_quality = -1;

                    // Updating targets in trackers
                    Trackers[vch]->SetTarget(i, *target_buffer);

                    target_buffer ++;
                }
            }

            ResetFlag = false;
        }
    }

    _ParallelLoop(procInfo, vch, VideoChannels)
    {
        // Region of interest might change at run-time
        if (Trackers[vch]) Trackers[vch]->SetROI(ROI[vch]);
    }

    if (WarpingParallel) {

        _SynchronizeThreads(procInfo);

        vch = procInfo->id >> 1;
        if (vch < VideoChannels && Trackers[vch]) {
            WarpImage(img, vch, procInfo->id);
        }
    }

    _SynchronizeThreads(procInfo);

    targetcount = OutputTargets.GetMaxTargets();
    if (targetcount < 1) return SVL_OK;

    _ParallelLoop(procInfo, vch, VideoChannels)
    {
        if (!Trackers[vch]) continue;

        if (RigidBody) {
            if (!WarpingParallel) WarpImage(img, vch);
            img = WarpedImage;
        }

        // Processing data
        Trackers[vch]->PreProcessImage(*img, vch);
        for (i = 0; i < Iterations; i ++) {
            Trackers[vch]->Track(*img, vch);
        }

        target_buffer = Targets.Pointer(vch, 0);

        // Retrieving results
        for (i = 0; i < targetcount; i ++) {

            if (target_buffer->used) Trackers[vch]->GetTarget(i, *target_buffer);
            else target_buffer->visible = false;

            target_buffer ++;
        }

        if (RigidBody) {
            UpdateMosaicImage(vch, img->GetWidth(vch), img->GetHeight(vch), procInfo->id);
        }
    }

    _SynchronizeThreads(procInfo);

    if (RigidBody) {
        _OnSingleThread(procInfo)
        {
            ReconstructRigidBody();
        }
    }

    _OnSingleThread(procInfo)
    {
        for (vch = 0; vch < VideoChannels; vch ++) {

            if (!Trackers[vch]) continue;

            target_buffer = Targets.Pointer(vch, 0);
            flag.SetRef(targetcount, OutputTargets.GetFlagPointer());
            confidence.SetRef(targetcount, OutputTargets.GetConfidencePointer(vch));
            position.SetRef(2, targetcount, OutputTargets.GetPositionPointer(vch));

            // Storing results
            for (i = 0; i < targetcount; i ++) {

                flag.Element(i)        = target_buffer->used;
                confidence.Element(i)  = target_buffer->conf;
                position.Element(0, i) = (target_buffer->pos.x * 1000 + position.Element(0, i) * weight) / weightsum;
                position.Element(1, i) = (target_buffer->pos.y * 1000 + position.Element(1, i) * weight) / weightsum;

                target_buffer ++;
            }
        }

        // Pushing samples to async outputs
        svlFilterOutput* output;
        double ts = syncInput->GetTimestamp();

        output = GetOutput("targets");
        if (output) {
            OutputTargets.SetTimestamp(ts);
            output->PushSample(&OutputTargets);
        }
        if (RigidBody) {
            output = GetOutput("warpedimage");
            if (output) {
                WarpedImage->SetTimestamp(ts);
                output->PushSample(WarpedImage);
            }
            output = GetOutput("mosaicimage");
            if (output) {
                Mosaic->SetTimestamp(ts);
                output->PushSample(Mosaic);
            }
        }
    }

    return SVL_OK;
}

int svlFilterImageTracker::Release()
{
    for (unsigned int i = 0; i < Trackers.size(); i ++) {
        if (Trackers[i]) Trackers[i]->Release();
    }
    if (WarpedImage) {
        delete WarpedImage;
        WarpedImage = 0;
    }
    if (Mosaic) {
        delete Mosaic;
        Mosaic = 0;
    }

    RigidBodyAngle.SetSize(0);
    RigidBodyScale.SetSize(0);
    WarpedRigidBodyAngle.SetSize(0);
    WarpedRigidBodyScale.SetSize(0);
    WarpInternals.SetSize(0);

    return SVL_OK;
}

void svlFilterImageTracker::ReconstructRigidBody()
{
    const unsigned int targetcount = Targets.cols();
    double dconf, sum_conf, ax, ay, rx, ry, vx, vy, angle, cos_an, sin_an, scale;
    double proto_ax, proto_ay, proto_vx, proto_vy, proto_dist;
    vctDynamicMatrixRef<int> proto_pos;
    svlTarget2D *target;

    unsigned int i, j;
    int conf;

    for (j = 0; j < VideoChannels; j ++) {

        if (!Trackers[j]) continue;

        proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(j));

        ax = ay = proto_ax = proto_ay = cos_an = sin_an = scale = sum_conf = 0.0;

        // Compute center of weight
        for (i = 0; i < targetcount; i ++) {
            target = Targets.Pointer(j, i);

            if (target->visible) {

                conf      = target->conf;
                ax       += target->pos.x           * conf;
                ay       += target->pos.y           * conf;
                proto_ax += proto_pos.Element(0, i) * conf;
                proto_ay += proto_pos.Element(1, i) * conf;
                sum_conf += conf;
            }
        }
        if (sum_conf < 0.0001) return;

        ax       /= sum_conf;
        ay       /= sum_conf;
        proto_ax /= sum_conf;
        proto_ay /= sum_conf;

        // Compute angle and scale
        sum_conf = 0.0;

        for (i = 0; i < targetcount; i ++) {
            target = Targets.Pointer(j, i);

            if (target->visible) {

                dconf      = target->conf;
                vx         = static_cast<double>(target->pos.x) - ax;
                vy         = static_cast<double>(target->pos.y) - ay;
                proto_vx   = static_cast<double>(proto_pos.Element(0, i)) - proto_ax;
                proto_vy   = static_cast<double>(proto_pos.Element(1, i)) - proto_ay;
                proto_dist = sqrt(proto_vx * proto_vx + proto_vy * proto_vy);

                if (proto_dist >= 0.0001) {
                    scale += dconf * sqrt(vx * vx + vy * vy);
                    sum_conf += dconf * proto_dist;

                    // Sum-up angle
                    dconf  *= proto_dist;
                    angle   = atan2(vy, vx) - atan2(proto_vy, proto_vx);
                    cos_an += dconf * cos(angle);
                    sin_an += dconf * sin(angle);
                }
            }
        }
        if (sum_conf < 0.0001) return;

        RigidBodyScale[j] = scale / sum_conf;
        RigidBodyAngle[j] = atan2(sin_an, cos_an);

        // Checking rigid body transformation constraints
        if (RigidBodyScale[j] < RigidBodyScaleLow) RigidBodyScale[j] = RigidBodyScaleLow;
        else if (RigidBodyScale[j] > RigidBodyScaleHigh) RigidBodyScale[j] = RigidBodyScaleHigh;
        if (RigidBodyAngle[j] < RigidBodyAngleLow) RigidBodyAngle[j] = RigidBodyAngleLow;
        else if (RigidBodyAngle[j] >RigidBodyAngleHigh) RigidBodyAngle[j] = RigidBodyAngleHigh;

        // Reconstruct rigid body based on prototype
        WarpedRigidBodyAngle[j] -= RigidBodyAngle[j];
        WarpedRigidBodyScale[j] /= RigidBodyScale[j];

        scale = 1.0 / WarpedRigidBodyScale[j];
        angle = -WarpedRigidBodyAngle[j];
        cos_an = cos(angle);
        sin_an = sin(angle);
        rx = ROICenter[j].X();
        ry = ROICenter[j].Y();

        for (i = 0; i < targetcount; i ++) {
            target = Targets.Pointer(j, i);

            if (target->used) {
                vx = proto_pos.Element(0, i) - proto_ax;
                vy = proto_pos.Element(1, i) - proto_ay;

                target->pos.x = static_cast<int>(vx + ax);
                target->pos.y = static_cast<int>(vy + ay);

                if (Trackers[j]) Trackers[j]->SetTarget(i, *target);

                vx += ax - rx;
                vy += ay - ry;

                target->pos.x = static_cast<int>(rx + scale * (vx * cos_an - vy * sin_an));
                target->pos.y = static_cast<int>(ry + scale * (vx * sin_an + vy * cos_an));
            }
        }
    }
}

void svlFilterImageTracker::WarpImage(svlSampleImage* image, unsigned int videoch, int threadid)
{
    double c = cos(WarpedRigidBodyAngle[videoch]);
    double s = sin(WarpedRigidBodyAngle[videoch]);
    double sc = WarpedRigidBodyScale[videoch];

    const int border = 50;

    const int x1 = std::max(0, ROI[videoch].left - border);
    const int y1 = std::max(0, ROI[videoch].top - border);
    const int x2 = std::min(static_cast<int>(image->GetWidth(videoch) - 1), ROI[videoch].right + border);
    const int y2 = y1;
    const int x3 = x2;
    const int y3 = std::min(static_cast<int>(image->GetHeight(videoch) - 1), ROI[videoch].bottom + border);
    const int x4 = x1;
    const int y4 = y3;

    const int cx = (x1 + x2 + x3 + x4) / 4;
    const int cy = (y1 + y2 + y3 + y4) / 4;

    ROICenter[videoch].X() = cx;
    ROICenter[videoch].Y() = cy;

    const int wx1 = static_cast<int>(sc * (c * (x1 - cx) - s * (y1 - cy))) + cx;
    const int wy1 = static_cast<int>(sc * (s * (x1 - cx) + c * (y1 - cy))) + cy;
    const int wx2 = static_cast<int>(sc * (c * (x2 - cx) - s * (y2 - cy))) + cx;
    const int wy2 = static_cast<int>(sc * (s * (x2 - cx) + c * (y2 - cy))) + cy;
    const int wx3 = static_cast<int>(sc * (c * (x3 - cx) - s * (y3 - cy))) + cx;
    const int wy3 = static_cast<int>(sc * (s * (x3 - cx) + c * (y3 - cy))) + cy;
    const int wx4 = static_cast<int>(sc * (c * (x4 - cx) - s * (y4 - cy))) + cx;
    const int wy4 = static_cast<int>(sc * (s * (x4 - cx) + c * (y4 - cy))) + cy;

    svlTriangle tri_in, tri_out;

    if (threadid >= 0) {
        if (threadid & 1) {
            tri_in.Assign(x1, y1, x3, y3, x4, y4);
            tri_out.Assign(wx1, wy1, wx3, wy3, wx4, wy4);
        }
        else {
            tri_in.Assign(x1, y1, x2, y2, x3, y3);
            tri_out.Assign(wx1, wy1, wx2, wy2, wx3, wy3);
        }
        svlDraw::WarpTriangle(image, videoch, tri_in, WarpedImage, videoch, tri_out, WarpInternals[threadid]);
    }
    else {
        tri_in.Assign(x1, y1, x3, y3, x4, y4);
        tri_out.Assign(wx1, wy1, wx3, wy3, wx4, wy4);
        svlDraw::WarpTriangle(image, videoch, tri_in, WarpedImage, videoch, tri_out, WarpInternals[0]);
        tri_in.Assign(x1, y1, x2, y2, x3, y3);
        tri_out.Assign(wx1, wy1, wx2, wy2, wx3, wy3);
        svlDraw::WarpTriangle(image, videoch, tri_in, WarpedImage, videoch, tri_out, WarpInternals[0]);
    }
}

int svlFilterImageTracker::UpdateMosaicImage(unsigned int videoch, unsigned int width, unsigned int height, int threadid)
{
    svlTrackerMSBruteForce* tracker = dynamic_cast<svlTrackerMSBruteForce*>(Trackers[videoch]);
    if (!tracker) return SVL_FAIL;

    const int mosaic_width  = Mosaic->GetWidth(videoch);
    const int mosaic_height = Mosaic->GetHeight(videoch);
    const int mosaic_center_x = mosaic_width  >> 1;
    const int mosaic_center_y = mosaic_height >> 1;
    const int mosaic_stride = mosaic_width * 3;
    const int image_center_x = width  >> 1;
    const int image_center_y = height >> 1;
    const int targetcount = Targets.size();
    const int tmpl_radius = tracker->GetTemplateRadius();
    const int tmpl_size = tmpl_radius * 2 + 1;
    const int tmpl_stride = tmpl_size * 3;
    unsigned char *mosaic_data = Mosaic->GetUCharPointer(videoch);

    svlTarget2D *target = 0;
    unsigned char *tdata, *mdata;
    int i, k, txf, tyf, mxf, mxt, myf, myt, w, h;
    vctDynamicVectorRef<unsigned char> template_ref;
    unsigned int j;
    vctInt2 pos;


    memset(mosaic_data, 0, Mosaic->GetDataSize(videoch));

    for (j = 0; j < VideoChannels; j ++) {
        for (i = 0; i < targetcount; i ++) {
            target = Targets.Pointer(j, i);

            if (!target->used || target->feature_quality < 0) continue;

            tracker->GetFeatureRef(i, template_ref);

            InitialTargets.GetPosition(i, pos, j);

            mxf = mosaic_center_x - image_center_x + pos.X() - tmpl_radius;
            mxt = mxf + tmpl_size;
            if (mxf < 0) {
                txf = -mxf;
                mxf = 0;
            }
            else {
                txf = 0;
            }
            if (mxt > mosaic_width) mxt = mosaic_width;

            myf = mosaic_center_y - image_center_y + pos.Y() - tmpl_radius;
            myt = myf + tmpl_size;
            if (myf < 0) {
                tyf = -myf;
                myf = 0;
            }
            else {
                tyf = 0;
            }
            if (myt > mosaic_height) myt = mosaic_height;

            w = (mxt - mxf) * 3;
            h = myt - myf;
            if (w <= 0 || h <= 0) continue;

            tdata = template_ref.Pointer() + (tyf * tmpl_size + txf) * 3;
            mdata = mosaic_data + (myf * mosaic_width + mxf) * 3;

            for (k = 0; k < h; k ++) {
                memcpy(mdata, tdata, w);
                tdata += tmpl_stride;
                mdata += mosaic_stride;
            }
        }
    }
    
    return SVL_OK;
}


/**********************************/
/*** svlImageTracker class ********/
/**********************************/

svlImageTracker::svlImageTracker() :
    Initialized(false),
    Width(0),
    Height(0)
{
}

bool svlImageTracker::IsInitialized()
{
    return Initialized;
}

int svlImageTracker::SetImageSize(unsigned int width, unsigned int height)
{
    if (Initialized) return SVL_FAIL;

    Width = width;
    Height = height;

    return SVL_OK;
}

void svlImageTracker::SetROI(const svlRect & rect)
{
    ROI = rect;
    ROI.Normalize();
    ROI.Trim(0, Width, 0, Height);
}

void svlImageTracker::SetROI(int left, int top, int right, int bottom)
{
    ROI.left   = left;
    ROI.top    = top;
    ROI.right  = right;
    ROI.bottom = bottom;
    ROI.Normalize();
    ROI.Trim(0, Width, 0, Height);
}

int svlImageTracker::SetTargetCount(unsigned int targetcount)
{
    if (Initialized) return SVL_FAIL;

    Targets.SetSize(targetcount);
    svlTarget2D target;
    target.used            = false;
    target.visible         = false;
    target.conf            = 0;
    target.pos.x           = 0;
    target.pos.y           = 0;
    target.feature_quality = -1;
    Targets.SetAll(target);

    return SVL_OK;
}

int svlImageTracker::SetTarget(unsigned int targetid, const svlTarget2D & target)
{
    if (targetid >= Targets.size()) return SVL_FAIL;

    svlTarget2D & mytarget = Targets[targetid];
    mytarget.used            = target.used;
    mytarget.visible         = target.visible;
    mytarget.conf            = target.conf;
    mytarget.pos             = target.pos;

    return SVL_OK;
}

int svlImageTracker::GetTarget(unsigned int targetid, svlTarget2D & target)
{
    if (targetid >= Targets.size()) return SVL_FAIL;

    svlTarget2D & mytarget = Targets[targetid];
    target.used            = mytarget.used;
    target.visible         = mytarget.visible;
    target.conf            = mytarget.conf;
    target.pos             = mytarget.pos;
    target.feature_quality = mytarget.feature_quality;

    return SVL_OK;
}

int svlImageTracker::Initialize()
{
    return SVL_OK;
}

void svlImageTracker::ResetTargets()
{
}

int svlImageTracker::PreProcessImage(svlSampleImage & CMN_UNUSED(image), unsigned int CMN_UNUSED(videoch))
{
    return SVL_OK;
}

void svlImageTracker::Release()
{
}

