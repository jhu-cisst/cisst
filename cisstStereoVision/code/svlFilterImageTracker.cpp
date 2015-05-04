/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

//#include <cisstNumerical/nmrSVD.h>

#include <math.h>

#define __PI    3.1415926535898
#define __2PI   6.2831853071795


/***********************************/
/*** svlFilterImageTracker class ***/
/***********************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageTracker, svlFilterBase)

svlFilterImageTracker::svlFilterImageTracker() :
    svlFilterBase(),
    VideoChannels(0),
    RigidBody(false),
    RigidBodyInitialized(false),
    ResetFlag(false),
    FramesToSkip(0),
    TargetTrajectorySmoothingWeight(0.0),
    RigidBodyTransformSmoothingWeight(0.0),
    RigidBodyAngleLow(- __PI),
    RigidBodyAngleHigh(__PI),
    RigidBodyScaleLow(0.01),
    RigidBodyScaleHigh(100.0),
    Iterations(1),
    WarpedImage(0),
    RigidBodyIterations(1),
    Mosaic(0),
    MosaicWidth(1000),
    MosaicHeight(1000)
{
    AddInput("input",   true);
    AddInput("targets", false);

    AddInputType("input",   svlTypeImageRGB);
    AddInputType("input",   svlTypeImageRGBStereo);
    AddInputType("targets", svlTypeTargets);

    AddOutput("output",      true);
    AddOutput("warpedimage", false);
    AddOutput("mosaicimage", false);
    AddOutput("targets",     false);

    SetOutputType("targets", svlTypeTargets);

    Trackers.SetSize(SVL_MAX_CHANNELS);
    Trackers.SetAll(0);

    RigidBodyError.SetSize(RigidBodyIterations);
    RigidBodyError.SetAll(-1);

    ROIRect.SetSize(SVL_MAX_CHANNELS);
    ROIRect.SetAll(svlRect(0, 0, 4096, 4096));
    ROIRectInternal = ROIRect;
    ROIEllipse.SetSize(SVL_MAX_CHANNELS);
    ROIEllipse.SetAll(svlEllipse(-1, -1, 0, 0, 0.0));
    ROIEllipseInternal = ROIEllipse;
    ROICenter.SetSize(SVL_MAX_CHANNELS);

    WarpedROIRect.SetSize(SVL_MAX_CHANNELS);
    WarpedROIRect.SetAll(svlRect(0, 0, 4096, 4096));
    WarpedROIEllipse.SetSize(SVL_MAX_CHANNELS);
    WarpedROIEllipse.SetAll(svlEllipse(-1, -1, 0, 0, 0.0));
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

void svlFilterImageTracker::SetTargetTrajectorySmoothing(double weight)
{
    if (weight < 0.0) TargetTrajectorySmoothingWeight = 0.0;
    else TargetTrajectorySmoothingWeight = weight;
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

void svlFilterImageTracker::SetRigidBodyTransformSmoothing(double weight)
{
    if (weight < 0.0) RigidBodyTransformSmoothingWeight = 0.0;
    else RigidBodyTransformSmoothingWeight = weight;
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

int svlFilterImageTracker::SetRigidBodyTransform(const vct3x3 & transform, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS || !Trackers[videoch] || !WarpedImage) return SVL_FAIL;

    RigidBodyTransform[videoch].Assign(transform);

    // Extract rotation matrix from frame and make a copy of it
    vctFixedSizeConstMatrixRef<double, 2, 2, 3, 1> c_mat2x2(transform);
    vct2x2 mat2x2(c_mat2x2);

    // Angle
    vctRot2 rot2(mat2x2);
    vctAnRot2 anrot2(rot2, false);
    double angle = anrot2.Angle();

    // Scale
    double scale = cos(angle) / transform.Element(0, 0);

    // Translation
    vct2 res1;
    res1[0] = (transform.Element(0, 2) - WarpedImage->GetWidth(videoch)  / 2);
    res1[1] = (transform.Element(1, 2) - WarpedImage->GetHeight(videoch) / 2);

    // Inverse
    double det = mat2x2.Element(0, 0) * mat2x2.Element(1, 1) - mat2x2.Element(0, 1) * mat2x2.Element(1, 0);
    mat2x2.Element(0, 0) =  mat2x2.Element(1, 1) / det;
    mat2x2.Element(1, 1) =  mat2x2.Element(0, 0);
    mat2x2.Element(0, 1) = -mat2x2.Element(0, 1) / det;
    mat2x2.Element(1, 0) = -mat2x2.Element(0, 1);

    // Transform translation
    vct2 res2;
    res2.ProductOf(mat2x2, res1);

    // Store results
    WarpedRigidBodyScale[videoch] = scale;
    WarpedRigidBodyAngle[videoch] = angle;

    const unsigned int targetcount = static_cast<unsigned int>(Targets.cols());
    svlTarget2D *target = Targets.Pointer(videoch, 0);
    vctDynamicMatrixRef<int> proto_pos;
    const double ax = res2[0];
    const double ay = res2[1];

    proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(videoch));

    for (unsigned int i = 0; i < targetcount; i ++) {

        if (target->used) {
            target->pos.x = static_cast<int>(proto_pos.Element(0, i) + ax);
            target->pos.y = static_cast<int>(proto_pos.Element(1, i) + ay);

            Trackers[videoch]->SetTarget(i, *target);
        }

        target ++;
    }

    return SVL_OK;
}

int svlFilterImageTracker::GetRigidBodyTransform(vct3x3 & transform, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;
    transform.Assign(RigidBodyTransform[videoch]);
    return SVL_OK;
}

vct3x3 svlFilterImageTracker::GetRigidBodyTransform(unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) {
        vct3x3 xform = vct3x3::Eye();
        return xform;
    }

    return RigidBodyTransform[videoch];
}

void svlFilterImageTracker::GetRigidBodyError(vctDynamicVector<int> & errors)
{
    if (errors.size() != RigidBodyIterations) errors.SetSize(RigidBodyIterations);
    errors.Assign(RigidBodyError);
}

int svlFilterImageTracker::SetROI(const svlRect & rect, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    ROIRect[videoch] = rect;

    return SVL_OK;
}

int svlFilterImageTracker::SetROI(int left, int top, int right, int bottom, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    ROIRect[videoch].left   = left;
    ROIRect[videoch].top    = top;
    ROIRect[videoch].right  = right;
    ROIRect[videoch].bottom = bottom;

    return SVL_OK;
}

int svlFilterImageTracker::SetROI(const svlEllipse & ellipse, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    ROIEllipse[videoch] = ellipse;

    return SVL_OK;
}

int svlFilterImageTracker::GetROI(svlRect & rect, unsigned int videoch) const
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    rect = ROIRect[videoch];

    return SVL_OK;
}

int svlFilterImageTracker::GetROI(svlEllipse & ellipse, unsigned int videoch) const
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    ellipse = ROIEllipse[videoch];

    return SVL_OK;
}

int svlFilterImageTracker::OnChangeCenterRect(const svlRect & rect, unsigned int videoch)
{
    return SetROI(rect, videoch);
}

int svlFilterImageTracker::OnChangeCenterEllipse(const svlEllipse & ellipse, unsigned int videoch)
{
    const int border = 0;
    return SetROI(svlEllipse(ellipse.cx, ellipse.cy, std::max(ellipse.rx - border, 1), std::max(ellipse.ry - border, 1), ellipse.angle), videoch);
}

int svlFilterImageTracker::SetMosaicSize(unsigned int width, unsigned int height)
{
    if (width < 100 || height < 100) return SVL_FAIL;
    MosaicWidth = width;
    MosaicHeight = height;
    return SVL_OK;
}

const svlSampleImage* svlFilterImageTracker::GetMosaicImage() const
{
    return Mosaic;
}

int svlFilterImageTracker::OnConnectInput(svlFilterInput &input, svlStreamType type)
{
    if (!input.IsTrunk()) return SVL_OK;

    // Check if type is on the supported list
    if (!input.IsTypeSupported(type)) return SVL_FAIL;

    SetOutputType("output", type);
    SetOutputType("warpedimage", type);
    SetOutputType("mosaicimage", type);

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
        RigidBodyTransform.SetSize(VideoChannels);

        Homography.SetSize(VideoChannels);

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

        Mosaic = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
        if (!Mosaic) return SVL_FAIL;
        Mosaic->SetSize(MosaicWidth, MosaicHeight);
        Mosaic->SetTimestamp(syncInput->GetTimestamp());
        GetOutput("mosaicimage")->SetupSample(Mosaic);

        MosaicAccuBuffer.SetSize(VideoChannels);
        MosaicAccuCount.SetSize(VideoChannels);
        for (unsigned int i = 0; i < VideoChannels; i ++) {
            MosaicAccuBuffer[i].SetSize(Mosaic->GetDataSize(i));
            MosaicAccuCount[i].SetSize(Mosaic->GetWidth(i) * Mosaic->GetHeight(i));
        }
    }

    syncOutput = syncInput;
    OutputTargets.SetTimestamp(syncInput->GetTimestamp());
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
        _OnSingleThread(procInfo) {
            PushSamplesToAsyncOutputs(syncInput->GetTimestamp());
            FrameCount ++;
        }
        return SVL_OK;
    }

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    svlTarget2D *target_buffer;
    vctDynamicVectorRef<int> flag;
    vctDynamicVectorRef<int> confidence;
    vctDynamicMatrixRef<int> position;
    svlEllipse ell;

    const int weight = static_cast<int>(1000.0 * TargetTrajectorySmoothingWeight);
    const int weightsum = weight + 1000;
    unsigned int vch, i, targetcount = 0;

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

            // Copy the input targets into the current output buffer
            if (target_input) InitialTargets.CopyOf(*target_input);
            OutputTargets.CopyOf(InitialTargets);

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

            RigidBodyAngle.SetAll(0.0);
            RigidBodyScale.SetAll(1.0);
            WarpedRigidBodyAngle.SetAll(0.0);
            WarpedRigidBodyScale.SetAll(1.0);

            RigidBodyInitialized = false;
            ResetFlag = false;
        }
    }

    _SynchronizeThreads(procInfo);

    _ParallelLoop(procInfo, vch, VideoChannels)
    {
        // Region of interest may change at run-time
        if (Trackers[vch]) {
            // Get all ROIs at once from public accessor
            ROIRectInternal[vch] = ROIRect[vch];
            ROIEllipseInternal[vch] = ROIEllipse[vch];

            const bool cropped_image = true;
            if (cropped_image) {
                const int cx = img->GetWidth() / 2;
                const int cy = img->GetHeight() / 2;
                const int roi_rx = (ROIRectInternal[vch].right - ROIRectInternal[vch].left) / 2;
                const int roi_ry = (ROIRectInternal[vch].bottom - ROIRectInternal[vch].top) / 2;
                ROIRectInternal[vch].Assign(cx - roi_rx, cy - roi_ry, cx + roi_rx, cy + roi_ry);
                ROIEllipseInternal[vch].cx = cx;
                ROIEllipseInternal[vch].cy = cy;
            }

            if (ROIEllipseInternal[vch].rx > 0 && ROIEllipseInternal[vch].ry > 0) {
                // If ellipse is valid use its bounding rectangle
                ROIEllipseInternal[vch].GetBoundingRect(ROIRectInternal[vch]);
            }
            else {
                // If ellipse is invalid use user-provided rectangle
                ROIEllipseInternal[vch].Assign(-1, -1, 0, 0, 0.0);
            }

            ROIRectInternal[vch].Trim(0, img->GetWidth(vch), 0, img->GetHeight(vch));
        }
    }

    if (WarpingParallel) {

        _SynchronizeThreads(procInfo);

        vch = procInfo->ID >> 1;
        if (vch < VideoChannels && Trackers[vch]) {
            WarpImage(img, vch, Trackers[vch]->GetROIMargin(), procInfo->ID);
        }
    }

    _SynchronizeThreads(procInfo);

    targetcount = OutputTargets.GetMaxTargets();
    if (targetcount < 1) return SVL_OK;

    _ParallelLoop(procInfo, vch, VideoChannels)
    {
        if (!Trackers[vch]) continue;

        if (RigidBody) {
            if (!WarpingParallel) WarpImage(img, vch, Trackers[vch]->GetROIMargin());

            Trackers[vch]->SetROI(WarpedROIRect[vch]);
            Trackers[vch]->SetROI(WarpedROIEllipse[vch]);

            // Pre-processing data
            Trackers[vch]->PreProcessImage(*WarpedImage, vch);
        }
        else {
            const int roi_margin = Trackers[vch]->GetROIMargin();

            svlRect _rect(ROIRectInternal[vch]);
            _rect.left   += roi_margin;
            _rect.top    += roi_margin;
            _rect.right  -= roi_margin;
            _rect.bottom -= roi_margin;
            Trackers[vch]->SetROI(_rect);

            svlEllipse _ell(ROIEllipseInternal[vch]);
            _ell.rx -= roi_margin;
            _ell.ry -= roi_margin;
            Trackers[vch]->SetROI(_ell);

            // Pre-processing data
            Trackers[vch]->PreProcessImage(*img, vch);
        }
    }

    if (RigidBody) {
        img = WarpedImage;
    }

    _SynchronizeThreads(procInfo);

    // Processing data
    for (vch = 0; vch < VideoChannels; vch ++) {
        if (!Trackers[vch]) continue;

        for (i = 0; i < Iterations; i ++) {
            Trackers[vch]->Track(procInfo, *img, vch);
        }
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        for (vch = 0; vch < VideoChannels; vch ++) {

            if (!Trackers[vch]) continue;

            // Retrieve results
            target_buffer = Targets.Pointer(vch, 0);

            for (i = 0; i < targetcount; i ++) {

                if (target_buffer->used) Trackers[vch]->GetTarget(i, *target_buffer);
                else target_buffer->visible = false;

                target_buffer ++;
            }
        }

        if (RigidBody) {

            // It can be put back once the two mosaic images are registered to each other
//            LinkChannelsVertically();

            for (i = 0; i < RigidBodyIterations; i ++) RigidBodyError[i] = ReconstructRigidBody();
            // TO DO: fix channel '0'
            BackprojectRigidBody(img->GetWidth(0), img->GetHeight(0));

            //ComputeHomography();
            //BackprojectHomography();

            for (vch = 0; vch < VideoChannels; vch ++) {

                if (!Trackers[vch]) continue;

                UpdateMosaicImage(vch, img->GetWidth(vch), img->GetHeight(vch));
            }
        }

        for (vch = 0; vch < VideoChannels; vch ++) {

            if (!Trackers[vch]) continue;

            // Store results
            target_buffer = Targets.Pointer(vch, 0);
            flag.SetRef(targetcount, OutputTargets.GetFlagPointer());
            confidence.SetRef(targetcount, OutputTargets.GetConfidencePointer(vch));
            position.SetRef(2, targetcount, OutputTargets.GetPositionPointer(vch));

            for (i = 0; i < targetcount; i ++) {

                flag.Element(i)        = target_buffer->used;
                confidence.Element(i)  = target_buffer->conf;
                position.Element(0, i) = (target_buffer->pos.x * 1000 + position.Element(0, i) * weight) / weightsum;
                position.Element(1, i) = (target_buffer->pos.y * 1000 + position.Element(1, i) * weight) / weightsum;

                target_buffer ++;
            }
        }

        PushSamplesToAsyncOutputs(syncInput->GetTimestamp());
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
    MosaicAccuBuffer.SetSize(0);
    MosaicAccuCount.SetSize(0);

    RigidBodyAngle.SetSize(0);
    RigidBodyScale.SetSize(0);
    WarpedRigidBodyAngle.SetSize(0);
    WarpedRigidBodyScale.SetSize(0);
    WarpInternals.SetSize(0);

    RigidBodyError.SetAll(-1);

    return SVL_OK;
}

void svlFilterImageTracker::LinkChannelsVertically()
{
    if (VideoChannels < 2) return;

    const unsigned int targetcount = static_cast<unsigned int>(Targets.cols());
    svlTarget2D *target_0, *target;
    unsigned int i, vch, visible_sum;
    int avrg_y;

    target_0 = Targets.Pointer(0, 0);
    for (i = 0; i < targetcount; i ++) {

        // Find targets that are visible on all video channels
        avrg_y = 0;
        visible_sum = 0;
        target = target_0;
        for (vch = 0; vch < VideoChannels; vch ++) {
            if (target->visible) {
                avrg_y += target->pos.y;
                visible_sum ++;
            }
            target += targetcount;
        }
        if (visible_sum == VideoChannels) {
            // Set target on all channels to the average vertical position
            avrg_y /= VideoChannels;
            target = target_0;
            for (vch = 0; vch < VideoChannels; vch ++) {
                target->pos.y = avrg_y;
                target += targetcount;
            }
        }

        target_0 ++;
    }
}

int svlFilterImageTracker::ReconstructRigidBody()
{
    const unsigned int targetcount = static_cast<unsigned int>(Targets.cols());
    double dconf, sum_conf = 0.0, ax, ay, vx, vy, angle, cos_an, sin_an, scale;
    double proto_ax, proto_ay, proto_vx, proto_vy, proto_dist;
    double avrg_scale, avrg_angle, avrg_conf;
    vctDynamicMatrixRef<int> proto_pos;
    svlTarget2D *target;
    unsigned int i, vch;
    int dx, dy, conf, bp_error, bp_error_sum, visible_sum;

    avrg_scale = 0.0;
    avrg_angle = 0.0;
    avrg_conf  = 0.0;

    for (vch = 0; vch < VideoChannels; vch ++) {

        proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(vch));

        ax = ay = proto_ax = proto_ay = cos_an = sin_an = scale = sum_conf = 0.0;

        // Compute center of weight
        target = Targets.Pointer(vch, 0);
        for (i = 0; i < targetcount; i ++) {

            if (target->visible) {

                conf      = target->conf;
                ax       += target->pos.x           * conf;
                ay       += target->pos.y           * conf;
                proto_ax += proto_pos.Element(0, i) * conf;
                proto_ay += proto_pos.Element(1, i) * conf;
                sum_conf += conf;
            }

            target ++;
        }
        if (sum_conf < 0.0001) continue;

        ax       /= sum_conf;
        ay       /= sum_conf;
        proto_ax /= sum_conf;
        proto_ay /= sum_conf;

        // Compute angle and scale
        sum_conf = 0.0;

        target = Targets.Pointer(vch, 0);
        for (i = 0; i < targetcount; i ++) {

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

            target ++;
        }
        if (sum_conf < 0.0001) continue;

        RigidBodyScale[vch] = scale / sum_conf;
        RigidBodyAngle[vch] = atan2(sin_an, cos_an);

        double prevangle = WarpedRigidBodyAngle[vch];
        double prevscale = WarpedRigidBodyScale[vch];

        // Reconstruct rigid body based on prototype
        WarpedRigidBodyAngle[vch] -= RigidBodyAngle[vch];
        WarpedRigidBodyScale[vch] /= RigidBodyScale[vch];

        WarpedRigidBodyAngle[vch] = (WarpedRigidBodyAngle[vch] + prevangle * RigidBodyTransformSmoothingWeight) /
                                    (1.0 + RigidBodyTransformSmoothingWeight);
        WarpedRigidBodyScale[vch] = (WarpedRigidBodyScale[vch] + prevscale * RigidBodyTransformSmoothingWeight) /
                                    (1.0 + RigidBodyTransformSmoothingWeight);

        // Checking rigid body transformation constraints
        if (WarpedRigidBodyScale[vch] < RigidBodyScaleLow) WarpedRigidBodyScale[vch] = RigidBodyScaleLow;
        else if (WarpedRigidBodyScale[vch] > RigidBodyScaleHigh) WarpedRigidBodyScale[vch] = RigidBodyScaleHigh;
        if (WarpedRigidBodyAngle[vch] < RigidBodyAngleLow) WarpedRigidBodyAngle[vch] = RigidBodyAngleLow;
        else if (WarpedRigidBodyAngle[vch] >RigidBodyAngleHigh) WarpedRigidBodyAngle[vch] = RigidBodyAngleHigh;

        T_ax[vch] = ax;
        T_ay[vch] = ay;
        T_proto_ax[vch] = proto_ax;
        T_proto_ay[vch] = proto_ay;

        avrg_scale += WarpedRigidBodyScale[vch] * sum_conf;
        avrg_angle += WarpedRigidBodyAngle[vch] * sum_conf;
        avrg_conf  += sum_conf;
    }

    // Calculate confidence-weighted-average of rigid body parameters between video channels
    if (sum_conf < 0.0001) return -1;
    avrg_scale /= avrg_conf;
    avrg_angle /= avrg_conf;

    // Backproject transformed model onto output targets
    for (bp_error_sum = 0, visible_sum = 0, vch = 0; vch < VideoChannels; vch ++) {

        WarpedRigidBodyScale[vch] = avrg_scale;
        WarpedRigidBodyAngle[vch] = avrg_angle;

        // Calculate inverse warping
        angle  = -avrg_angle;
        T_sin_an[vch] = sin_an = sin(angle);
        T_cos_an[vch] = cos_an = cos(angle);
        T_scale[vch]  = scale  = 1.0 / avrg_scale;

        // Initialize variables
        proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(vch));
        ax = T_ax[vch];
        ay = T_ay[vch];
        proto_ax = T_proto_ax[vch];
        proto_ay = T_proto_ay[vch];
        target = Targets.Pointer(vch, 0);

        // Calculate backprojection error and correct confidence with error
        for (i = 0; i < targetcount; i ++) {
            if (target->visible) {

                dx = static_cast<int>(proto_pos.Element(0, i) - proto_ax + ax) - target->pos.x;
                dy = static_cast<int>(proto_pos.Element(1, i) - proto_ay + ay) - target->pos.y;
                bp_error = dx * dx + dy * dy;

                conf = target->conf;
                bp_error_sum += bp_error * conf;
                visible_sum ++;

                conf -= bp_error;
                if (conf < 1) conf = 1;
                target->conf = conf;
            }
            target ++;
        }
    }

    // Return backprojection error
    return (bp_error_sum / visible_sum);
}

void svlFilterImageTracker::BackprojectRigidBody(int width, int height)
{
    const unsigned int targetcount = static_cast<unsigned int>(Targets.cols());
    double ax, ay, rx, ry, vx, vy, proto_ax, proto_ay, sin_an, cos_an, scale;
    vctDynamicMatrixRef<int> proto_pos;
    svlTarget2D *target;
    unsigned int i, vch;
    vct3x3 xform, xform2 = vct3x3::Eye();


    for (vch = 0; vch < VideoChannels; vch ++) {
        if (!Trackers[vch]) continue;

        proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(vch));
        ax = T_ax[vch];
        ay = T_ay[vch];
        rx = ROICenter[vch].X();
        ry = ROICenter[vch].Y();
        proto_ax = T_proto_ax[vch];
        proto_ay = T_proto_ay[vch];
        sin_an = T_sin_an[vch];
        cos_an = T_cos_an[vch];
        scale = T_scale[vch];
        target = Targets.Pointer(vch, 0);

        // Calculate rigid body transformation
        vx = T_ax[vch] - T_proto_ax[vch];
        vy = T_ay[vch] - T_proto_ay[vch];
        xform = vct3x3::Eye();
        xform.Element(0, 0) = cos_an * scale;
        xform.Element(0, 1) = -sin_an * scale;
        xform.Element(1, 0) = sin_an * scale;
        xform.Element(1, 1) = cos_an * scale;
        xform.Element(0, 2) = ROICenter[vch].X() + scale * (vx * cos_an - vy * sin_an);
        xform.Element(1, 2) = ROICenter[vch].Y() + scale * (vx * sin_an + vy * cos_an);
        xform2.Element(0, 2) = static_cast<int>(width  / 2) - ROICenter[vch].X();
        xform2.Element(1, 2) = static_cast<int>(height / 2) - ROICenter[vch].Y();
        RigidBodyTransform[vch].ProductOf(xform, xform2);

        // Backproject transformed model onto output targets
        for (i = 0; i < targetcount; i ++) {

            if (target->used) {
                vx = proto_pos.Element(0, i) - proto_ax + ax;
                vy = proto_pos.Element(1, i) - proto_ay + ay;

                target->pos.x = static_cast<int>(vx);
                target->pos.y = static_cast<int>(vy);

                Trackers[vch]->SetTarget(i, *target);

                vx += -rx;
                vy += -ry;

                target->pos.x = static_cast<int>(rx + scale * (vx * cos_an - vy * sin_an));
                target->pos.y = static_cast<int>(ry + scale * (vx * sin_an + vy * cos_an));
            }

            target ++;
        }
    }
}

int svlFilterImageTracker::ComputeHomography()
{
/*
    const unsigned int targetcount = Targets.cols();
    vctDynamicMatrixRef<int> proto_pos;
    svlTarget2D *target = 0;
    unsigned int i, j, vch;
    double x, y, proto_x, proto_y, ax, ay, apx, apy, conf, sum_conf;

    for (vch = 0; vch < VideoChannels; vch ++) {

        proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(vch));

        unsigned int visiblecount;

        // Compute center of weight
        for (i = 0, ax = 0, ay = 0, apx = 0, apy = 0, sum_conf = 0, visiblecount = 0, target = Targets.Pointer(vch, 0);
             i < targetcount;
             i ++, target ++) {
            if (target->visible) {
                conf = target->conf;
                ax += target->pos.x;// * conf;
                ay += target->pos.y;// * conf;
                apx += proto_pos.Element(0, i);// * conf;
                apy += proto_pos.Element(1, i);// * conf;
                sum_conf += conf;
                visiblecount ++;
            }
        }
        ax /= visiblecount;//sum_conf;
        ay /= visiblecount;//sum_conf;
        apx /= visiblecount;//sum_conf;
        apy /= visiblecount;//sum_conf;

        vctDynamicMatrix<double> A(visiblecount * 2, 9, VCT_COL_MAJOR);

        for (i = 0, j = 0, target = Targets.Pointer(vch, 0); i < targetcount; i ++, target ++) {
            if (target->visible) {
                x = target->pos.x - ax;
                y = target->pos.y - ay;
                proto_x = proto_pos.Element(0, i) - apx;
                proto_y = proto_pos.Element(1, i) - apy;
                A.Row(j).Assign(-proto_x, -proto_y, -1.0, 0.0, 0.0, 0.0, proto_x * x, proto_y * x, x);
                A.Row(j + 1).Assign(0.0, 0.0, 0.0, -proto_x, -proto_y, -1.0, proto_x * y, proto_y * y, y);
                j += 2;
            }
        }

        nmrSVDDynamicData svdData(A);
        nmrSVD(A, svdData);

        for (i = 0; i < 9; i ++) {
            Homography[vch][i] = svdData.Vt().Element(8, i) / svdData.Vt().Element(8, 8);
        }

        std::cerr << "Video channel: " << vch << std::endl;
        std::cout << "Homography: (" << j << ") " << Homography[vch] << std::endl
                  << "Singular values: " << svdData.S() << std::endl
                  << "Eigen vectors:" << std::endl << svdData.Vt().TransposeRef() << std::endl << std::endl;
    }
*/
    return 0;
}

