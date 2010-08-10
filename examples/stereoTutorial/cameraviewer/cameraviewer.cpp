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


using namespace std;


class svlOverlayAsyncOutputProperties : public svlOverlayStaticText
{
public:
    svlOverlayAsyncOutputProperties();
    svlOverlayAsyncOutputProperties(unsigned int videoch,
                                    bool visible,
                                    svlFilterOutput* output,
                                    svlRect rect,
                                    double fontsize,
                                    svlRGB txtcolor);
    svlOverlayAsyncOutputProperties(unsigned int videoch,
                                    bool visible,
                                    svlFilterOutput* output,
                                    svlRect rect,
                                    double fontsize,
                                    svlRGB txtcolor,
                                    svlRGB bgcolor);
    virtual ~svlOverlayAsyncOutputProperties();

protected:
    virtual void DrawInternal(svlSampleImage* bgimage, svlSample* input);

private:
    svlFilterOutput* Output;
};


/*********************************************/
/*** svlOverlayAsyncOutputProperties class ***/
/*********************************************/

svlOverlayAsyncOutputProperties::svlOverlayAsyncOutputProperties() :
    svlOverlayStaticText(),
    Output(0)
{
}

svlOverlayAsyncOutputProperties::svlOverlayAsyncOutputProperties(unsigned int videoch,
                                                                 bool visible,
                                                                 svlFilterOutput* output,
                                                                 svlRect rect,
                                                                 double fontsize,
                                                                 svlRGB txtcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor),
    Output(output)
{
}

svlOverlayAsyncOutputProperties::svlOverlayAsyncOutputProperties(unsigned int videoch,
                                                                 bool visible,
                                                                 svlFilterOutput* output,
                                                                 svlRect rect,
                                                                 double fontsize,
                                                                 svlRGB txtcolor,
                                                                 svlRGB bgcolor) :
    svlOverlayStaticText(videoch, visible, "", rect, fontsize, txtcolor, bgcolor),
    Output(output)
{
}

svlOverlayAsyncOutputProperties::~svlOverlayAsyncOutputProperties()
{
}

void svlOverlayAsyncOutputProperties::DrawInternal(svlSampleImage* bgimage, svlSample* CMN_UNUSED(input))
{
    if (Output) {
        double usageratio = Output->GetBufferUsageRatio();
        int dropped = Output->GetDroppedSampleCount();

        std::stringstream strstr;
        strstr << "Buffer: " << std::fixed << std::setprecision(1) << usageratio * 100.0 << "%, Dropped: " << dropped;
        SetText(strstr.str());
    }

    svlOverlayStaticText::DrawInternal(bgimage, 0);
}



///////////////////////////////////
//     Window callback class     //
///////////////////////////////////

class CViewerWindowCallback : public svlImageWindowCallbackBase
{
public:
    CViewerWindowCallback() :
        svlImageWindowCallbackBase()
        ,ImageWriterFilter(0)
        ,RecorderFilter(0)
        ,SplitterOutput(0)
        ,Recording(false)
    {
    }

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
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
                            RecorderFilter->Pause();
                            SplitterOutput->SetBlock(true);
                            Recording = false;
                            cout << endl << " >>> Recording paused <<<" << endl;
                        }
                        else {
                            SplitterOutput->SetBlock(false);
                            RecorderFilter->Record(-1);
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

    svlFilterImageFileWriter* ImageWriterFilter;
    svlFilterVideoFileWriter* RecorderFilter;
    svlFilterOutput* SplitterOutput;
    bool Recording;
};


////////////////////
//  CameraViewer  //
////////////////////

int CameraViewer(bool interpolation, bool save, int width, int height)
{
    svlInitialize();

    // instantiating SVL stream and filters
    svlStreamManager stream(1);
    svlFilterSourceVideoCapture source(1);
    svlFilterImageChannelSwapper rgb_swapper;
    svlFilterSplitter splitter;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterImageOverlay overlay;
    CViewerWindowCallback window_cb;
    svlFilterVideoFileWriter videowriter;
    svlFilterImageFileWriter imagewriter;
    svlFilterImageWindow window2;

    // setup source
    // Delete "device.dat" to reinitialize input device
    if (source.LoadSettings("device.dat") != SVL_OK) {
        cout << endl;
        source.DialogSetup();
    }

    // setup splitter
    splitter.AddOutput("output2", 8);
    svlFilterOutput* output = splitter.GetOutput("output2");

    // setup writer
    if (save == true) {
        videowriter.DialogFilePath();
        videowriter.DialogCodec();
        videowriter.Pause();
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
    if (save == true) {
        window_cb.RecorderFilter = &videowriter;
        window_cb.SplitterOutput = output;
    }
    window_cb.ImageWriterFilter = &imagewriter;
    window.SetEventHandler(&window_cb);
    window.SetTitle("Camera Viewer");

    // Add buffer status overlay
    svlOverlayAsyncOutputProperties buffer_overlay(SVL_LEFT,
                                                   true,
                                                   output,
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


    // chain filters to pipeline
    stream.SetSourceFilter(&source);
#if 0 // RGB input: needs to be converted to BGR
    source.GetOutput()->Connect(rgb_swapper.GetInput());
    rgb_swapper.GetOutput()->Connect(imagewriter.GetInput());
#else // BGR input
    source.GetOutput()->Connect(imagewriter.GetInput());
#endif
    if (width > 0 && height > 0) {
        imagewriter.GetOutput()->Connect(resizer.GetInput());
        resizer.GetOutput()->Connect(splitter.GetInput());
    }
    else {
        imagewriter.GetOutput()->Connect(splitter.GetInput());
    }
    splitter.GetOutput()->Connect(overlay.GetInput());
    overlay.GetOutput()->Connect(window.GetInput());

    if (save == true) {
        // put the recorder on a branch in order to enable buffering
        output->SetBlock(true);
//        output->Connect(window2.GetInput());
        output->Connect(videowriter.GetInput());
    }

    cerr << endl << "Starting stream... ";

    // initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

    cerr << "Done" << endl;

    // wait for keyboard input in command window
    int ch;
    bool paused;

    paused = false;

    cerr << endl << "Keyboard commands:" << endl << endl;
    cerr << "  In image window:" << endl;
    if (save == true) {
        cerr << "    SPACE - Video recorder control: Record/Pause" << endl;
    }
    cerr << "    's'   - Take image snapshot" << endl;
    cerr << "  In command window:" << endl;
    cerr << "    'i'   - Adjust image properties" << endl;
    cerr << "    'p'   - Pause/Resume capture" << endl;
    cerr << "    'q'   - Quit" << endl << endl;

    do {
        ch = cmnGetChar();

        switch (ch) {
            case 'i':
                // Adjust image properties
                cerr << endl << endl;
                source.DialogImageProperties();
                cerr << endl << endl;
            break;

            case 'p':
                if (paused) {
                    // Resume playback
                    source.Play();
                    paused = false;
                    cerr << "Capture resumed..." << endl;
                }
                else {
                    // Pause source
                    source.Pause();
                    paused = true;
                    cerr << "Capture paused..." << endl;
                }
            break;
                
            default:
            break;
        }
    } while (ch != 'q');

    cerr << endl;

    // stop stream
    stream.Stop();

    // save settings
    source.SaveSettings("device.dat");

    // release stream
    stream.Release();

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

