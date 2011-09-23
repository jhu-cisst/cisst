/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstOpenNI/svlFilterSourceKinect.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstStereoVision/svlImageProcessing.h>
#include <cisstOpenNI/cisstOpenNI.h>

#define SAMPLE_CONFIG_PATH "/Users/vagvoba/Code/Kinect/avin2-SensorKinect-2d13967/OpenNI/Data/SamplesConfig.xml"


/***********************************/
/*** svlFilterSourceKinect class ***/
/***********************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSourceKinect, svlFilterSourceBase)

svlFilterSourceKinect::svlFilterSourceKinect() :
    svlFilterSourceBase(),
    OutputRGB(0),
    OutputDepth(0),
    Kinect(0)
{
    AddOutput("rgb", true);
    SetOutputType("rgb", svlTypeImageRGB);

    AddOutput("depth", false);
    SetOutputType("depth", svlTypeImageMono16);

    const unsigned int width  = 640;
    const unsigned int height = 480;

    OutputRGB = new svlSampleImageRGB;
    OutputRGB->SetSize(width, height);
    OutputDepth = new svlSampleImageMono16;
    OutputDepth->SetSize(width, height);
}

svlFilterSourceKinect::~svlFilterSourceKinect()
{
    if (OutputRGB) delete OutputRGB;
    if (OutputDepth) delete OutputDepth;
}

unsigned int svlFilterSourceKinect::GetWidth() const
{
    return OutputRGB->GetWidth();
}

unsigned int svlFilterSourceKinect::GetHeight() const
{
    return OutputRGB->GetHeight();
}

int svlFilterSourceKinect::Initialize(svlSample* &syncOutput)
{
    if (OutputRGB == 0 || OutputDepth == 0) return SVL_FAIL;

    Kinect = new cisstOpenNI(1);
    Kinect->Configure(SAMPLE_CONFIG_PATH);

    syncOutput = OutputRGB;

    svlFilterOutput* output = GetOutput("depth");
    if (!output) return SVL_FAIL;
    output->SetupSample(OutputDepth);

    return SVL_OK;
}

int svlFilterSourceKinect::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    syncOutput = OutputRGB;

    _OnSingleThread(procInfo)
    {
        Kinect->Update(WAIT_AND_UPDATE_ALL);

        // Acquire color and depth images
        Kinect->GetRGBImage(OutputRGB->GetMatrixRef());
        Kinect->GetDepthImageRaw(OutputDepth->GetMatrixRef());

        // Post-process captured data
        svlImageProcessing::SwapColorChannels(OutputRGB, 0, OutputRGB, 0);

        // Push depth image to async output
        svlFilterOutput* output = GetOutput("depth");
        if (!output) return SVL_FAIL;
        OutputDepth->SetTimestamp(OutputRGB->GetTimestamp());
        output->PushSample(OutputDepth);
    }

    return SVL_OK;
}

int svlFilterSourceKinect::Release()
{
    if (Kinect) {
        Kinect->CleanupExit();
        delete Kinect;
        Kinect = 0;
    }

    return SVL_OK;
}

