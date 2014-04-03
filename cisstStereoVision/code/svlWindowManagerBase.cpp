/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstStereoVision/svlWindowManagerBase.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>


/***************************************/
/*** svlWindowEventHandlerBase class ***/
/***************************************/

svlWindowEventHandlerBase::~svlWindowEventHandlerBase()
{
}

void svlWindowEventHandlerBase::OnNewFrame(unsigned int CMN_UNUSED(frameid))
{
}

void svlWindowEventHandlerBase::OnUserEvent(unsigned int CMN_UNUSED(winid), bool CMN_UNUSED(ascii), unsigned int CMN_UNUSED(eventid))
{
}

void svlWindowEventHandlerBase::GetMousePos(int & x, int & y)
{
    x = MouseX;
    y = MouseY;
}

void svlWindowEventHandlerBase::SetMousePos(int x, int y)
{
    MouseX = x;
    MouseY = y;
}


/**********************************/
/*** svlWindowManagerBase class ***/
/**********************************/

svlWindowManagerBase::svlWindowManagerBase() :
    Timestamp(-1.0),
    NumOfWins(1),
    Width(0),
    Height(0),
    PosX(0),
    PosY(0),
    EventHandler(0),
    InitReadySignal(0)
{
}

svlWindowManagerBase::svlWindowManagerBase(unsigned int numofwins) :
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

svlWindowManagerBase::~svlWindowManagerBase()
{
    if (Width) delete [] Width;
    if (Height) delete [] Height;
    if (PosX) delete [] PosX;
    if (PosY) delete [] PosY;
    if (InitReadySignal) delete InitReadySignal;
}

void svlWindowManagerBase::SetNumberOfWindows(unsigned int numofwins)
{
    NumOfWins = numofwins;
}

void svlWindowManagerBase::SetEventHandler(svlWindowEventHandlerBase* handler)
{
    EventHandler = handler;
}

void svlWindowManagerBase::SetTitleText(const std::string title)
{
    Title = title;
}

void svlWindowManagerBase::SetTimestamp(double timestamp)
{
    Timestamp = timestamp;
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

void svlWindowManagerBase::LockBuffers()
{
}

void svlWindowManagerBase::UnlockBuffers()
{
}

void svlWindowManagerBase::OnNewFrame(unsigned int frameid)
{
    if (EventHandler) EventHandler->OnNewFrame(frameid);
}

void svlWindowManagerBase::OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
{
    if (EventHandler) EventHandler->OnUserEvent(winid, ascii, eventid);
}

void svlWindowManagerBase::GetMousePos(int& x, int& y)
{
    if (EventHandler) EventHandler->GetMousePos(x, y);
}

void svlWindowManagerBase::SetMousePos(int x, int y)
{
    if (EventHandler) EventHandler->SetMousePos(x, y);
}

