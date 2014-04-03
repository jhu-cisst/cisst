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

#include "winWin32.h"
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>


/****************************/
/*** svlWindowWin32 class ***/
/****************************/

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

svlWindowWin32::svlWindowWin32(unsigned int _id)
{
    ID = _id;    
    hWnd = NULL;
    ClientOffsetX = 0;
    ClientOffsetY = 0;
}

svlWindowWin32::~svlWindowWin32()
{
    Destroy();
}

int svlWindowWin32::Create(unsigned int width, unsigned int height, bool show,
                         const std::string title, int titleid,
                         bool borderless, int blposx, int blposy)
{
    Destroy();

    std::string classnamestr;
    std::ostringstream classnametext;
    classnametext << "svlW32WindowClass#" << ID;
    classnamestr = classnametext.str();

    std::ostringstream ostring;
    if (title.length() > 0) {
        if (titleid >= 0) ostring << title << " #" << titleid;
        else ostring << title;
    }
    else {
        if (titleid >= 0) ostring << title << "svlFilterImageWindow #" << titleid;
        else ostring << "svlFilterImageWindow";
    }
    Title = ostring.str();

    // Registering window class
    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX); 
    wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
    wcex.lpfnWndProc	= reinterpret_cast<WNDPROC>(WndProc);
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= NULL;
    wcex.hIcon			= NULL;     // default icon
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= reinterpret_cast<HBRUSH>(5 + 1);
    wcex.lpszMenuName	= NULL;     // no menu
    wcex.lpszClassName	= classnamestr.c_str();
    wcex.hIconSm		= NULL;     // default icon
    RegisterClassEx(&wcex);

    // Compute total window size
    RECT rect;
    rect.left = 0;
    rect.right = width;
    rect.top = 0;
    rect.bottom = height;
    if (!borderless) {
        AdjustWindowRect(&rect, WS_CAPTION|WS_BORDER, FALSE);
        ClientOffsetX = rect.left;
        ClientOffsetY = rect.top;
    }
    else {
        ClientOffsetX = 0;
        ClientOffsetY = 0;
    }

    if (blposx == DEFAULT_WINDOW_POS) blposx = CW_USEDEFAULT;
    if (blposy == DEFAULT_WINDOW_POS) blposy = 0;

    // Perform application initialization:
    if (!borderless) {
        hWnd = CreateWindow(classnametext.str().c_str(),
                            Title.c_str(),
                            WS_OVERLAPPED,              // style: non-resizable, no system menu
                            blposx,                     // window position [x]
                            blposy,                     // window position [y]
                            rect.right - rect.left,     // window size [width]
                            rect.bottom - rect.top,     // window size [height]
                            NULL,                       // parent
                            NULL,
                            NULL,
                            NULL);
    }
    else {
        hWnd = CreateWindow(classnametext.str().c_str(),
                            Title.c_str(),
                            WS_POPUP,                   // style: non-resizable, no system menu, no border
                            blposx,                     // window position [x]
                            blposy,                     // window position [y]
                            rect.right - rect.left,     // window size [width]
                            rect.bottom - rect.top,     // window size [height]
                            NULL,                       // parent
                            NULL,
                            NULL,
                            NULL);
    }
    if (!hWnd) return -1;

    Show(show);

    return 0;
}

int svlWindowWin32::ProcessMessage(MSG* msg)
{
    if (msg == NULL) return -1;
    if (msg->hwnd == hWnd) {
		TranslateMessage(msg);
		DispatchMessage(msg);
        return 0;
    }
    return 1;
}

void svlWindowWin32::Destroy()
{
    if (hWnd) {
        DestroyWindow(hWnd);
        hWnd = NULL;
    }
}

int svlWindowWin32::Show(bool show)
{
    if (hWnd) {
        if (show) {
            ShowWindow(hWnd, SW_SHOWNORMAL);
            UpdateWindow(hWnd);
        }
        else {
            ShowWindow(hWnd, SW_HIDE);
        }
        return 0;
    }
    return -1;
}

void svlWindowWin32::GetTitle(std::string & title)
{
    title = Title;
}

