/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2006

  (C) Copyright 2006-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlSyncPoint.h>
#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlFilterSourceBase.h>
#include <cisstStereoVision/svlStreamProc.h>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaCriticalSection.h>

#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerLocal.h>

/*************************************/
/*** svlStreamManager class **********/
/*************************************/

CMN_IMPLEMENT_SERVICES(svlStreamManager);

svlStreamManager::svlStreamManager() :
    ThreadCount(1),
    SyncPoint(0),
    CS(0),
    StreamSource(0),
    Initialized(false),
    Running(false),
    StreamStatus(SVL_STREAM_CREATED)
{
    CreateInterfaces();
}

svlStreamManager::svlStreamManager(unsigned int threadcount) :
    ThreadCount(std::max(1u, threadcount)),
    SyncPoint(0),
    CS(0),
    StreamSource(0),
    Initialized(false),
    Running(false),
    StreamStatus(SVL_STREAM_CREATED)
{
    CreateInterfaces();
    // To do: autodetect the number of available processor cores
}

svlStreamManager::~svlStreamManager()
{
    Release();
}

int svlStreamManager::SetSourceFilter(svlFilterSourceBase * source)
{
    if (source == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetSourceFilter: null source pointer provided for stream \""
                                 << this->GetName() << "\"" << std::endl;
        return SVL_FAIL;
    }
    if (source == StreamSource) {
        CMN_LOG_CLASS_INIT_DEBUG << "SetSourceFilter: same pointer provided for stream \""
                                 << this->GetName() << "\"" << std::endl;
        return SVL_OK;
    }
    if (Initialized) {
        CMN_LOG_CLASS_INIT_ERROR << "SetSourceFilter: stream \"" << this->GetName()
                                 << "\" is already initialized, can't change associated source filter" << std::endl;        
        return SVL_ALREADY_INITIALIZED;
    }
    
    // PK TEMP: This could be done elsewhere
    mtsManagerLocal::GetInstance()->AddComponent(this);

    StreamSource = source;
    mtsManagerLocal::GetInstance()->AddComponent(StreamSource);
    CMN_LOG_CLASS_INIT_DEBUG << "SetSourceFilter: filter \"" << source->GetName() << "\" associated to stream \""
                             << this->GetName() << "\"" << std::endl;
    return SVL_OK;
}


