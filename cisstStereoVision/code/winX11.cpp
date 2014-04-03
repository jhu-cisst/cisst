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

#include "winX11.h"

#include <cisstOSAbstraction/osaSleep.h>
#if CISST_SVL_HAS_XV
#include <cisstStereoVision/svlConverters.h>
#endif // CISST_SVL_HAS_XV

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


/*********************************/
/*** svlWindowManagerX11 class ***/
/*********************************/

svlWindowManagerX11::svlWindowManagerX11(unsigned int numofwins) : svlWindowManagerBase(numofwins)
{
    LButtonDown = false;
    RButtonDown = false;
    DestroyFlag = false;
    DestroyedSignal = 0;
    ImageCounter = 0;
    xScreen = 0;
    xDisplay = 0;
    xWindows = 0;
    xGCs = 0;
    Titles = 0;
    CustomTitles = 0;
    CustomTitleEnabled = 0;
#if CISST_SVL_HAS_XV
    xvImg = 0;
    xvShmInfo = 0;
    xvPort = 0;
#else // CISST_SVL_HAS_XV
    xImg = 0;
    xImageBuffers = 0;
#endif // CISST_SVL_HAS_XV
}

svlWindowManagerX11::~svlWindowManagerX11()
{
    Destroy();
}

int svlWindowManagerX11::DoModal(bool show, bool fullscreen)
{
    Destroy();
    DestroyFlag = false;

    unsigned int i, atom_count;
    int x, y, prevright, prevbottom;
    unsigned int lastimage = 0;
    unsigned long black, white;
    XSizeHints wsh;

#if CISST_SVL_HAS_XV
    Atom atoms[3];
    unsigned int xvadaptorcount;
    XvAdaptorInfo *xvai;
    XVisualInfo xvvinfo;
    bool xvupdateimage = true;
#else // CISST_SVL_HAS_XV
    Atom atoms[2];
#endif // CISST_SVL_HAS_XV

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

#if CISST_SVL_HAS_XV
    // check if 24bpp is suppoted by the display
    if (XMatchVisualInfo(xDisplay, xScreen, 24, TrueColor, &xvvinfo) == 0) goto labError;
#endif // CISST_SVL_HAS_XV

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
#if CISST_SVL_HAS_XV
    atoms[2] = XInternAtom(xDisplay, "XV_SYNC_TO_VBLANK", False);
#endif // CISST_SVL_HAS_XV

    // create title strings
    Titles = new std::string[NumOfWins];
    CustomTitles = new std::string[NumOfWins];
    CustomTitleEnabled = new int[NumOfWins];

#if CISST_SVL_HAS_XV
    xvImg = new XvImage*[NumOfWins];
    xvShmInfo = new XShmSegmentInfo[NumOfWins];
    xvPort = new XvPortID[NumOfWins];
    if (xvImg == 0 || xvShmInfo == 0 || xvPort == 0) goto labError;
    memset(xvImg, 0, NumOfWins * sizeof(XvImage*));
    memset(xvShmInfo, 0, NumOfWins * sizeof(XShmSegmentInfo));
    memset(xvPort, 0, NumOfWins * sizeof(XvPortID));
#else // CISST_SVL_HAS_XV
    // create images
    xImageBuffers = new unsigned char*[NumOfWins];
    for (i = 0; i < NumOfWins; i ++) {
        xImageBuffers[i] = new unsigned char[Width[i] * Height[i] * 4];
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
                               reinterpret_cast<char*>(xImageBuffers[i]),
                               Width[i],
                               Height[i],
                               32,
                               0);
    }
#endif // CISST_SVL_HAS_XV

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
        XSelectInput(xDisplay, xWindows[i], ExposureMask|PointerMotionMask|ButtonPressMask|KeyPressMask);

        // set window colormap
        XSetWindowColormap(xDisplay, xWindows[i], DefaultColormapOfScreen(DefaultScreenOfDisplay(xDisplay)));

