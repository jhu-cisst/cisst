/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageWindowTargetSelect.h>
#include <cisstStereoVision/svlDraw.h>
#include <cisstStereoVision/svlFilterOutput.h>


/**********************************************/
/*** svlFilterImageWindowTargetSelect class ***/
/**********************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageWindowTargetSelect)

svlFilterImageWindowTargetSelect::svlFilterImageWindowTargetSelect() :
    svlWindowEventHandlerBase(),
    svlFilterImageWindow(),
    DisplayImage(0),
    CrosshairColor(32, 32, 255),
    CrosshairRadius(12),
    CrosshairThickness(2),
    SendTargets(false),
    AlwaysSend(false),
    EnableAdd(true),
    EnableModify(true),
    ButtonDown(false),
    SelectedTarget(-1),
    SelectedTargetWindow(-1)
{
    AddOutput("targets", false);
    GetOutput("targets")->SetType(svlTypeTargets);
}

int svlFilterImageWindowTargetSelect::SetMaxTargets(unsigned int maxtargets)
{
    if (IsInitialized()) return SVL_FAIL;
    Targets.SetSize(2, maxtargets, SVL_MAX_CHANNELS);
    Targets.ResetTargets();
    return SVL_OK;
}

void svlFilterImageWindowTargetSelect::SetEnableAdd(bool enable)
{
    EnableAdd = enable;
}

void svlFilterImageWindowTargetSelect::SetEnableModify(bool enable)
{
    EnableModify = enable;
}

void svlFilterImageWindowTargetSelect::SetAlwaysSendTargets(bool enable)
{
    AlwaysSend = enable;
}

void svlFilterImageWindowTargetSelect::SetCrosshairStyle(svlRGB color, unsigned int radius, unsigned int thickness)
{
    CrosshairColor = color;
    CrosshairRadius = radius;
    CrosshairThickness = thickness;
}

void svlFilterImageWindowTargetSelect::SetTargets(const svlSampleTargets& targets)
{
    Targets = targets;
}

void svlFilterImageWindowTargetSelect::GetTargets(svlSampleTargets& targets) const
{
    targets = Targets;
}

void svlFilterImageWindowTargetSelect::SetFullScreen(const bool & fullscreen)
{
    svlFilterImageWindow::SetFullScreen(fullscreen);
}

void svlFilterImageWindowTargetSelect::SetEventHandler(svlWindowEventHandlerBase* handler)
{
    svlFilterImageWindow::SetEventHandler(handler);
}

void svlFilterImageWindowTargetSelect::GetFullScreen(bool & fullscreen) const
{
    svlFilterImageWindow::GetFullScreen(fullscreen);
}

int svlFilterImageWindowTargetSelect::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    if (!DisplayImage) DisplayImage = syncInput->GetNewInstance();
    DisplayImage->CopyOf(syncInput);

    SelectionOffset.SetAll(0);
    SelectedTarget = -1;
    SelectedTargetWindow = -1;
    ButtonDown = false;

    syncOutput = syncInput;

    GetOutput("targets")->SetupSample(&Targets);

    svlFilterImageWindow::SetEventHandler(this);
    return svlFilterImageWindow::Initialize(DisplayImage, syncOutput);
}

int svlFilterImageWindowTargetSelect::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;

    svlSampleImage* image = dynamic_cast<svlSampleImage*>(syncInput);
    svlSampleImage* winimage = dynamic_cast<svlSampleImage*>(DisplayImage);
    const unsigned int videochannels = image->GetVideoChannels();
    const unsigned int maxtargets = Targets.GetMaxTargets();
    vctDynamicMatrixRef<int> position;
    unsigned char r, g, b;
    unsigned int i, idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        memcpy(winimage->GetUCharPointer(idx), image->GetUCharPointer(idx), image->GetDataSize(idx));
        winimage->SetTimestamp(image->GetTimestamp());

        if (maxtargets > 0) {
            position.SetRef(2, maxtargets, Targets.GetPositionPointer(idx));

            for (i = 0; i < maxtargets; i ++) {
                if (Targets.GetFlag(i) > 0) {
                    if (SelectedTargetWindow != static_cast<int>(idx) ||
                        SelectedTarget != static_cast<int>(i)) {
                        r = CrosshairColor.r;
                        g = CrosshairColor.g;
                        b = CrosshairColor.b;
                    }
                    else {
                        r = 100; g = 255; b = 100;
                    }
                    svlDraw::Crosshair(dynamic_cast<svlSampleImage*>(DisplayImage), idx,
                                       position.Element(0, i), position.Element(1, i),
                                       r, g, b, CrosshairRadius, CrosshairThickness);
                }
            }
        }
    }

    _SynchronizeThreads(procInfo);

    svlSample* sample = 0;
    int ret = svlFilterImageWindow::Process(procInfo, DisplayImage, sample);

    _OnSingleThread(procInfo)
    {
        if (SendTargets || AlwaysSend) {
            GetOutput("targets")->PushSample(&Targets);
            SendTargets = false;
        }
    }

    return ret;
}

void svlFilterImageWindowTargetSelect::OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
{
    if (!EnableAdd && !EnableModify) return;

    if (ascii) {
        if (eventid == ' ') {
            SendTargets = true;
            return;
        }
        if (EnableModify && SelectedTargetWindow == static_cast<int>(winid) && SelectedTarget >= 0) {
            switch (eventid) {
                case 'd':
                    Targets.SetFlag(SelectedTarget, 0);
                    SelectedTarget = -1;
                    SelectedTargetWindow = -1;
                break;

                default:
                return;
            }
        }
    }
    else {
        const unsigned int maxtargets = Targets.GetMaxTargets();
        if (maxtargets == 0) return;

        vctDynamicVectorRef<int> confidence;
        vctDynamicMatrixRef<int> position;
        unsigned int i, targetid = -1;
        int curx, cury, x, y, dist, mindist;

        confidence.SetRef(maxtargets, Targets.GetConfidencePointer(winid));
        position.SetRef(2, maxtargets, Targets.GetPositionPointer(winid));

        switch (eventid) {
            case winInput_LBUTTONDOWN:
                GetMousePos(curx, cury);

                if (SelectedTarget < 0) {
                    if (EnableAdd) {
                        // Find an unused target
                        targetid = 0;
                        while (targetid < maxtargets && Targets.GetFlag(targetid) > 0) targetid ++;
                        if (targetid >= maxtargets) return;

                        Targets.SetFlag(targetid, 1);
                        confidence.Element(targetid) = 255;
                        position.Element(0, targetid) = curx;
                        position.Element(1, targetid) = cury;
                        SelectionOffset.SetAll(0);
                        SelectedTarget = targetid;
                        SelectedTargetWindow = winid;

                        // Add other points on other windows to the same location
                        for (i = 0; i < Targets.GetChannels(); i ++) {
                            if (i == winid) continue;
                            confidence.SetRef(maxtargets, Targets.GetConfidencePointer(i));
                            position.SetRef(2, maxtargets, Targets.GetPositionPointer(i));
                            confidence.Element(targetid) = 0;
                            position.Element(0, targetid) = curx;
                            position.Element(1, targetid) = cury;
                        }
                    }
                }
                else {
                    SelectionOffset[0] = position.Element(0, SelectedTarget) - curx;
                    SelectionOffset[1] = position.Element(1, SelectedTarget) - cury;
                }

                ButtonDown = true;
            break;

            case winInput_LBUTTONUP:
                ButtonDown = false;
            break;

            case winInput_MOUSEMOVE:
                GetMousePos(curx, cury);

                if (!ButtonDown) {
                    if (SelectedTargetWindow == static_cast<int>(winid) && SelectedTarget >= 0) {
                        x = position.Element(0, SelectedTarget) - curx;
                        y = position.Element(1, SelectedTarget) - cury;
                        dist = x*x + y*y;
                        if (dist > 100) {
                            SelectedTarget = -1;
                            SelectedTargetWindow = -1;
                        }
                    }
                    if (SelectedTarget < 0) {
                        mindist = 0x7FFFFFFF;
                        for (i = 0; i < maxtargets; i ++) {
                            if (Targets.GetFlag(i) > 0) {
                                x = position.Element(0, i) - curx;
                                y = position.Element(1, i) - cury;
                                dist = x*x + y*y;
                                if (dist < mindist) {
                                    mindist = dist;
                                    targetid = i;
                                }
                            }
                        }
                        if (mindist <= 100) {
                            SelectedTarget = targetid;
                            SelectedTargetWindow = winid;
                        }
                    }
                }
                else {
                    if (SelectedTargetWindow == static_cast<int>(winid) && SelectedTarget >= 0) {
                        Targets.SetFlag(SelectedTarget, 1);
                        confidence.Element(SelectedTarget) = 255;
                        position.Element(0, SelectedTarget) = curx + SelectionOffset[0];
                        position.Element(1, SelectedTarget) = cury + SelectionOffset[1];
                    }
                }
            break;

            default:
            return;
        }
    }
}

