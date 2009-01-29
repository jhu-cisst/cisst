/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <iostream>
#include <cisstStereoVision.h>

using namespace std;


int main()
{
    // Creating SVL objects
    svlStreamManager stream(4); // number of threads per stream
    svlStreamEntity *branch;

    svlDummySource video_source(svlTypeImageRGB); // try svlTypeImageRGBStereo for stereo image source
    svlImageResizer resizer;
    svlImageWindow window;
    svlUnsharpMask unsharpmask;
    svlImageWindow window2;

    // Setup dummy video source
    video_source.SetDimensions(320, 240);
    video_source.SetTargetFrequency(30.0);
    video_source.EnableNoiseImage(true);

    // Setup image resizer
    // (Tip: enable OpenCV in CMake for higher performance)
    resizer.SetOutputRatio(2.0, 2.0, SVL_LEFT);
    resizer.SetOutputRatio(2.0, 2.0, SVL_RIGHT);
    resizer.EnableInterpolation();

    // Setup window
    window.SetTitleText("Uniform noise");

    // Setup unsharp masking
    // (Tip: enable OpenCV in CMake for higher performance)
    unsharpmask.SetAmount(200);
    unsharpmask.SetRadius(3);

    // Setup branch window
    window2.SetTitleText("Unsharp Masking");

    // Chain filters to trunk
    if (stream.Trunk().Append(&video_source) != SVL_OK ||
        stream.Trunk().Append(&resizer)      != SVL_OK ||
        stream.Trunk().Append(&window)       != SVL_OK) goto labError;

    // Adding a branching to the stream right after the source filter
    branch = stream.CreateBranchAfterFilter(&video_source);
    // Chain filters to branch
    if (branch->Append(&unsharpmask) != SVL_OK ||
        branch->Append(&window2)     != SVL_OK) goto labError;

    cout << "Streaming is just about to start." << endl;
    cout << "Press any key and ENTER to stop stream..." << endl;

    // Initialize and start stream
    if (stream.Start() != SVL_OK) goto labError;

    // Wait for user input
    char ch; cin.get(ch); cin.get(ch);
    cout << endl;

    // Safely stopping and deconstructing stream before de-allocation
    stream.EmptyFilterList();

    cout << "Success... Quitting." << endl;
    return 1;

labError:
    cout << "Error occured... Quitting." << endl;
    return 1;
}

