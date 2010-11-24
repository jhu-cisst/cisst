/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Balazs Vagvolgyi
  Created on: 2008

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#ifdef _WIN32
#include "winWin32.h"
#else
#if CISST_SVL_HAS_X11
#include "winX11.h"
#endif // CISST_SVL_HAS_X11
#endif // _WIN32

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


/*****************************************/
/*** svlImageWindowCallbackBase class ****/
/*****************************************/


svlImageWindowCallbackBase::~svlImageWindowCallbackBase()
{
}

void svlImageWindowCallbackBase::OnNewFrame(unsigned int CMN_UNUSED(frameid))
{
}

void svlImageWindowCallbackBase::OnUserEvent(unsigned int CMN_UNUSED(winid), bool CMN_UNUSED(ascii), unsigned int CMN_UNUSED(eventid))
{
}

void svlImageWindowCallbackBase::GetMousePos(int & x, int & y)
{
    x = MouseX;
    y = MouseY;
}

void svlImageWindowCallbackBase::SetMousePos(int x, int y)
{
    MouseX = x;
    MouseY = y;
}


/*************************************/
/*** CWindowManagerBase class ********/
/*************************************/

CWindowManagerBase::CWindowManagerBase(unsigned int numofwins) :
    Timestamp(-1.0),
    NumOfWins(numofwins),
    Width(0),
    Height(0),
    PosX(0),
    PosY(0),
    EventHandler(0),
    InitReadySignal(0)
{
}

CWindowManagerBase::~CWindowManagerBase()
{
    if (Width) delete [] Width;
    if (Height) delete [] Height;
    if (PosX) delete [] PosX;
    if (PosY) delete [] PosY;
    if (InitReadySignal) delete InitReadySignal;
}

void CWindowManagerBase::SetCallback(svlImageWindowCallbackBase* callback)
{
    EventHandler = callback;
}

void CWindowManagerBase::SetTitleText(const std::string title)
{
    Title = title;
}

void CWindowManagerBase::SetTimestamp(double timestamp)
{
    Timestamp = timestamp;
}

int CWindowManagerBase::SetClientSize(unsigned int width, unsigned int height, unsigned int winid)
{
    if (winid >= NumOfWins) return -2;

    // initialize on first call
    if (Width == 0 || Height == 0) {
        Width = new unsigned int[NumOfWins];
        Height = new unsigned int[NumOfWins];
        for (unsigned int i = 0; i < NumOfWins; i ++) {
            Width[i] = width;
            Height[i] = height;
        }
    }
    else {
        Width[winid] = width;
        Height[winid] = height;
    }

    return 0;
}

int CWindowManagerBase::SetWindowPosition(int x, int y, unsigned int winid)
{
    if (winid >= NumOfWins) return -2;

    // initialize on first call
    if (PosX == 0 || PosY == 0) {
        PosX = new int[NumOfWins];
        PosY = new int[NumOfWins];
        for (unsigned int i = 0; i < NumOfWins; i ++) {
            PosX[i] = x + 25 * i;
            PosY[i] = y + 25 * i;
        }
    }
    else {
        PosX[winid] = x;
        PosY[winid] = y;
    }

    return 0;
}

void CWindowManagerBase::ResetInitEvent()
{
    if (InitReadySignal != 0) delete InitReadySignal;
    InitReadySignal = new osaThreadSignal;
}

int CWindowManagerBase::WaitForInitEvent()
{
    if (InitReadySignal == 0) return -1;
    if (!InitReadySignal->Wait(1.0)) return -2;
    return 0;
}

void CWindowManagerBase::LockBuffers()
{
}

void CWindowManagerBase::UnlockBuffers()
{
}

void CWindowManagerBase::OnNewFrame(unsigned int frameid)
{
    if (EventHandler) EventHandler->OnNewFrame(frameid);
}

void CWindowManagerBase::OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
{
    if (EventHandler) EventHandler->OnUserEvent(winid, ascii, eventid);
}

void CWindowManagerBase::GetMousePos(int& x, int& y)
{
    if (EventHandler) EventHandler->GetMousePos(x, y);
}

void CWindowManagerBase::SetMousePos(int x, int y)
{
    if (EventHandler) EventHandler->SetMousePos(x, y);
}


/******************************************/
/*** CWindowManagerThreadProc class *******/
/******************************************/

void* CWindowManagerThreadProc::Proc(svlFilterImageWindow* obj)
{
    bool fullscreen;
    obj->GetFullScreen(fullscreen);
    obj->WindowManager->DoModal(true, fullscreen);
	return this;
}


