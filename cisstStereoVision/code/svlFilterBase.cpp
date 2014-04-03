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

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>


/*************************************/
/*** svlFilterBase class *************/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterBase, mtsComponent)

svlFilterBase::svlFilterBase() :
    mtsComponent(),
    FrameCounter(0),
    StateTable(3, "StateTable"),
    Enabled(true),
    EnabledInternal(true),
    Initialized(false),
    Running(false),
    AutoType(false),
    PrevInputTimestamp(-1.0)
{
}

svlFilterBase::~svlFilterBase()
{
    svlFilterInput * input;
    mtsComponent::InterfacesInputMapType::iterator iterinputs;
    for (iterinputs = InterfacesInput.begin();
         iterinputs != InterfacesInput.end();
         iterinputs ++) {
        input = dynamic_cast<svlFilterInput *>(iterinputs->second);
        if (input) {
            delete input;
        }
    }
    svlFilterOutput * output;
    mtsComponent::InterfacesOutputMapType::iterator iteroutputs;
    for (iteroutputs = InterfacesOutput.begin();
         iteroutputs != InterfacesOutput.end();
         iteroutputs ++) {
        output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
        if (output) {
            delete output;
        }
    }
}

bool svlFilterBase::IsInitialized() const
{
    return Initialized;
}

bool svlFilterBase::IsRunning() const
{
    return Running;
}

unsigned int svlFilterBase::GetFrameCounter() const
{
    return FrameCounter;
}

svlFilterInput* svlFilterBase::GetInput() const
{
    svlFilterInput * input;
    mtsComponent::InterfacesInputMapType::const_iterator iterinputs;
    for (iterinputs = InterfacesInput.begin();
         iterinputs != InterfacesInput.end();
         iterinputs ++) {
        input = dynamic_cast<svlFilterInput *>(iterinputs->second);
        if (input->Trunk) {
            return input;
        }
    }
    return 0;
}

svlFilterOutput* svlFilterBase::GetOutput() const
{
    svlFilterOutput * output;
    mtsComponent::InterfacesOutputMapType::const_iterator iteroutputs;
    for (iteroutputs = InterfacesOutput.begin();
         iteroutputs != InterfacesOutput.end();
         iteroutputs ++) {
        output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
        if (output->Trunk) {
            return output;
        }
    }
    return 0;
}

svlFilterInput* svlFilterBase::GetInput(const std::string &inputname) const
{
    return dynamic_cast<svlFilterInput *>(GetInterfaceInput(inputname));
}

svlFilterOutput* svlFilterBase::GetOutput(const std::string &outputname) const
{
    return dynamic_cast<svlFilterOutput *>(GetInterfaceOutput(outputname));
}

svlFilterInput* svlFilterBase::AddInput(const std::string &inputname, bool trunk)
{
    if (trunk) {
        svlFilterInput * input;
        mtsComponent::InterfacesInputMapType::const_iterator iterinputs;
        // Check if there is already a trunk input
        for (iterinputs = InterfacesInput.begin();
             iterinputs != InterfacesInput.end();
             iterinputs ++) {
            input = dynamic_cast<svlFilterInput *>(iterinputs->second);
            if (input->Trunk) {
                CMN_LOG_CLASS_INIT_ERROR << "AddInput: there is already a Trunk input for this filter" << std::endl;
                return 0;
            }
        }
    }

    svlFilterInput * input = new svlFilterInput(this, trunk, inputname);
    AddInterfaceInputExisting(inputname, input);
    return input;
}

svlFilterOutput* svlFilterBase::AddOutput(const std::string &outputname, bool trunk)
{
    if (trunk) {
        svlFilterOutput * output;
        mtsComponent::InterfacesOutputMapType::const_iterator iteroutputs;
        // Check if there is already a trunk output
        for (iteroutputs = InterfacesOutput.begin();
             iteroutputs != InterfacesOutput.end();
             iteroutputs ++) {
            output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
            if (output->Trunk) {
                CMN_LOG_CLASS_INIT_ERROR << "AddOutput: there is already a Trunk output for this filter" << std::endl;
                return 0;
            }
        }
    }

    svlFilterOutput * output = new svlFilterOutput(this, trunk, outputname);
    AddInterfaceOutputExisting(outputname, output);
    return output;
}

int svlFilterBase::AddInputType(const std::string &inputname, svlStreamType type)
{
    svlFilterInput * input = GetInput(inputname);
    if (input) {
        return input->AddType(type);
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddInputType: can't find input \"" << inputname << "\"" << std::endl;
    return SVL_FAIL;
}

int svlFilterBase::SetOutputType(const std::string &outputname, svlStreamType type)
{
    svlFilterOutput * output = GetOutput(outputname);
    if (output) {
        return output->SetType(type);
    }
    CMN_LOG_CLASS_INIT_ERROR << "SetOutputType: can't find output \"" << outputname << "\"" << std::endl;
    return SVL_FAIL;
}

void svlFilterBase::SetAutomaticOutputType(bool autotype)
{
    AutoType = autotype;
}

void svlFilterBase::SetEnable(const bool & enable)
{
    Enabled = enable;
    if (!Running) EnabledInternal = enable;
}

void svlFilterBase::GetEnable(bool & enable) const
{
    enable = EnabledInternal;
}

void svlFilterBase::Enable()
{
    Enabled = true;
    if (!Running) EnabledInternal = true;
}

void svlFilterBase::Disable()
{
    Enabled = false;
    if (!Running) EnabledInternal = false;
}

bool svlFilterBase::IsEnabled() const
{
    return EnabledInternal;
}

bool svlFilterBase::IsDisabled() const
{
    return !EnabledInternal;
}

int svlFilterBase::OnConnectInput(svlFilterInput & CMN_UNUSED(input), svlStreamType CMN_UNUSED(type))
{
    // Needs to be overloaded to handle manual type setup
    return SVL_OK;
}

int svlFilterBase::Initialize(svlSample* CMN_UNUSED(syncInput), svlSample* &CMN_UNUSED(syncOutput))
{
    return SVL_OK;
}

int svlFilterBase::OnStart(unsigned int CMN_UNUSED(procCount))
{
    return SVL_OK;
}

int svlFilterBase::Process(svlProcInfo* CMN_UNUSED(procInfo), svlSample* CMN_UNUSED(syncInput), svlSample* &CMN_UNUSED(syncOutput))
{
    return SVL_OK;
}

void svlFilterBase::OnStop()
{
}

int svlFilterBase::Release()
{
    return SVL_OK;
}

int svlFilterBase::IsDataValid(svlStreamType type, svlSample* data)
{
    if (data == 0) return SVL_NO_INPUT_DATA;
    if (type != data->GetType()) return SVL_INVALID_INPUT_TYPE;
    if (data->IsInitialized() == false) return SVL_NO_INPUT_DATA;
    return SVL_OK;
}

bool svlFilterBase::IsNewSample(svlSample* sample)
{
    return (sample && sample->GetTimestamp() > PrevInputTimestamp) ? true : false;
}

