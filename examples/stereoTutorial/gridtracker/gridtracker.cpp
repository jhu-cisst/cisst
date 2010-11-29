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


///////////////////////////////
// CBackgroundTracker filter //
///////////////////////////////

class CBackgroundTracker : public svlFilterBase
{
public:
    CBackgroundTracker() :
        svlFilterBase(),
        GridWidth(0),
        GridHeight(0),
        BgPoly(0)
    {
        AddInput("input");
        AddInputType("input", svlTypeTargets);

        AddOutput("output");
        SetAutomaticOutputType(true);
    }

protected:
    int Initialize(svlSample* syncInput, svlSample* &syncOutput)
    {
        syncOutput = syncInput;
        return SVL_OK;
    }

    int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
    {
        syncOutput = syncInput;

        _OnSingleThread(procInfo)
        {
            vctInt2 pos1, pos2, pos3, pos4;

            unsigned int idx_ul = (GridHeight / 2 - 5) * GridWidth + ((GridWidth / 2) - 5);
            unsigned int idx_ur = (GridHeight / 2 - 5) * GridWidth + ((GridWidth / 2) + 5);
            unsigned int idx_lr = (GridHeight / 2 + 5) * GridWidth + ((GridWidth / 2) + 5);
            unsigned int idx_ll = (GridHeight / 2 + 5) * GridWidth + ((GridWidth / 2) - 5);

            svlSampleTargets* targets  = dynamic_cast<svlSampleTargets*>(syncInput);
            if (targets) {
                if (targets->GetPosition(idx_ul, pos1, 0) == SVL_OK &&
                    targets->GetPosition(idx_ur, pos2, 0) == SVL_OK &&
                    targets->GetPosition(idx_lr, pos3, 0) == SVL_OK &&
                    targets->GetPosition(idx_ll, pos4, 0) == SVL_OK) {

                    BgPoly->SetPoint(0, pos1);
                    BgPoly->SetPoint(1, pos2);
                    BgPoly->SetPoint(2, pos3);
                    BgPoly->SetPoint(3, pos4);
                    BgPoly->SetPoint(4, pos1);
                }
            }
        }

        return SVL_OK;
    }

public:
    void SetBgPoly(svlOverlayStaticPoly* poly)
    {
        BgPoly = poly;
    }

    void SetGridSize(unsigned int width, unsigned int height)
    {
        GridWidth = width;
        GridHeight = height;
    }

private:
    unsigned int GridWidth;
    unsigned int GridHeight;
    svlOverlayStaticPoly* BgPoly;
};


//////////////////////////////////
//             main             //
//////////////////////////////////

int main(int CMN_UNUSED(argc), char** CMN_UNUSED(argv))
{
    bool showgrid = true;

    svlInitialize();

    svlStreamManager stream(2);
    svlFilterSourceVideoFile source(1);
    svlFilterImageCenterFinder centerfinder;
    svlFilterImageCropper cropper;
    svlFilterImageTracker tracker;
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window;

    // setup source
    source.DialogFilePath();

    centerfinder.AddReceiver(&cropper);
    centerfinder.SetMask(false);
    centerfinder.SetThreshold(15);
    cropper.SetRectangle(0, 0, 400, 400);

    // setup tracker
    svlTrackerMSBruteForce trackeralgo;
    trackeralgo.SetParameters(svlNCC, // metric
                              8,      // template radius
                              50,     // search radius
                              4,      // number of scales
                              0, 0.0);
    tracker.SetMovingAverageSmoothing(0.0);
    tracker.SetIterations(1);
    tracker.SetRigidBody(true);
    tracker.SetRigidBodyConstraints(-0.4, 0.4, 0.9, 1.1);
    tracker.SetTracker(trackeralgo);
    tracker.SetROI(100, 100, 300, 300);

    const int radius = 40;
    const int distance = 10;

    const int targetcount = (radius * 2 + 1) * (radius * 2 + 1);
    svlSampleTargets targets;
    vctInt2 position;
    int i, j, c = 0;

    targets.SetSize(2, targetcount, 1);
    for (j = -radius; j <= radius; j ++) {
        for (i = -radius; i <= radius; i ++) {
            position.X() = 200 + distance * i;
            position.Y() = 200 + distance * j;
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
    if (showgrid) overlay.AddOverlay(targets_overlay);

    svlOverlayStaticPoly poly_overlay;
    svlOverlayStaticPoly::Type points(5, svlPoint2D(-1, -1));
    poly_overlay.SetPoints(points);
    poly_overlay.SetColor(svlRGB(255, 255, 255));
    overlay.AddOverlay(poly_overlay);
    CBackgroundTracker bgtracker;
    bgtracker.SetBgPoly(&poly_overlay);
    bgtracker.SetGridSize(radius * 2 + 1, radius * 2 + 1);

    // Add framerate overlay
    svlOverlayFramerate bg_fps_overlay(SVL_LEFT,              // background video channel
                                       true,                  // visible
                                       &overlay,              // filter
                                       svlRect(4, 4, 47, 20), // bounding rectangle
                                       14.0,                  // font size
                                       svlRGB(255, 255, 255), // text color
                                       svlRGB(0, 0, 128));    // background color
    overlay.AddOverlay(bg_fps_overlay);

    // Add framerate overlay
    svlOverlayFramerate tracker_fps_overlay(SVL_LEFT,
                                            true,
                                            &tracker,
                                            svlRect(4, 24, 47, 40),
                                            14.0,
                                            svlRGB(255, 255, 255),
                                            svlRGB(0, 128, 0));
    overlay.AddOverlay(tracker_fps_overlay);

    // chain filters to pipeline
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(centerfinder.GetInput());
    centerfinder.GetOutput()->Connect(cropper.GetInput());
    cropper.GetOutput()->Connect(tracker.GetInput());
    tracker.GetOutput()->Connect(overlay.GetInput());
    overlay.GetOutput()->Connect(window.GetInput());

    tracker.GetOutput("targets")->Connect(bgtracker.GetInput());
    bgtracker.GetOutput()->Connect(overlay.GetInput("targets"));

    // start stream
    stream.Play();

    int ch = 0;
    while (ch != 'q') ch = cmnGetChar();

    // release pipeline
    stream.Release();

    return 1;
}

