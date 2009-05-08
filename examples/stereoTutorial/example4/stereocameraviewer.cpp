/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifdef _WIN32
#include <conio.h>
#endif // _WIN32

#include <iostream>
#include <string>
#include <cisstStereoVision.h>

#ifdef __GNUC__
#include <curses.h>
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif // __GNUC__

using namespace std;


////////////////////////
//     FPS filter     //
////////////////////////

class CFPSFilter : public svlFilterBase
{
public:
    CFPSFilter() :
        svlFilterBase(),
        FrameCount(0)
    {
        AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    }

protected:
    int Initialize(svlSample* inputdata = 0)
    {
        OutputData = inputdata;
        return SVL_OK;
    }

    int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0)
    {
        _OnSingleThread(procInfo) {
            if ((FrameCount % 30) == 0) {
#ifdef _WIN32
                DWORD now;
                now = ::GetTickCount();

                if (FrameCount > 0) {
                    DWORD msec = now - StartMSec;
                    printf("\rFrame #: %07d; %02.2f frames per second  ", FrameCount, (double)30000 / msec);
                }

                StartMSec = now;
#endif // _WIN32

#ifdef __GNUC__
                timeval now;
                gettimeofday(&now, 0);

                if (FrameCount > 0) {
                    int sec = now.tv_sec - StartSec;
                    int usec = now.tv_usec - StartUSec;
                    usec += 1000000 * sec;
                    printf("\rFrame #: %07d; %02.2f frames per second  ", FrameCount, (double)30000000 / usec);
                    fflush(stdout);
                }

                StartSec = now.tv_sec;
                StartUSec = now.tv_usec;
#endif // __GNUC__
            }
            FrameCount ++;
        }
        return SVL_OK;
    }

private:
    unsigned int FrameCount;
#ifdef _WIN32
    DWORD StartMSec;
#endif // _WIN32
#ifdef __GNUC__
    unsigned int StartSec;
    unsigned int StartUSec;
#endif // __GNUC__
};


///////////////////////////////////
//     Window callback class     //
///////////////////////////////////

class CViewerWindowCallback : public svlImageWindowCallbackBase
{
public:
    CViewerWindowCallback() : svlImageWindowCallbackBase()
    {
        VideoWriterFilter = 0;
        ImageWriterFilter = 0;
        ShowFramerate = true;
        Recording = false;
    }

    void OnNewFrame(unsigned int frameid)
    {
        if (ShowFramerate) {
            if ((frameid % 30) == 0) {
#ifdef _WIN32
                DWORD now;
                now = ::GetTickCount();

                if (frameid > 0) {
                    DWORD msec = now - StartMSec;
                    printf("\rFrame #: %07d; %02.2f frames per second  ", frameid, (double)30000 / msec);
                }

                StartMSec = now;
#endif // _WIN32

#ifdef __GNUC__
                timeval now;
                gettimeofday(&now, 0);

                if (frameid > 0) {
                    int sec = now.tv_sec - StartSec;
                    int usec = now.tv_usec - StartUSec;
                    usec += 1000000 * sec;
                    printf("\rFrame #: %07d; %02.2f frames per second  ", frameid, (double)30000000 / usec);
                    fflush(stdout);
                }

                StartSec = now.tv_sec;
                StartUSec = now.tv_usec;
#endif // __GNUC__
            }
        }
    }

    void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
    {
        // handling user inputs
        if (ascii) {
            switch (eventid) {
                case ' ':
                {
                    if (VideoWriterFilter) {
                        if (Recording) {
                            VideoWriterFilter->Pause();
                            Recording = false;
                            cout << endl << " >>> Recording paused <<<" << endl;
                        }
                        else {
                            VideoWriterFilter->Record(-1);
                            Recording = true;
                            cout << endl << " >>> Recording started <<<" << endl;
                        }
                    }
                }
                break;

                case 's':
                {
                    if (ImageWriterFilter) {
                        ImageWriterFilter->Record(1);
                        cout << endl << " >>> Snapshots saved <<<" << endl;
                    }
                }
                break;

                default:
                    return;
            }
        }
    }

    svlImageFileWriter* ImageWriterFilter;
    svlVideoFileWriter* VideoWriterFilter;
    bool Recording;

