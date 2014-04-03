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

#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>


/***************************************/
/*** svlFilterImageOverlay class *******/
/***************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageOverlay, svlFilterBase)

svlFilterImageOverlay::svlFilterImageOverlay() :
    svlFilterBase(),
    FirstOverlay(0),
    LastOverlay(0),
    ImageInputsToAddUsed(0),
    MatrixInputsToAddUsed(0),
    TargetInputsToAddUsed(0),
    BlobInputsToAddUsed(0),
    TextInputsToAddUsed(0),
    OverlaysToAddUsed(0),
    ImageInputsToAdd(10),
    MatrixInputsToAdd(10),
    TargetInputsToAdd(10),
    BlobInputsToAdd(10),
    TextInputsToAdd(10),
    OverlaysToAdd(10),
    EnableInputSync(true),
    EnableTransformSync(true)
{
    svlFilterBase::AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterImageOverlay::~svlFilterImageOverlay()
{
    _TransformCacheMap::iterator iterxform;
    // Release all blocking threads
    for (iterxform = TransformCache.begin();
         iterxform != TransformCache.end();
         iterxform ++) {
        if (iterxform->second.signal) iterxform->second.signal->Raise();
    }
    // Delete signals
    for (iterxform = TransformCache.begin();
         iterxform != TransformCache.end();
         iterxform ++) {
        if (iterxform->second.signal) {
            delete iterxform->second.signal;
            iterxform->second.signal = 0;
        }
    }
    // TO DO: this needs to be fixed sometime
/*
    while (FirstOverlay) {
        FirstOverlay->Used = false;
        FirstOverlay = FirstOverlay->Next;
    }
*/
}

int svlFilterImageOverlay::AddInputImage(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    CS.Enter();

    while (1) {
        if (IsInputAlreadyQueued(name)) break;

        unsigned int size = static_cast<unsigned int>(ImageInputsToAdd.size());
        ImageInputsToAddUsed ++;
        if (ImageInputsToAddUsed > size) ImageInputsToAdd.resize(size + 10);
        ImageInputsToAdd[ImageInputsToAddUsed - 1] = name;

        break;
    }

    CS.Leave();

    return SVL_OK;
}

int svlFilterImageOverlay::AddInputMatrix(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    CS.Enter();

    while (1) {
        if (IsInputAlreadyQueued(name)) break;

        unsigned int size = static_cast<unsigned int>(MatrixInputsToAdd.size());
        MatrixInputsToAddUsed ++;
        if (MatrixInputsToAddUsed > size) MatrixInputsToAdd.resize(size + 10);
        MatrixInputsToAdd[MatrixInputsToAddUsed - 1] = name;

        break;
    }

    CS.Leave();

    return SVL_OK;
}

int svlFilterImageOverlay::AddInputTargets(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    CS.Enter();

    while (1) {
        if (IsInputAlreadyQueued(name)) break;

        unsigned int size = static_cast<unsigned int>(TargetInputsToAdd.size());
        TargetInputsToAddUsed ++;
        if (TargetInputsToAddUsed > size) TargetInputsToAdd.resize(size + 10);
        TargetInputsToAdd[TargetInputsToAddUsed - 1] = name;

        break;
    }

    CS.Leave();

    return SVL_OK;
}

int svlFilterImageOverlay::AddInputBlobs(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    CS.Enter();

    while (1) {
        if (IsInputAlreadyQueued(name)) break;

        unsigned int size = static_cast<unsigned int>(BlobInputsToAdd.size());
        BlobInputsToAddUsed ++;
        if (BlobInputsToAddUsed > size) BlobInputsToAdd.resize(size + 10);
        BlobInputsToAdd[BlobInputsToAddUsed - 1] = name;

        break;
    }

    CS.Leave();

    return SVL_OK;
}

int svlFilterImageOverlay::AddInputText(const std::string &name)
{
    if (GetInput(name)) return SVL_FAIL;

    CS.Enter();

    while (1) {
        if (IsInputAlreadyQueued(name)) break;

        unsigned int size = static_cast<unsigned int>(TextInputsToAdd.size());
        TextInputsToAddUsed ++;
        if (TextInputsToAddUsed > size) TextInputsToAdd.resize(size + 10);
        TextInputsToAdd[TextInputsToAddUsed - 1] = name;

        break;
    }

    CS.Leave();

    return SVL_OK;
}

