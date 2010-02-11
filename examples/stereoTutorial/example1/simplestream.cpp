/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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
#include <cisstCommon/cmnGetChar.h>

using namespace std;


void PrintAllRegistered()
{
    int i = 0;
    cmnClassRegister::const_iterator iter;
    for (iter = cmnClassRegister::begin(); iter != cmnClassRegister::end(); iter ++) {
        std::cout << ++ i << ") " << (*iter).first << std::endl;
    }
}

void PrintFilterList()
{
    int i = 0;
    svlFilterBase* filter;
    cmnClassRegister::const_iterator iter;
    cmnGenericObject* go;
    for (iter = cmnClassRegister::begin(); iter != cmnClassRegister::end(); iter ++) {
        go = (*iter).second->Create();
        filter = dynamic_cast<svlFilterBase*>(go);
        if (filter) {
            std::cout << ++ i << ") " << (*iter).first << std::endl;
        }
        (*iter).second->Delete(go);
    }
}

int main()
{
    bool noise = true;

    svlInitialize();
    PrintAllRegistered();
//    PrintFilterList();

    // Creating SVL objects
    svlStreamManager stream(2); // number of threads per stream

    svlFilterSourceDummy video_source;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterUnsharpMask unsharpmask;
    svlFilterImageWindow window2;

    // Setup dummy video source
    video_source.SetTargetFrequency(10.0);

    if (noise) {
        video_source.SetType(svlTypeImageRGB);
        video_source.SetDimensions(320, 240);
        video_source.EnableNoiseImage(true);
    }
    else {
        svlSampleImageRGB image;
        svlImageIO::Read(image, 0, "Winter.png");
        video_source.SetImage(image);
    }

    // Setup image resizer
    // (Tip: enable OpenCV in CMake for higher performance)
    resizer.SetOutputRatio(0.5, 0.5, SVL_LEFT);
    resizer.SetOutputRatio(0.5, 0.5, SVL_RIGHT);
    resizer.EnableInterpolation();

    // Setup window
    window.SetTitleText("Resizing");

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

    // Adding a branch to the stream right after the source filter
    stream.CreateBranchAfterFilter(&video_source, "mybranch", 2);
    // Chain filters to branch
    if (stream.Branch("mybranch").Append(&unsharpmask) != SVL_OK ||
        stream.Branch("mybranch").Append(&window2)     != SVL_OK) goto labError;

    cout << "Streaming is just about to start." << endl;
    cout << "Press any key to stop stream..." << endl;

    // Initialize and start stream
    if (stream.Start() != SVL_OK) goto labError;

    // Wait for user input
    cmnGetChar();

    // Safely stopping and deconstructing stream before de-allocation
    stream.RemoveAll();

    cout << "Success... Quitting." << endl;
    return 1;

labError:
    cout << "Error occured... Quitting." << endl;

    return 1;
}

