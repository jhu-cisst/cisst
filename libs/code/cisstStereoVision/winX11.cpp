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

#include "winX11.h"

#include <cisstOSAbstraction/osaSleep.h>

// Motif window hints
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)

// bit definitions for MwmHints.functions
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

// bit definitions for MwmHints.decorations
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)


/*************************************/
/*** CX11WindowManager class *********/
/*************************************/

CX11WindowManager::CX11WindowManager(unsigned int numofwins) : svlWindowManagerBase(numofwins)
{
    LButtonDown = false;
    RButtonDown = false;
    DestroyFlag = false;
    DestroyedSignal = 0;
    xScreen = 0;
    xDisplay = 0;
    xWindows = 0;
    xGCs = 0;
    xImg = 0;
    Titles = 0;
    CustomTitles = 0;
    CustomTitleEnabled = 0;
    ImageBuffers = 0;

    // create drawing critical section and counter
    csImage = new osaCriticalSection[NumOfWins];
    ImageCounter = new unsigned int[NumOfWins];
}

CX11WindowManager::~CX11WindowManager()
{
    Destroy();

    // destroy drawing critical section and counter
    delete [] csImage;
    delete(ImageCounter);
}

int CX11WindowManager::DoModal(bool show, bool fullscreen)
{
    Destroy();
    DestroyFlag = false;

    unsigned int i, atom_count;
    int x, y, prevright, prevbottom;
    unsigned int *lastimage = new unsigned int[NumOfWins];
    unsigned long black, white;
    Atom atoms[2];
    XSizeHints wsh;

    // setting decoration hints for borderless mode
    struct {
        unsigned long 	flags;
        unsigned long 	functions;
        unsigned long 	decorations;
        signed long 	input_mode;
        unsigned long 	status;
    } mwm;
    mwm.flags = MWM_HINTS_DECORATIONS;
    mwm.decorations = 0;
    mwm.functions = 0;
    mwm.input_mode = 0;
    mwm.status = 0;
    
    // resetting DestroyedSignal event
    if (DestroyedSignal) delete(DestroyedSignal);
    DestroyedSignal = new osaThreadSignal();
    if (DestroyedSignal == 0) goto labError;

    // initialize display and pick default screen
    xDisplay = XOpenDisplay(reinterpret_cast<char*>(0));
    xScreen = DefaultScreen(xDisplay);

    // pick colors
    black = BlackPixel(xDisplay, xScreen);
    white = WhitePixel(xDisplay, xScreen);

    // create windows
    xWindows = new Window[NumOfWins];
    memset(xWindows, 0, NumOfWins * sizeof(Window));
    xGCs = new GC[NumOfWins];
    memset(xGCs, 0, NumOfWins * sizeof(GC));

    // create atoms for overriding default window behaviours
    atoms[0] = XInternAtom(xDisplay, "WM_DELETE_WINDOW", False);
    atoms[1] = XInternAtom(xDisplay, "_MOTIF_WM_HINTS", False);

    // create title strings
    Titles = new std::string[NumOfWins];
    CustomTitles = new std::string[NumOfWins];
    CustomTitleEnabled = new int[NumOfWins];

    // create images
    ImageBuffers = new unsigned char*[NumOfWins];
    for (i = 0; i < NumOfWins; i ++) {
        ImageBuffers[i] = new unsigned char[Width[i] * Height[i] * 4];
        lastimage[i] = 0;
    }

    xImg = new XImage*[NumOfWins];
    memset(xImg, 0, NumOfWins * sizeof(XImage*));
    if (xImg == 0) goto labError;
    for (i = 0; i < NumOfWins; i ++) {
        xImg[i] = XCreateImage(xDisplay,
                               DefaultVisual(xDisplay, xScreen),
                               24,
                               ZPixmap,
                               0,
                               reinterpret_cast<char*>(ImageBuffers[i]),
                               Width[i],
                               Height[i],
                               32,
                               0);
    }

    prevright = prevbottom = 0;
    for (i = 0; i < NumOfWins; i ++) {
        if (PosX == 0 || PosY == 0) {
            if (fullscreen) {
                x = prevright;
                y = 0;
                prevright += Width[i];
            }
            else {
                x = prevright;
                y = prevbottom;
                prevright += 50;
                prevbottom += 50;
            }
        }
        else {
            x = PosX[i];
            y = PosY[i];
        }

        xWindows[i] = XCreateSimpleWindow(xDisplay, DefaultRootWindow(xDisplay),
                                          x,
                                          y,
                                          Width[i],
                                          Height[i],
                                          0,
                                          black,
                                          white);
        if (xWindows[i] == 0) goto labError;

        // overriding default behaviours:
        //    - borderless mode
        //    - closing window
        if (fullscreen) {
            XChangeProperty(xDisplay, xWindows[i],
                            atoms[1], atoms[1], 32,
                            PropModeReplace, reinterpret_cast<unsigned char*>(&mwm), 5);
            atom_count = 2;
        }
        else {
            atom_count = 1;
        }
        XSetWMProtocols(xDisplay, xWindows[i], atoms, atom_count);

        wsh.flags = PPosition|PSize;
        wsh.x = x;
        wsh.y = y;
        wsh.width = Width[i];
        wsh.height = Height[i];
        XSetNormalHints(xDisplay, xWindows[i], &wsh);

        // set window title
        CustomTitleEnabled[i] = 0;

        std::ostringstream ostring;
        if (Title.length() > 0) {
            if (NumOfWins > 0) ostring << Title << " #" << i;
            else ostring << Title;
        }
        else {
            if (NumOfWins > 0) ostring << Title << "svlImageWindow #" << i;
            else ostring << "svlImageWindow";
        }

        Titles[i] = ostring.str();
        XSetStandardProperties(xDisplay, xWindows[i],
                               Titles[i].c_str(), Titles[i].c_str(),
                               None, NULL, 0, NULL);

        // set even mask
        XSelectInput(xDisplay, xWindows[i],
                     ExposureMask|PointerMotionMask|ButtonPressMask|KeyPressMask);

        // create graphics context
        xGCs[i] = XCreateGC(xDisplay, xWindows[i], 0, 0);

        XSetWindowColormap(xDisplay, xWindows[i], DefaultColormapOfScreen(DefaultScreenOfDisplay(xDisplay)));

        // set default colors
        XSetBackground(xDisplay, xGCs[i], white);
        XSetForeground(xDisplay, xGCs[i], black);

        // clear window
        XClearWindow(xDisplay, xWindows[i]);

        // show window if requested
        if (show) XMapRaised(xDisplay, xWindows[i]);
    }

    // signal that initialization is done
    if (InitReadySignal) InitReadySignal->Raise();

	// main message loop
    XEvent event;
    KeySym code;
    unsigned int winid;

    while (1) {
        if (XPending(xDisplay)) {
            XNextEvent(xDisplay, &event);

            // find recipient
            for (winid = 0; winid < NumOfWins; winid ++) {
                if (event.xany.window == xWindows[winid]) break;
            }
            if (winid == NumOfWins) continue;

            // override default window behaviour
            if (event.type == ClientMessage) {
                if (static_cast<unsigned long>(event.xclient.data.l[0]) == atoms[0]) {
                    // X11 server wants to close window
                    // Do nothing.... we will destroy it ourselves later
                }
                continue;
            }

            // window should be closed
            if (event.type == UnmapNotify) {
                printf("destroy\n");
                if (xGCs[winid]) {
                    XFreeGC(xDisplay, xGCs[winid]);
                    xGCs[winid] = 0;
                }
                xWindows[winid] = 0;
                continue;
            }

            // window should be updated
            if (event.type == Expose && event.xexpose.count == 0) {
                XClearWindow(xDisplay, xWindows[winid]);
                continue;
            }

            if (event.type == KeyPress) {
                code = XLookupKeysym(&event.xkey, 0);
                if (code >= 48 && code <= 57) { // ascii numbers
                    OnUserEvent(winid, true, code);
                    continue;
                }
                if (code >= 97 && code <= 122) { // ascii letters
                    OnUserEvent(winid, true, code);
                    continue;
                }
                if (code == 13 ||
                    code == 32) { // special characters with correct ascii code
                    OnUserEvent(winid, true, code);
                    continue;
                }
                if (code >= 0xffbe && code <= 0xffc9) { // F1-F12
                    OnUserEvent(winid, false, winInput_KEY_F1 + (code - 0xffbe));
                    continue;
                }
                switch (code) {
                    case 0xFF55:
                        OnUserEvent(winid, false, winInput_KEY_PAGEUP);
                    break;

                    case 0xFF56:
                        OnUserEvent(winid, false, winInput_KEY_PAGEDOWN);
                    break;

                    case 0xFF50:
                        OnUserEvent(winid, false, winInput_KEY_HOME);
                    break;

                    case 0xFF57:
                        OnUserEvent(winid, false, winInput_KEY_END);
                    break;

                    case 0xFF63:
                        OnUserEvent(winid, false, winInput_KEY_INSERT);
                    break;

                    case 0xFFFF:
                        OnUserEvent(winid, false, winInput_KEY_DELETE);
                    break;

                    case 0xFF51:
                        OnUserEvent(winid, false, winInput_KEY_LEFT);
                    break;

                    case 0xFF53:
                        OnUserEvent(winid, false, winInput_KEY_RIGHT);
                    break;

                    case 0xFF52:
                        OnUserEvent(winid, false, winInput_KEY_UP);
                    break;

                    case 0xFF54:
                        OnUserEvent(winid, false, winInput_KEY_DOWN);
                    break;
                }
                continue;
            }

            if (event.type == ButtonPress) {
                if (event.xbutton.button == Button1) {
                    if (!LButtonDown && !RButtonDown) {
                        LButtonDown = true;
                        XGrabPointer(xDisplay, xWindows[winid], false,
                                     PointerMotionMask|ButtonReleaseMask,
                                     GrabModeAsync, GrabModeAsync,
                                     None,
                                     None,
                                     CurrentTime);
                    }
                    OnUserEvent(winid, false, winInput_LBUTTONDOWN);
                }
                else if (event.xbutton.button == Button2) {
                    if (!LButtonDown && !RButtonDown) {
                        RButtonDown = true;
                        XGrabPointer(xDisplay, xWindows[winid], false,
                                     PointerMotionMask|ButtonReleaseMask,
                                     GrabModeAsync, GrabModeAsync,
                                     None,
                                     None,
                                     CurrentTime);
                    }
                    OnUserEvent(winid, false, winInput_RBUTTONDOWN);
                }
            }
            
            if (event.type == ButtonRelease) {
                if (event.xbutton.button == Button1) {
                    OnUserEvent(winid, false, winInput_LBUTTONUP);
                    if (LButtonDown && !RButtonDown) {
                        LButtonDown = false;
                        XUngrabPointer(xDisplay, CurrentTime);
                    }
                }
                else if (event.xbutton.button == Button2) {
                    OnUserEvent(winid, false, winInput_RBUTTONUP);
                    if (LButtonDown && !RButtonDown) {
                        RButtonDown = false;
                        XUngrabPointer(xDisplay, CurrentTime);
                    }
                }
            }

            if (event.type == MotionNotify) {
                SetMousePos(static_cast<short>(event.xmotion.x), static_cast<short>(event.xmotion.y));
                OnUserEvent(winid, false, winInput_MOUSEMOVE);
            }
        }
        else {
            if (DestroyFlag) break;
            for (winid = 0; winid < NumOfWins; winid ++) {
                if (ImageCounter[winid] > lastimage[winid]) {
                    csImage[winid].Enter();

                        lastimage[winid] = ImageCounter[winid];
                        xImg[winid]->data = reinterpret_cast<char*>(ImageBuffers[winid]);
                        XPutImage(xDisplay, xWindows[winid], xGCs[winid], xImg[winid], 0, 0, 0, 0, Width[winid], Height[winid]);

                    csImage[winid].Leave();

                    if (CustomTitleEnabled[winid] < 0) {
                        // Restore original timestamp
                        XSetStandardProperties(xDisplay, xWindows[winid],
                                               Titles[winid].c_str(), Titles[winid].c_str(),
                                               None, NULL, 0, NULL);
                    }
                    else if (CustomTitleEnabled[winid] > 0) {
                        // Set custom timestamp
                        XSetStandardProperties(xDisplay, xWindows[winid],
                                               CustomTitles[winid].c_str(), CustomTitles[winid].c_str(),
                                               None, NULL, 0, NULL);
                    }
                }
            }
            osaSleep(0.01);
        }
    }

labError:

    delete(lastimage);

    for (i = 0; i < NumOfWins; i ++) {
        if (xGCs[i]) XFreeGC(xDisplay, xGCs[i]);
        if (xWindows[i]) XDestroyWindow(xDisplay, xWindows[i]);
    }
    XCloseDisplay(xDisplay);

    if (xImg) {
        for (i = 0; i < NumOfWins; i ++) {
            if (xImg[i]) XDestroyImage(xImg[i]); 
        }
        delete [] xImg; 
        xImg = 0;
    }
    if (xGCs) {
        delete [] xGCs;
        xGCs = 0;
    }
    if (xWindows) {
        delete [] xWindows;
        xWindows = 0;
    }
    if (Titles) {
        delete [] Titles;
        Titles = 0;
    }
    if (CustomTitles) {
        delete [] CustomTitles;
        CustomTitles = 0;
    }
    if (CustomTitleEnabled) {
        delete [] CustomTitleEnabled;
        CustomTitleEnabled = 0;
    }
    if (ImageBuffers) {
        delete [] ImageBuffers;
        ImageBuffers = 0;
    }

    xScreen = 0;
    xDisplay = 0;

    if (DestroyedSignal) DestroyedSignal->Raise();

    return 0;
}