void svlFilterImageTracker::BackprojectHomography()
{
/*
    const unsigned int targetcount = Targets.cols();
    double rx, ry, proto_x, proto_y;
    vctDynamicMatrixRef<int> proto_pos;
    vctFixedSizeVectorRef<double, 9, 1> h;
    svlTarget2D *target;
    unsigned int i, vch;

    for (vch = 0; vch < VideoChannels; vch ++) {
        if (!Trackers[vch]) continue;

        proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(vch));
        h.SetRef(Homography[vch]);

        // Backproject transformed model onto output targets
        for (i = 0, target = Targets.Pointer(vch, 0); i < targetcount; i ++, target ++) {
            if (target->used) {
                proto_x = proto_pos.Element(0, i);
                proto_y = proto_pos.Element(1, i);

                rx = (proto_x * h[0] + proto_y * h[1] + h[2]) / (proto_x * h[6] + proto_y * h[7] + 1.0);
                ry = (proto_x * h[3] + proto_y * h[4] + h[5]) / (proto_x * h[6] + proto_y * h[7] + 1.0);

                target->pos.x = rx;
                target->pos.y = ry;

//                std::cerr << "(" << x << ", " << y << "), "
//                          << "(" << proto_x << ", " << proto_y << "), "
//                          << "(" << rx << ", " << ry << ")" << std::endl;
            }
        }
    }
*/
}

