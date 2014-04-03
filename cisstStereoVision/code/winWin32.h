/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifndef _winWin32_h
#define _winWin32_h

#include <cisstStereoVision/svlWindowManagerBase.h>

#include <windows.h>

class svlWindowWin32
{
public:
    svlWindowWin32(unsigned int _id);
    virtual ~svlWindowWin32();

    virtual int Create(unsigned int width, unsigned int height, bool show,
                       const std::string title, int titleid = -1,
                       bool borderless = false, int blposx = 0, int blposy = 0);
    int ProcessMessage(MSG* msg);
    virtual int Show(bool show = true);
    virtual void Destroy();
    HWND GetHandle() { return hWnd; }
    void GetTitle(std::string & title);
    void SetTitle(const std::string title);

private:
    std::string Title;
    HWND hWnd;
    unsigned int ID;
    int ClientOffsetX;
    int ClientOffsetY;
};

class svlWindowManagerWin32 : public svlWindowManagerBase
{
public:
    svlWindowManagerWin32(unsigned int numofwins);
    ~svlWindowManagerWin32();

    int DoModal(bool show, bool fullscreen);
    void Show(bool show, int winid);
    void LockBuffers();
    void UnlockBuffers();
    void SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid);
    void DrawImages();
    void Destroy();
    void DestroyThreadSafe();
    int FilterMessage(unsigned int winid, MSG* msg);

private:
    bool LButtonDown;
    bool RButtonDown;
    svlWindowWin32** Windows;
    HWND* WindowHandles;
    unsigned int* LineSize;
    unsigned int* Padding;
    unsigned char** ImageBuffers;
    unsigned int* ImageBufferSizes;
    BITMAPINFO* BitmapInfos;
    osaCriticalSection csImage;
};

#endif // _winWin32_h

