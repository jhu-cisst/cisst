/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstStereoVision/svlFilterToolTracker.h>
#include <cisstStereoVision/svlFilterOutput.h>

/******************************************/
/*** svlFilterToolTracker class ***********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterToolTracker)

svlFilterToolTracker::svlFilterToolTracker() :
    svlFilterBase(),
    Algorithm(0),
    WarpedImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(false);
    GetOutput()->SetType(svlTypeTargets);

    memset(&(Targets[SVL_LEFT]), 0, sizeof(TargetType));
    memset(&(Targets[SVL_RIGHT]), 0, sizeof(TargetType));
}

svlFilterToolTracker::~svlFilterToolTracker()
{
    Release();
}

int svlFilterToolTracker::SetAlgorithm(svlToolTrackerAlgorithmBase* algorithm)
{
    if (IsInitialized()) return SVL_ALREADY_INITIALIZED;
    if (algorithm == 0) return SVL_FAIL;

    Algorithm = algorithm;

    return SVL_OK;
}

int svlFilterToolTracker::SetInitialTarget(TargetType & target, unsigned int videoch)
{
    if (videoch > 1) return SVL_FAIL;

    Targets[videoch] = target;

    return SVL_FAIL;
}

int svlFilterToolTracker::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    if (Algorithm == 0) return SVL_FAIL;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);

    Release();

    unsigned int videochannels = img->GetVideoChannels();
    vctDynamicMatrixRef<int> position;

    WarpedImage = dynamic_cast<svlSampleImage*>(img->GetNewInstance());
    WarpedImage->SetSize(512, 512);

    // Creating output data structure
    OutputTargets.SetDimensions(4);
    OutputTargets.SetChannels(videochannels);

    // Set input parameters in algorithm
    Algorithm->SetInput(WarpedImage->GetWidth(), WarpedImage->GetHeight(), videochannels);

    position.SetRef(4, videochannels, OutputTargets.GetPositionPointer());

    for (unsigned int i = 0; i < videochannels; i ++) {

        if (OutputTargets.GetFlag(i) > 0) {
            // Storing temporary results until tracking starts
            position.Element(0, i) = static_cast<int>(Targets[i].tooltipos.X());
            position.Element(1, i) = static_cast<int>(Targets[i].tooltipos.Y());
            position.Element(2, i) = static_cast<int>(Targets[i].orientation);
            position.Element(3, i) = static_cast<int>(Targets[i].scale);

            // Initializing targets in algorithm
            Algorithm->SetInitialTarget(Targets[i], i);
        }
    }

    // Initializing trackers
    Algorithm->Initialize();

    syncOutput = &OutputTargets;

    return SVL_OK;
}

int svlFilterToolTracker::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = &OutputTargets;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels) {

        // compute warping matrix
        CvMat* affine_mat = cvCreateMat(2,3,CV_32FC1);
        CvPoint2D32f point2d;
        point2d.x = static_cast<float>(Targets[idx].tooltipos.X());
        point2d.y = static_cast<float>(Targets[idx].tooltipos.Y());
        cv2DRotationMatrix(point2d, Targets[idx].orientation, Targets[idx].scale, affine_mat);
        // Add translation
        cvmSet(affine_mat ,0 ,2, -1.0 * point2d.x);
        cvmSet(affine_mat ,1 ,2, -1.0 * point2d.y);

        // warp input image
        cvWarpAffine(img->IplImageRef(idx), WarpedImage->IplImageRef(idx), affine_mat);

        cvReleaseMat(&affine_mat);

        // debug window
        cvShowImage("WarpedImage", WarpedImage->IplImageRef(idx));
    }

    _SynchronizeThreads(procInfo);

    // Multithreaded processing
    Algorithm->Process(procInfo, WarpedImage);

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo) {
        // convert warped parameters back to input image frame
        for (idx = 0; idx < videochannels; idx ++) {
            Algorithm->GetTarget(Targets[idx], idx);
            // TO DO
        }

        // store results
        vctDynamicMatrixRef<int> position;

        position.SetRef(4, videochannels, OutputTargets.GetPositionPointer());

        for (idx = 0; idx < videochannels; idx ++) {

            if (OutputTargets.GetFlag(idx) > 0) {
                position.Element(0, idx) = static_cast<int>(Targets[idx].tooltipos.X());
                position.Element(1, idx) = static_cast<int>(Targets[idx].tooltipos.Y());
                position.Element(2, idx) = static_cast<int>(Targets[idx].orientation);
                position.Element(3, idx) = static_cast<int>(Targets[idx].scale);
            }
        }
    }

    return SVL_OK;
}

int svlFilterToolTracker::Release()
{
    if (Algorithm) Algorithm->Release();
    if (WarpedImage) {
        delete WarpedImage;
        WarpedImage = 0;
    }
    return SVL_OK;
}


/*******************************************/
/*** svlToolTrackerAlgorithmBase class *****/
/*******************************************/

svlToolTrackerAlgorithmBase::svlToolTrackerAlgorithmBase() :
    Initialized(false),
    Width(0),
    Height(0),
    VideoChannels(0)
{
    memset(&(Targets[SVL_LEFT]), 0, sizeof(svlFilterToolTracker::TargetType));
    memset(&(Targets[SVL_RIGHT]), 0, sizeof(svlFilterToolTracker::TargetType));
}

svlToolTrackerAlgorithmBase::~svlToolTrackerAlgorithmBase()
{
}

void svlToolTrackerAlgorithmBase::SetInput(unsigned int width, unsigned int height, unsigned int videochannels)
{
    Width = width;
    Height = height;
    VideoChannels = videochannels;
}

void svlToolTrackerAlgorithmBase::SetInitialTarget(svlFilterToolTracker::TargetType & target, unsigned int videoch)
{
    Targets[videoch] = target;
}

void svlToolTrackerAlgorithmBase::GetTarget(svlFilterToolTracker::TargetType & target, unsigned int videoch)
{
    target =  Targets[videoch];
}

int svlToolTrackerAlgorithmBase::Initialize()
{
    // derived classes may override this method
    // to allocate resources before tracking
    return SVL_OK;
}

void svlToolTrackerAlgorithmBase::Release()
{
    // derived classes may override this method
    // to release resources allocated in the Initialize method
}
