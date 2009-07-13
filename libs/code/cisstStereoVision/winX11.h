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

#ifndef _winX11_h
#define _winX11_h

#include <cisstStereoVision/svlImageWindow.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

class CX11WindowManager : public svlWindowManagerBase
{
public:
    CX11WindowManager(unsigned int numofwins);
    ~CX11WindowManager();

    // method overrides
    int DoModal(bool show, bool fullscreen);
    void Show(bool show, int winid);
    void DrawImageThreadSafe(unsigned char* buffer, unsigned int buffersize, unsigned int winid);
    void Destroy();
    void DestroyThreadSafe();

private:
    bool LButtonDown;
    bool RButtonDown;
    int xScreen;
    Display *xDisplay;
    Window *xWindows;
    GC *xGCs;
    XImage **xImg;
    std::string *Titles;
    std::string *CustomTitles;
    int *CustomTitleEnabled;
    bool DestroyFlag;
    osaThreadSignal *DestroyedSignal;
    osaCriticalSection *csImage;
    unsigned int *ImageCounter;
    unsigned char **ImageBuffers;
};

#endif // _winX11_h

