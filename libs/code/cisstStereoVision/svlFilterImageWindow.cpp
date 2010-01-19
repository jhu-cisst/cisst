/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageWindow.cpp 534 2009-07-13 20:02:33Z bvagvol1 $
  
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

#ifdef _WIN32
#include "winWin32.h"
#else
#if (CISST_SVL_HAS_X11 == ON)
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

void svlImageWindowCallbackBase::OnNewFrame(unsigned int CMN_UNUSED(frameid))
{
}

void svlImageWindowCallbackBase::OnUserEvent(unsigned int CMN_UNUSED(winid), bool CMN_UNUSED(ascii), unsigned int CMN_UNUSED(eventid))
{
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
    Callback(0),
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

void CWindowManagerBase::SetTitleText(const std::string title)
{
    Title = title;
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

void CWindowManagerBase::OnNewFrame(unsigned int frameid)
{
    if (Callback) Callback->OnNewFrame(frameid);
}

void CWindowManagerBase::OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
{
    if (Callback) Callback->OnUserEvent(winid, ascii, eventid);
}

void CWindowManagerBase::GetMousePos(int& x, int& y)
{
    if (Callback) Callback->GetMousePos(x, y);
}

void CWindowManagerBase::SetMousePos(int x, int y)
{
    if (Callback) Callback->SetMousePos(x, y);
}


/******************************************/
/*** CWindowManagerThreadProc class *******/
/******************************************/

void* CWindowManagerThreadProc::Proc(svlFilterImageWindow* obj)
{
    obj->WindowManager->DoModal(true, obj->GetFullScreen());
	return this;
}


/***********************************/
/*** svlFilterImageWindow class ****/
/***********************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageWindow)

svlFilterImageWindow::svlFilterImageWindow() :
    svlFilterBase(),
    cmnGenericObject(),
    TimestampEnabled(false),
    FullScreenFlag(false),
    PositionSetFlag(false),
    Thread(0),
    ThreadProc(0),
    WindowManager(0),
    Callback(0)
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
}

svlFilterImageWindow::~svlFilterImageWindow()
{
    Release();
}

void svlFilterImageWindow::SetWindowPosition(int x, int y, unsigned int videoch)
{
    if (videoch == SVL_LEFT || videoch == SVL_RIGHT) {
        if (PositionSetFlag) {
            PosX[videoch] = x;
            PosY[videoch] = y;
        }
        else {
            PosX[SVL_LEFT] = PosX[SVL_RIGHT] = x;
            PosY[SVL_LEFT] = PosY[SVL_RIGHT] = y;
        }
        PositionSetFlag = true;
    }
}

void svlFilterImageWindow::SetTitleText(const std::string title)
{
    Title = title;
}

void svlFilterImageWindow::EnableTimestampInTitle(bool enable)
{
    if (enable) TimestampEnabled = 1; // display timestamp
    else if (TimestampEnabled == 1) TimestampEnabled = -1; // restore original title
}

int svlFilterImageWindow::Initialize(svlSample* inputdata)
{
    Release();

    if (GetInputType() == svlTypeImageRGB) {
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(inputdata);

#ifdef _WIN32
        WindowManager = new CWin32WindowManager(1);
#else
#if (CISST_SVL_HAS_X11 == ON)
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
        svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(inputdata);

#ifdef _WIN32
        WindowManager = new CWin32WindowManager(2);
#else
#if (CISST_SVL_HAS_X11 == ON)
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
    WindowManager->SetCallback(Callback);

    // Start GUI thread
    ThreadProc = new CWindowManagerThreadProc;
    Thread = new osaThread;
    StopThread = false;
    WindowManager->ResetInitEvent();
    Thread->Create<CWindowManagerThreadProc, svlFilterImageWindow*>(ThreadProc,
                                                                &CWindowManagerThreadProc::Proc,
                                                                this);
    WindowManager->WaitForInitEvent();

    OutputData = inputdata;

    return SVL_OK;
}

int svlFilterImageWindow::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata)) {
          return SVL_ALREADY_PROCESSED;
      }
    ///////////////////////////////////////////

    // Passing the same image for the next filter
    OutputData = inputdata;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _OnSingleThread(procInfo)
    {
        WindowManager->LockBuffers();

        if (TimestampEnabled == 1) WindowManager->SetTimestamp(inputdata->GetTimestamp());
        else if (TimestampEnabled == -1) {
            WindowManager->SetTimestamp(-1.0);
            TimestampEnabled = 0;
        }
        else {
            WindowManager->SetTimestamp(0.0);
        }
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
        if (Callback) Callback->OnNewFrame(FrameCounter);
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

