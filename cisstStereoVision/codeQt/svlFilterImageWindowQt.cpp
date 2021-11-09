/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageWindowQt.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include "winQt4OpenGL.h"


/****************************************/
/*** svlWindowManagerThreadProc class ***/
/****************************************/

class svlWindowManagerThreadProc
{
public:
    void* Proc(svlFilterImageWindowQt* obj)
    {
        bool fullscreen;
        obj->GetFullScreen(fullscreen);
        obj->WindowManager->DoModal(true, fullscreen);
        return this;
    }
};


/************************************/
/*** svlFilterImageWindowQt class ***/
/************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageWindowQt)

svlFilterImageWindowQt::svlFilterImageWindowQt() :
    svlFilterBase(),
    FullScreenFlag(false),
    PositionSetFlag(false),
    Thread(0),
    ThreadProc(0),
    WindowManager(0),
    EventHandler(0)
{
    CreateInterfaces();
    
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterImageWindowQt::~svlFilterImageWindowQt()
{
    Release();
}

int svlFilterImageWindowQt::SetPosition(const int x, const int y, const unsigned int videoch)
{
    if (videoch > 1) return SVL_FAIL;

    if (PositionSetFlag) {
        PosX[videoch] = x;
        PosY[videoch] = y;
    }
    else {
        PosX[SVL_LEFT] = PosX[SVL_RIGHT] = x;
        PosY[SVL_LEFT] = PosY[SVL_RIGHT] = y;
    }
    PositionSetFlag = true;

    return SVL_OK;
}

int svlFilterImageWindowQt::GetPosition(int & x, int & y, unsigned int videoch) const
{
    if (videoch > 1 || !PositionSetFlag) return SVL_FAIL;

    x = PosX[videoch];
    y = PosY[videoch];

    return SVL_OK;
}

void svlFilterImageWindowQt::SetEventHandler(svlWindowEventHandlerBase* handler)
{
    EventHandler = handler;
}

void svlFilterImageWindowQt::SetFullScreen(const bool & fullscreen)
{
    FullScreenFlag = fullscreen;
}

void svlFilterImageWindowQt::SetTitle(const std::string & title)
{
    Title = title;
}

void svlFilterImageWindowQt::GetFullScreen(bool & fullscreen) const
{
    fullscreen = FullScreenFlag;
}

void svlFilterImageWindowQt::GetTitle(std::string & title) const
{
    title = Title;
}

int svlFilterImageWindowQt::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    if (GetInput()->GetType() == svlTypeImageRGB) {
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(syncInput);

        WindowManager = svlWindowManagerQt4OpenGL::New(1);
        if (WindowManager == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Initialize: failed to find supported window manager (Qt4 with OpenGL)" << std::endl;
            return SVL_FAIL;
        }
        WindowManager->SetClientSize(img->GetWidth(), img->GetHeight(), 0);
        if (PositionSetFlag) {
            WindowManager->SetWindowPosition(PosX[SVL_LEFT], PosY[SVL_LEFT], SVL_LEFT);
        }
    }
    else {
        svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(syncInput);

        WindowManager = svlWindowManagerQt4OpenGL::New(2);
        if (WindowManager == 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Initialize: failed to find supported window manager (Qt4 with OpenGL)" << std::endl;
            return SVL_FAIL;
        }
        WindowManager->SetClientSize(stimg->GetWidth(SVL_LEFT), stimg->GetHeight(SVL_LEFT), 0);
        WindowManager->SetClientSize(stimg->GetWidth(SVL_RIGHT), stimg->GetHeight(SVL_RIGHT), 1);
        if (PositionSetFlag) {
            WindowManager->SetWindowPosition(PosX[SVL_LEFT], PosY[SVL_LEFT], SVL_LEFT);
            WindowManager->SetWindowPosition(PosX[SVL_RIGHT], PosY[SVL_RIGHT], SVL_RIGHT);
        }
    }

    WindowManager->SetTitleText(Title);
    WindowManager->SetEventHandler(EventHandler);

    // Start GUI thread
    ThreadProc = new svlWindowManagerThreadProc;
    Thread = new osaThread;
    StopThread = false;
    WindowManager->ResetInitEvent();
    Thread->Create<svlWindowManagerThreadProc, svlFilterImageWindowQt*>(ThreadProc,
                                                                        &svlWindowManagerThreadProc::Proc,
                                                                        this);
    WindowManager->WaitForInitEvent();

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterImageWindowQt::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
//    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _OnSingleThread(procInfo)
    {
        WindowManager->LockBuffers();
    }

    _SynchronizeThreads(procInfo);

    _ParallelLoop(procInfo, idx, videochannels)
    {
        WindowManager->SetImageBuffer(img->GetUCharPointer(idx), img->GetDataSize(idx), idx);
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        WindowManager->UnlockBuffers();
        WindowManager->DrawImages();
        if (EventHandler) EventHandler->OnNewFrame(FrameCounter);
    }

    return SVL_OK;
}

int svlFilterImageWindowQt::Release()
{
    if (Thread) {
        WindowManager->DestroyThreadSafe();
        Thread->Wait();
        delete Thread;
        Thread = 0;
    }
    if (ThreadProc) {
        delete ThreadProc;
        ThreadProc = 0;
    }
    if (WindowManager) {
        WindowManager->Delete();
        WindowManager = 0;
    }
    return SVL_OK;
}

void svlFilterImageWindowQt::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterImageWindowQt::SetFullScreen,       this, "SetFullScreen");
        provided->AddCommandWrite(&svlFilterImageWindowQt::SetTitle,            this, "SetTitle");
        provided->AddCommandWrite(&svlFilterImageWindowQt::SetPositionLCommand, this, "SetPosition");
        provided->AddCommandWrite(&svlFilterImageWindowQt::SetPositionLCommand, this, "SetLeftPosition");
        provided->AddCommandWrite(&svlFilterImageWindowQt::SetPositionRCommand, this, "SetRightPosition");
        provided->AddCommandRead (&svlFilterImageWindowQt::GetFullScreen,       this, "GetFullScreen");
        provided->AddCommandRead (&svlFilterImageWindowQt::GetTitle,            this, "GetTitle");
        provided->AddCommandRead (&svlFilterImageWindowQt::GetPositionLCommand, this, "GetPosition");
        provided->AddCommandRead (&svlFilterImageWindowQt::GetPositionLCommand, this, "GetLeftPosition");
        provided->AddCommandRead (&svlFilterImageWindowQt::GetPositionRCommand, this, "GetRightPosition");
    }
}

void svlFilterImageWindowQt::SetPositionLCommand(const vctInt2 & position)
{
    SetPosition(position[0], position[1], SVL_LEFT);
}

void svlFilterImageWindowQt::SetPositionRCommand(const vctInt2 & position)
{
    SetPosition(position[0], position[1], SVL_RIGHT);
}

void svlFilterImageWindowQt::GetPositionLCommand(vctInt2 & position) const
{
    if (!PositionSetFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionLCommand: failed to get position; position has not yet been initialized" << std::endl;
        return;
    }
    GetPosition(position[0], position[1], SVL_LEFT);
}

void svlFilterImageWindowQt::GetPositionRCommand(vctInt2 & position) const
{
    if (!PositionSetFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionRCommand: failed to get position; position has not yet been initialized" << std::endl;
        return;
    }
    GetPosition(position[0], position[1], SVL_RIGHT);
}

