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

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlWindowManagerBase.h>
#include <cisstStereoVision/svlFilterImageTranslation.h>
#include <cisstStereoVision/svlFilterImageFileWriter.h>
#include <cisstStereoVision/svlFilterVideoFileWriter.h>
#include <cisstStereoVision/svlFilterImageExposureCorrection.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterVideoExposureManager.h>
#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterStereoImageJoiner.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>

using namespace std;


////////////////////////////////////////
//     Window event handler class     //
////////////////////////////////////////

class CViewerEventHandler : public svlWindowEventHandlerBase
{
public:
    CViewerEventHandler() :
        svlWindowEventHandlerBase()
		,AdjustmentsEnabled(false)
		,MousePressed(false)
		,OffsetX(0)
        ,OffsetY(0)
		,ImageShifter(0)
        ,ImageWriterFilter(0)
        ,VideoWriterFilter(0)
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
                {
                    if (ImageWriterFilter) {
                        ImageWriterFilter->Record(1);
                        cout << endl << " >>> Snapshots saved <<<" << endl;
                    }
                }
                break;

                case ' ':
                {
                    if (VideoWriterFilter) {
                        if (Recording) {
                            VideoWriterFilter->PauseAtTime();
                            SplitterOutput->SetBlock(true);
                            Recording = false;
                            cout << endl << " >>> Recording paused <<<" << endl;
                        }
                        else {
                            SplitterOutput->SetBlock(false);
                            VideoWriterFilter->RecordAtTime();
                            Recording = true;
                            cout << endl << " >>> Recording started <<<" << endl;
                        }
                    }
                }
                break;

