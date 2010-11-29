/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFilterImageTracker.cpp 1236 2010-02-26 20:38:21Z adeguet1 $

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
    Iterations(1)
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

    Trackers.SetSize(SVL_MAX_CHANNELS);
    Trackers.SetAll(0);

    ROI.SetSize(SVL_MAX_CHANNELS);
    ROI.SetAll(svlRect(0, 0, 4096, 4096));
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
        RigidBodyScale.SetSize(VideoChannels);
    }

    syncOutput = syncInput;
    GetOutput("targets")->SetupSample(&OutputTargets);

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

                // Resize work target buffer
                targetcount = OutputTargets.GetMaxTargets();
                Targets.SetSize(VideoChannels, targetcount);
                target_buffer = Targets.Pointer(vch, 0);

                flag.SetRef(targetcount, OutputTargets.GetFlagPointer());
                confidence.SetRef(targetcount, OutputTargets.GetConfidencePointer(vch));
                position.SetRef(2, targetcount, OutputTargets.GetPositionPointer(vch));

                for (i = 0; i < targetcount; i ++) {

                    // Storing temporary results until tracking starts
                    target_buffer->used    = flag.Element(i) ? true : false;
                    target_buffer->visible = false;
                    target_buffer->conf    = confidence.Element(i);
                    target_buffer->pos.x   = position.Element(0, i);
                    target_buffer->pos.y   = position.Element(1, i);

                    // Updating targets in trackers
                    Trackers[vch]->SetTarget(i, *target_buffer);

                    target_buffer ++;
                }
            }

            ResetFlag = false;
        }
    }

    _SynchronizeThreads(procInfo);

    targetcount = OutputTargets.GetMaxTargets();
    if (targetcount < 1) return SVL_OK;

    _ParallelLoop(procInfo, vch, VideoChannels)
    {
        if (!Trackers[vch]) continue;

        // Region of interest might change at run-time
        Trackers[vch]->SetROI(ROI[vch]);

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

            target_buffer  = Targets.Pointer(vch, 0);
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

        GetOutput("targets")->PushSample(&OutputTargets);
    }

    return SVL_OK;
}

int svlFilterImageTracker::Release()
{
    for (unsigned int i = 0; i < Trackers.size(); i ++) {
        if (Trackers[i]) Trackers[i]->Release();
    }
    return SVL_OK;
}

void svlFilterImageTracker::ReconstructRigidBody()
{
    const unsigned int targetcount = Targets.cols();
    double dconf, sum_conf, ax, ay, vx, vy, angle, angle2, angle3, scale;
    double proto_ax, proto_ay, proto_vx, proto_vy, proto_dist;
    vctDynamicMatrixRef<int> proto_pos;
    svlTarget2D *target;

    unsigned int i, j;
    int conf;

    for (j = 0; j < VideoChannels; j ++) {

        if (!Trackers[j]) continue;

        proto_pos.SetRef(2, targetcount, InitialTargets.GetPositionPointer(j));

        ax = ay = proto_ax = proto_ay = angle2 = angle3 = scale = sum_conf = 0.0;

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
                    angle2 += dconf * cos(angle);
                    angle3 += dconf * sin(angle);
                }
            }
        }
        if (sum_conf < 0.0001) return;

        RigidBodyScale[j] = scale / sum_conf;
        RigidBodyAngle[j] = atan2(angle3, angle2);

        // Checking rigid body transformation constraints
        if (RigidBodyScale[j] < RigidBodyScaleLow) RigidBodyScale[j] = RigidBodyScaleLow;
        else if (RigidBodyScale[j] > RigidBodyScaleHigh) RigidBodyScale[j] = RigidBodyScaleHigh;
        if (RigidBodyAngle[j] < RigidBodyAngleLow) RigidBodyAngle[j] = RigidBodyAngleLow;
        else if (RigidBodyAngle[j] >RigidBodyAngleHigh) RigidBodyAngle[j] = RigidBodyAngleHigh;

        // Reconstruct rigid body based on prototype
        scale = RigidBodyScale[j];
        angle = RigidBodyAngle[j];

        for (i = 0; i < targetcount; i ++) {
            target = Targets.Pointer(j, i);

            if (target->used) {

                angle2 = cos(angle);
                angle3 = sin(angle);
                vx = proto_pos.Element(0, i) - proto_ax;
                vy = proto_pos.Element(1, i) - proto_ay;

                target->pos.x = static_cast<int>(ax + scale * (vx * angle2 - vy * angle3));
                target->pos.y = static_cast<int>(ay + scale * (vx * angle3 + vy * angle2));

                if (Trackers[j]) Trackers[j]->SetTarget(i, *target);
            }
        }
    }
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
    target.used    = false;
    target.visible = false;
    target.conf    = 0;
    target.pos.x   = 0;
    target.pos.y   = 0;
    Targets.SetAll(target);

    return SVL_OK;
}

int svlImageTracker::SetTarget(unsigned int targetid, const svlTarget2D & target)
{
    if (targetid >= Targets.size()) return SVL_FAIL;
    Targets[targetid] = target;
    return SVL_OK;
}

int svlImageTracker::GetTarget(unsigned int targetid, svlTarget2D & target)
{
    if (targetid >= Targets.size()) return SVL_FAIL;
    target = Targets[targetid];
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

