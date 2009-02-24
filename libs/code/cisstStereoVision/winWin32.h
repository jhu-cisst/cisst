/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#include <cisstStereoVision/svlImageWindow.h>

#include <windows.h>

class CWin32Window
{
public:
    CWin32Window(unsigned int id);
    virtual ~CWin32Window();

    virtual int Create(unsigned int width, unsigned int height, bool show,
                       const std::string title, int titleid = -1,
                       bool borderless = false, int blposx = 0, int blposy = 0);
    int ProcessMessage(MSG* msg);
    virtual int Show(bool show = true);
    virtual void Destroy();
    HWND GetHandle() { return hWnd; }

private:
    HWND hWnd;
    unsigned int ID;
    int ClientOffsetX;
    int ClientOffsetY;
};

class CWin32WindowManager : public svlWindowManagerBase
{
public:
    CWin32WindowManager(unsigned int numofwins);
    ~CWin32WindowManager();

    int DoModal(bool show, bool fullscreen);
    void Show(bool show, int winid);
    void DrawImageThreadSafe(unsigned char* buffer, unsigned int buffersize, unsigned int winid);
    void Destroy();
    void DestroyThreadSafe();
    int FilterMessage(unsigned int winid, MSG* msg);

private:
    bool LButtonDown;
    bool RButtonDown;
    CWin32Window** Windows;
    HWND* WindowHandles;
    unsigned int* LineSize;
    unsigned int* Padding;
    unsigned char** ImageBuffers;
    unsigned int* ImageBufferSizes;
    BITMAPINFO* BitmapInfos;
    osaCriticalSection* csImage;
};

#endif // _winWin32_h