#if CISST_SVL_HAS_XV
        // query shared memory extension
        if (!XShmQueryExtension(xDisplay)) goto labError;

        // query video adaptors
        if (XvQueryAdaptors(xDisplay, DefaultRootWindow(xDisplay), &xvadaptorcount, &xvai) != Success) goto labError;
        xvPort[i] = xvai->base_id + i;
        XvFreeAdaptorInfo(xvai);

        // overriding default Xvideo vertical sync behavior
        XvSetPortAttribute (xDisplay, xvPort[i], atoms[2], 1);
#endif // CISST_SVL_HAS_XV

        // create graphics context
        xGCs[i] = XCreateGC(xDisplay, xWindows[i], 0, 0);

        // set default colors
        XSetBackground(xDisplay, xGCs[i], white);
        XSetForeground(xDisplay, xGCs[i], black);

#if CISST_SVL_HAS_XV
        // create image in shared memory
        xvImg[i] = XvShmCreateImage(xDisplay, xvPort[i], 0x32595559/*YUV2*/, 0, Width[i], Height[i], &(xvShmInfo[i]));
        if (xvImg[i]->width < static_cast<int>(Width[i]) || xvImg[i]->height < static_cast<int>(Height[i])) {
            CMN_LOG_INIT_ERROR << "DoModal - image too large for XV to display (requested="
                               << Width[i] << "x" << Height[i] << "; allowed="
                               << xvImg[i]->width << "x" << xvImg[i]->height << ")" << std::endl;
            goto labError;
        }
        xvShmInfo[i].shmid = shmget(IPC_PRIVATE, xvImg[i]->data_size, IPC_CREAT | 0777);
        xvShmInfo[i].shmaddr = xvImg[i]->data = reinterpret_cast<char*>(shmat(xvShmInfo[i].shmid, 0, 0));
        xvShmInfo[i].readOnly = False;
        if (!XShmAttach(xDisplay, &(xvShmInfo[i]))) goto labError;
#endif // CISST_SVL_HAS_XV

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
        osaSleep(0.001);
#if CISST_SVL_HAS_XV
        if (!xvupdateimage) {
            for (int events = XPending(xDisplay); events > 0; events --) {
#else // CISST_SVL_HAS_XV
        if (XPending(xDisplay)) {
#endif // CISST_SVL_HAS_XV
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
                    else if (event.xbutton.button == Button3) {
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
                    else if (event.xbutton.button == Button3) {
                        OnUserEvent(winid, false, winInput_RBUTTONUP);
                        if (!LButtonDown && RButtonDown) {
                            RButtonDown = false;
                            XUngrabPointer(xDisplay, CurrentTime);
                        }
                    }
                }

                if (event.type == MotionNotify) {
                    SetMousePos(static_cast<short>(event.xmotion.x), static_cast<short>(event.xmotion.y));
                    OnUserEvent(winid, false, winInput_MOUSEMOVE);
                }

#if CISST_SVL_HAS_XV
                // image update complete
                if (event.type == XShmGetEventBase(xDisplay) + ShmCompletion) {
                    xvupdateimage = true;
                    continue;
                }
            }
            if (!xvupdateimage) signalImage.Wait(0.01);
#endif // CISST_SVL_HAS_XV
        }
        else {
            if (DestroyFlag) break;

            if (ImageCounter > lastimage) {
                csImage.Enter();
                    lastimage = ImageCounter;

#if CISST_SVL_HAS_XV
                    for (i = 0; i < NumOfWins; i ++) {
                        XvShmPutImage(xDisplay,
                                      xvPort[i],
                                      xWindows[i],
                                      xGCs[i],
                                      xvImg[i],
                                      0, 0, Width[i], Height[i],
                                      0, 0, Width[i], Height[i], True);
                    }
                    xvupdateimage = false;
#else // CISST_SVL_HAS_XV
                    for (i = 0; i < NumOfWins; i ++) {
                        xImg[i]->data = reinterpret_cast<char*>(xImageBuffers[i]);
                        XPutImage(xDisplay,
                                  xWindows[i],
                                  xGCs[i],
                                  xImg[i],
                                  0, 0, 0, 0,
                                  Width[i], Height[i]);
                    }
#endif // CISST_SVL_HAS_XV
/*
                    for (i = 0; i < NumOfWins; i ++) {
                        if (CustomTitleEnabled[i] < 0) {
                            // Restore original timestamp
                            XSetStandardProperties(xDisplay, xWindows[i],
                                                   Titles[i].c_str(), Titles[i].c_str(),
                                                   None, NULL, 0, NULL);
                        }
                        else if (CustomTitleEnabled[i] > 0) {
                            // Set custom timestamp
                            XSetStandardProperties(xDisplay, xWindows[i],
                                                   CustomTitles[i].c_str(), CustomTitles[i].c_str(),
                                                   None, NULL, 0, NULL);
                        }
                    }
*/
                csImage.Leave();
            }
        }
    }