    bool ShowFramerate;
#ifdef _WIN32
    DWORD StartMSec;
#endif // _WIN32
#ifdef __GNUC__
    unsigned int StartSec;
    unsigned int StartUSec;
#endif // __GNUC__
};


////////////////////
//  CameraViewer  //
////////////////////

int CameraViewer(bool save, bool interpolation, int width, int height)
{
    // instantiating SVL stream and filters
    svlStreamManager viewer_stream(8);
    svlVideoCaptureSource viewer_source(true);
    svlImageResizer viewer_resizer;
    svlImageWindow viewer_window;
    CViewerWindowCallback viewer_window_cb;
    svlImageFileWriter viewer_imagewriter;
    svlVideoFileWriter viewer_videowriter;
    CFPSFilter viewer_fps;

    // setup source
    // Delete "device.dat" to reinitialize input device
    if (viewer_source.LoadSettings("stereodevice.dat") != SVL_OK) {
        cout << endl;
        viewer_source.DialogSetup(SVL_LEFT);
        viewer_source.DialogSetup(SVL_RIGHT);
    }

    // setup video writer
    if (save == true) {
        viewer_videowriter.DialogFilePath(SVL_LEFT);
        viewer_videowriter.DialogFilePath(SVL_RIGHT);
        viewer_videowriter.SetCompressionLevel(1); // 0-9
        viewer_videowriter.Pause();
    }

    // setup image writer
    viewer_imagewriter.SetFilePath("left_", "bmp", SVL_LEFT);
    viewer_imagewriter.SetFilePath("right_", "bmp", SVL_RIGHT);
    viewer_imagewriter.EnableTimestamps();
    viewer_imagewriter.Pause();

    // setup resizer
    if (width > 0 && height > 0) {
        viewer_resizer.EnableInterpolation(interpolation);
        viewer_resizer.SetOutputSize(width, height, SVL_LEFT);
        viewer_resizer.SetOutputSize(width, height, SVL_RIGHT);
    }

    // setup image window
    viewer_window_cb.ImageWriterFilter = &viewer_imagewriter;
    if (save == true) {
        viewer_window_cb.VideoWriterFilter = &viewer_videowriter;
    }
    viewer_window.SetCallback(&viewer_window_cb);
    viewer_window.SetTitleText("Camera Viewer");
    viewer_window.EnableTimestampInTitle();

    // chain filters to pipeline
    if (viewer_stream.Trunk().Append(&viewer_source) != SVL_OK) goto labError;
    if (save == true) {
        if (viewer_stream.Trunk().Append(&viewer_videowriter) != SVL_OK) goto labError;
    }
    if (viewer_stream.Trunk().Append(&viewer_imagewriter) != SVL_OK) goto labError;
    if (width > 0 && height > 0) {
        if (viewer_stream.Trunk().Append(&viewer_resizer) != SVL_OK) goto labError;
    }
    if (viewer_stream.Trunk().Append(&viewer_fps) != SVL_OK) goto labError;
    if (viewer_stream.Trunk().Append(&viewer_window) != SVL_OK) goto labError;

    cerr << endl << "Starting stream... ";

    // initialize and start stream
    if (viewer_stream.Start() != SVL_OK) goto labError;

    cerr << "Done" << endl;

#ifdef __GNUC__
    ////////////////////////////////////////////////////
    // modify terminal settings for single key inputs
    struct  termios ksettings;
    struct  termios new_ksettings;
    int     kbrd;
    kbrd = open("/dev/tty",O_RDWR);
    
    #if (CISST_OS == CISST_LINUX)
        ioctl(kbrd, TCGETS, &ksettings);
        new_ksettings = ksettings;
        new_ksettings.c_lflag &= !ICANON;
        new_ksettings.c_lflag &= !ECHO;
        ioctl(kbrd, TCSETS, &new_ksettings);
        ioctl(kbrd, TIOCNOTTY);
    #endif // (CISST_OS == CISST_LINUX)
    #if (CISST_OS == CISST_DARWIN)
        ioctl(kbrd, TIOCGETA, &ksettings);
        new_ksettings = ksettings;
        new_ksettings.c_lflag &= !ICANON;
        new_ksettings.c_lflag &= !ECHO;
        ioctl(kbrd, TIOCSETA, &new_ksettings);
        ////////////////////////////////////////////////////
    #endif // (CISST_OS == CISST_DARWIN)
#endif

    // wait for keyboard input in command window
#ifdef _WIN32
    int ch;
#endif
#ifdef __GNUC__
    char ch;
#endif

    do {
        cerr << endl << "Keyboard commands:" << endl << endl;
        cerr << "  In image window:" << endl;
        if (save == true) {
            cerr << "    SPACE - Video recorder control: Record/Pause" << endl;
        }
        cerr << "    's'   - Take image snapshots" << endl;
        cerr << "  In command window:" << endl;
        cerr << "    '1'   - Adjust LEFT image properties" << endl;
        cerr << "    '2'   - Adjust RIGHT image properties" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

#ifdef _WIN32
        ch = _getch();
#endif
#ifdef __GNUC__
        ch = getchar();
#endif
        switch (ch) {
            case '1':
                viewer_window_cb.ShowFramerate = false;
                cerr << endl << endl;
                viewer_source.DialogImageProperties(SVL_LEFT);
                cerr << endl;
                viewer_window_cb.ShowFramerate = true;
            break;

            case '2':
                viewer_window_cb.ShowFramerate = false;
                cerr << endl << endl;
                viewer_source.DialogImageProperties(SVL_RIGHT);
                cerr << endl;
                viewer_window_cb.ShowFramerate = true;
            break;

            default:
            break;
        }
    } while (ch != 'q');

#ifdef __GNUC__
    ////////////////////////////////////////////////////
    // reset terminal settings    
    #if (CISST_OS == CISST_LINUX)
        ioctl(kbrd, TCSETS, &ksettings);
    #endif // (CISST_OS == CISST_LINUX)
    #if (CISST_OS == CISST_DARWIN)
        ioctl(kbrd, TIOCSETA, &ksettings);
    #endif // (CISST_OS == CISST_DARWIN)
    
    close(kbrd);
    ////////////////////////////////////////////////////
#endif

    cerr << endl;

    // stop stream
    viewer_stream.Stop();

    // save settings
    viewer_source.SaveSettings("stereodevice.dat");

    // destroy pipeline
    viewer_stream.EmptyFilterList();

labError:
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

int main(int argc, char** argv)
{
    cerr << endl << "stereoTutorialStereoCameraViewer - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl;
    cerr << "Enter 'stereoTutorialStereoCameraViewer -?' for help." << endl;

    //////////////////////////////
    // parsing arguments
    int i, options, ivalue, width, height;
    bool interpolation, save;

    options = argc - 1;
    save = false;
    interpolation = false;
    width = -1;
    height = -1;

    for (i = 1; i <= options; i ++) {
        if (argv[i][0] != '-') continue;

        switch (argv[i][1]) {
            case '?':
                cerr << "Command line format:" << endl;
                cerr << "     stereoTutorialStereoCameraViewer [options]" << endl;
                cerr << "Options:" << endl;
                cerr << "     -v        Save video files" << endl;
                cerr << "     -i        Interpolation ON [default: OFF]" << endl;
                cerr << "     -w#       Displayed image width" << endl;
                cerr << "     -h#       Displayed image height" << endl;
                cerr << "Examples:" << endl;
                cerr << "     stereoTutorialStereoCameraViewer" << endl;
                cerr << "     stereoTutorialStereoCameraViewer -w800 -h600" << endl;
                cerr << "     stereoTutorialStereoCameraViewer -v -i -w1024 -h768" << endl;
                return 1;
            break;

            case 'i':
                interpolation = true;
            break;

            case 'v':
                save = true;
            break;

            case 'w':
                ivalue = ParseNumber(argv[i] + 2, 4);
                if (ivalue > 0) width = ivalue;
            break;

            case 'h':
                ivalue = ParseNumber(argv[i] + 2, 4);
                if (ivalue > 0) height = ivalue;
            break;

            default:
                // NOP
            break;
        }
    }

    //////////////////////////////
    // starting viewer

    CameraViewer(save, interpolation, width, height);

    cerr << "Quit" << endl;
    return 1;
}

