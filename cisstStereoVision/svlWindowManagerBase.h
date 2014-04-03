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

#ifndef _svlWindowManagerBase_h
#define _svlWindowManagerBase_h

#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlWindowManagerBase;
class osaThread;
class osaThreadSignal;


class CISST_EXPORT svlWindowEventHandlerBase
{
friend class svlWindowManagerBase;

public:
    virtual ~svlWindowEventHandlerBase();

    virtual void OnNewFrame(unsigned int frameid);
    virtual void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);
    void GetMousePos(int & x, int & y);

private:
    int MouseX;
    int MouseY;

    // called by the Window Manager
    void SetMousePos(int x, int y);
};


class CISST_EXPORT svlWindowManagerBase
{
public:
    svlWindowManagerBase();
    svlWindowManagerBase(unsigned int numofwins);
    virtual ~svlWindowManagerBase();
    void SetNumberOfWindows(unsigned int numofwins);
    void SetEventHandler(svlWindowEventHandlerBase* handler);
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
//    virtual void Destroy() = 0;
    virtual void DestroyThreadSafe() = 0;

protected:
    std::string Title;
    double Timestamp;
    unsigned int NumOfWins;
    unsigned int *Width, *Height;
    int *PosX, *PosY;
    svlWindowEventHandlerBase* EventHandler;
    osaThreadSignal *InitReadySignal;

    void OnNewFrame(unsigned int frameid);
    void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);
    void GetMousePos(int& x, int& y);
    void SetMousePos(int x, int y);
};

#endif // _svlWindowManagerBase_h