void svlFilterImageTracker::WarpImage(svlSampleImage* image, unsigned int videoch, int roi_margin, int threadid)
{
    const double c = cos(WarpedRigidBodyAngle[videoch]);
    const double s = sin(WarpedRigidBodyAngle[videoch]);
    const double sc = WarpedRigidBodyScale[videoch];

    const int border = 50;

    int x1 = std::max(0, ROIRectInternal[videoch].left - border);
    int y1 = std::max(0, ROIRectInternal[videoch].top - border);
    int x2 = std::min(static_cast<int>(image->GetWidth(videoch) - 1), ROIRectInternal[videoch].right + border);
    int y2 = y1;
    int x3 = x2;
    int y3 = std::min(static_cast<int>(image->GetHeight(videoch) - 1), ROIRectInternal[videoch].bottom + border);
    int x4 = x1;
    int y4 = y3;

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

    if (threadid >= 0) {
        svlTriangle tri_in, tri_out;
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
        svlQuad quad_in (x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4);
        svlQuad quad_out(wx1, wy1, wx2, wy2, wx3, wy3, wx4, wy4);
        svlDraw::WarpQuad(image, videoch, quad_in, WarpedImage, videoch, quad_out, WarpInternals[videoch]);
    }

    // Warp ROIs
    if (threadid < 0 || (threadid & 1) == 0) {

        // Rect
        x1 = std::max(0, ROIRectInternal[videoch].left + roi_margin);
        y1 = std::max(0, ROIRectInternal[videoch].top) + roi_margin;
        x2 = std::min(static_cast<int>(image->GetWidth(videoch) - 1), ROIRectInternal[videoch].right + 1 - roi_margin);
        y2 = y1;
        x3 = x2;
        y3 = std::min(static_cast<int>(image->GetHeight(videoch) - 1), ROIRectInternal[videoch].bottom + 1 - roi_margin);
        x4 = x1;
        y4 = y3;

        WarpedROIRect[videoch].x1 = static_cast<int>(sc * (c * (x1 - cx) - s * (y1 - cy))) + cx;
        WarpedROIRect[videoch].y1 = static_cast<int>(sc * (s * (x1 - cx) + c * (y1 - cy))) + cy;
        WarpedROIRect[videoch].x2 = static_cast<int>(sc * (c * (x2 - cx) - s * (y2 - cy))) + cx;
        WarpedROIRect[videoch].y2 = static_cast<int>(sc * (s * (x2 - cx) + c * (y2 - cy))) + cy;
        WarpedROIRect[videoch].x3 = static_cast<int>(sc * (c * (x3 - cx) - s * (y3 - cy))) + cx;
        WarpedROIRect[videoch].y3 = static_cast<int>(sc * (s * (x3 - cx) + c * (y3 - cy))) + cy;
        WarpedROIRect[videoch].x4 = static_cast<int>(sc * (c * (x4 - cx) - s * (y4 - cy))) + cx;
        WarpedROIRect[videoch].y4 = static_cast<int>(sc * (s * (x4 - cx) + c * (y4 - cy))) + cy;

        // Ellipse
        x1 = ROIEllipseInternal[videoch].cx;
        y1 = ROIEllipseInternal[videoch].cy;

        WarpedROIEllipse[videoch].cx = static_cast<int>(sc * (c * (x1 - cx) - s * (y1 - cy))) + cx;
        WarpedROIEllipse[videoch].cy = static_cast<int>(sc * (s * (x1 - cx) + c * (y1 - cy))) + cy;
        WarpedROIEllipse[videoch].rx = static_cast<int>(sc * (ROIEllipseInternal[videoch].rx - roi_margin));
        WarpedROIEllipse[videoch].ry = static_cast<int>(sc * (ROIEllipseInternal[videoch].ry - roi_margin));
        WarpedROIEllipse[videoch].angle = ROIEllipseInternal[videoch].angle + WarpedRigidBodyAngle[videoch];
    }
}

