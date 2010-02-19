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

#include <cisstStereoVision/svlFilterToolTracker.h>

using namespace std;

/******************************************/
/*** svlFilterToolTracker class ***********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterToolTracker)

svlFilterToolTracker::svlFilterToolTracker() :
    svlFilterBase(),
    cmnGenericObject(),
    Algorithm(0),
    WarpedImage(0)
{
    AddSupportedType(svlTypeImageRGB, svlTypePointCloud);
    AddSupportedType(svlTypeImageRGBStereo, svlTypePointCloud);

    svlSamplePointCloud* points = new svlSamplePointCloud;

    OutputData = points;

    memset(&(Target[SVL_LEFT]), 0, sizeof(TargetType));
    memset(&(Target[SVL_RIGHT]), 0, sizeof(TargetType));
}

svlFilterToolTracker::~svlFilterToolTracker()
{
    Release();

    if (OutputData) delete OutputData;
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

    Target[videoch] = target;

    return SVL_FAIL;
}

int svlFilterToolTracker::Initialize(svlSample* inputdata)
{
    if (Algorithm == 0) return SVL_FAIL;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);
    svlSamplePointCloud* points = dynamic_cast<svlSamplePointCloud*>(OutputData);

    Release();

    unsigned int videochannels = img->GetVideoChannels();

    WarpedImage = dynamic_cast<svlSampleImageBase*>(img->GetNewInstance());
    WarpedImage->SetSize(512, 512);

    // Creating ouptut data structure
    points->SetSize(3, videochannels);
    points->points.SetAll(0.0);

    // Set input parameters in algorithm
    Algorithm->SetInput(WarpedImage->GetWidth(), WarpedImage->GetHeight(), videochannels);

    for (unsigned int i = 0; i < videochannels; i ++) {

        // Storing temporary results until tracking starts
        points->points.Element(0, i) = Target[i].tooltipos.X();
        points->points.Element(1, i) = Target[i].tooltipos.Y();
        points->points.Element(2, i) = Target[i].orientation;
        points->points.Element(3, i) = Target[i].scale;

        // Initializing targets in algorithm
        Algorithm->SetInitialTarget(Target[i], i);
    }

    // Initializing trackers
    Algorithm->Initialize();

    return SVL_OK;
}

int svlFilterToolTracker::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);
    svlSamplePointCloud* points = dynamic_cast<svlSamplePointCloud*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels) {

        // compute warping matrix
        CvMat* affine_mat = cvCreateMat(2,3,CV_32FC1);
        CvPoint2D32f point2d;
        point2d.x = static_cast<float>(Target[idx].tooltipos.X());
        point2d.y = static_cast<float>(Target[idx].tooltipos.Y());
        cv2DRotationMatrix(point2d, Target[idx].orientation, Target[idx].scale, affine_mat);
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
            Algorithm->GetTarget(Target[idx], idx);
            // TO DO
        }

        // store results
        for (idx = 0; idx < videochannels; idx ++) {
            points->points.Element(0, idx) = Target[idx].tooltipos.X();
            points->points.Element(1, idx) = Target[idx].tooltipos.Y();
            points->points.Element(2, idx) = Target[idx].orientation;
            points->points.Element(3, idx) = Target[idx].scale;
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
    memset(&(Target[SVL_LEFT]), 0, sizeof(svlFilterToolTracker::TargetType));
    memset(&(Target[SVL_RIGHT]), 0, sizeof(svlFilterToolTracker::TargetType));
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
    Target[videoch] = target;
}

void svlToolTrackerAlgorithmBase::GetTarget(svlFilterToolTracker::TargetType & target, unsigned int videoch)
{
    target =  Target[videoch];
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
