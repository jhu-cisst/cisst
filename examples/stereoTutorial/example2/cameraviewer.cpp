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


///////////////////////////////////
//  Icon drawer callback class   //
///////////////////////////////////

class CViewerIconDrawerCallback : public svlImageSamplerCallbackBase
{
public:
    CViewerIconDrawerCallback() :
        svlImageSamplerCallbackBase(),
        RecordFlag(false),
        AspectRatio(1.0)
    {
    }

    void FrameCallback(svlSampleImageBase* image,
                       svlBMPFileHeader* fileheader1, svlDIBHeader* dibheader1,
                       svlBMPFileHeader* fileheader2, svlDIBHeader* dibheader2)
    {
        if (RecordFlag && ((GetFrameCounter() / 15) % 2) == 0) {

            svlRGB* img = (svlRGB*)(((svlSampleImageRGB*)image)->GetPointer());
            unsigned int w = ((svlSampleImageRGB*)image)->GetWidth();

            unsigned int i, j;
            svlRGB *tptr;
            unsigned int iconwidth = w / 50;
            unsigned int iconheight = (unsigned int)(AspectRatio * iconwidth);

            // draw icon
            for (j = 0; j < iconheight; j ++) {
                tptr = img + j * w;
                for (i = 0; i < iconwidth; i ++) {
                    tptr->R = (255 + tptr->R) / 2;
                    tptr->G /= 2;
                    tptr->B /= 2;
                    tptr ++;
                }
            }
        }
    }

    void SetAspectRatio(double ratio) { AspectRatio = ratio; }
    void Record() { RecordFlag = true; }
    void Pause() { RecordFlag = false; }

private:
    bool RecordFlag;
    double AspectRatio;
};

///////////////////////////////////
//     Window callback class     //
///////////////////////////////////

class CViewerWindowCallback : public svlImageWindowCallbackBase
{
public:
    CViewerWindowCallback() : svlImageWindowCallbackBase()
    {
        IconDrawerFilter = 0;
        RecorderFilter = 0;
        Recording = false;
        ShowFramerate = true;
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
                    if (RecorderFilter && IconDrawerFilter) {
                        if (Recording) {
                            ((svlVideoFileWriter*)RecorderFilter)->Pause();
                            IconDrawerFilter->Pause();
                            Recording = false;
                            cout << endl << " >>> Recording paused <<<" << endl;
                        }
                        else {
                            ((svlVideoFileWriter*)RecorderFilter)->Record(-1);
                            IconDrawerFilter->Record();
                            Recording = true;
                            cout << endl << " >>> Recording started <<<" << endl;
                        }
                    }
                }
                break;

                default:
                    return;
            }
        }
    }

    CViewerIconDrawerCallback* IconDrawerFilter;
    svlFilterBase* RecorderFilter;
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

int CameraViewer(bool save, unsigned int fps, bool interpolation, int width, int height)
{
    // instantiating SVL stream and filters
    svlStreamManager viewer_stream(2);
    svlVideoCaptureSource viewer_source(false);
    svlImageResizer viewer_resizer;
    svlImageSampler viewer_icondrawer;
    CViewerIconDrawerCallback viewer_icondrawer_cb;
    svlImageWindow viewer_window;
    CViewerWindowCallback viewer_window_cb;
    svlVideoFileWriter viewer_writer;

    // setup source
    // Delete "device.dat" to reinitialize input device
    if (viewer_source.LoadSettings("device.dat") != SVL_OK) {
        cout << endl;
        viewer_source.DialogSetup();
    }

    // setup writer
    if (save == true) {
        if (viewer_writer.LoadCodecSettings("codec.dat") != SVL_OK) {
            viewer_writer.DialogCodec();
            viewer_writer.SetFramerate(static_cast<double>(fps));
            viewer_writer.SetKeyFrameInteval(std::max(1u, fps / 3));
        }
        cout << endl;
        viewer_writer.DialogFilePath();
        cout << endl;
        viewer_writer.Pause();
    }

    // setup resizer
    if (width > 0 && height > 0) {
        viewer_resizer.EnableInterpolation(interpolation);
        viewer_resizer.SetOutputSize(width, height);
    }

    // setup icon drawer
    viewer_icondrawer.SetCallback(&viewer_icondrawer_cb);
    if (width > 0 && height > 0) {
        viewer_icondrawer_cb.SetAspectRatio((4.0 * height) / (3.0 * width));
    }
    else {
        viewer_icondrawer_cb.SetAspectRatio(1.0);
    }

    // setup image window
    if (save == true) {
        viewer_window_cb.IconDrawerFilter = &viewer_icondrawer_cb;
        viewer_window_cb.RecorderFilter = &viewer_writer;
    }
    viewer_window.SetCallback(&viewer_window_cb);
    viewer_window.SetTitleText("Camera Viewer");

    // chain filters to pipeline
    if (viewer_stream.Trunk().Append(&viewer_source) != SVL_OK) goto labError;
    if (save == true) {
        if (viewer_stream.Trunk().Append(&viewer_writer) != SVL_OK) goto labError;
    }
    if (width > 0 && height > 0) {
        if (viewer_stream.Trunk().Append(&viewer_resizer) != SVL_OK) goto labError;
    }
    if (save == true) {
        if (viewer_stream.Trunk().Append(&viewer_icondrawer) != SVL_OK) goto labError;
    }
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
        cerr << "  In command window:" << endl;
        cerr << "    'i'   - Adjust image properties" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

#ifdef _WIN32
        ch = _getch();
#endif
#ifdef __GNUC__
        ch = getchar();
#endif
        switch (ch) {
            case 'i':
                // Adjust image properties
                viewer_window_cb.ShowFramerate = false;
                cerr << endl << endl;
                viewer_source.DialogImageProperties();
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
    viewer_source.SaveSettings("device.dat");
    if (save == true) viewer_writer.SaveCodecSettings("codec.dat");

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
    cerr << endl << "svlCameraViewer - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl;
    cerr << "Enter 'svlCameraViewer -?' for help." << endl;

    //////////////////////////////
    // parsing arguments
    int i, options, ivalue, width, height, fps;
    bool interpolation, save;

    options = argc - 1;
    interpolation = false;
    width = -1;
    height = -1;
    save = false;
    fps = 30;

    for (i = 1; i <= options; i ++) {
        if (argv[i][0] != '-') continue;

        switch (argv[i][1]) {
            case '?':
                cerr << "Command line format:" << endl;
                cerr << "     svlCameraViewer [options]" << endl;
                cerr << "Options:" << endl;
                cerr << "     -f        Save video file" << endl;
                cerr << "     -r#       Video file frame rate (frames per second) [default: 30]" << endl;
                cerr << "     -i        Interpolation ON [default: OFF]" << endl;
                cerr << "     -w#       Displayed image width" << endl;
                cerr << "     -h#       Displayed image height" << endl;
                cerr << "Examples:" << endl;
                cerr << "     svlCameraViewer" << endl;
                cerr << "     svlCameraViewer -w800 -h600" << endl;
                cerr << "     svlCameraViewer -f -i -w1024 -h768" << endl;
                return 1;
            break;

            case 'i':
                interpolation = true;
            break;

            case 'f':
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

            case 'r':
                ivalue = ParseNumber(argv[i] + 2, 3);
                if (ivalue > 0) fps = ivalue;
            break;

            default:
                // NOP
            break;
        }
    }

    //////////////////////////////
    // starting viewer

    CameraViewer(save, fps, interpolation, width, height);

    cerr << "Quit" << endl;
    return 1;
}