void svlFilterImageOverlay::AddOverlay(svlOverlay & overlay)
{
    CS.Enter();

        unsigned int size = static_cast<unsigned int>(OverlaysToAdd.size());
        OverlaysToAddUsed ++;
        if (OverlaysToAddUsed > size) OverlaysToAdd.resize(size + 10);
        OverlaysToAdd[OverlaysToAddUsed - 1] = &overlay;
        overlay.Used = true;

    CS.Leave();
}

int svlFilterImageOverlay::AddQueuedItems()
{
    if (IsRunning()) return SVL_FAIL;
    AddQueuedItemsInternal();
    return SVL_OK;
}

int svlFilterImageOverlay::RemoveOverlay(svlOverlay & overlay)
{
    CS.Enter();

        for (svlOverlay* p_overlay = FirstOverlay;
             p_overlay;
             p_overlay = p_overlay->Next) {

            if (p_overlay == &overlay) {
                if (IsRunning()) {
                    overlay.MarkedForRemoval = svlOverlay::_Remove;
                }
                else {
                    if (&overlay == FirstOverlay) {
                        if (overlay.Prev) {
                            CMN_LOG_CLASS_RUN_WARNING << "RemoveOverlay: FirstOverlay is not first in the overlay queue" << std::endl;
                        }
                        // Remove from linked list
                        FirstOverlay = overlay.Next;
                        if (overlay.Next) overlay.Next->Prev = 0;
                    }
                    else {
                        if (overlay.Prev == 0) {
                            CS.Leave();

                            CMN_LOG_CLASS_RUN_ERROR << "RemoveOverlay: pointer to previous transformation is zero" << std::endl;
                            return SVL_FAIL;
                        }
                        // Remove from linked list
                        overlay.Prev->Next = overlay.Next;
                        if (overlay.Next) overlay.Next->Prev = overlay.Prev;
                    }
                    // Reset overlay connections
                    overlay.Prev = 0;
                    overlay.Next = 0;
                    overlay.MarkedForRemoval = svlOverlay::_DoNotRemove;
                }

                CS.Leave();
                return SVL_OK;
            }
        }

    CS.Leave();

    CMN_LOG_CLASS_RUN_WARNING << "RemoveOverlay: overlay not registered to this overlay filter" << std::endl;
    return SVL_FAIL;
}

int svlFilterImageOverlay::RemoveAndDeleteOverlay(svlOverlay* overlay)
{
    if (!overlay) return SVL_FAIL;

    CS.Enter();

        for (svlOverlay* p_overlay = FirstOverlay;
             p_overlay;
             p_overlay = p_overlay->Next) {

            if (p_overlay == overlay) {
                if (IsRunning()) {
                    overlay->MarkedForRemoval = svlOverlay::_RemoveAndDelete;
                }
                else {
                    if (overlay == FirstOverlay) {
                        if (overlay->Prev) {
                            CMN_LOG_CLASS_RUN_WARNING << "RemoveAndDeleteOverlay: FirstOverlay is not first in the overlay queue" << std::endl;
                        }
                        // Remove from linked list
                        FirstOverlay = overlay->Next;
                        if (overlay->Next) overlay->Next->Prev = 0;
                    }
                    else {
                        if (overlay->Prev == 0) {
                            CS.Leave();

                            CMN_LOG_CLASS_RUN_ERROR << "RemoveAndDeleteOverlay: pointer to previous transformation is zero" << std::endl;
                            return SVL_FAIL;
                        }
                        // Remove from linked list
                        overlay->Prev->Next = overlay->Next;
                        if (overlay->Next) overlay->Next->Prev = overlay->Prev;
                    }
                    // Delete object
                    delete overlay;
                }

                CS.Leave();
                return SVL_OK;
            }
        }

    CS.Leave();

    CMN_LOG_CLASS_RUN_WARNING << "RemoveAndDeleteOverlay: overlay not registered to this overlay filter" << std::endl;
    return SVL_FAIL;
}

void svlFilterImageOverlay::SetEnableInputSync(bool enabled)
{
    EnableInputSync = enabled;
}

