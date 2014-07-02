/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 Author(s):  Balazs Vagvolgyi
 Created on: 2010

 (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights
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
#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterImageExposureCorrection.h>
#include <cisstStereoVision/svlFilterImageTranslation.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterStereoImageJoiner.h>

#if CISST_SVL_HAS_OPENCV
  #include <cisstStereoVision/svlFilterImageCenterFinder.h>
  #include <cisstStereoVision/svlFilterImageZoom.h>
#endif // CISST_SVL_HAS_OPENCV

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
    ,Gamma(0)
    ,ImageShifter(0)
    {
    }

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
        double gamma;

        // handling user inputs
        if (ascii) {
            switch (eventid) {
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
    svlFilterImageExposureCorrection* Gamma;
    svlFilterImageTranslation* ImageShifter;
};


/////////////////////////
//  StereoVideoPlayer  //
/////////////////////////

int StereoVideoPlayer(const string& filepath_left, const string& filepath_right,
                      bool interpolation, int width, int height, int fullscreen, int offsetx)
{
    svlInitialize();

    // instantiating SVL stream and filters
    svlStreamManager stream(8);
    svlFilterSourceVideoFile source(2);
    svlFilterImageExposureCorrection gamma;
#if CISST_SVL_HAS_OPENCV
    svlFilterImageCenterFinder centerfinder;
    svlFilterImageZoom zoom;
#endif // CISST_SVL_HAS_OPENCV
    svlFilterImageTranslation shifter;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterImageOverlay overlay;
    svlFilterStereoImageJoiner joiner;
    CViewerEventHandler window_eh;

    // Add timestamp overlay
    svlOverlayTimestamp ts_overlay(SVL_LEFT,               // background video channel
                                   true,                   // visible
                                   &window,                // filter
                                   svlRect(4, 4, 124, 20), // bounding rectangle
                                   14.0,                   // font size
                                   svlRGB(255, 255, 255),  // text color
                                   svlRGB(0, 0, 128));     // background color
    overlay.AddOverlay(ts_overlay);

#if CISST_SVL_HAS_OPENCV
    // setup center finder and zoom
    centerfinder.AddReceiver(&zoom);
    centerfinder.SetThreshold(40);
    zoom.SetZoom(2.0, SVL_LEFT);
    zoom.SetZoom(2.0, SVL_RIGHT);
    zoom.SetInterpolation(true);
#endif // CISST_SVL_HAS_OPENCV

    // setup gamma correction
    gamma.SetGamma(20.0);

    // setup source
    source.SetFilePath(filepath_left, SVL_LEFT);
    source.SetFilePath(filepath_right, SVL_RIGHT);

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
    window_eh.Gamma = &gamma;
    window_eh.ImageShifter = &shifter;
    window.SetEventHandler(&window_eh);
    window.SetTitle("Camera Viewer");
	if (fullscreen >= 0) {
        window.SetFullScreen(true);
		if (fullscreen == 0) {
            window.SetPosition(offsetx, 0);
            window.SetPosition(offsetx, height / 2);
		}
		else if (fullscreen == 1) {
            window.SetPosition(offsetx, 0, SVL_LEFT);
            window.SetPosition(offsetx + width / 2, 0, SVL_RIGHT);
		}
		else if (fullscreen == 2) {
            window.SetPosition(offsetx, 0);
		}
	}

    // chain filters to pipeline
    svlFilterOutput *output;

    // Add source
    stream.SetSourceFilter(&source);
    output = source.GetOutput();

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

#if CISST_SVL_HAS_OPENCV
    // Add center finder
    output->Connect(centerfinder.GetInput());
    output = centerfinder.GetOutput();

    // Add zoom
    output->Connect(zoom.GetInput());
    output = zoom.GetOutput();
#endif // CISST_SVL_HAS_OPENCV

    // Add gamma
    output->Connect(gamma.GetInput());
    output = gamma.GetOutput();

    // Add joiner if stereo mode = interlaced
	if (fullscreen == 2) {
        output->Connect(joiner.GetInput());
        output = joiner.GetOutput();
	}

    // Add overlay
    output->Connect(overlay.GetInput());
    output = overlay.GetOutput();

    // Add window
    output->Connect(window.GetInput());
    output = window.GetOutput();

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
        cerr << "  In command window:" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

        ch = cmnGetChar();
    } while (ch != 'q');

    cerr << endl;

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

void PrintHelp()
{
    cerr << "Command line format:" << endl;
    cerr << "     svlExStereoVideoPlayer left_video_file right_video_file [options]" << endl;
    cerr << "Options:" << endl;
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
    cerr << "     svlExStereoVideoPlayer left.cvi right.cvi" << endl;
    cerr << "     svlExStereoVideoPlayer left.avi right.avi -w800 -h600 -i -f1" << endl;
}

int main(int argc, char** argv)
{
    cerr << endl << "svlExStereoVideoPlayer - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl;
    cerr << "Enter 'svlExStereoVideoPlayer -?' for help." << endl;

    //////////////////////////////
    // parsing arguments
    int i, options, ivalue, width, height, fullscreen, offsetx;
    bool interpolation;

    options = argc - 1;
    if (options < 2) {
        PrintHelp();
        return 1;
    }

    interpolation = false;
    width = -1;
    height = -1;
	fullscreen = -1;
	offsetx = 0;

    string file_left = argv[1];
    string file_right = argv[2];

    for (i = 3; i <= options; i ++) {
        if (argv[i][0] != '-') continue;

        switch (argv[i][1]) {
            case '?':
                PrintHelp();
                return 1;
            break;

            case 'i':
                interpolation = true;
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

    StereoVideoPlayer(file_left, file_right, interpolation, width, height, fullscreen, offsetx);

    cerr << "Quit" << endl;
    return 1;
}