                case 'a':
                {
					AdjustmentsEnabled = AdjustmentsEnabled ? false : true;
					if (AdjustmentsEnabled) {
						cout << endl << " >>> Adjustments  enabled <<<" << endl;
					}
					else {
						cout << endl << " >>> Adjustments disabled <<<" << endl;
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
		else {

			switch (eventid) {
				case winInput_LBUTTONDOWN:
                    if (AdjustmentsEnabled && !MousePressed) {
						MousePressed = true;
						GetMousePos(MouseOriginX, MouseOriginY);
						MouseOriginX += OffsetX;
						MouseOriginY += OffsetY;
					}
				break;

				case winInput_LBUTTONUP:
                    if (MousePressed) {
						MousePressed = false;
					}
				break;

				case winInput_MOUSEMOVE:
                    if (MousePressed && AdjustmentsEnabled && ImageShifter) {
						int x, y;
						GetMousePos(x, y);
						OffsetX = MouseOriginX - x;
						OffsetY = MouseOriginY - y;
                        ImageShifter->SetTranslation(OffsetX / 2 + (OffsetX % 2), OffsetY /2 + (OffsetY % 2), SVL_LEFT);
						ImageShifter->SetTranslation(OffsetX / -2, OffsetY / -2, SVL_RIGHT);
					}
				break;

				default:
					return;
			}
		}
    }

	bool AdjustmentsEnabled;
	bool MousePressed;
	int OffsetX;
	int OffsetY;
	int MouseOriginX;
	int MouseOriginY;
    svlFilterImageTranslation* ImageShifter;
    svlFilterImageFileWriter* ImageWriterFilter;
    svlFilterVideoFileWriter* VideoWriterFilter;
    svlFilterImageExposureCorrection* Gamma;
    svlFilterOutput* SplitterOutput;
    bool Recording;
};


////////////////////
//  CameraViewer  //
////////////////////

int CameraViewer(bool interpolation, bool save, int width, int height, int fullscreen, int offsetx)
{
    svlInitialize();

    // instantiating SVL stream and filters
    svlStreamManager stream(4);
    svlFilterSourceVideoCapture source(2);
    svlFilterVideoExposureManager exposure;
    svlFilterImageExposureCorrection gamma;
    svlFilterSplitter splitter;
    svlFilterImageTranslation shifter;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterImageOverlay overlay;
    svlFilterStereoImageJoiner joiner;
    CViewerEventHandler window_eh;
    svlFilterImageFileWriter imagewriter;
    svlFilterVideoFileWriter videowriter;

    // setup source
    // Delete "device.dat" to reinitialize input device
    if (source.LoadSettings("stereodevice.dat") != SVL_OK) {
        cout << endl;
        source.DialogSetup(SVL_LEFT);
        source.DialogSetup(SVL_RIGHT);
    }

    // setup exposure manager
    exposure.SetVideoCaptureFilter(&source);
    exposure.SetSaturationTolerance(0.1);
    exposure.SetSaturationThreshold(230);
    exposure.SetMaxShutter(1305);
    exposure.SetMaxGain(1000);
    exposure.SetEnable(false);

    // setup gamma correction
    gamma.SetGamma(0.0);

    // setup splitter
    splitter.AddOutput("output2", 8, 100);
    svlFilterOutput* splitteroutput = splitter.GetOutput("output2");

    // setup video writer
    if (save == true) {
        videowriter.DialogOpenFile(SVL_LEFT);
        videowriter.DialogOpenFile(SVL_RIGHT);
        videowriter.Pause();
    }

    // setup image writer
    imagewriter.SetFilePath("left_", "bmp", SVL_LEFT);
    imagewriter.SetFilePath("right_", "bmp", SVL_RIGHT);
    imagewriter.EnableTimestamps();
    imagewriter.Pause();

    // setup resizer
    if (width > 0 && height > 0) {
        resizer.SetInterpolation(interpolation);
		if (fullscreen == 0) {
		    resizer.SetOutputSize(width, height / 2, SVL_LEFT);
			resizer.SetOutputSize(width, height / 2, SVL_RIGHT);
		}
		else if (fullscreen == 1) {
		    resizer.SetOutputSize(width / 2, height, SVL_LEFT);
			resizer.SetOutputSize(width / 2, height, SVL_RIGHT);
		}
		else if (fullscreen == 2) {
            resizer.SetOutputSize(width, height / 2, SVL_LEFT);
            resizer.SetOutputSize(width, height / 2, SVL_RIGHT);
            joiner.SetLayout(svlLayoutInterlaced);
		}
		else {
		    resizer.SetOutputSize(width, height, SVL_LEFT);
			resizer.SetOutputSize(width, height, SVL_RIGHT);
		}
    }

    // setup image window
    window_eh.ImageWriterFilter = &imagewriter;
	window_eh.ImageShifter = &shifter;
    window_eh.Gamma = &gamma;
    if (save == true) {
        window_eh.VideoWriterFilter = &videowriter;
        window_eh.SplitterOutput = splitteroutput;
    }
    window.SetEventHandler(&window_eh);
    window.SetTitle("Camera Viewer");
	if (fullscreen >= 0) {
		window.SetFullScreen(true);
		if (fullscreen == 0) {
			window.SetPosition(offsetx, 0, SVL_LEFT);
			window.SetPosition(offsetx, height / 2, SVL_RIGHT);
		}
		else if (fullscreen == 1) {
			window.SetPosition(offsetx, 0, SVL_LEFT);
			window.SetPosition(offsetx + width / 2, 0, SVL_RIGHT);
		}
		else if (fullscreen == 2) {
			window.SetPosition(offsetx, 0);
		}
	}

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
    svlOverlayFramerate fps_overlay(SVL_LEFT,               // background video channel
                                    true,                   // visible
                                    &window,                // filter
                                    svlRect(4, 24, 47, 40), // bounding rectangle
                                    14.0,                   // font size
                                    svlRGB(255, 255, 255),  // text color
                                    svlRGB(128, 0, 0));     // background color
    overlay.AddOverlay(fps_overlay);

    // chain filters to pipeline
    svlFilterOutput *output;

    // Add source
    stream.SetSourceFilter(&source);
        output = source.GetOutput();

        // Add exposure correction
    output->Connect(exposure.GetInput());
        output = exposure.GetOutput();
#if 0
    // Add gamma correction
    output->Connect(gamma.GetInput());
        output = gamma.GetOutput();
#endif
    // Add splitter
    output->Connect(splitter.GetInput());
        output = splitter.GetOutput();

    // Add shifter if fullscreen
	if (fullscreen >= 0) {
        output->Connect(shifter.GetInput());
            output = shifter.GetOutput();
	}

    // Add resizer if required
    if (width > 0 && height > 0) {
        output->Connect(resizer.GetInput());
            output = resizer.GetOutput();
    }

    // Add joiner if stereo mode = interlaced
	if (fullscreen == 2) {
        output->Connect(joiner.GetInput());
            output = joiner.GetOutput();
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

    if (save == true) {
        // If saving enabled, then add video writer on separate branch
        splitteroutput->SetBlock(true);
        splitteroutput->Connect(videowriter.GetInput());
    }

    cerr << endl << "Starting stream... ";

    // initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

    cerr << "Done" << endl;

    // wait for keyboard input in command window
    int ch;

    do {
        cerr << endl << "Keyboard commands:" << endl << endl;
        cerr << "  In image window:" << endl;
        cerr << "    'a'   - Enable/disable adjustments" << endl;
        cerr << "    's'   - Take image snapshots" << endl;
        cerr << "    '9'   - Reduce gamma" << endl;
        cerr << "    '0'   - Increase gamma" << endl;
        if (save == true) {
            cerr << "    SPACE - Video recorder control: Record/Pause" << endl;
        }
        cerr << "  In command window:" << endl;
        cerr << "    '1'   - Adjust LEFT image properties" << endl;
        cerr << "    '2'   - Adjust RIGHT image properties" << endl;
        cerr << "    'e'   - Auto exposure OFF/ON" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

        ch = cmnGetChar();

        switch (ch) {
            case '1':
                cerr << endl << endl;
                source.DialogImageProperties(SVL_LEFT);
                cerr << endl;
            break;

            case '2':
                cerr << endl << endl;
                source.DialogImageProperties(SVL_RIGHT);
                cerr << endl;
            break;

            case 'e':
            {
                bool autoexposure;
                exposure.GetEnable(autoexposure);
                autoexposure = autoexposure ? 0 : 1;
                exposure.SetEnable(autoexposure);
                cerr << "Auto exposure = " << (autoexposure ? "ON" : "OFF") << endl;
            }
            break;

            default:
            break;
        }
    } while (ch != 'q');

    cerr << endl;

    if (save == true) {
        splitteroutput->SetBlock(false);
    }

    // stop stream
    stream.Stop();

    // save settings
    source.SaveSettings("stereodevice.dat");

    // release stream
    stream.Release();
    stream.DisconnectAll();

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
    cerr << endl << "svlExStereoCameraViewer - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl;
    cerr << "Enter 'svlExStereoCameraViewer -?' for help." << endl;

    //////////////////////////////
    // parsing arguments
    int i, options, ivalue, width, height, fullscreen, offsetx;
    bool interpolation, save;

    options = argc - 1;
    save = false;
    interpolation = false;
    width = -1;
    height = -1;
	fullscreen = -1;
	offsetx = 0;

    for (i = 1; i <= options; i ++) {
        if (argv[i][0] != '-') continue;

        switch (argv[i][1]) {
            case '?':
                cerr << "Command line format:" << endl;
                cerr << "     svlExStereoCameraViewer [options]" << endl;
                cerr << "Options:" << endl;
                cerr << "     -v        Save video files" << endl;
                cerr << "     -i        Interpolation ON [default: OFF]" << endl;
                cerr << "     -w#       Displayed image width" << endl;
                cerr << "     -h#       Displayed image height" << endl;
				cerr << "     -f#       Fullscreen mode" << endl;
				cerr << "                  [width and height needs to be specified, default: OFF]" << endl;
				cerr << "                  0 - vertical" << endl;
				cerr << "                  1 - horizontal" << endl;
				cerr << "                  2 - interlaced" << endl;
                cerr << "     -x#       Horizontal window position [used only with -f]" << endl;
                cerr << "Examples:" << endl;
                cerr << "     svlExStereoCameraViewer" << endl;
                cerr << "     svlExStereoCameraViewer -v -i -w800 -h600" << endl;
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

            case 'f':
				if (width > 0 && height > 0) {
					ivalue = ParseNumber(argv[i] + 2, 1);
					if (ivalue >= 0 && ivalue <= 2) fullscreen = ivalue;
				}
            break;

            case 'x':
				if (width > 0 && height > 0 && fullscreen >= 0) {
					ivalue = ParseNumber(argv[i] + 2, 4);
					if (ivalue > 0) offsetx = ivalue;
				}
            break;

            default:
                // NOP
            break;
        }
    }

    //////////////////////////////
    // starting viewer

    CameraViewer(interpolation, save, width, height, fullscreen, offsetx);

    cerr << "Quit" << endl;
    return 1;
}