bool svlFilterImageOverlay::GetEnableInputSync() const
{
    return EnableInputSync;
}

void svlFilterImageOverlay::SetEnableTransformSync(bool enabled)
{
    EnableTransformSync = enabled;
}

bool svlFilterImageOverlay::GetEnableTransformSync() const
{
    return EnableTransformSync;
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

    _OnSingleThread(procInfo)
    {
        double current_time = syncInput->GetTimestamp();

        // Add queued inputs and overlays in a thread safe manner
        if (ImageInputsToAddUsed  ||
            MatrixInputsToAddUsed ||
            TargetInputsToAddUsed ||
            BlobInputsToAddUsed   ||
            TextInputsToAddUsed   ||
            OverlaysToAddUsed) AddQueuedItemsInternal();

        // Update transformations
        TransformCS.Enter();
            svlOverlay* overlay = FirstOverlay;
            for (; overlay; overlay = overlay->Next) {
                if (overlay->TransformID >= 0) {

                    _TransformCacheMap::iterator iterxform;
                    iterxform = TransformCache.find(overlay->TransformID);
                    if (iterxform != TransformCache.end()) {

                        if (!EnableTransformSync ||
                            !overlay->GetTransformSynchronized()) {
                            overlay->SetTransform(iterxform->second.frame, iterxform->second.timestamp);
                        }
                        else {
                            while (EnableTransformSync &&
                                   overlay->GetTransformSynchronized() &&
                                   (iterxform->second.timestamp < current_time)) {

                                // Transform is not recent
                                bool success = false;
                                TransformCS.Leave();
                                    // Wait for new transform
                                    while (!success &&
                                           overlay->GetVisible() &&
                                           IsRunning() &&
                                           EnableTransformSync &&
                                           overlay->GetTransformSynchronized() &&
                                           iterxform->second.signal) {
                                        success = iterxform->second.signal->Wait(0.1);
                                    }
                                TransformCS.Enter();
                                if (!success) break;
                            }
                            overlay->SetTransform(iterxform->second.frame, iterxform->second.timestamp);
                        }
                    }
                    else {
                        CMN_LOG_CLASS_RUN_DEBUG << "Process: failed to find transformation: " << overlay->TransformID << std::endl;
                    }
                }
            }
        TransformCS.Leave();

        _SampleCacheMap::iterator itersample;
        svlSampleImage* src_image = dynamic_cast<svlSampleImage*>(syncInput);
        svlOverlayInput* overlayinput = 0;
        svlFilterInput* input = 0;
        svlSample* ovrlsample = 0;
        svlOverlay* t_overlay = 0;

        overlay = FirstOverlay;
        while (overlay) {

            if (overlay->MarkedForRemoval == svlOverlay::_Remove) {
                t_overlay = overlay->Next;
                RemoveOverlayInternal(overlay);
                overlay = t_overlay;
                continue;
            }
            else if (overlay->MarkedForRemoval == svlOverlay::_RemoveAndDelete) {
                t_overlay = overlay->Next;
                RemoveAndDeleteOverlayInternal(overlay);
                overlay = t_overlay;
                continue;
            }

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
                        if (ovrlsample) {
                            if (EnableInputSync && overlayinput->GetInputSynchronized()) {
                                if (ovrlsample->GetTimestamp() >= current_time) {
                                // Sample is most recent
                                    overlay->Draw(src_image, ovrlsample);
                                }
                                else {
                                // Sample is not recent
                                    do {
                                        ovrlsample = input->PullSample(true, 0.025);
                                        if (ovrlsample) itersample->second = ovrlsample;
                                    }
                                    while (IsRunning() &&
                                           EnableInputSync &&
                                           overlayinput->GetInputSynchronized() &&
                                           (!ovrlsample || ovrlsample->GetTimestamp() < current_time));
                                    if (IsRunning()) overlay->Draw(src_image, ovrlsample);
                                }
                            }
                            else {
                                overlay->Draw(src_image, ovrlsample);
                            }
                        }
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

void svlFilterImageOverlay::OnStop()
{
    // Remove overlay objects that we didn't have a chance to remove earlier
    svlOverlay *overlay = FirstOverlay, *t_overlay = 0;
    while (overlay) {
        if (overlay->MarkedForRemoval == svlOverlay::_Remove) {
            t_overlay = overlay->Next;
            RemoveOverlayInternal(overlay);
            overlay = t_overlay;
            continue;
        }
        else if (overlay->MarkedForRemoval == svlOverlay::_RemoveAndDelete) {
            t_overlay = overlay->Next;
            RemoveAndDeleteOverlayInternal(overlay);
            overlay = t_overlay;
            continue;
        }
        overlay = overlay->Next;
    }
}

void svlFilterImageOverlay::CreateInterfaces()
{
    mtsInterfaceProvided* provided = AddInterfaceProvided("Transformations", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterImageOverlay::SetTransform,  this, "SetTransform");
        provided->AddCommandWrite(&svlFilterImageOverlay::SetTransforms, this, "SetTransforms");
    }
}

void svlFilterImageOverlay::SetTransform(const ThisType::ImageTransform & transform)
{
    TransformCS.Enter();
        _TransformCacheMap::iterator iterxform;
        iterxform = TransformCache.find(transform.ID);
        if (iterxform != TransformCache.end()) {
            iterxform->second.frame     = transform.frame;
            iterxform->second.timestamp = transform.timestamp;
            if (iterxform->second.signal) iterxform->second.signal->Raise();
        }
        else {
            TransformInternal transform_internal;
            transform_internal.frame     = transform.frame;
            transform_internal.timestamp = transform.timestamp;
            transform_internal.signal    = new osaThreadSignal;
            TransformCache[transform.ID] = transform_internal;
            if (transform_internal.signal) transform_internal.signal->Raise();
            CMN_LOG_CLASS_RUN_DEBUG << "SetTransform - new transformation added: " << transform.ID << std::endl;
        }
    TransformCS.Leave();
}

void svlFilterImageOverlay::SetTransforms(const vctDynamicVector<ThisType::ImageTransform> & transforms)
{
    TransformCS.Enter();
        for (unsigned int i = 0; i < transforms.size(); i ++) {
            TransformInternal transform_internal;
            _TransformCacheMap::iterator iterxform;
            iterxform = TransformCache.find(transforms[i].ID);
            if (iterxform != TransformCache.end()) {
                iterxform->second.frame     = transforms[i].frame;
                iterxform->second.timestamp = transforms[i].timestamp;
                if (iterxform->second.signal) iterxform->second.signal->Raise();
            }
            else {
                transform_internal.frame     = transforms[i].frame;
                transform_internal.timestamp = transforms[i].timestamp;
                transform_internal.signal    = new osaThreadSignal;
                TransformCache[transforms[i].ID] = transform_internal;
                if (transform_internal.signal) transform_internal.signal->Raise();
                CMN_LOG_CLASS_RUN_DEBUG << "SetTransforms - new transformation added: " << transforms[i].ID << std::endl;
            }
        }
    TransformCS.Leave();
}

bool svlFilterImageOverlay::IsInputAlreadyQueued(const std::string &name)
{
    unsigned int i;
    for (i = 0; i < ImageInputsToAddUsed;  i ++) if (ImageInputsToAdd[i]  == name) return true;
    for (i = 0; i < MatrixInputsToAddUsed; i ++) if (MatrixInputsToAdd[i] == name) return true;
    for (i = 0; i < TargetInputsToAddUsed; i ++) if (TargetInputsToAdd[i] == name) return true;
    for (i = 0; i < BlobInputsToAddUsed;   i ++) if (BlobInputsToAdd[i]   == name) return true;
    for (i = 0; i < TextInputsToAddUsed;   i ++) if (TextInputsToAdd[i]   == name) return true;
    return false;
}

void svlFilterImageOverlay::AddQueuedItemsInternal()
{
    CS.Enter();

    unsigned int i;

    for (i = 0; i < ImageInputsToAddUsed; i ++) {
        svlFilterInput* input = AddInput(ImageInputsToAdd[i], false);
        if (!input) continue;
        input->AddType(svlTypeImageRGB);
        input->AddType(svlTypeImageRGBStereo);
        SampleCache[input] = 0;
    }

    for (i = 0; i < MatrixInputsToAddUsed; i ++) {
        svlFilterInput* input = AddInput(MatrixInputsToAdd[i], false);
        if (!input) continue;
        input->AddType(svlTypeMatrixInt8);
        input->AddType(svlTypeMatrixInt16);
        input->AddType(svlTypeMatrixInt32);
        input->AddType(svlTypeMatrixInt64);
        input->AddType(svlTypeMatrixUInt8);
        input->AddType(svlTypeMatrixUInt16);
        input->AddType(svlTypeMatrixUInt32);
        input->AddType(svlTypeMatrixUInt64);
        input->AddType(svlTypeMatrixFloat);
        input->AddType(svlTypeMatrixDouble);
        SampleCache[input] = 0;
    }

    for (i = 0; i < TargetInputsToAddUsed; i ++) {
        svlFilterInput* input = AddInput(TargetInputsToAdd[i], false);
        if (!input) continue;
        input->AddType(svlTypeTargets);
        SampleCache[input] = 0;
    }

    for (i = 0; i < BlobInputsToAddUsed; i ++) {
        svlFilterInput* input = AddInput(BlobInputsToAdd[i], false);
        if (!input) continue;
        input->AddType(svlTypeBlobs);
        SampleCache[input] = 0;
    }

    for (i = 0; i < TextInputsToAddUsed; i ++) {
        svlFilterInput* input = AddInput(TextInputsToAdd[i], false);
        if (!input) continue;
        input->AddType(svlTypeText);
        SampleCache[input] = 0;
    }

    for (i = 0; i < OverlaysToAddUsed; i ++) {
        if (LastOverlay) {
            LastOverlay->Next = OverlaysToAdd[i];
            OverlaysToAdd[i]->Prev = LastOverlay;
            LastOverlay = OverlaysToAdd[i];
            LastOverlay->Next = 0;
        }
        else {
            FirstOverlay = LastOverlay = OverlaysToAdd[i];
            FirstOverlay->Prev = 0;
            FirstOverlay->Next = 0;
        }
    }

    ImageInputsToAddUsed  = 0;
    MatrixInputsToAddUsed = 0;
    TargetInputsToAddUsed = 0;
    BlobInputsToAddUsed   = 0;
    TextInputsToAddUsed   = 0;
    OverlaysToAddUsed     = 0;

    CS.Leave();
}

void svlFilterImageOverlay::RemoveOverlayInternal(svlOverlay* overlay)
{
    CS.Enter();

        if (overlay == FirstOverlay) {
            if (overlay->Prev) {
                CMN_LOG_CLASS_RUN_WARNING << "RemoveAndDeleteOverlayInternal: FirstOverlay is not first in the overlay queue" << std::endl;
            }
            // Remove from linked list
            FirstOverlay = overlay->Next;
            if (overlay->Next) overlay->Next->Prev = 0;
        }
        else {
            if (overlay->Prev == 0) {
                CS.Leave();

                CMN_LOG_CLASS_RUN_ERROR << "RemoveAndDeleteOverlayInternal: pointer to previous transformation is zero" << std::endl;
                return;
            }
            // Remove from linked list
            overlay->Prev->Next = overlay->Next;
            if (overlay->Next) overlay->Next->Prev = overlay->Prev;
        }
        // Reset overlay connections
        overlay->Prev = 0;
        overlay->Next = 0;
        overlay->MarkedForRemoval = svlOverlay::_DoNotRemove;

    CS.Leave();
}

void svlFilterImageOverlay::RemoveAndDeleteOverlayInternal(svlOverlay* overlay)
{
    CS.Enter();

        if (overlay == FirstOverlay) {
            if (overlay->Prev) {
                CMN_LOG_CLASS_RUN_WARNING << "RemoveAndDeleteOverlayInternal: FirstOverlay is not first in the overlay queue" << std::endl;
            }
            // Remove from linked list
            FirstOverlay = overlay->Next;
            if (overlay->Next) overlay->Next->Prev = 0;
        }
        else {
            if (overlay->Prev == 0) {
                CS.Leave();

                CMN_LOG_CLASS_RUN_ERROR << "RemoveAndDeleteOverlayInternal: pointer to previous transformation is zero" << std::endl;
                return;
            }
            // Remove from linked list
            overlay->Prev->Next = overlay->Next;
            if (overlay->Next) overlay->Next->Prev = overlay->Prev;
        }
        // Delete object
        delete overlay;

    CS.Leave();
}
