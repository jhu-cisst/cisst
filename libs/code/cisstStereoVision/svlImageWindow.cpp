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

#include <cisstStereoVision/svlImageWindow.h>

#ifdef __GNUC__
#include "winX11.h"
#endif // __GNUC__

#ifdef _WIN32
#include "winWin32.h"
#endif // _WIN32


/***************************************/
/*** svlWindowManagerBase class ********/
/***************************************/

svlWindowManagerBase::svlWindowManagerBase(unsigned int numofwins) :
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

svlWindowManagerBase::~svlWindowManagerBase()
{
    if (Width) delete [] Width;
    if (Height) delete [] Height;
    if (PosX) delete [] PosX;
    if (PosY) delete [] PosY;
    if (InitReadySignal) delete InitReadySignal;
}

void svlWindowManagerBase::SetTitleText(const std::string title)
{
    Title = title;
}

int svlWindowManagerBase::SetClientSize(unsigned int width, unsigned int height, unsigned int winid)
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

int svlWindowManagerBase::SetWindowPosition(int x, int y, unsigned int winid)
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

void svlWindowManagerBase::ResetInitEvent()
{
    if (InitReadySignal != 0) delete InitReadySignal;
    InitReadySignal = new osaThreadSignal;
}

int svlWindowManagerBase::WaitForInitEvent()
{
    if (InitReadySignal == 0) return -1;
    if (!InitReadySignal->Wait(1.0)) return -2;
    return 0;
}

void svlWindowManagerBase::OnNewFrame(unsigned int frameid)
{
    if (Callback) Callback->OnNewFrame(frameid);
}

void svlWindowManagerBase::OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
{
    if (Callback) Callback->OnUserEvent(winid, ascii, eventid);
}

void svlWindowManagerBase::GetMousePos(int& x, int& y)
{
    if (Callback) Callback->GetMousePos(x, y);
}

void svlWindowManagerBase::SetMousePos(int x, int y)
{
    if (Callback) Callback->SetMousePos(x, y);
}


/********************************************/
/*** svlWindowManagerThreadProc class *******/
/********************************************/

void* svlWindowManagerThreadProc::Proc(svlImageWindow* obj)
{
    obj->WindowManager->DoModal(true, obj->GetFullScreen());
	return this;
}


/********************************/
/*** svlImageWindow class *******/
/********************************/

svlImageWindow::svlImageWindow() :
    svlFilterBase(),
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

svlImageWindow::~svlImageWindow()
{
    Release();
}

void svlImageWindow::SetWindowPosition(int x, int y, unsigned int videoch)
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

void svlImageWindow::SetTitleText(const std::string title)
{
    Title = title;
}

void svlImageWindow::EnableTimestampInTitle(bool enable)
{
    if (enable) TimestampEnabled = 1; // display timestamp
    else if (TimestampEnabled == 1) TimestampEnabled = -1; // restore original title
}

int svlImageWindow::Initialize(svlSample* inputdata)
{
    Release();

    if (GetInputType() == svlTypeImageRGB) {
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(inputdata);

#ifdef __GNUC__
        WindowManager = new CX11WindowManager(1);
#endif // __GNUC__

#ifdef _WIN32
        WindowManager = new CWin32WindowManager(1);
#endif // _WIN32

        if (WindowManager == 0) return SVL_FAIL;
        WindowManager->SetClientSize(img->GetWidth(), img->GetHeight(), 0);
        if (PositionSetFlag) {
            WindowManager->SetWindowPosition(PosX[SVL_LEFT], PosY[SVL_LEFT], SVL_LEFT);
        }
    }
    else {
        svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(inputdata);

#ifdef __GNUC__
        WindowManager = new CX11WindowManager(2);
#endif // __GNUC__

#ifdef _WIN32
        WindowManager = new CWin32WindowManager(2);
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
    ThreadProc = new svlWindowManagerThreadProc;
    Thread = new osaThread;
    StopThread = false;
    WindowManager->ResetInitEvent();
    Thread->Create<svlWindowManagerThreadProc, svlImageWindow*>(ThreadProc,
                                                                &svlWindowManagerThreadProc::Proc,
                                                                this);
    WindowManager->WaitForInitEvent();

    OutputData = inputdata;

    return SVL_OK;
}

int svlImageWindow::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
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

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (TimestampEnabled == 1) WindowManager->SetTimestamp(inputdata->GetTimestamp());
        else if (TimestampEnabled == -1) {
            WindowManager->SetTimestamp(-1.0);
            TimestampEnabled = 0;
        }
        WindowManager->DrawImageThreadSafe(reinterpret_cast<unsigned char*>(img->GetPointer(idx)), img->GetDataSize(idx), idx);
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        if (Callback) Callback->OnNewFrame(FrameCounter);
    }

    return SVL_OK;
}

int svlImageWindow::Release()
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

