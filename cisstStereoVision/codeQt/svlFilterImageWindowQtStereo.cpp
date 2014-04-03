/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageWindowQtStereo.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include "winQt4OpenGLStereo.h"


/****************************************/
/*** svlWindowManagerThreadProc class ***/
/****************************************/

class svlWindowManagerThreadProc
{
public:
    void* Proc(svlFilterImageWindowQtStereo* obj)
    {
        bool fullscreen;
        obj->GetFullScreen(fullscreen);
        obj->WindowManager->DoModal(true, fullscreen);
        return this;
    }
};


/******************************************/
/*** svlFilterImageWindowQtStereo class ***/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageWindowQtStereo)

svlFilterImageWindowQtStereo::svlFilterImageWindowQtStereo() :
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
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterImageWindowQtStereo::~svlFilterImageWindowQtStereo()
{
    Release();
}

void svlFilterImageWindowQtStereo::SetPosition(const int x, const int y)
{
    PosX = x;
    PosY = y;
    PositionSetFlag = true;
}

int svlFilterImageWindowQtStereo::GetPosition(int & x, int & y) const
{
    if (!PositionSetFlag) return SVL_FAIL;
    x = PosX;
    y = PosY;
    return SVL_OK;
}

void svlFilterImageWindowQtStereo::SetEventHandler(svlWindowEventHandlerBase* handler)
{
    EventHandler = handler;
}

void svlFilterImageWindowQtStereo::SetFullScreen(const bool & fullscreen)
{
    FullScreenFlag = fullscreen;
}

void svlFilterImageWindowQtStereo::SetTitle(const std::string & title)
{
    Title = title;
}

void svlFilterImageWindowQtStereo::GetFullScreen(bool & fullscreen) const
{
    fullscreen = FullScreenFlag;
}

void svlFilterImageWindowQtStereo::GetTitle(std::string & title) const
{
    title = Title;
}

int svlFilterImageWindowQtStereo::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(syncInput);

    WindowManager = svlWindowManagerQt4OpenGLStereo::New();
    if (WindowManager == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: failed to find supported window manager (Qt4 with OpenGLStereo)" << std::endl;
        return SVL_FAIL;
    }
    WindowManager->SetClientSize(img->GetWidth(), img->GetHeight(), 0);
    if (PositionSetFlag) {
        WindowManager->SetWindowPosition(PosX, PosY, 0);
    }

    WindowManager->SetTitleText(Title);
    WindowManager->SetEventHandler(EventHandler);

    // Start GUI thread
    ThreadProc = new svlWindowManagerThreadProc;
    Thread = new osaThread;
    StopThread = false;
    WindowManager->ResetInitEvent();
    Thread->Create<svlWindowManagerThreadProc, svlFilterImageWindowQtStereo*>(ThreadProc,
                                                                              &svlWindowManagerThreadProc::Proc,
                                                                              this);
    WindowManager->WaitForInitEvent();

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterImageWindowQtStereo::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
//    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    _OnSingleThread(procInfo)
    {
        WindowManager->LockBuffers();
        WindowManager->SetImage(dynamic_cast<svlSampleImageRGBStereo*>(syncInput));
        WindowManager->UnlockBuffers();
        WindowManager->DrawImages();
        if (EventHandler) EventHandler->OnNewFrame(FrameCounter);
    }

    return SVL_OK;
}

int svlFilterImageWindowQtStereo::Release()
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

void svlFilterImageWindowQtStereo::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterImageWindowQtStereo::SetFullScreen,      this, "SetFullScreen");
        provided->AddCommandWrite(&svlFilterImageWindowQtStereo::SetTitle,           this, "SetTitle");
        provided->AddCommandWrite(&svlFilterImageWindowQtStereo::SetPositionCommand, this, "SetPosition");
        provided->AddCommandRead (&svlFilterImageWindowQtStereo::GetFullScreen,      this, "GetFullScreen");
        provided->AddCommandRead (&svlFilterImageWindowQtStereo::GetTitle,           this, "GetTitle");
        provided->AddCommandRead (&svlFilterImageWindowQtStereo::GetPositionCommand, this, "GetPosition");
    }
}

void svlFilterImageWindowQtStereo::SetPositionCommand(const vctInt2 & position)
{
    SetPosition(position[0], position[1]);
}

void svlFilterImageWindowQtStereo::GetPositionCommand(vctInt2 & position) const
{
    if (!PositionSetFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionCommand: failed to get position; position has not yet been initialized" << std::endl;
        return;
    }
    GetPosition(position[0], position[1]);
}