labError:

#if CISST_SVL_HAS_XV
    if (xvShmInfo) {
        for (i = 0; i < NumOfWins; i ++) {
            XShmDetach(xDisplay, &(xvShmInfo[i]));
            shmdt(xvShmInfo[i].shmaddr);
        }
        delete [] xvShmInfo; 
        xvShmInfo = 0;
    }
#endif // CISST_SVL_HAS_XV
    XSync(xDisplay, 0);
    for (i = 0; i < NumOfWins; i ++) {
        if (xGCs[i]) XFreeGC(xDisplay, xGCs[i]);
        if (xWindows[i]) XDestroyWindow(xDisplay, xWindows[i]);
    }
    XCloseDisplay(xDisplay);
#if CISST_SVL_HAS_XV
    if (xvImg) {
        for (i = 0; i < NumOfWins; i ++) {
            if (xvImg[i]) XFree(xvImg[i]); 
        }
        delete [] xvImg; 
        xvImg = 0;
    }
    if (xvPort) {
        delete [] xvPort; 
        xvPort = 0;
    }
#else // CISST_SVL_HAS_XV
    if (xImg) {
        for (i = 0; i < NumOfWins; i ++) {
            if (xImg[i]) XDestroyImage(xImg[i]); 
        }
        delete [] xImg; 
        xImg = 0;
    }
    if (xImageBuffers) {
        delete [] xImageBuffers;
        xImageBuffers = 0;
    }
#endif // CISST_SVL_HAS_XV
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

    xScreen = 0;
    xDisplay = 0;

    if (DestroyedSignal) DestroyedSignal->Raise();

    return 0;
}

void svlWindowManagerX11::Show(bool show, int winid)
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

void svlWindowManagerX11::LockBuffers()
{
    csImage.Enter();
}

void svlWindowManagerX11::UnlockBuffers()
{
    csImage.Leave();
}

void svlWindowManagerX11::SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid)
{
    if (xDisplay == 0 || buffer == 0 || winid >= NumOfWins) return;

#if (CISST_SVL_HAS_XV == OFF)
    if (xImageBuffers == 0 || xImageBuffers[winid] == 0)  return;
    unsigned char *framebuffer = xImageBuffers[winid];
#endif // CISST_SVL_HAS_XV
 
    const unsigned int pixsize = Width[winid] * Height[winid];
    if (buffersize > (pixsize * 3)) return;
/*
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
*/
#if CISST_SVL_HAS_XV
    // convert image to YUV2 directly into the output buffer
    svlConverter::BGR24toYUV422(buffer,
                                reinterpret_cast<unsigned char*>(xvImg[winid]->data),
                                pixsize);
#else // CISST_SVL_HAS_XV
    // store image in buffer
    for (unsigned int i = 0; i < pixsize; i ++) {
        *framebuffer = *buffer; framebuffer ++; buffer ++;
        *framebuffer = *buffer; framebuffer ++; buffer ++;
        *framebuffer = *buffer; framebuffer ++; buffer ++;
        *framebuffer = 0;  framebuffer ++;
    }
#endif // CISST_SVL_HAS_XV
}

void svlWindowManagerX11::DrawImages()
{
    ImageCounter ++;
    signalImage.Raise();
}

void svlWindowManagerX11::Destroy()
{
    DestroyFlag = true;
    signalImage.Raise();

    if (DestroyedSignal) {
        DestroyedSignal->Wait(1.0);
        delete(DestroyedSignal);
        DestroyedSignal = 0;
    }
}

void svlWindowManagerX11::DestroyThreadSafe()
{
    Destroy();
}