int svlFilterImageTracker::UpdateMosaicImage(unsigned int videoch, unsigned int width, unsigned int height)
{
    svlTrackerMSBruteForce* tracker = dynamic_cast<svlTrackerMSBruteForce*>(Trackers[videoch]);
    if (!tracker) return SVL_FAIL;

    const int mosaic_pixel_count = Mosaic->GetWidth(videoch) * Mosaic->GetHeight(videoch);
    const int mosaic_width  = Mosaic->GetWidth(videoch);
    const int mosaic_height = Mosaic->GetHeight(videoch);
    const int mosaic_center_x = mosaic_width  >> 1;
    const int mosaic_center_y = mosaic_height >> 1;
    const int image_center_x = width  >> 1;
    const int image_center_y = height >> 1;
    const int targetcount = static_cast<int>(Targets.cols());
    const int tmpl_radius = tracker->GetTemplateRadius();
    const int tmpl_size = tmpl_radius * 2 + 1;

    svlTarget2D *target = 0;
    unsigned char *tdata, *count_buffer, *cdata, *out_mos_buffer;
    unsigned short *accu_buffer, *mdata;
    unsigned char r, g, b;
    int i, k, l, txf, tyf, mxf, mxt, myf, myt, w, h, weight, dx, dy;
    vctDynamicVectorRef<unsigned char> template_ref;
    vctInt2 pos;


    accu_buffer = MosaicAccuBuffer[videoch].Pointer();
    count_buffer = MosaicAccuCount[videoch].Pointer();
    out_mos_buffer = Mosaic->GetUCharPointer(videoch);

    memset(accu_buffer, 0, MosaicAccuBuffer[videoch].size() * sizeof(unsigned short));
    memset(count_buffer, 0, MosaicAccuCount[videoch].size() * sizeof(unsigned char));

    for (i = 0; i < targetcount; i ++) {
        target = Targets.Pointer(videoch, i);

        if (!target->used || target->feature_quality < 0) continue;

        tracker->GetImageRef(i, template_ref);
        tdata = template_ref.Pointer();
        if (!tdata) continue;

        InitialTargets.GetPosition(i, pos, videoch);

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

        w = mxt - mxf;
        h = myt - myf;
        if (w <= 0 || h <= 0) continue;

        tdata += (tyf * tmpl_size + txf) * 3;
        k = myf * mosaic_width + mxf;
        mdata = accu_buffer + k * 3;
        cdata = count_buffer + k;

        const int tmpl_stride = (tmpl_size - w) * 3;
        const int mosaic_stride = (mosaic_width - w) * 3;

        for (l = 0; l < h; l ++) {

            if (l > tmpl_radius) {
                dy = l - tmpl_radius;
            }
            else {
                dy = tmpl_radius - l;
            }

            for (k = 0; k < w; k ++) {

                if (k > tmpl_radius) {
                    dx = k - tmpl_radius;
                }
                else {
                    dx = tmpl_radius - k;
                }
                if (dx > dy) weight = tmpl_radius - dx;
                else         weight = tmpl_radius - dy;

                if (weight < 0) weight = 0;

                *mdata += weight * (*tdata); mdata ++; tdata ++;
                *mdata += weight * (*tdata); mdata ++; tdata ++;
                *mdata += weight * (*tdata); mdata ++; tdata ++;
                cdata[k] += weight;
            }
            tdata += tmpl_stride;
            mdata += mosaic_stride;
            cdata += mosaic_width;
        }
    }

    for (i = 0; i < mosaic_pixel_count; i ++) {
        k = *count_buffer; count_buffer ++;
        if (k) {
            b = (*accu_buffer) / k; accu_buffer ++;
            g = (*accu_buffer) / k; accu_buffer ++;
            r = (*accu_buffer) / k; accu_buffer ++;
            if (r == 0 && g == 0 && b == 0) r = g = b = 1;
            *out_mos_buffer = b; out_mos_buffer ++;
            *out_mos_buffer = g; out_mos_buffer ++;
            *out_mos_buffer = r; out_mos_buffer ++;
        }
        else {
            *out_mos_buffer = 0; out_mos_buffer ++;
            *out_mos_buffer = 0; out_mos_buffer ++;
            *out_mos_buffer = 0; out_mos_buffer ++;
            accu_buffer += 3;
        }
    }

    return SVL_OK;
}

