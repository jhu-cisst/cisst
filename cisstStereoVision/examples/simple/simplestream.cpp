/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Balazs Vagvolgyi
  Created on: 2006

  (C) Copyright 2006-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnGetChar.h>
#include <cisstCommon/cmnClassRegister.h>

#include <cisstStereoVision/svlInitializer.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceDummy.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterImageUnsharpMask.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterImageWindow.h>

#include <iostream>
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

    svlFilterSourceDummy source;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterSplitter splitter;
    svlFilterImageUnsharpMask filtering;
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window2;

    // Setup dummy video source
    source.SetTargetFrequency(30.0);

    if (noise) {
        source.SetType(svlTypeImageRGB);
        source.SetDimensions(640, 480);
        source.EnableNoiseImage(true);
    }
    else {
        svlSampleImageRGB image;
        svlImageIO::Read(image, 0, "Winter.png");
        source.SetImage(image);
    }

    // Setup image resizer
    // (Tip: enable OpenCV in CMake for higher performance)
    resizer.SetOutputRatio(0.5, 0.5, SVL_LEFT);
    resizer.SetOutputRatio(0.5, 0.5, SVL_RIGHT);
    resizer.SetInterpolation(true);

    // Setup window
    window.SetTitle("Window 1");

    // Setup unsharp masking
    // (Tip: enable OpenCV in CMake for higher performance)
    filtering.SetAmount(200);
    filtering.SetRadius(3);

    // Setup overlays
    // Add image overlay
    overlay.AddInputImage("image");
    svlOverlayImage image_overlay(SVL_LEFT,        // background video channel
                                  true,            // visible
                                  "image",         // image input name
                                  SVL_LEFT,        // image input channel
                                  vctInt2(20, 60), // position
                                  128);            // alpha (transparency)
    overlay.AddOverlay(image_overlay);

    // Add static rectangle overlay
    svlOverlayStaticRect rect_overlay(SVL_LEFT,                  // background video channel
                                      true,                      // visible
                                      svlRect(200, 15, 274, 64), // rectangle size and position
                                      svlRGB(128, 64, 64),       // color
                                      true);                     // filled
    overlay.AddOverlay(rect_overlay);

    // Add static ellipse overlay
    svlOverlayStaticEllipse ellipse_overlay(SVL_LEFT,             // background video channel
                                            true,                 // visible
                                            svlPoint2D(440, 327), // rectangle size and position
                                            48,                   // radius horizontally
                                            32,                   // radius vertically
                                            60.0,                 // angle
                                            svlRGB(64, 128, 64),  // color
                                            true);                // filled
    overlay.AddOverlay(ellipse_overlay);

    // Add static triangle overlay
    svlOverlayStaticTriangle tri_overlay(SVL_LEFT,            // background video channel
                                         true,                // visible
                                         200, 250,            // triangle corners
                                         350, 231,
                                         254, 303,
                                         svlRGB(64, 64, 200), // color
                                         true);              // filled
    overlay.AddOverlay(tri_overlay);

    // Add static text overlay
    svlOverlayStaticText text_overlay(SVL_LEFT,                   // background video channel
                                      true,                       // visible
                                      "1234567890 | gfx",         // static text
                                      svlRect(50, 130, 200, 146), // bounding rectangle
                                      14.0,                       // font size
                                      svlRGB(255, 255, 255),      // text color
                                      svlRGB(32, 32, 32));        // background color
    overlay.AddOverlay(text_overlay);

    // Add framerate overlay
    svlOverlayFramerate fps_overlay(SVL_LEFT,              // background video channel
                                    true,                  // visible
                                    &overlay,              // filter
                                    svlRect(4, 4, 47, 20), // bounding rectangle
                                    14.0,                  // font size
                                    svlRGB(255, 200, 200), // text color
                                    svlRGB(32, 32, 32));   // background color
    overlay.AddOverlay(fps_overlay);

    // Add bar overlay
    svlOverlayStaticBar bar_overlay(SVL_LEFT,                    // background video channel
                                    true,                        // visible
                                    vct2(-10.0, 25.0),           // range
                                    2.8,                         // value
                                    false,                       // vertical
                                    svlRect(400, 200, 620, 220), // bounding rectangle
                                    svlRGB(160, 200, 160),       // color
                                    svlRGB(100, 128, 100),       // background color
                                    2,                           // border width
                                    svlRGB(64, 64, 64));         // border color
    overlay.AddOverlay(bar_overlay);

    overlay.AddQueuedItems();

    // Setup branch window
    window2.SetTitle("Window 2");

    // Add new output to splitter
    splitter.AddOutput("output2");

    // Chain filters to trunk
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(splitter.GetInput());
    splitter.GetOutput()->Connect(filtering.GetInput());
    filtering.GetOutput()->Connect(overlay.GetInput());
    overlay.GetOutput()->Connect(window.GetInput());

    splitter.GetOutput("output2")->Connect(resizer.GetInput());
    resizer.GetOutput()->Connect(window2.GetInput());
    window2.GetOutput()->Connect(overlay.GetInput("image"));

    cout << "Streaming is just about to start." << endl;
    cout << "Press any key to stop stream..." << endl;

    // Initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

    // Wait for user input
    cmnGetChar();

    // Safely stopping and deconstructing stream before de-allocation
    stream.Release();
    stream.DisconnectAll();

    cout << "Success... Quitting." << endl;
    return 1;

labError:
    cout << "Error occured... Quitting." << endl;

    return 1;
}
