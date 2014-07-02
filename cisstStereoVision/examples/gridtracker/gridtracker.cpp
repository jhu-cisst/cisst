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

#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterImageTracker.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterImageFileWriter.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlTrackerMSBruteForce.h>

//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Error: no filename specified" << std::endl;
        return 0;
    }

    std::string filepath(argv[1]);
    const int radius = 20;
    const int distance = 32;
    unsigned int width, height;
    double framerate;

    svlInitialize();

    svlStreamManager stream(8);
    svlFilterSourceVideoFile source(1);
    svlFilterImageTracker tracker;
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window;
    svlFilterImageWindow window2;
    svlFilterImageFileWriter writer;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window3;

    // setup source
    if (filepath.empty()) {
        source.DialogFilePath();
        source.GetFilePath(filepath);
    }
    else {
        source.SetFilePath(filepath);
    }
    // Get video dimensions
    svlVideoCodecBase* codec = svlVideoIO::GetCodec(filepath);
    if (!codec) return 0;
    if (codec->Open(filepath, width, height, framerate) != SVL_OK) {
        svlVideoIO::ReleaseCodec(codec);
        return 0;
    }
    svlVideoIO::ReleaseCodec(codec);

    // setup tracker algorithm
    svlTrackerMSBruteForce trackeralgo;
    trackeralgo.SetErrorMetric(svlNCC);
    trackeralgo.SetScales(3);
    trackeralgo.SetTemplateRadius(16);
    trackeralgo.SetSearchRadius(30);
    trackeralgo.SetOverwriteTemplates(false);
    trackeralgo.SetTemplateUpdateWeight(0.1);
    trackeralgo.SetConfidenceThreshold(0.0);

    // setup tracker
    tracker.SetRigidBodyTransformSmoothing(10.0);
    tracker.SetIterations(1);
    tracker.SetRigidBody(true);
    tracker.SetRigidBodyConstraints(-1.5, 1.5, 0.5, 2.0);
    tracker.SetTracker(trackeralgo);
    tracker.SetROI(width / 2 - width / 6, height / 2 - height / 6,
                   width / 2 + width / 6, height / 2 + height / 6);
    tracker.SetFrameSkip(3);

    const int targetcount = (radius * 2 + 1) * (radius * 2 + 1);
    svlSampleTargets targets;
    vctInt2 position;
    int i, j, c = 0;

    targets.SetSize(2, targetcount, 1);
    for (j = -radius; j <= radius; j ++) {
        for (i = -radius; i <= radius; i ++) {
            position.X() = width / 2  + distance * i;
            position.Y() = height / 2 + distance * j;
            targets.SetFlag(c, 1);
            targets.SetConfidence(c, 255);
            targets.SetPosition(c, position);
            c ++;
        }
    }
    tracker.GetInput("targets")->PushSample(&targets);

    // Setup overlay
    overlay.AddInputTargets("targets");
    svlOverlayTargets targets_overlay(SVL_LEFT,  // background video channel
                                      true,      // visible
                                      "targets", // filter input name for overlay
                                      SVL_LEFT,  // overlay video channel
                                      true,      // confidence coloring
                                      false,     // draw crosshairs
                                      3);        // target size
    overlay.AddOverlay(targets_overlay);

    // Add framerate overlay
    svlOverlayFramerate bg_fps_overlay(SVL_LEFT,              // background video channel
                                       true,                  // visible
                                       &overlay,              // filter
                                       svlRect(4, 4, 47, 20), // bounding rectangle
                                       14.0,                  // font size
                                       svlRGB(255, 255, 255), // text color
                                       svlRGB(0, 0, 128));    // background color
    overlay.AddOverlay(bg_fps_overlay);

    overlay.AddQueuedItems();

    // chain filters to pipeline
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(tracker.GetInput());
    tracker.GetOutput()->Connect(overlay.GetInput());
    tracker.GetOutput("targets")->Connect(overlay.GetInput("targets"));
    overlay.GetOutput()->Connect(window.GetInput());

    // start stream
    stream.Play();

    int ch = 0;
    while (ch != 'q') {
        ch = cmnGetChar();
        switch (ch) {
            case ' ':
                tracker.GetInput("targets")->PushSample(&targets);
                std::cerr << "Tracker reinitialized" << std::endl;
            break;
        }
    }

    // release pipeline
    stream.Release();
    stream.DisconnectAll();

    std::cerr << "Quit" << std::endl;
    return 1;
}
