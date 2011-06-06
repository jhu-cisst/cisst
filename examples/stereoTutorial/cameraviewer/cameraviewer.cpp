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


#include <cisstStereoVision.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsComponentViewer.h>
#include <cisstOSAbstraction/osaSleep.h>

//#define USE_QT_DIALOGS

#ifdef USE_QT_DIALOGS
    #include <cisstStereoVision/svlQtWidgetVideoEncoder.h>
#endif


using namespace std;

bool interpolation = false;
bool save = false;
int  width = -1;
int  height = -1;


////////////////////////////////////////
//     Window event handler class     //
////////////////////////////////////////

class CViewerEventHandler : public svlWindowEventHandlerBase
{
public:
    CViewerEventHandler() :
        svlWindowEventHandlerBase()
        ,ImageWriterFilter(0)
        ,RecorderFilter(0)
        ,Gamma(0)
        ,SplitterOutput(0)
        ,Recording(false)
    {
    }

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
        double gamma;

        // handling user inputs
        if (ascii) {
            switch (eventid) {
                case 's':
                    if (ImageWriterFilter) {
                        ImageWriterFilter->Record(1);
                        cout << endl << " >>> Snapshot saved <<<" << endl;
                    }
                break;

                case ' ':
                    if (RecorderFilter) {
                        if (Recording) {
                            RecorderFilter->PauseAtTime();
                            SplitterOutput->SetBlock(true);
                            Recording = false;
                            cout << endl << " >>> Recording paused <<<" << endl;
                        }
                        else {
                            SplitterOutput->SetBlock(false);
                            RecorderFilter->RecordAtTime();
                            Recording = true;
                            cout << endl << " >>> Recording started <<<" << endl;
                        }
                    }
                break;

                case '9':
                    if (Gamma) {
                        Gamma->GetGamma(gamma);
                        cout << endl << " >>> Gamma: " << gamma << endl;
                        Gamma->SetGamma(gamma - 5.0);
                    }
                break;

                case '0':
                    if (Gamma) {
                        Gamma->GetGamma(gamma);
                        cout << endl << " >>> Gamma: " << gamma << endl;
                        Gamma->SetGamma(gamma + 5.0);
                    }
                break;

                default:
                    return;
            }
        }
    }

    svlFilterImageFileWriter* ImageWriterFilter;
    svlFilterVideoFileWriter* RecorderFilter;
    svlFilterImageExposureCorrection* Gamma;
    svlFilterOutput* SplitterOutput;
    bool Recording;
};


////////////////////
//  CameraViewer  //
////////////////////

