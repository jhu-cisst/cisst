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

#ifndef _winX11_h
#define _winX11_h

#include <cisstStereoVision/svlWindowManagerBase.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#if CISST_SVL_HAS_XV
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>
#endif // CISST_SVL_HAS_XV


class svlWindowManagerX11 : public svlWindowManagerBase
{
public:
    svlWindowManagerX11(unsigned int numofwins);
    ~svlWindowManagerX11();

    // method overrides
    int DoModal(bool show, bool fullscreen);
    void Show(bool show, int winid);
    void LockBuffers();
    void UnlockBuffers();
    void SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid);
    void DrawImages();
    void Destroy();
    void DestroyThreadSafe();

private:
    bool LButtonDown;
    bool RButtonDown;
    int xScreen;
    Display *xDisplay;
    Window *xWindows;
    GC *xGCs;
    std::string *Titles;
    std::string *CustomTitles;
    int *CustomTitleEnabled;
    bool DestroyFlag;
    osaThreadSignal *DestroyedSignal;
    osaCriticalSection csImage;
    osaThreadSignal signalImage;
    unsigned int ImageCounter;

#if CISST_SVL_HAS_XV
    XvImage **xvImg;
    XShmSegmentInfo *xvShmInfo;
    XvPortID *xvPort;
#else // CISST_SVL_HAS_XV
    XImage **xImg;
    unsigned char **xImageBuffers;
#endif // CISST_SVL_HAS_XV
};

#endif // _winX11_h


