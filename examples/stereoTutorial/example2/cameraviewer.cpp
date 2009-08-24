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


#include <iostream>
#include <string>
#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstStereoVision.h>

using namespace std;

#if (CISST_SVL_HAS_ZLIB == ON)
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
                       svlBMPFileHeader* CMN_UNUSED(fileheader1), svlDIBHeader* CMN_UNUSED(dibheader1),
                       svlBMPFileHeader* CMN_UNUSED(fileheader2), svlDIBHeader* CMN_UNUSED(dibheader2))
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
#endif // CISST_SVL_HAS_ZLIB


////////////////////////
//     FPS filter     //
////////////////////////

class CFPSFilter : public svlFilterBase
{
public:
    CFPSFilter() :
        svlFilterBase(),
        Manager(0),
        ShowFramerate(true)

    {
        AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    }

protected:
    int Initialize(svlSample* inputdata)
    {
        OutputData = inputdata;
        return SVL_OK;
    }

    int ProcessFrame(ProcInfo* procInfo, svlSample* CMN_UNUSED(inputdata) = 0)
    {
        if (!ShowFramerate) return SVL_OK;

        _OnSingleThread(procInfo) {
            unsigned int framecount = GetFrameCounter();
            if ((framecount % 30) == 0) {
#ifdef _WIN32
                DWORD now;
                now = ::GetTickCount();

                if (framecount > 0) {
                    DWORD msec = now - StartMSec;
                    printf("\rFrame #: %07d; %02.2f fps (Buffer: %.0f%%, Dropped: %d)     ",
                           framecount,
                           (double)30000 / msec,
                           Manager->Branch("Recorder").GetBufferUsageRatio() * 100.0,
                           Manager->Branch("Recorder").GetDroppedSampleCount());
                }

                StartMSec = now;
#endif // _WIN32

#ifdef __GNUC__
                timeval now;
                gettimeofday(&now, 0);

                if (framecount > 0) {
                    int sec = now.tv_sec - StartSec;
                    int usec = now.tv_usec - StartUSec;
                    usec += 1000000 * sec;
                    printf("\rFrame #: %07d; %02.2f fps (Buffer: %.0f%%, Dropped: %d)     ",
                           framecount,
                           (double)30000000 / usec,
                           Manager->Branch("Recorder").GetBufferUsageRatio() * 100.0,
                           Manager->Branch("Recorder").GetDroppedSampleCount());
                    fflush(stdout);
                }

                StartSec = now.tv_sec;
                StartUSec = now.tv_usec;
#endif // __GNUC__
            }
        }

        return SVL_OK;
    }

public:
    svlStreamManager* Manager;
    bool ShowFramerate;
    bool Recording;

private:
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
#if (CISST_SVL_HAS_ZLIB == ON)
        IconDrawerFilter = 0;
        RecorderFilter = 0;
        Manager = 0;
        Recording = false;
#endif // CISST_SVL_HAS_ZLIB
    }

#if (CISST_SVL_HAS_ZLIB == ON)
    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
        // handling user inputs
        if (ascii) {
            switch (eventid) {
                case ' ':
                    if (RecorderFilter && IconDrawerFilter) {
                        if (Recording) {
                            ((svlFilterVideoFileWriter*)RecorderFilter)->Pause();
                            Manager->Branch("Recorder").BlockInput(true);
                            IconDrawerFilter->Pause();
                            Recording = false;
                            cout << endl << " >>> Recording paused <<<" << endl;
                        }
                        else {
                            Manager->Branch("Recorder").BlockInput(false);
                            ((svlFilterVideoFileWriter*)RecorderFilter)->Record(-1);
                            IconDrawerFilter->Record();
                            Recording = true;
                            cout << endl << " >>> Recording started <<<" << endl;
                        }
                    }
                break;

                default:
                    return;
            }
        }
    }
#endif // CISST_SVL_HAS_ZLIB

#if (CISST_SVL_HAS_ZLIB == ON)
    svlStreamManager* Manager;
    CViewerIconDrawerCallback* IconDrawerFilter;
    svlFilterBase* RecorderFilter;
    bool Recording;
#endif // CISST_SVL_HAS_ZLIB
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
    svlFilterSourceVideoCapture viewer_source(false);
    svlFilterImageResizer viewer_resizer;
    svlFilterImageWindow viewer_window;
    CViewerWindowCallback viewer_window_cb;
#if (CISST_SVL_HAS_ZLIB == ON)
    svlFilterImageSampler viewer_icondrawer;
    CViewerIconDrawerCallback viewer_icondrawer_cb;
    svlFilterVideoFileWriter viewer_writer;
#endif // CISST_SVL_HAS_ZLIB
    CFPSFilter viewer_fps;

    // setup source
    // Delete "device.dat" to reinitialize input device
    if (viewer_source.LoadSettings("device.dat") != SVL_OK) {
        cout << endl;
        viewer_source.DialogSetup();
    }