void CameraViewer()
{
#ifdef USE_QT_DIALOGS
    START_QT_ENVIRONMENT
#endif

    mtsComponentViewer *componentViewer = 0;

    svlInitialize();

    // instantiating SVL stream and filters
    svlStreamManager stream(1);
    svlFilterSourceVideoCapture source(1);
    svlFilterVideoExposureManager exposure;
    svlFilterImageExposureCorrection gamma;
#if (CISST_OS == CISST_WINDOWS)
    svlFilterImageChannelSwapper rgb_swapper;
#endif // (CISST_OS == CISST_WINDOWS)
    svlFilterSplitter splitter;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterImageOverlay overlay;
    CViewerEventHandler window_eh;
    svlFilterVideoFileWriter videowriter;
    svlFilterImageFileWriter imagewriter;

    // setup source
    // Delete "device.dat" to reinitialize input device
    if (source.LoadSettings("device.dat") != SVL_OK) {
        cout << endl;
        source.DialogSetup();
    }

    // setup exposure manager
    exposure.SetVideoCaptureFilter(&source);
    exposure.SetSaturationTolerance(0.1);
    exposure.SetSaturationThreshold(230);
    exposure.SetMaxShutter(1305);
    exposure.SetMaxGain(1000);

    // setup gamma correction
    gamma.SetGamma(0.0);

    // setup splitter
    splitter.AddOutput("output2", 8, 3);
    svlFilterOutput* splitteroutput = splitter.GetOutput("output2");

    // setup writer
    if (save == true) {

#ifdef USE_QT_DIALOGS
        std::string extension, filename = "video.avi";
        svlVideoIO::GetExtension(filename, extension);

        videowriter.SetFilePath(filename);
        videowriter.DialogFramerate();

        svlQtWidgetVideoEncoder widget(filename);
        if (widget.WaitForClose()) {
            svlVideoIO::Compression* codec_params = widget.GetCodecParams();
            videowriter.SetCodecParams(codec_params);
            svlVideoIO::ReleaseCompression(codec_params);
        }

        videowriter.OpenFile();
        videowriter.Pause();
#else
        videowriter.DialogOpenFile();
        videowriter.Pause();
#endif
    }

    // setup image writer
    imagewriter.SetFilePath("image_", "bmp");
    imagewriter.EnableTimestamps();
    imagewriter.Pause();

    // setup resizer
    if (width > 0 && height > 0) {
        resizer.SetInterpolation(interpolation);
        resizer.SetOutputSize(width, height);
    }

    // setup image window
    window_eh.ImageWriterFilter = &imagewriter;
    window_eh.Gamma = &gamma;
    if (save == true) {
        window_eh.RecorderFilter = &videowriter;
        window_eh.SplitterOutput = splitteroutput;
    }
    window.SetEventHandler(&window_eh);
    window.SetTitle("Camera Viewer");

    // Add buffer status overlay
    svlOverlayAsyncOutputProperties buffer_overlay(SVL_LEFT,
                                                   true,
                                                   splitteroutput,
                                                   svlRect(4, 4, 225, 20),
                                                   14.0,
                                                   svlRGB(255, 255, 255),
                                                   svlRGB(0, 128, 0));
    if (save) overlay.AddOverlay(buffer_overlay);

    // Add framerate overlay
    svlOverlayFramerate fps_overlay(SVL_LEFT,
                                    true,
                                    &window,
                                    svlRect(4, 24, 47, 40),
                                    14.0,
                                    svlRGB(255, 255, 255),
                                    svlRGB(128, 0, 0));
    overlay.AddOverlay(fps_overlay);

    cerr << "Assembling stream..." << endl;

    // chain filters to pipeline
    svlFilterOutput *output;

    // Add source
    stream.SetSourceFilter(&source);
        output = source.GetOutput();

#if (CISST_OS == CISST_WINDOWS)
    output->Connect(rgb_swapper.GetInput());
        output = rgb_swapper.GetOutput();
#endif // (CISST_OS == CISST_WINDOWS)

    // Add exposure correction
    output->Connect(exposure.GetInput());
        output = exposure.GetOutput();

    // Add gamma correction
    output->Connect(gamma.GetInput());
        output = gamma.GetOutput();

    // Add resizer if required
    if (width > 0 && height > 0) {
        output->Connect(resizer.GetInput());
            output = resizer.GetOutput();
    }

    // Add splitter
    output->Connect(splitter.GetInput());
        output = splitter.GetOutput();

    if (save == true) {
        // If saving enabled, then add video writer on separate branch
        splitteroutput->SetBlock(true);
        splitteroutput->Connect(videowriter.GetInput());
    }

    // Add image file writer
    output->Connect(imagewriter.GetInput());
        output = imagewriter.GetOutput();

    // Add overlay
    output->Connect(overlay.GetInput());
        output = overlay.GetOutput();

    // Add window
    output->Connect(window.GetInput());
        output = window.GetOutput();

    cerr << "Starting stream... ";

    // initialize and start stream
    if (stream.Play() != SVL_OK) return;

    cerr << "Done" << endl;

    // wait for keyboard input in command window
    int ch;

    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In image window:" << endl;
    cerr << "    's'   - Take image snapshot" << endl;
    cerr << "    '9'   - Reduce gamma" << endl;
    cerr << "    '0'   - Increase gamma" << endl;
    if (save == true) {
        cerr << "    SPACE - Video recorder control: Record/Pause" << endl;
    }
    cerr << "  In command window:" << endl;
    if (save == true) {
        cerr << "    'v'   - Start new video file" << endl;
    }
    cerr << "    'i'   - Adjust image properties" << endl;
    cerr << "    'c'   - Start Component Viewer (requires uDrawGraph)" << endl;
    cerr << "    'q'   - Quit" << endl << endl;

    do {
        ch = cmnGetChar();

        switch (ch) {
            case 'v':
                if (save == true) {
                    videowriter.CloseFile();
                    videowriter.ResetCodec();
                    videowriter.DialogOpenFile();
                }
            break;

            case 'i':
                // Adjust image properties
                cerr << endl << endl;
                source.DialogImageProperties();
                cerr << endl << endl;
            break;

            case 'c':
                 // create and add Component Viewer
                if (!componentViewer) {
                    mtsManagerLocal *LCM = mtsManagerLocal::GetInstance();
                    componentViewer = new mtsComponentViewer("ComponentViewer");
                    LCM->AddComponent(componentViewer);
                    // NOTE: currently need to call CreateAll and StartAll
                    // (rather than componentViewer->Create, componentViewer->Start)
                    // to be sure that LCM and GCM are created. This would not be
                    // the case if CreateAll and StartAll were called earlier.
                    LCM->CreateAll();
                    LCM->StartAll();
                }
            break;

            default:
            break;
        }
    } while (ch != 'q');

    // stop stream
    stream.Stop();

    // save settings
    source.SaveSettings("device.dat");

    // release stream
    stream.Release();

    cerr << "Stream released" << endl;

#ifdef USE_QT_DIALOGS
    STOP_QT_ENVIRONMENT
#endif
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
    int j, options, ivalue;

    options = argc - 1;

    for (j = 1; j <= options; j ++) {
        if (argv[j][0] != '-') continue;

        switch (argv[j][1]) {
            case '?':
                cerr << "Command line format:" << endl;
                cerr << "     stereoTutorialCameraViewer [options]" << endl;
                cerr << "Options:" << endl;
                cerr << "     -v        Save video file" << endl;
                cerr << "     -i        Interpolation ON [default: OFF]" << endl;
                cerr << "     -w#       Displayed image width" << endl;
                cerr << "     -h#       Displayed image height" << endl;
                cerr << "Examples:" << endl;
                cerr << "     stereoTutorialCameraViewer" << endl;
                cerr << "     stereoTutorialCameraViewer -v -i -w1024 -h768" << endl;
                return 1;
            break;

            case 'i':
                interpolation = true;
            break;

            case 'v':
                save = true;
            break;

            case 'w':
                ivalue = ParseNumber(argv[j] + 2, 4);
                if (ivalue > 0) width = ivalue;
            break;

            case 'h':
                ivalue = ParseNumber(argv[j] + 2, 4);
                if (ivalue > 0) height = ivalue;
            break;

            default:
                // NOP
            break;
        }
    }

    //////////////////////////////
    // starting viewer

    CameraViewer();

    cerr << "Quit" << endl;
    return 1;
}

