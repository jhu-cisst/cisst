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
		Show(true),
        FrameCount(0)
    {
        AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
        AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    }

protected:
    int Initialize(svlSample* inputdata)
    {
        OutputData = inputdata;
        return SVL_OK;
    }

    int ProcessFrame(svlProcInfo* procInfo, svlSample* CMN_UNUSED(inputdata) = 0)
    {
		if (!Show) return SVL_OK;

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

public:
	bool Show;

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
    CViewerWindowCallback() :
        svlImageWindowCallbackBase()
		,AdjustmentsEnabled(false)
		,MousePressed(false)
		,OffsetX(0)
        ,OffsetY(0)
		,ImageRectifier(0)
        ,ImageWriterFilter(0)
        ,VideoWriterFilter(0)
        ,Recording(false)
    {
    }

    void OnUserEvent(unsigned int CMN_UNUSED(winid), bool ascii, unsigned int eventid)
    {
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
                    if (MousePressed && AdjustmentsEnabled && ImageRectifier) {
						int x, y;
						GetMousePos(x, y);
						OffsetX = MouseOriginX - x;
						OffsetY = MouseOriginY - y;
						ImageRectifier->SetSimpleTransform(OffsetX / 2 + (OffsetX % 2), OffsetY /2 + (OffsetY % 2), SVL_LEFT);
						ImageRectifier->SetSimpleTransform(OffsetX / -2, OffsetY / -2, SVL_RIGHT);
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
    svlFilterImageRectifier* ImageRectifier;
    svlFilterImageFileWriter* ImageWriterFilter;
    svlFilterVideoFileWriter* VideoWriterFilter;
    bool Recording;
};


////////////////////
//  CameraViewer  //
////////////////////

int CameraViewer(bool interpolation, bool save, int width, int height, int fullscreen, int offsetx)
{
    svlInitialize();

    // instantiating SVL stream and filters
    svlStreamManager viewer_stream(8);
    svlFilterSourceVideoCapture viewer_source(2);
    svlFilterImageRectifier viewer_rectifier;
    svlFilterImageResizer viewer_resizer;
    svlFilterImageWindow viewer_window;
    svlFilterStereoImageJoiner viewer_joiner;
    CViewerWindowCallback viewer_window_cb;
    svlFilterImageFileWriter viewer_imagewriter;
    svlFilterVideoFileWriter viewer_videowriter;
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
        viewer_videowriter.DialogCodec(SVL_LEFT);
        viewer_videowriter.DialogFilePath(SVL_RIGHT);
        viewer_videowriter.DialogCodec(SVL_RIGHT);
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
		if (fullscreen == 0) {
		    viewer_resizer.SetOutputSize(width, height / 2, SVL_LEFT);
			viewer_resizer.SetOutputSize(width, height / 2, SVL_RIGHT);
		}
		else if (fullscreen == 1) {
		    viewer_resizer.SetOutputSize(width / 2, height, SVL_LEFT);
			viewer_resizer.SetOutputSize(width / 2, height, SVL_RIGHT);
		}
		else if (fullscreen == 2) {
		    viewer_resizer.SetOutputSize(width, height / 2, SVL_LEFT);
			viewer_resizer.SetOutputSize(width, height / 2, SVL_RIGHT);
			viewer_joiner.SetLayout(svlFilterStereoImageJoiner::VerticalInterlaced);
		}
		else {
		    viewer_resizer.SetOutputSize(width, height, SVL_LEFT);
			viewer_resizer.SetOutputSize(width, height, SVL_RIGHT);
		}
    }

    // setup image window
    viewer_window_cb.ImageWriterFilter = &viewer_imagewriter;
	viewer_window_cb.ImageRectifier = &viewer_rectifier;
    if (save == true) {
        viewer_window_cb.VideoWriterFilter = &viewer_videowriter;
    }
    viewer_window.SetCallback(&viewer_window_cb);
    viewer_window.SetTitleText("Camera Viewer");
//    viewer_window.EnableTimestampInTitle();
	if (fullscreen >= 0) {
		viewer_window.SetFullScreen();
		if (fullscreen == 0) {
			viewer_window.SetWindowPosition(offsetx, 0, SVL_LEFT);
			viewer_window.SetWindowPosition(offsetx, height / 2, SVL_RIGHT);
		}
		else if (fullscreen == 1) {
			viewer_window.SetWindowPosition(offsetx, 0, SVL_LEFT);
			viewer_window.SetWindowPosition(offsetx + width / 2, 0, SVL_RIGHT);
		}
		else if (fullscreen == 2) {
			viewer_window.SetWindowPosition(offsetx, 0);
		}
		viewer_rectifier.EnableSimpleMode();
	}

    // chain filters to pipeline
    if (viewer_stream.Trunk().Append(&viewer_source) != SVL_OK) goto labError;
    if (save == true) {
        if (viewer_stream.Trunk().Append(&viewer_videowriter) != SVL_OK) goto labError;
    }
    if (viewer_stream.Trunk().Append(&viewer_imagewriter) != SVL_OK) goto labError;
	if (fullscreen >= 0) {
        if (viewer_stream.Trunk().Append(&viewer_rectifier) != SVL_OK) goto labError;
	}
    if (width > 0 && height > 0) {
        if (viewer_stream.Trunk().Append(&viewer_resizer) != SVL_OK) goto labError;
    }
	if (fullscreen == 2) {
        if (viewer_stream.Trunk().Append(&viewer_joiner) != SVL_OK) goto labError;
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
        cerr << "    'a'   - Enable/disable adjustments" << endl;
        cerr << "    's'   - Take image snapshots" << endl;
        if (save == true) {
            cerr << "    SPACE - Video recorder control: Record/Pause" << endl;
        }
        cerr << "  In command window:" << endl;
        cerr << "    '1'   - Adjust LEFT image properties" << endl;
        cerr << "    '2'   - Adjust RIGHT image properties" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

        ch = cmnGetChar();

        switch (ch) {
            case '1':
                viewer_fps.Show = false;
                cerr << endl << endl;
                viewer_source.DialogImageProperties(SVL_LEFT);
                cerr << endl;
                viewer_fps.Show = true;
            break;

            case '2':
                viewer_fps.Show = false;
                cerr << endl << endl;
                viewer_source.DialogImageProperties(SVL_RIGHT);
                cerr << endl;
                viewer_fps.Show = true;
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
                cerr << "     stereoTutorialStereoCameraViewer [options]" << endl;
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
                cerr << "     stereoTutorialStereoCameraViewer" << endl;
                cerr << "     stereoTutorialStereoCameraViewer -v -i -w800 -h600" << endl;
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

