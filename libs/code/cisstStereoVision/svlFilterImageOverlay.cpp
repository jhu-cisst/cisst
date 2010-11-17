/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

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

#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>

/***************************************/
/*** svlFilterImageOverlay class *******/
/***************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageOverlay)

svlFilterImageOverlay::svlFilterImageOverlay() :
    svlFilterBase(),
    FirstOverlay(0),
    LastOverlay(0),
    OverlaysToAddUsed(0),
    OverlaysToAdd(10)
{
    svlFilterBase::AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
//    AddInputType("input", svlTypeImageRGBA);
    AddInputType("input", svlTypeImageRGBStereo);
//    AddInputType("input", svlTypeImageRGBAStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterImageOverlay::~svlFilterImageOverlay()
{
}

int svlFilterImageOverlay::AddInputImage(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    svlFilterInput* input = AddInput(name, false);
    if (!input) return SVL_FAIL;

    input->AddType(svlTypeImageRGB);
//    input->AddType(svlTypeImageRGBA);
    input->AddType(svlTypeImageRGBStereo);
//    input->AddType(svlTypeImageRGBAStereo);

    SampleCache[input] = 0;

    return SVL_OK;
}

int svlFilterImageOverlay::AddInputTargets(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    svlFilterInput* input = svlFilterBase::AddInput(name, false);
    if (!input) return SVL_FAIL;

    input->AddType(svlTypeTargets);

    SampleCache[input] = 0;

    return SVL_OK;
}

int svlFilterImageOverlay::AddInputText(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    svlFilterInput* input = svlFilterBase::AddInput(name, false);
    if (!input) return SVL_FAIL;

    input->AddType(svlTypeText);

    SampleCache[input] = 0;

    return SVL_OK;
}

void svlFilterImageOverlay::AddOverlay(svlOverlay & overlay)
{
    CS.Enter();

        unsigned int size = OverlaysToAdd.size();

        OverlaysToAddUsed ++;
        if (OverlaysToAddUsed > size) OverlaysToAdd.resize(size + 10);

        OverlaysToAdd[OverlaysToAddUsed - 1] = &overlay;

    CS.Leave();
}

int svlFilterImageOverlay::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterImageOverlay::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfDisabled();

    _OnSingleThread(procInfo) {
        // Add pending overlays in a thread safe manner
        if (OverlaysToAddUsed) AddOverlaysInternal();

        _SampleCacheMap::iterator itersample;
        svlSampleImage* src_image = dynamic_cast<svlSampleImage*>(syncInput);
        svlOverlayInput* overlayinput = 0;
        svlFilterInput* input = 0;
        svlOverlay* overlay = FirstOverlay;
        svlSample* ovrlsample = 0;

        while (overlay) {
            // Cross casting to the input base class
            overlayinput = dynamic_cast<svlOverlayInput*>(overlay);
            if (overlayinput) {
            // Overlays with input

                input = GetInput(overlayinput->InputName);
                if (input && overlayinput->IsInputTypeValid(input->GetType())) {

                    itersample = SampleCache.find(input);
                    if (itersample != SampleCache.end()) {
                        ovrlsample = input->PullSample(true, 0.0);
                        if (ovrlsample) itersample->second = ovrlsample;
                        else ovrlsample = itersample->second;
                        if (ovrlsample) overlay->Draw(src_image, ovrlsample);
                    }
                }
            }
            else {
            // Overlays without input
                overlay->Draw(src_image, 0);
            }

            overlay = overlay->Next;
        }
    }

    return SVL_OK;
}

void svlFilterImageOverlay::AddOverlaysInternal()
{
    CS.Enter();

        for (unsigned int i = 0; i < OverlaysToAddUsed; i ++) {
            if (LastOverlay) {
                LastOverlay->Next = OverlaysToAdd[i];
                OverlaysToAdd[i]->Prev = LastOverlay;
                LastOverlay = OverlaysToAdd[i];
            }
            else {
                FirstOverlay = LastOverlay = OverlaysToAdd[i];
            }
        }

        OverlaysToAddUsed = 0;

    CS.Leave();
}