void svlWindowWin32::SetTitle(const std::string title)
{
    SetWindowText(hWnd, title.c_str());
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	    case WM_CLOSE: // called on Alt+F4
            // NOP
		break;

	    case WM_PAINT:
            SendMessage(hwnd, WM_USER_PAINT, 0, 0);
		break;

	    default:
		    return DefWindowProc(hwnd, message, wparam, lparam);
	}

	return 0;
}


/***********************************/
/*** svlWindowManagerWin32 class ***/
/***********************************/

svlWindowManagerWin32::svlWindowManagerWin32(unsigned int numofwins) : svlWindowManagerBase(numofwins)
{
    Windows = 0;
    WindowHandles = 0;
    LineSize = 0;
    Padding = 0;
    ImageBuffers = 0;
    ImageBufferSizes = 0;
    BitmapInfos = 0;
    LButtonDown = false;
    RButtonDown = false;
}

svlWindowManagerWin32::~svlWindowManagerWin32()
{
    // Critical section: starts
    csImage.Enter();

        Destroy();

    csImage.Leave();
    // Critical section: ends
}

int svlWindowManagerWin32::DoModal(bool show, bool fullscreen)
{
    Destroy();

    unsigned int i, posx = 0;

    WindowHandles = new HWND[NumOfWins];
    Windows = new svlWindowWin32*[NumOfWins];
    for (i = 0; i < NumOfWins; i ++) {
        Windows[i] = new svlWindowWin32(i);
    }

    if (PosX == 0 || PosY == 0) {
        if (NumOfWins == 1) {
            Windows[0]->Create(Width[0], Height[0], true, Title, -1, fullscreen, DEFAULT_WINDOW_POS, DEFAULT_WINDOW_POS);
        }
        else {
            for (i = 0; i < NumOfWins; i ++) {
                Windows[i]->Create(Width[i], Height[i], true, Title, static_cast<int>(i), fullscreen, posx, 0);
                posx += Width[i];
            }
        }
    }
    else {
        if (NumOfWins == 1) {
            Windows[0]->Create(Width[0], Height[0], true, Title, -1, fullscreen, PosX[0], PosY[0]);
        }
        else {
            for (i = 0; i < NumOfWins; i ++) {
                Windows[i]->Create(Width[i], Height[i], true, Title, static_cast<int>(i), fullscreen, PosX[i], PosY[i]);
            }
        }
    }

    // getting window handles
    for (i = 0; i < NumOfWins; i ++) WindowHandles[i] = Windows[i]->GetHandle();

    // allocating image buffers
    LineSize = new unsigned int[NumOfWins];
    Padding = new unsigned int[NumOfWins];
    ImageBuffers = new unsigned char*[NumOfWins];
    ImageBufferSizes = new unsigned int[NumOfWins];
    for (i = 0; i < NumOfWins; i ++) {
        LineSize[i] = Width[i] * 3;
        Padding[i] = (4 - (LineSize[i] % 4)) % 4;
        LineSize[i] += Padding[i];
        ImageBufferSizes[i] = LineSize[i] * Height[i] * 3;
        ImageBuffers[i] = new unsigned char[ImageBufferSizes[i]];
    }

    // allocating bitmapinfo structures
    BitmapInfos = new BITMAPINFO[NumOfWins];
    memset(BitmapInfos, 0, NumOfWins * sizeof(BITMAPINFO));
    for (i = 0; i < NumOfWins; i ++) {
        BitmapInfos[i].bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        BitmapInfos[i].bmiHeader.biWidth =  Width[i];
	    BitmapInfos[i].bmiHeader.biHeight = -1 * static_cast<int>(Height[i]);
	    BitmapInfos[i].bmiHeader.biPlanes = 1;
	    BitmapInfos[i].bmiHeader.biBitCount = 24;
    }

    // signal that initialization is done
    if (InitReadySignal) InitReadySignal->Raise();

	// main message loop
    MSG msg;
    bool anyonealive;
	while (GetMessage(&msg, NULL, 0, 0)) {
        // finding recipient
        for (i = 0; i < NumOfWins; i ++)
            if (msg.hwnd == WindowHandles[i]) break;

        // dispatching message
        if (i < NumOfWins && FilterMessage(i, &msg) == 0)
            Windows[i]->ProcessMessage(&msg);

        // updating handles and looking for windows still opened
        anyonealive = false;
        if (Windows && WindowHandles) {
            for (i = 0; i < NumOfWins; i ++) {
                WindowHandles[i] = Windows[i]->GetHandle();
                if (WindowHandles[i]) anyonealive = true;
            }
        }
        if (!anyonealive) break;
	}

    return 0;
}