int svlStreamManager::Initialize(void)
{
    if (Initialized) {
        CMN_LOG_CLASS_INIT_WARNING << "Initialize: stream \"" << this->GetName()
                                   << "\" is already initialized" << std::endl;
        return SVL_ALREADY_INITIALIZED;
    }

    svlSample *inputsample, *outputsample = 0;
    svlFilterSourceBase * source = StreamSource;
    svlFilterBase *prevfilter, *filter;
    mtsComponent::InterfacesOutputMapType::iterator iteroutputs;
    svlFilterOutput * output;
    svlFilterInput * input;
    int err;

    if (source == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: stream \"" << this->GetName()
                                 << "\" is not associated to any source" << std::endl;
        return SVL_NO_SOURCE_IN_LIST;
    }

    // Initialize the stream, starting from the stream source
    err = source->Initialize(outputsample);
    if (err != SVL_OK) {
        Release();
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: stream \"" << this->GetName()
                                 << "\" failed to initialize source \""
                                 << source->GetName() << "\"" << std::endl;

        return err;
    }
    source->Initialized = true;

    // Initialize non-trunk filter outputs
    for (iteroutputs = source->InterfacesOutput.begin();
         iteroutputs != source->InterfacesOutput.end();
         iteroutputs ++) {
        output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
        if (output) {
            if (!output->IsTrunk() && output->Stream) {
                err = output->Stream->Initialize();
                if (err != SVL_OK) {
                    Release();
                    CMN_LOG_CLASS_INIT_ERROR << "Initialize: stream \"" << this->GetName()
                                             << "\" failed to initialize stream \""
                                             << output->Stream->GetName() << "\"" << std::endl;
                    return err;
                }
            }
        }
    }

    prevfilter = source;

    // Get next filter in the trunk
    output = source->GetOutput();
    filter = 0;
    // Check if trunk output exists
    if (output) {
        input = output->Connection;
        // Check if trunk output is connected to a trunk input
        if (input && input->Trunk) filter = input->Filter;
    }

    // Going downstream filter by filter
    while (filter != 0) {

        // Pass samples downstream
        inputsample = outputsample; outputsample = 0;

        // Check if the previous output is valid input for the next filter
        err = filter->IsDataValid(filter->GetInput()->GetType(), inputsample);
        if (err != SVL_OK) {
            Release();
            CMN_LOG_CLASS_INIT_ERROR << "Initialize: stream \"" << this->GetName()
                                     << "\" found invalid data for filter \""
                                     << output->GetName() << "\"" << std::endl;
            return err;
        }
        err = filter->Initialize(inputsample, outputsample);
        if (err != SVL_OK) {
            Release();
            CMN_LOG_CLASS_INIT_ERROR << "Initialize: stream \"" << this->GetName()
                                     << "\" failed to initialize filter \""
                                     << filter->GetName() << "\"" << std::endl;
            return err;
        }
        filter->Initialized = true;

        // Initialize non-trunk filter outputs
        for (iteroutputs = filter->InterfacesOutput.begin();
             iteroutputs != filter->InterfacesOutput.end();
             iteroutputs ++) {
            output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
            if (output) {
                if (!output->IsTrunk() && output->Stream) {
                    err = output->Stream->Initialize();
                    if (err != SVL_OK) {
                        Release();
                        return err;
                    }
                }
            }
        }

        prevfilter = filter;

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    Initialized = true;
    StreamStatus = SVL_STREAM_INITIALIZED;
    CMN_LOG_CLASS_INIT_DEBUG << "Initialize: stream \"" << this->GetName() << "\" initialized" << std::endl;
    return SVL_OK;
}

void svlStreamManager::Release(void)
{
    if (!Initialized) {
        CMN_LOG_CLASS_INIT_DEBUG << "Release: stream \"" << this->GetName()
                                 << "\" is not initialized, release is not necessary" << std::endl;
        return;
    }

    // stop the stream to make sure we are in a consistent state
    Stop();

    size_t i;
    mtsComponent::InterfacesOutputMapType::iterator iteroutputs;
    svlFilterOutput * output;
    svlFilterInput * input;

    // There might be a thread object still open (in case of an internal shutdown)
    for (i = 0; i < StreamProcInstance.size(); i ++) {
        if (StreamProcInstance[i]) {
            delete StreamProcInstance[i];
            StreamProcInstance[i] = 0;
        }
    }
    for (i = 0; i < StreamProcThread.size(); i ++) {
        if (StreamProcThread[i]) {
            delete StreamProcThread[i];
            StreamProcThread[i] = 0;
        }
    }

    // Release the stream, starting from the stream source
    svlFilterBase *filter = StreamSource;
    while (filter) {

        // Release filter
        if (filter->Initialized) {
            if (filter->Release() != SVL_OK) {
                CMN_LOG_CLASS_RUN_WARNING << "Release: stream \"" << this->GetName()
                                          << "\" ran into an error while releasing filter \""
                                          << filter->GetName() << "\"" << std::endl;
            }
            filter->Initialized = false;
        }

        // Release non-trunk filter outputs
        for (iteroutputs = filter->InterfacesOutput.begin();
             iteroutputs != filter->InterfacesOutput.end();
             iteroutputs ++) {
            output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
            if (output) {
                if (!output->IsTrunk() && output->Stream) {
                    output->Stream->Release();
                }
            }
        }

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    Initialized = false;
    StreamStatus = SVL_STREAM_RELEASED;
}


bool svlStreamManager::IsInitialized(void) const
{
    return Initialized;
}

int svlStreamManager::Play(void)
{
    if (Running) {
        CMN_LOG_CLASS_RUN_WARNING << "Play: stream \"" << this->GetName()
                                  << "\" already running" << std::endl;
        return SVL_ALREADY_RUNNING;
    }

    int err;
    size_t i;
    mtsComponent::InterfacesOutputMapType::iterator iteroutputs;
    svlFilterOutput * output;
    svlFilterInput * input;

    if (!Initialized) {
        // Try to initialize it if it hasn't been done before
        err = Initialize();
        if (err != SVL_OK) {
            CMN_LOG_CLASS_RUN_ERROR << "Play: stream \"" << this->GetName()
                                    << "\" failed to initialize" << std::endl;
            return err;
        }
    }

    Running = true;
    // PK TEMP: Send event indicating that stream source is now running
    this->State = mtsComponentState::ACTIVE;
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    EventGeneratorChangeState(mtsComponentStateChange(LCM->GetProcessName(), this->GetName(), 
                                                      mtsComponentState::ACTIVE));

    // Call OnStart for all filters in the trunk
    svlFilterBase * filter = StreamSource;
    while (filter) {
        filter->Running = true;
        if (filter->OnStart(ThreadCount) != SVL_OK) {
            Stop();
            CMN_LOG_CLASS_RUN_ERROR << "Play: filter \"" << filter->GetName()
                                    << "\" \"OnStart\" method failed while starting stream \""
                                    << this->GetName() << "\"" << std::endl;
            return SVL_FAIL;
        }
        // PK TEMP: Send event indicating that filter is now running
        filter->State = mtsComponentState::ACTIVE;
        EventGeneratorChangeState(mtsComponentStateChange(LCM->GetProcessName(), filter->GetName(), 
                                                          mtsComponentState::ACTIVE));

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    // There might be a thread object still open (in case of an internal shutdown)
    for (i = 0; i < StreamProcInstance.size(); i ++) {
        if (StreamProcInstance[i]) {
            delete StreamProcInstance[i];
            StreamProcInstance[i] = 0;
        }
    }
    for (i = 0; i < StreamProcThread.size(); i ++) {
        if (StreamProcThread[i]) {
            delete StreamProcThread[i];
            StreamProcThread[i] = 0;
        }
    }

    // Allocate new thread control object array
    StreamProcInstance.SetSize(ThreadCount);
    StreamProcThread.SetSize(ThreadCount);

    // Create thread synchronization object
    if (ThreadCount > 1) {
        SyncPoint = new svlSyncPoint;
        SyncPoint->Count(ThreadCount);
        CS = new osaCriticalSection;
    }

    StopThread = false;
    StreamStatus = SVL_STREAM_RUNNING;

    // Initialize media control events
    if (StreamSource->PlayCounter != 0) StreamSource->PauseAtFrameID = -1;
    else StreamSource->PauseAtFrameID = 0;

    for (i = 0; i < ThreadCount; i ++) {
        // Starting multi thread processing
        StreamProcInstance[i] = new svlStreamProc(ThreadCount, static_cast<unsigned int>(i));
        StreamProcThread[i] = new osaThread;
        StreamProcThread[i]->Create<svlStreamProc, svlStreamManager*>(StreamProcInstance[i], &svlStreamProc::Proc, this);
    }

    // Start all filter outputs recursively, if any
    filter = StreamSource;
    while (filter != 0) {

        // Start non-trunk filter outputs
        for (iteroutputs = filter->InterfacesOutput.begin();
             iteroutputs != filter->InterfacesOutput.end();
             iteroutputs ++) {
            output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
            if (output) {
                if (!output->IsTrunk() && output->Stream) {
                    err = output->Stream->Play();
                    if (err != SVL_OK) {
                        CMN_LOG_CLASS_RUN_ERROR << "Play: stream \"" << output->Stream->GetName()
                                                << "\" Play method failed while starting stream \""
                                                << this->GetName() << "\"" << std::endl;
                        Release();
                        return err;
                    }
                }
            }
        }

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "Play: stream \"" << this->GetName() << "\" started" << std::endl;
    StreamStatus = SVL_STREAM_RUNNING;
    return SVL_OK;
}


void svlStreamManager::Stop(void)
{
    if (!Running) return;

    mtsComponent::InterfacesOutputMapType::iterator iteroutputs;
    svlFilterOutput * output;
    svlFilterInput * input;

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    // Stop all filter outputs recursively, if any
    svlFilterBase *filter = StreamSource;
    while (filter != 0) {

        // Stop non-trunk filter outputs
        for (iteroutputs = filter->InterfacesOutput.begin();
             iteroutputs != filter->InterfacesOutput.end();
             iteroutputs ++) {
            output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
            if (output) {
                if (!output->IsTrunk() && output->Stream) {
                    output->Stream->Stop();
                }
            }
        }

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    // Set running flags to false
    filter = StreamSource;
    while (filter) {
        filter->Running = false;

        // PK TEMP: Send event indicating that filter is now stopped
        filter->State = mtsComponentState::READY;
        EventGeneratorChangeState(mtsComponentStateChange(LCM->GetProcessName(), filter->GetName(), 
                                                          mtsComponentState::READY));

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    Running = false;
    StopThread = true;

    // PK TEMP: Send event indicating that stream source is now stopped
    this->State = mtsComponentState::READY;
    EventGeneratorChangeState(mtsComponentStateChange(LCM->GetProcessName(), this->GetName(), 
                                                      mtsComponentState::READY));

    // Stopping multi thread processing and delete thread objects
    for (size_t i = 0; i < ThreadCount; i ++) {
        if (StreamProcThread[i]) {
            StreamProcThread[i]->Wait();
            delete StreamProcThread[i];
            StreamProcThread[i] = 0;
        }
        if (StreamProcInstance[i]) {
            delete StreamProcInstance[i];
            StreamProcInstance[i] = 0;
        }
    }

    // Release thread control arrays and objects
    StreamProcThread.SetSize(0);
    StreamProcInstance.SetSize(0);
    if (SyncPoint) {
        delete SyncPoint;
        SyncPoint = 0;
    }
    if (CS) {
        delete CS;
        CS = 0;
    }

    // Call OnStop for all filters in the trunk
    filter = StreamSource;
    while (filter) {
        filter->OnStop();

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    StreamStatus = SVL_STREAM_STOPPED;
}

void svlStreamManager::InternalStop(unsigned int callingthreadID)
{
    if (!Running) return;

    mtsComponent::InterfacesOutputMapType::iterator iteroutputs;
    svlFilterOutput * output;
    svlFilterInput * input;

    // Stop all filter outputs recursively, if any
    svlFilterBase * filter = StreamSource;
    while (filter != 0) {

        // Stop non-trunk filter outputs
        for (iteroutputs = filter->InterfacesOutput.begin();
             iteroutputs != filter->InterfacesOutput.end();
             iteroutputs ++) {
            output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
            if (!output->IsTrunk() && output->Stream) {
                output->Stream->Stop();
            }
        }

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    filter = StreamSource;
    while (filter) {
        filter->Running = false;

        // PK TEMP: Send event indicating that filter is now stopped
        filter->State = mtsComponentState::READY;
        EventGeneratorChangeState(mtsComponentStateChange(LCM->GetProcessName(), filter->GetName(), 
                                                          mtsComponentState::READY));

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }

    Running = false;
    StopThread = true;

    // PK TEMP: Send event indicating that stream source is now stopped
    this->State = mtsComponentState::READY;
    EventGeneratorChangeState(mtsComponentStateChange(LCM->GetProcessName(), this->GetName(), 
                                                      mtsComponentState::READY));

    // Stopping multi thread processing and delete thread objects
    for (size_t i = 0; i < ThreadCount; i ++) {
        if (i != callingthreadID) {
            if (StreamProcThread[i]) {
                StreamProcThread[i]->Wait();
                delete StreamProcThread[i];
                StreamProcThread[i] = 0;
            }
            if (StreamProcInstance[i]) {
                delete StreamProcInstance[i];
                StreamProcInstance[i] = 0;
            }
        }
        else {
            // Skip calling thread!
            // That will be deleted at next Start() or Release()
        }
    }

    // Release thread control arrays and objects
    if (SyncPoint) {
        delete SyncPoint;
        SyncPoint = 0;
    }
    if (CS) {
        delete CS;
        CS = 0;
    }

    // Call OnStop for all filters in the trunk
    filter = StreamSource;
    while (filter) {
        filter->OnStop();

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
        }
    }
}

bool svlStreamManager::IsRunning(void) const
{
    return Running;
}

int svlStreamManager::WaitForStop(double timeout)
{
    if (timeout < 0.0) timeout = 1000000000.0;
    while (Running && timeout > 0.0) {
        timeout -= 0.2;
        osaSleep(0.2);
    }
    if (!Running) return SVL_OK;
    else return SVL_WAIT_TIMEOUT;
}

int svlStreamManager::GetStreamStatus(void) const
{
    return StreamStatus;
}

void svlStreamManager::DisconnectAll(void)
{
    // First make sure that the stream is released
    Release();

    mtsComponent::InterfacesOutputMapType::iterator iteroutputs;
    svlFilterOutput * output;
    svlFilterInput * input;

    // Disconnect all filters
    svlFilterBase *filter = StreamSource;
    while (filter) {

        // Disconnect non-trunk filter outputs
        for (iteroutputs = filter->InterfacesOutput.begin();
             iteroutputs != filter->InterfacesOutput.end();
             iteroutputs ++) {
            output = dynamic_cast<svlFilterOutput *>(iteroutputs->second);
            if (output) {
                if (!output->IsTrunk()) {
                    // Call DisconnectAll() on branch recursively
                    if (output->Stream) output->Stream->DisconnectAll();
                    // Disconnect async output
                    output->Disconnect();
                }
            }
        }

        // Get next filter in the trunk
        output = filter->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected to a trunk input
            if (input && input->Trunk) filter = input->Filter;
            output->Disconnect();
        }
    }
}

void svlStreamManager::CreateInterfaces(void)
{
    mtsInterfaceProvided * interfaceProvided = this->AddInterfaceProvided("Control", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (interfaceProvided) {
        interfaceProvided->AddCommandWrite(&svlStreamManager::SetSourceFilterCommand, this, "SetSourceFilter");
        interfaceProvided->AddCommandVoid(&svlStreamManager::PlayCommand, this, "Play");
        interfaceProvided->AddCommandVoid(&svlStreamManager::InitializeCommand, this, "Initialize");
        interfaceProvided->AddCommandVoid(&svlStreamManager::Release, this, "Release");
    }
}

void svlStreamManager::PlayCommand(void)
{
    if (this->Play() != SVL_OK) {
        CMN_LOG_CLASS_RUN_ERROR << "PlayCommand: error occurred in method \"Play\" for stream \""
                                << this->GetName() << "\"" << std::endl;
    }
}

void svlStreamManager::InitializeCommand(void)
{
    if (this->Initialize() != SVL_OK) {
        CMN_LOG_CLASS_RUN_ERROR << "InitializeCommand: error occurred in method \"Initialize\" for stream \""
                                << this->GetName() << "\"" << std::endl;
    }
}

void svlStreamManager::SetSourceFilterCommand(const mtsStdString & source)
{
    // look for the source in the component manager
    mtsManagerLocal * taskManager = mtsManagerLocal::GetInstance();
    std::string sourceName = source; // remove timestamp 
    mtsComponent * component = taskManager->GetComponent(sourceName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "SetSourceFilterCommand: stream \"" << this->GetName()
                                 << "\" can't find component \"" << sourceName << "\"" << std::endl;
        return;
    }
    // make sure this is a source filter
    svlFilterSourceBase * filter = dynamic_cast<svlFilterSourceBase *>(component);
    if (!filter) {
        CMN_LOG_CLASS_INIT_ERROR << "SetSourceFilterCommand: stream \"" << this->GetName()
                                 << "\" can't use component \"" << sourceName
                                 << "\" as it doesn't seem to be of type \"svlFilterSourceBase\"" << std::endl;
        return;
    }
    // finally call the SetSourceFilter method
    SetSourceFilter(filter);
}
