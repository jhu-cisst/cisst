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
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterImageWindowTargetSelect.h>
#include <cisstStereoVision/svlFilterImageTracker.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlTrackerMSBruteForce.h>

#define __CAMERA_SOURCE


//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int CMN_UNUSED(argc), char** CMN_UNUSED(argv))
{
    svlInitialize();

    svlStreamManager stream(2);
#ifdef __CAMERA_SOURCE
    svlFilterSourceVideoCapture source(1);
#else // __CAMERA_SOURCE
    svlFilterSourceVideoFile source(1);
#endif // __CAMERA_SOURCE
    svlFilterImageWindowTargetSelect selector;
    svlFilterImageTracker tracker;
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window;

    // setup source
#ifdef __CAMERA_SOURCE
    if (source.LoadSettings("pointtracker.dat") != SVL_OK) {
        source.DialogSetup();
        source.SaveSettings("pointtracker.dat");
    }
#else // __CAMERA_SOURCE
    source.DialogFilePath();
#endif // __CAMERA_SOURCE

    // setup selector
    selector.SetMaxTargets(10);
    selector.SetTitle("Select targets (then press SPACE)");

    // setup tracker algorithm
    svlTrackerMSBruteForce trackeralgo;
    trackeralgo.SetErrorMetric(svlNCC);
    trackeralgo.SetScales(3);
    trackeralgo.SetTemplateRadius(30);
    trackeralgo.SetOverwriteTemplates(false);
    trackeralgo.SetTemplateUpdateWeight(0.1);
    trackeralgo.SetSearchRadius(40);

    // setup tracker
    tracker.SetRigidBodyTransformSmoothing(5.0);
    tracker.SetRigidBody(true);
    tracker.SetRigidBodyConstraints(-1.5, 1.5, 0.5, 2.0);
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

    overlay.AddQueuedItems();

    // setup results window
    window.SetPosition(50, 50);
    window.SetTitle("Tracking results");

    // chain filters to pipeline
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(selector.GetInput());
    selector.GetOutput()->Connect(tracker.GetInput());
    tracker.GetOutput()->Connect(overlay.GetInput());
    overlay.GetOutput()->Connect(window.GetInput());

    selector.GetOutput("targets")->Connect(tracker.GetInput("targets"));
    tracker.GetOutput("targets")->Connect(overlay.GetInput("targets"));

    // start stream
    stream.Play();

    int ch = 0;
    while (ch != 'q') ch = cmnGetChar();

    // release pipeline
    stream.Release();
    stream.DisconnectAll();

    return 1;
}