void svlWindowManagerWin32::Show(bool show, int winid)
{
    if (Windows && winid < static_cast<int>(NumOfWins)) {
        // Critical section: starts
        csImage.Enter();

            if (winid < 0)
                for (unsigned int i = 0; i < NumOfWins; i ++) Windows[i]->Show(show);
            else
                Windows[static_cast<unsigned int>(winid)]->Show(show);

        csImage.Leave();
        // Critical section: ends
    }
}

void svlWindowManagerWin32::LockBuffers()
{
    csImage.Enter();
}

void svlWindowManagerWin32::UnlockBuffers()
{
    csImage.Leave();
}

void svlWindowManagerWin32::SetImageBuffer(unsigned char *buffer, unsigned int buffersize, unsigned int winid)
{
    if (Windows && winid < static_cast<int>(NumOfWins) &&
        Padding != 0 && LineSize != 0 && ImageBufferSizes != 0 && ImageBuffers != 0 &&
        ImageBufferSizes[winid] >= buffersize) {

        if (Padding[winid] == 0) {
            memcpy(ImageBuffers[winid], buffer, buffersize);
        }
        else {
            const unsigned int width = Width[winid] * 3;
            const unsigned int height = Height[winid];
            const unsigned int linesize = LineSize[winid];
            unsigned char* src = buffer;
            unsigned char* dst = ImageBuffers[winid];
            for (unsigned int i = 0; i < height; i ++) {
                memcpy(dst, src, width);
                src += width;
                dst += linesize;
            }
        }
/*
        // Display timestamp if requested
        if (Timestamp > 0.0) {
            std::string title;
            Windows[winid]->GetTitle(title);
            std::ostringstream timestampstring;
            timestampstring << " (timestamp=" << std::fixed << std::setprecision(3) << Timestamp << ")";
            title += timestampstring.str();

            // Critical section: starts
            csImage.Enter();

                Windows[winid]->SetTitle(title);

            csImage.Leave();
            // Critical section: ends
        }
        else {
            if (Timestamp < 0.0) {
                // Restore original timestamp
                std::string title;
                Windows[winid]->GetTitle(title);

                // Critical section: starts
                csImage.Enter();

                    Windows[winid]->SetTitle(title);

                csImage.Leave();
                // Critical section: ends
            }
        }
*/
    }
}

void svlWindowManagerWin32::DrawImages()
{
    if (Windows && Padding != 0 && LineSize != 0 && ImageBufferSizes != 0 && ImageBuffers != 0) {
        RECT rect;
        for (unsigned int i = 0; i < NumOfWins; i ++) {
            // force update
            rect.left = 0;
            rect.right = Width[i];
            rect.top = 0;
            rect.bottom = Height[i];

            // Critical section: starts
            csImage.Enter();

                InvalidateRect(WindowHandles[i], &rect, FALSE);
                UpdateWindow(WindowHandles[i]);

            csImage.Leave();
            // Critical section: ends
        }
    }
}

void svlWindowManagerWin32::Destroy()
{
    unsigned int i;
    if (Windows){
        for (i = 0; i < NumOfWins; i ++)
            if (Windows[i]) delete Windows[i];
        delete [] Windows;
        Windows = 0;
    }
    if (WindowHandles) {
        delete [] WindowHandles;
        WindowHandles = 0;
    }
    if (LineSize) {
        delete [] LineSize;
        LineSize = 0;
    }
    if (Padding) {
        delete [] Padding;
        Padding = 0;
    }
    if (ImageBuffers) {
        for (i = 0; i < NumOfWins; i ++)
            if (ImageBuffers[i]) delete [] ImageBuffers[i];
        delete [] ImageBuffers;
        ImageBuffers = 0;
    }
    if (ImageBufferSizes) {
        delete [] ImageBufferSizes;
        ImageBufferSizes = 0;
    }
    if (BitmapInfos) {
        delete [] BitmapInfos;
        BitmapInfos = 0;
    }
}