void svlFilterImageTracker::PushSamplesToAsyncOutputs(double timestamp)
{
    svlFilterOutput* output;

    output = GetOutput("targets");
    if (output) {
        OutputTargets.SetTimestamp(timestamp);
        output->PushSample(&OutputTargets);
    }
    if (RigidBody) {
        output = GetOutput("warpedimage");
        if (output) {
            WarpedImage->SetTimestamp(timestamp);
            output->PushSample(WarpedImage);
        }
        output = GetOutput("mosaicimage");
        if (output) {
            Mosaic->SetTimestamp(timestamp);
            output->PushSample(Mosaic);
        }
    }
}


/**********************************/
/*** svlImageTracker class ********/
/**********************************/

svlImageTracker::svlImageTracker() :
    Initialized(false),
    Width(0),
    Height(0),
    ROIRect(-1, -1, -1, -1, -1, -1, -1, -1),
    ROIEllipse(-1, -1, 0, 0, 0.0)
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
    svlRect _rect(rect);
    _rect.Normalize();
    _rect.Trim(0, Width, 0, Height);
    ROIRect.Assign(_rect);
}

void svlImageTracker::SetROI(int left, int top, int right, int bottom)
{
    SetROI(svlRect(left, top, right, bottom));
}

void svlImageTracker::SetROI(const svlQuad & quad)
{
    ROIRect.Assign(quad);
}

void svlImageTracker::SetROI(const svlEllipse & ellipse)
{
    ROIEllipse = ellipse;
}

int svlImageTracker::GetROIMargin()
{
    return 0;
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

int svlImageTracker::Track(svlProcInfo* procInfo, svlSampleImage & image, unsigned int videoch)
{
    if (procInfo->count > videoch) {
        if (procInfo->ID == videoch) return Track(image, videoch);
    }
    else {
        if (procInfo->ID == 0) return Track(image, videoch);
    }
    return SVL_OK;
}

void svlImageTracker::Release()
{
}

