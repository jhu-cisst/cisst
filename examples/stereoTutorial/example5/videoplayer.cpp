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


////////////////////
//  Video Player  //
////////////////////

int VideoPlayer(const std::string pathname)
{
    svlInitialize();

    // instantiating SVL stream and filters
    svlStreamManager viewer_stream(4);
    svlFilterSourceVideoFile viewer_source(1);
    svlFilterImageWindow viewer_window;

    // setup source
    if (pathname.empty()) {
        viewer_source.DialogFilePath();
    }
    else {
        if (viewer_source.SetFilePath(pathname) != SVL_OK) {
            cerr << endl << "Error while opening file... " << endl;
            goto labError;
        }
    }

    // setup image window
    viewer_window.SetTitleText("Video Player");
    viewer_window.EnableTimestampInTitle();

    // chain filters to pipeline
    if (viewer_stream.Trunk().Append(&viewer_source) != SVL_OK) goto labError;
    if (viewer_stream.Trunk().Append(&viewer_window) != SVL_OK) goto labError;

    cerr << endl << "Starting stream... ";

    // initialize and start stream
    if (viewer_stream.Start() != SVL_OK) goto labError;

    cerr << "Done" << endl;

    // wait for keyboard input in command window
    int ch;

    do {
        cerr << endl << "Keyboard commands:" << endl << endl;
        cerr << "  In command window:" << endl;
        cerr << "    'q'   - Quit" << endl << endl;

        ch = cmnGetChar();
        osaSleep(1.0 * cmn_ms);
    } while (ch != 'q');

    cerr << endl;

    // stop stream
    viewer_stream.Stop();

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
    cerr << endl << "stereoTutorialVideoPlayer - cisstStereoVision example by Balazs Vagvolgyi" << endl;
    cerr << "See http://www.cisst.org/cisst for details." << endl << endl;
    cerr << "Command line format:" << endl;
    cerr << "     stereoTutorialVideoPlayer [pathname-optional]" << endl;
    cerr << "Example:" << endl;
    cerr << "     stereoTutorialVideoPlayer video.cvi" << endl;

    if (argc > 1) VideoPlayer(argv[1]);
    else VideoPlayer("");

    cerr << "Quit" << endl << endl;
    return 1;
}