void svlWindowManagerWin32::DestroyThreadSafe()
{
    if (WindowHandles) {
        for (unsigned int i = 0; i < NumOfWins; i ++)
            if (WindowHandles[i]) {
                PostMessage(WindowHandles[i], WM_USER_DESTROY, 0, 0);
                break; // it's enough to send the message to one of the windows
            }
    }
}

int svlWindowManagerWin32::FilterMessage(unsigned int winid, MSG* msg)
{
	PAINTSTRUCT ps;
    unsigned int code;
	HDC hdc;

    switch (msg->message) {
	    case WM_PAINT:
	    case WM_USER_PAINT:
            // Critical section: starts
//            csImage.Enter();

		        hdc = BeginPaint(msg->hwnd, &ps);
                if (ImageBuffers[winid]) {
                    SetDIBitsToDevice(hdc,
                                      0, 0,
                                      Width[winid], Height[winid],
                                      0, 0,
                                      0, Height[winid],
                                      ImageBuffers[winid],
                                      &(BitmapInfos[winid]),
                                      DIB_RGB_COLORS);
                }
    		    EndPaint(msg->hwnd, &ps);

//            csImage.Leave();
            // Critical section: ends
		break;

        case WM_MOUSEMOVE:
            SetMousePos(static_cast<short>(LOWORD(msg->lParam)), static_cast<short>(HIWORD(msg->lParam)));
            OnUserEvent(winid, false, winInput_MOUSEMOVE);
        break;

        case WM_LBUTTONDOWN:
            if (!LButtonDown && !RButtonDown) {
                LButtonDown = true;
                SetCapture(msg->hwnd);
            }
            OnUserEvent(winid, false, winInput_LBUTTONDOWN);
        break;

        case WM_LBUTTONUP:
            OnUserEvent(winid, false, winInput_LBUTTONUP);
            if (LButtonDown && !RButtonDown) {
                LButtonDown = false;
                ReleaseCapture();
            }
        break;

        case WM_RBUTTONDOWN:
            if (!LButtonDown && !RButtonDown) {
                RButtonDown = true;
                SetCapture(msg->hwnd);
            }
            OnUserEvent(winid, false, winInput_RBUTTONDOWN);
        break;

        case WM_RBUTTONUP:
            OnUserEvent(winid, false, winInput_RBUTTONUP);
            if (!LButtonDown && RButtonDown) {
                RButtonDown = false;
                ReleaseCapture();
            }
        break;

	    case WM_KEYDOWN:
            code = static_cast<unsigned int>(msg->wParam);
            if (code >= 48 && code <= 57) { // ascii numbers
                OnUserEvent(winid, true, code);
                break;
            }
            if (code >= 65 && code <= 90) { // ascii letters
                OnUserEvent(winid, true, code + 32);
                break;
            }
            if (code == 13 ||
                code == 32) { // special characters with correct ascii code
                OnUserEvent(winid, true, code);
                break;
            }
            if (code >= 112 && code <= 123) { // F1-F12
                OnUserEvent(winid, false, code);
                break;
            }
            if ((msg->lParam | 0x800000) == 0) {
                return 0; // yet to be processed
            }
            else {
                // Virtual keys
		        switch (msg->wParam) {
		            case winInput_KEY_PAGEUP:
		            case winInput_KEY_PAGEDOWN:
		            case winInput_KEY_HOME:
		            case winInput_KEY_END:
		            case winInput_KEY_INSERT:
		            case winInput_KEY_DELETE:
		            case winInput_KEY_LEFT:
		            case winInput_KEY_RIGHT:
		            case winInput_KEY_UP:
		            case winInput_KEY_DOWN:
                        OnUserEvent(winid, false, static_cast<unsigned int>(msg->wParam));
		            break;

		            default:
			            return 0; // yet to be processed
		        }
            }
	    break;

        case WM_USER_DESTROY:
            // Critical section: starts
            csImage.Enter();

                Destroy();

            csImage.Leave();
            // Critical section: ends
        break;

        default:
            return 0; // yet to be processed
    }
    return 1; // already processed
}

