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

#include <cisstStereoVision.h>
#include <cisstCommon/cmnGetChar.h>


//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int CMN_UNUSED(argc), char** CMN_UNUSED(argv))
{
    svlInitialize();

    svlStreamManager stream(2);
    svlFilterSourceVideoFile source(1);
    svlFilterSplitter splitter;
    svlFilterImageWindowTargetSelect selector;
    svlFilterImageTracker tracker;
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window;

    // setup source
    source.DialogFilePath();

    // setup selector
    selector.SetMaxTargets(20);
    selector.SetTitleText("Select targets (then press SPACE)");

    // setup tracker
    svlTrackerMSBruteForce trackeralgo;
    trackeralgo.SetParameters(svlNCC, // metric
                              25,     // template radius
                              25,     // search radius
                              3,      // number of scales
                              0, 0.0);
    tracker.SetMovingAverageSmoothing(0.0);
    tracker.SetRigidBody(true);
    tracker.SetRigidBodyConstraints(-0.4, 0.4, 0.8, 1.2);
    tracker.SetTracker(trackeralgo);

    // Setup overlay
    overlay.AddInputTargets("targets");
    svlOverlayTargets targets_overlay(SVL_LEFT,  // background video channel
                                      true,      // visible
                                      "targets", // filter input name for overlay
                                      SVL_LEFT,  // overlay video channel
                                      false,     // confidence coloring
                                      false,     // draw crosshairs
                                      3);        // target size
    overlay.AddOverlay(targets_overlay);

    // Add framerate overlay
    svlOverlayFramerate tracker_fps_overlay(SVL_LEFT,
                                            true,
                                            &tracker,
                                            svlRect(4, 4, 47, 20),
                                            14.0,
                                            svlRGB(255, 255, 255),
                                            svlRGB(0, 128, 0));
    overlay.AddOverlay(tracker_fps_overlay);

    // setup splitter
    splitter.AddOutput("tracker");

    // setup results window
    window.SetWindowPosition(50, 50);
    window.SetTitleText("Tracking results");

    // chain filters to pipeline
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(splitter.GetInput());
    splitter.GetOutput()->Connect(overlay.GetInput());
    overlay.GetOutput()->Connect(window.GetInput());

    splitter.GetOutput("tracker")->Connect(selector.GetInput());
    selector.GetOutput()->Connect(tracker.GetInput());
    selector.GetOutput("targets")->Connect(tracker.GetInput("targets"));
    tracker.GetOutput()->Connect(overlay.GetInput("targets"));

    // start stream
    stream.Start();

    int ch = 0;
    while (ch != 'q') ch = cmnGetChar();

    // release pipeline
    stream.Release();

    return 1;
}