void CX11WindowManager::Show(bool show, int winid)
{
    if (xDisplay && xWindows && winid < static_cast<int>(NumOfWins)) {
        if (winid < 0)
            for (unsigned int i = 0; i < NumOfWins; i ++) {
                if (show) XMapRaised(xDisplay, xWindows[i]);
                else XUnmapWindow(xDisplay, xWindows[i]);
            }
        else {
            if (show) XMapRaised(xDisplay, xWindows[winid]);
            else XUnmapWindow(xDisplay, xWindows[winid]);
        }
    }
}

void CX11WindowManager::DrawImageThreadSafe(unsigned char* buffer, unsigned int buffersize, unsigned int winid)
{
    if (xDisplay == 0 || buffer == 0 || winid >= NumOfWins ||
        ImageBuffers == 0 || ImageBuffers[winid] == 0)  return;
    const unsigned int pixsize = Width[winid] * Height[winid];
    if (buffersize > (pixsize * 3)) return;

    // Modify title if requested
    if (Timestamp > 0.0) {
        char timestampstring[32];
        sprintf(timestampstring, " (timestamp=%.3f)", Timestamp);
        CustomTitles[winid] = Titles[winid] + timestampstring;
        CustomTitleEnabled[winid] = 1;
    }
    else {
        if (Timestamp == 0.0) CustomTitleEnabled[winid] = 0;
        else CustomTitleEnabled[winid] = -1;
    }

    csImage[winid].Enter();

        // store image in buffer
        unsigned char *framebuffer = ImageBuffers[winid];
        for (unsigned int i = 0; i < pixsize; i ++) {
            *framebuffer = *buffer; framebuffer ++; buffer ++;
            *framebuffer = *buffer; framebuffer ++; buffer ++;
            *framebuffer = *buffer; framebuffer ++; buffer ++;
            *framebuffer = 0;  framebuffer ++;
        }
        ImageCounter[winid] ++;
    
    csImage[winid].Leave();
}

void CX11WindowManager::Destroy()
{
    DestroyFlag = true;

    if (DestroyedSignal) {
        DestroyedSignal->Wait(1.0);
        delete(DestroyedSignal);
        DestroyedSignal = 0;
    }
}

void CX11WindowManager::DestroyThreadSafe()
{
    Destroy();
}