/***********************************/
/*** svlFilterImageWindow class ****/
/***********************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageWindow)

svlFilterImageWindow::svlFilterImageWindow() :
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

svlFilterImageWindow::~svlFilterImageWindow()
{
    Release();
}

int svlFilterImageWindow::SetPosition(const int x, const int y, const unsigned int videoch)
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

int svlFilterImageWindow::GetPosition(int & x, int & y, unsigned int videoch) const
{
    if (videoch > 1 || !PositionSetFlag) return SVL_FAIL;

    x = PosX[videoch];
    y = PosY[videoch];

    return SVL_OK;
}

void svlFilterImageWindow::SetEventHandler(svlImageWindowCallbackBase* handler)
{
    EventHandler = handler;
}

void svlFilterImageWindow::SetFullScreen(const bool & fullscreen)
{
    FullScreenFlag = fullscreen;
}

void svlFilterImageWindow::SetTitle(const std::string & title)
{
    Title = title;
}

void svlFilterImageWindow::GetFullScreen(bool & fullscreen) const
{
    fullscreen = FullScreenFlag;
}

void svlFilterImageWindow::GetTitle(std::string & title) const
{
    title = Title;
}

int svlFilterImageWindow::SetWindowPosition(const int x, const int y, const unsigned int videoch)
{
    return SetPosition(x, y, videoch);
}

void svlFilterImageWindow::SetTitleText(const std::string & title)
{
    SetTitle(title);
}

void svlFilterImageWindow::SetCallback(svlImageWindowCallbackBase* callback)
{
    SetEventHandler(callback);
}

int svlFilterImageWindow::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    if (GetInput()->GetType() == svlTypeImageRGB) {
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(syncInput);

#ifdef _WIN32
        WindowManager = new CWin32WindowManager(1);
#else
#if CISST_SVL_HAS_X11
        WindowManager = new CX11WindowManager(1);
#endif // CISST_SVL_HAS_X11
#endif // _WIN32

        if (WindowManager == 0) return SVL_FAIL;
        WindowManager->SetClientSize(img->GetWidth(), img->GetHeight(), 0);
        if (PositionSetFlag) {
            WindowManager->SetWindowPosition(PosX[SVL_LEFT], PosY[SVL_LEFT], SVL_LEFT);
        }
    }
    else {
        svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(syncInput);

#ifdef _WIN32
        WindowManager = new CWin32WindowManager(2);
#else
#if CISST_SVL_HAS_X11
        WindowManager = new CX11WindowManager(2);
#endif // CISST_SVL_HAS_X11
#endif // _WIN32

        if (WindowManager == 0) return SVL_FAIL;
        WindowManager->SetClientSize(stimg->GetWidth(SVL_LEFT), stimg->GetHeight(SVL_LEFT), 0);
        WindowManager->SetClientSize(stimg->GetWidth(SVL_RIGHT), stimg->GetHeight(SVL_RIGHT), 1);
        if (PositionSetFlag) {
            WindowManager->SetWindowPosition(PosX[SVL_LEFT], PosY[SVL_LEFT], SVL_LEFT);
            WindowManager->SetWindowPosition(PosX[SVL_RIGHT], PosY[SVL_RIGHT], SVL_RIGHT);
        }
    }

    WindowManager->SetTitleText(Title);
    WindowManager->SetCallback(EventHandler);

    // Start GUI thread
    ThreadProc = new CWindowManagerThreadProc;
    Thread = new osaThread;
    StopThread = false;
    WindowManager->ResetInitEvent();
    Thread->Create<CWindowManagerThreadProc, svlFilterImageWindow*>(ThreadProc,
                                                                    &CWindowManagerThreadProc::Proc,
                                                                    this);
    WindowManager->WaitForInitEvent();

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterImageWindow::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
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

int svlFilterImageWindow::Release()
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
        delete WindowManager;
        WindowManager = 0;
    }
    return SVL_OK;
}

void svlFilterImageWindow::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterImageWindow::SetFullScreen,       this, "SetFullScreen");
        provided->AddCommandWrite(&svlFilterImageWindow::SetTitle,            this, "SetTitle");
        provided->AddCommandWrite(&svlFilterImageWindow::SetPositionLCommand, this, "SetPosition");
        provided->AddCommandWrite(&svlFilterImageWindow::SetPositionLCommand, this, "SetLeftPosition");
        provided->AddCommandWrite(&svlFilterImageWindow::SetPositionRCommand, this, "SetRightPosition");
        provided->AddCommandRead (&svlFilterImageWindow::GetFullScreen,       this, "GetFullScreen");
        provided->AddCommandRead (&svlFilterImageWindow::GetTitle,            this, "GetTitle");
        provided->AddCommandRead (&svlFilterImageWindow::GetPositionLCommand, this, "GetPosition");
        provided->AddCommandRead (&svlFilterImageWindow::GetPositionLCommand, this, "GetLeftPosition");
        provided->AddCommandRead (&svlFilterImageWindow::GetPositionRCommand, this, "GetRightPosition");
    }
}

void svlFilterImageWindow::SetPositionLCommand(const vctInt2 & position)
{
    SetPosition(position[0], position[1], SVL_LEFT);
}

void svlFilterImageWindow::SetPositionRCommand(const vctInt2 & position)
{
    SetPosition(position[0], position[1], SVL_RIGHT);
}

void svlFilterImageWindow::GetPositionLCommand(vctInt2 & position) const
{
    if (!PositionSetFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionLCommand: failed to get position; position has not yet been initialized" << std::endl;
        return;
    }
    GetPosition(position[0], position[1], SVL_LEFT);
}

void svlFilterImageWindow::GetPositionRCommand(vctInt2 & position) const
{
    if (!PositionSetFlag) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionRCommand: failed to get position; position has not yet been initialized" << std::endl;
        return;
    }
    GetPosition(position[0], position[1], SVL_RIGHT);
}

