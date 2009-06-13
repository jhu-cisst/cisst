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

#ifndef _svlImageWindow_h
#define _svlImageWindow_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#include <string>

#define winInput_KEY_F1             112
#define winInput_KEY_F2             113
#define winInput_KEY_F3             114
#define winInput_KEY_F4             115
#define winInput_KEY_F5             116
#define winInput_KEY_F6             117
#define winInput_KEY_F7             118
#define winInput_KEY_F8             119
#define winInput_KEY_F9             120
#define winInput_KEY_F10            121
#define winInput_KEY_F11            122
#define winInput_KEY_F12            123

#define winInput_KEY_PAGEUP         33
#define winInput_KEY_PAGEDOWN       34
#define winInput_KEY_HOME           36
#define winInput_KEY_END            35
#define winInput_KEY_INSERT         45
#define winInput_KEY_DELETE         46

#define winInput_KEY_LEFT           37
#define winInput_KEY_RIGHT          39
#define winInput_KEY_UP             38
#define winInput_KEY_DOWN           40

#define winInput_MOUSEMOVE          1000
#define winInput_LBUTTONDOWN        1001
#define winInput_LBUTTONUP          1002
#define winInput_RBUTTONDOWN        1003
#define winInput_RBUTTONUP          1004


class svlWindowManagerBase;

class CISST_EXPORT svlImageWindowCallbackBase
{
friend class svlWindowManagerBase;
friend class svlImageWindow;

public:
    virtual ~svlImageWindowCallbackBase() {}

protected:
    virtual void OnNewFrame(unsigned int CMN_UNUSED(frameid)) {}
    virtual void OnUserEvent(unsigned int CMN_UNUSED(winid), bool CMN_UNUSED(ascii), unsigned int CMN_UNUSED(eventid)) {}
    void GetMousePos(int & x, int & y) { x = MouseX; y = MouseY; }

private:
    int MouseX;
    int MouseY;

    // called by the Window Manager
    void SetMousePos(int x, int y) { MouseX = x; MouseY = y; }
};

class CISST_EXPORT svlWindowManagerBase
{
public:
    svlWindowManagerBase(unsigned int numofwins);
    virtual ~svlWindowManagerBase();
    void SetCallback(svlImageWindowCallbackBase* callback) { Callback = callback; }
    void SetTitleText(const std::string title);
    void SetTimestamp(double timestamp) { Timestamp = timestamp; }
    int SetClientSize(unsigned int width, unsigned int height, unsigned int winid);
    int SetWindowPosition(int x, int y, unsigned int winid);
    void ResetInitEvent();
    int WaitForInitEvent();

    // methods to overwrite
    virtual int DoModal(bool show, bool fullscreen) = 0;
    virtual void Show(bool show, int winid) = 0;
    virtual void DrawImageThreadSafe(unsigned char *buffer, unsigned int buffersize, unsigned int winid) = 0;
    virtual void Destroy() = 0;
    virtual void DestroyThreadSafe() = 0;

protected:
    std::string Title;
    double Timestamp;
    unsigned int NumOfWins;
    unsigned int *Width, *Height;
    int *PosX, *PosY;
    svlImageWindowCallbackBase* Callback;
    osaThreadSignal *InitReadySignal;

    void OnNewFrame(unsigned int frameid);
    void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);
    void GetMousePos(int& x, int& y);
    void SetMousePos(int x, int y);
};


class svlWindowManagerThreadProc;


class CISST_EXPORT svlImageWindow : public svlFilterBase
{
friend class svlWindowManagerThreadProc;

public:
    svlImageWindow();
    virtual ~svlImageWindow();

    void SetFullScreen(bool fullscreen = true) { FullScreenFlag = fullscreen; }
    bool GetFullScreen() { return FullScreenFlag; }
    void SetWindowPosition(int x, int y, unsigned int videoch = SVL_LEFT);
    void SetTitleText(const std::string title);
    void EnableTimestampInTitle(bool enable = true);
    void SetCallback(svlImageWindowCallbackBase* callback) { Callback = callback; }

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    int TimestampEnabled;
    bool FullScreenFlag;
    bool PositionSetFlag;
    int PosX[2], PosY[2];
    std::string Title;
    osaThread* Thread;
    svlWindowManagerThreadProc* ThreadProc;
    bool StopThread;

    svlWindowManagerBase* WindowManager;
    svlImageWindowCallbackBase* Callback;
};


class CISST_EXPORT svlWindowManagerThreadProc
{
public:
    svlWindowManagerThreadProc() {}
    ~svlWindowManagerThreadProc() {}
    void* Proc(svlImageWindow* obj);
};


#endif // _svlImageWindow_h

