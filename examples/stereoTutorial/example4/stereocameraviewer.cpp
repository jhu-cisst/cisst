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
#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstStereoVision.h>


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
    int Initialize(svlSample* inputdata)
    {
        OutputData = inputdata;
        return SVL_OK;
    }

    int ProcessFrame(ProcInfo* procInfo, svlSample* CMN_UNUSED(inputdata) = 0)
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
        ImageWriterFilter = 0;
#if (CISST_SVL_HAS_ZLIB == ON)
        VideoWriterFilter = 0;
        ShowFramerate = true;
        Recording = false;
#endif // CISST_SVL_HAS_ZLIB
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

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
        // handling user inputs
        if (ascii) {
            switch (eventid) {
#if (CISST_SVL_HAS_ZLIB == ON)
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
#endif // CISST_SVL_HAS_ZLIB

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

    svlFilterImageFileWriter* ImageWriterFilter;
#if (CISST_SVL_HAS_ZLIB == ON)
    svlFilterVideoFileWriter* VideoWriterFilter;
    bool Recording;
#endif // CISST_SVL_HAS_ZLIB

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

int CameraViewer(bool interpolation, bool save, int width, int height)
{
#if (CISST_SVL_HAS_ZLIB == OFF)
    save = false;
#endif // CISST_SVL_HAS_ZLIB

    // instantiating SVL stream and filters
    svlStreamManager viewer_stream(8);
    svlFilterSourceVideoCapture viewer_source(true);
    svlFilterImageResizer viewer_resizer;
    svlFilterImageWindow viewer_window;
    CViewerWindowCallback viewer_window_cb;
    svlFilterImageFileWriter viewer_imagewriter;
#if (CISST_SVL_HAS_ZLIB == ON)
    svlFilterVideoFileWriter viewer_videowriter;
#endif // CISST_SVL_HAS_ZLIB
    CFPSFilter viewer_fps;

    // setup source
    // Delete "device.dat" to reinitialize input device
    if (viewer_source.LoadSettings("stereodevice.dat") != SVL_OK) {
        cout << endl;
        viewer_source.DialogSetup(SVL_LEFT);
        viewer_source.DialogSetup(SVL_RIGHT);
    }

#if (CISST_SVL_HAS_ZLIB == ON)
    // setup video writer
    if (save == true) {
        viewer_videowriter.DialogFilePath(SVL_LEFT);
        viewer_videowriter.DialogFilePath(SVL_RIGHT);
        viewer_videowriter.SetCompressionLevel(1); // 0-9
        viewer_videowriter.Pause();
    }
#endif // CISST_SVL_HAS_ZLIB

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
#if (CISST_SVL_HAS_ZLIB == ON)
    if (save == true) {
        viewer_window_cb.VideoWriterFilter = &viewer_videowriter;
    }
#endif // CISST_SVL_HAS_ZLIB
    viewer_window.SetCallback(&viewer_window_cb);
    viewer_window.SetTitleText("Camera Viewer");
    viewer_window.EnableTimestampInTitle();

    // chain filters to pipeline
    if (viewer_stream.Trunk().Append(&viewer_source) != SVL_OK) goto labError;
#if (CISST_SVL_HAS_ZLIB == ON)
    if (save == true) {
        if (viewer_stream.Trunk().Append(&viewer_videowriter) != SVL_OK) goto labError;
    }
#endif // CISST_SVL_HAS_ZLIB
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

    // wait for keyboard input in command window
    int ch;

    do {
        cerr << endl << "Keyboard commands:" << endl << endl;
        cerr << "  In image window:" << endl;
#if (CISST_SVL_HAS_ZLIB == ON)
        if (save == true) {
            cerr << "    SPACE - Video recorder control: Record/Pause" << endl;
        }
#endif // CISST_SVL_HAS_ZLIB
        cerr << "    's'   - Take image snapshots" << endl;
        cerr << "  In command window:" << endl;
        cerr << "    '1'   - Adjust LEFT image properties" << endl;
        cerr << "    '2'   - Adjust RIGHT image properties" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

        ch = cmnGetChar();

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
        osaSleep(1.0 * cmn_ms);
    } while (ch != 'q');

    cerr << endl;

    // stop stream
    viewer_stream.Stop();

    // save settings
    viewer_source.SaveSettings("stereodevice.dat");

    // destroy pipeline
    viewer_stream.RemoveAll();

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
#if (CISST_SVL_HAS_ZLIB == ON)
                cerr << "     -v        Save video files" << endl;
#endif // CISST_SVL_HAS_ZLIB
                cerr << "     -i        Interpolation ON [default: OFF]" << endl;
                cerr << "     -w#       Displayed image width" << endl;
                cerr << "     -h#       Displayed image height" << endl;
                cerr << "Examples:" << endl;
                cerr << "     stereoTutorialStereoCameraViewer" << endl;
#if (CISST_SVL_HAS_ZLIB == ON)
                cerr << "     stereoTutorialStereoCameraViewer -v -i -w800 -h600" << endl;
#else // CISST_SVL_HAS_ZLIB
                cerr << "     stereoTutorialStereoCameraViewer -i -w1024 -h768" << endl;
#endif // CISST_SVL_HAS_ZLIB
                return 1;
            break;

            case 'i':
                interpolation = true;
            break;

#if (CISST_SVL_HAS_ZLIB == ON)
            case 'v':
                save = true;
            break;
#endif // CISST_SVL_HAS_ZLIB

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

    CameraViewer(interpolation, save, width, height);

    cerr << "Quit" << endl;
    return 1;
}

