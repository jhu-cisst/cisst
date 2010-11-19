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

#ifndef _svlFilterImageWindow_h
#define _svlFilterImageWindow_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class CWindowManagerBase;
class osaThread;
class osaThreadSignal;


class CISST_EXPORT svlImageWindowCallbackBase
{
friend class CWindowManagerBase;
friend class svlFilterImageWindow;

public:
    virtual ~svlImageWindowCallbackBase();

protected:
    virtual void OnNewFrame(unsigned int frameid);
    virtual void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);
    void GetMousePos(int & x, int & y);

private:
    int MouseX;
    int MouseY;

    // called by the Window Manager
    void SetMousePos(int x, int y);
};

class CISST_EXPORT CWindowManagerBase
{
public:
    CWindowManagerBase(unsigned int numofwins);
    virtual ~CWindowManagerBase();
    void SetCallback(svlImageWindowCallbackBase* callback);
    void SetTitleText(const std::string title);
    void SetTimestamp(double timestamp);
    int SetClientSize(unsigned int width, unsigned int height, unsigned int winid);
    int SetWindowPosition(int x, int y, unsigned int winid);
    void ResetInitEvent();
    int WaitForInitEvent();

    // methods to overwrite
    virtual int DoModal(bool show, bool fullscreen) = 0;
    virtual void Show(bool show, int winid) = 0;
    virtual void LockBuffers();
    virtual void UnlockBuffers();
    virtual void SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid) = 0;
    virtual void DrawImages() = 0;
    virtual void Destroy() = 0;
    virtual void DestroyThreadSafe() = 0;

protected:
    std::string Title;
    double Timestamp;
    unsigned int NumOfWins;
    unsigned int *Width, *Height;
    int *PosX, *PosY;
    svlImageWindowCallbackBase* EventHandler;
    osaThreadSignal *InitReadySignal;

    void OnNewFrame(unsigned int frameid);
    void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);
    void GetMousePos(int& x, int& y);
    void SetMousePos(int x, int y);
};


class CWindowManagerThreadProc;


class CISST_EXPORT svlFilterImageWindow : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class CWindowManagerThreadProc;

public:
    svlFilterImageWindow();
    virtual ~svlFilterImageWindow();

    virtual int SetPosition(const int x, const int y, const unsigned int videoch = SVL_LEFT);
    virtual int GetPosition(int & x, int & y, unsigned int videoch = SVL_LEFT) const;
    virtual void SetEventHandler(svlImageWindowCallbackBase* handler);

    virtual void SetFullScreen(const bool & fullscreen);
    virtual void SetTitle(const std::string & title);
    virtual void GetFullScreen(bool & fullscreen) const;
    virtual void GetTitle(std::string & title) const;

    // Deprecated methods: replace them with their corresponding new versions above
    virtual CISST_DEPRECATED int SetWindowPosition(const int x, const int y, const unsigned int videoch = SVL_LEFT);
    virtual CISST_DEPRECATED void SetTitleText(const std::string & title);
    virtual CISST_DEPRECATED void SetCallback(svlImageWindowCallbackBase* callback);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    bool FullScreenFlag;
    bool PositionSetFlag;
    int PosX[2], PosY[2];
    std::string Title;
    osaThread* Thread;
    CWindowManagerThreadProc* ThreadProc;
    bool StopThread;

    CWindowManagerBase* WindowManager;
    svlImageWindowCallbackBase* EventHandler;

protected:
    virtual void CreateInterfaces();
    virtual void SetPositionLCommand(const vctInt2 & position);
    virtual void SetPositionRCommand(const vctInt2 & position);
    virtual void GetPositionLCommand(vctInt2 & position) const;
    virtual void GetPositionRCommand(vctInt2 & position) const;
};


class CISST_EXPORT CWindowManagerThreadProc
{
public:
    CWindowManagerThreadProc() {}
    ~CWindowManagerThreadProc() {}
    void* Proc(svlFilterImageWindow* obj);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageWindow)

#endif // _svlFilterImageWindow_h

