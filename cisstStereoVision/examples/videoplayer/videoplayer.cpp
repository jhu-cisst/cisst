/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlWindowManagerBase.h>
#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>

#if CISST_HAS_QT4
    #include <cisstStereoVision/svlQtObjectFactory.h>
    #include <cisstStereoVision/svlQtWidgetFileOpen.h>
    #if CISST_HAS_OPENGL
        #include <cisstStereoVision/svlFilterImageWindowQt.h>
    #endif
    // Qt dialogs are disabled by default
    #define _USE_QT_        1
    #define _NO_CONSOLE_    1
#else
    #define _USE_QT_        0
    #define _NO_CONSOLE_    0
#endif

using namespace std;

////////////////////////////////////////
//     Window event handler class     //
////////////////////////////////////////

class CViewerEventHandler : public svlWindowEventHandlerBase
{
public:
    CViewerEventHandler() :
        svlWindowEventHandlerBase()
        ,Source(0)
        ,Paused(false)
        ,Quit(0)
    {
    }

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
        if (ascii) {
            switch (eventid) {
                case 'n':
                    if (Source) {
                        int pos = Source->GetPosition() - 100;
                        if (pos < 0) pos = 0;
                        Source->SetPosition(pos);
                    }
                break;

                case 'm':
                    if (Source) {
                        int pos = Source->GetPosition() + 100;
                        int len = Source->GetLength();
                        if (pos >= len) pos = len - 1;
                        Source->SetPosition(pos);
                    }
                break;

                case 'p':
                    if (Source) {
                        if (Paused) {
                            // Resume playback
                            Source->Play();
                            Paused = false;
                            cerr << "Playback resumed..." << endl;
                        }
                        else {
                            // Pause source
                            Source->Pause();
                            Paused = true;
                            cerr << "Playback paused..." << endl;
                        }
                    }
                break;

                case 'q':
                    if (Quit) Quit[0] = true;
                break;
            }
        }
    }

    svlFilterSourceVideoFile* Source;
    bool Paused;
    bool* Quit;
};


////////////////////
//  Video Player  //
////////////////////

int VideoPlayer(std::string pathname)
{
    bool quit = false;

    svlInitialize();

    // instantiating SVL stream and filters
    svlStreamManager stream(4);
    svlFilterSourceVideoFile source(1);
    svlFilterImageResizer resizer;
    svlFilterImageOverlay overlay;
#if _USE_QT_ && CISST_HAS_OPENGL
    svlFilterImageWindowQt window;
#else // _USE_QT_ && CISST_HAS_OPENGL
    svlFilterImageWindow window;
#endif // _USE_QT_ && CISST_HAS_OPENGL
    CViewerEventHandler window_cb;

    // setup resizer
    resizer.SetOutputRatio(0.5, 0.5);

    // setup overlay
    svlOverlayFramerate ovrl_fps(0, true, &overlay, svlRect(4, 24, 49, 41),
                                 15.0, svlRGB(255, 200, 200), svlRGB(32, 32, 32));
    overlay.AddOverlay(ovrl_fps);

    // Add timestamp overlay
    svlOverlayTimestamp ts_overlay(0, true, &window, svlRect(4, 4, 134, 21),
                                   15.0, svlRGB(255, 200, 200), svlRGB(32, 32, 32));
    overlay.AddOverlay(ts_overlay);

     // setup source
#if _NO_CONSOLE_
    svlQtWidgetFileOpen* widget_fileopen = svlQtWidgetFileOpen::New("Video Files", "avi;mpg;cvi;ncvi;njpg;");
    if (widget_fileopen->WaitForClose()) {
        source.SetFilePath(widget_fileopen->GetFilePath());
    }
    else return 0;
    widget_fileopen->Delete();
#else // _NO_CONSOLE_
    if (pathname.empty()) {
        source.DialogFilePath();
    }
    else {
        if (source.SetFilePath(pathname) != SVL_OK) {
            cerr << endl << "Wrong file name... " << endl;
            return 0;
        }
    }
#endif // _NO_CONSOLE_

    // setup image window
    window.SetTitle("Video Player");
    window.SetEventHandler(&window_cb);
    window_cb.Source = &source;
    window_cb.Quit   = &quit;

    // chain filters to pipeline
    stream.SetSourceFilter(&source);
#if 0
    source.GetOutput()->Connect(resizer.GetInput());
    resizer.GetOutput()->Connect(overlay.GetInput());
#else
    source.GetOutput()->Connect(overlay.GetInput());
#endif
    overlay.GetOutput()->Connect(window.GetInput());

    cerr << endl << "Starting stream... " << endl;
    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In image window:" << endl;
    cerr << "    'p'   - Pause/Resume playback" << endl;
    cerr << "    'n'   - Seek 100 frames back" << endl;
    cerr << "    'm'   - Seek 100 frames forward" << endl;
    cerr << "    'q'   - Quit" << endl;

    // initialize and start stream
    if (stream.Play() != SVL_OK) return 0;

    // wait for quit command
    while (!quit) osaSleep(0.1);

    cerr << endl;

    // stop and release stream
    stream.Release();
    stream.DisconnectAll();

    return 0;
}


//////////////////////////////////
//             main             //
//////////////////////////////////

int ParseNumber(char* string, unsigned int maxlen)
{
    if (string == 0 || maxlen == 0) return -1;

    int ivalue, j;
    char ch;

    // parse number
    j = 0;
    ivalue = 0;
    ch = string[j];
    // 4 digits max
    while (ch != 0 && j < (int)maxlen) {
        // check if number
        ch -= '0';
        if (ch > 9 || ch < 0) {
            ivalue = -1;
            break;
        }
        ivalue = ivalue * 10 + ch;
        // step to next digit
        j ++;
        ch = string[j];
    }
    if (j == 0) ivalue = -1;

    return ivalue;
}

int my_main(int argc, char** argv)
{
    cerr << "svlExVideoPlayer - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl << endl;
    cerr << "Command line format:" << endl;
    cerr << "     svlExVideoPlayer [pathname-optional]" << endl;
    cerr << "Example:" << endl;
    cerr << "     svlExVideoPlayer video.cvi" << endl;

    if (argc > 1) VideoPlayer(argv[1]);
    else VideoPlayer("");

    cerr << "Quit" << endl << endl;


    return 1;
}

SETUP_QT_ENVIRONMENT(my_main)