#if (CISST_SVL_HAS_ZLIB == ON)
    // setup icon drawer
    viewer_icondrawer.SetCallback(&viewer_icondrawer_cb);
    if (width > 0 && height > 0) {
        viewer_icondrawer_cb.SetAspectRatio((4.0 * height) / (3.0 * width));
    }
    else {
        viewer_icondrawer_cb.SetAspectRatio(1.0);
    }

    // setup writer
    if (save == true) {
        viewer_writer.DialogFilePath();
        viewer_writer.SetCompressionLevel(9); // 0-9
        viewer_writer.Pause();
    }
#endif // CISST_SVL_HAS_ZLIB

    // setup resizer
    if (width > 0 && height > 0) {
        viewer_resizer.EnableInterpolation(interpolation);
        viewer_resizer.SetOutputSize(width, height);
    }

    // setup image window
#if (CISST_SVL_HAS_ZLIB == ON)
    if (save == true) {
        viewer_window_cb.IconDrawerFilter = &viewer_icondrawer_cb;
        viewer_window_cb.RecorderFilter = &viewer_writer;
        viewer_window_cb.Manager = &viewer_stream;
        viewer_fps.Manager = &viewer_stream;
    }
#endif // CISST_SVL_HAS_ZLIB
    viewer_window.SetCallback(&viewer_window_cb);
    viewer_window.SetTitleText("Camera Viewer");
    viewer_window.EnableTimestampInTitle();

    // chain filters to pipeline
    if (viewer_stream.Trunk().Append(&viewer_source) != SVL_OK) goto labError;
    if (width > 0 && height > 0) {
        if (viewer_stream.Trunk().Append(&viewer_resizer) != SVL_OK) goto labError;
    }
#if (CISST_SVL_HAS_ZLIB == ON)
    if (save == true) {
        if (viewer_stream.Trunk().Append(&viewer_icondrawer) != SVL_OK) goto labError;
    }
#endif // CISST_SVL_HAS_ZLIB
    if (viewer_stream.Trunk().Append(&viewer_window) != SVL_OK) goto labError;
    if (viewer_stream.Trunk().Append(&viewer_fps) != SVL_OK) goto labError;

    // but the recorder on a branch in order to enable buffering
    viewer_stream.CreateBranchAfterFilter(&viewer_source, "Recorder", 200); // Buffer size in frames
#if (CISST_SVL_HAS_ZLIB == ON)
    if (save == true) {
        if (viewer_stream.Branch("Recorder").Append(&viewer_writer) != SVL_OK) goto labError;
    }
#endif // CISST_SVL_HAS_ZLIB

    cerr << endl << "Starting stream... ";

    // initialize and start stream
    if (viewer_stream.Start() != SVL_OK) goto labError;

    cerr << "Done" << endl;

    // wait for keyboard input in command window
    int ch;

    do {
        cerr << endl << "Keyboard commands:" << endl << endl;
#if (CISST_SVL_HAS_ZLIB == ON)
        cerr << "  In image window:" << endl;
        if (save == true) {
            cerr << "    SPACE - Video recorder control: Record/Pause" << endl;
        }
#endif // CISST_SVL_HAS_ZLIB
        cerr << "  In command window:" << endl;
        cerr << "    'i'   - Adjust image properties" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

        ch = cmnGetChar();

        switch (ch) {
            case 'i':
                // Adjust image properties
                viewer_fps.ShowFramerate = false;
                cerr << endl << endl;
                viewer_source.DialogImageProperties();
                cerr << endl;
                viewer_fps.ShowFramerate = true;
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
    viewer_source.SaveSettings("device.dat");

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
    cerr << endl << "stereoTutorialCameraViewer - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl;
    cerr << "Enter 'stereoTutorialCameraViewer -?' for help." << endl;

    //////////////////////////////
    // parsing arguments
    int i, options, ivalue, width, height;
    bool interpolation, save;

    options = argc - 1;
    interpolation = false;
    width = -1;
    height = -1;
    save = false;

    for (i = 1; i <= options; i ++) {
        if (argv[i][0] != '-') continue;

        switch (argv[i][1]) {
            case '?':
                cerr << "Command line format:" << endl;
                cerr << "     stereoTutorialCameraViewer [options]" << endl;
                cerr << "Options:" << endl;
#if (CISST_SVL_HAS_ZLIB == ON)
                cerr << "     -v        Save video file" << endl;
#endif // CISST_SVL_HAS_ZLIB
                cerr << "     -i        Interpolation ON [default: OFF]" << endl;
                cerr << "     -w#       Displayed image width" << endl;
                cerr << "     -h#       Displayed image height" << endl;
                cerr << "Examples:" << endl;
                cerr << "     stereoTutorialCameraViewer" << endl;
#if (CISST_SVL_HAS_ZLIB == ON)
                cerr << "     stereoTutorialCameraViewer -v -i -w1024 -h768" << endl;
#else // CISST_SVL_HAS_ZLIB
                cerr << "     stereoTutorialCameraViewer -i -w800 -h600" << endl;
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

