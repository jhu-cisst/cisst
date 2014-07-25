/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Wen P. Liu
  Created on: 2011

  (C) Copyright 2006-2014 Johns Hopkins University (JHU), All Rights
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
#include <cisstStereoVision/svlStreamManager.h>

#include <cisstStereoVision/svlFilterSourceDummy.h>
#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterImageUnsharpMask.h>
#include <cisstStereoVision/svlFilterImageWindow.h>
#include <cisstStereoVision/svlFilterImageRectifier.h>
#include <cisstStereoVision/svlFilterImageCameraCalibrationOpenCV.h>

#include <limits>

using namespace std;

bool debug = false;

/**************************************************************************************************
* getRectifier()
*	Set up a svlFilterImageRectifier object with given camera parameters
*
* Input:
*	cameraMatrix	const cv::Mat&						- Matrix representation of camera intrinsics
*	distCoeffs			const cv::Mat&					- Matrix representation of camera distortion coefficients

* Output:
*	svlFilterImageRectifier*							- A stereo vision library filter which undistorts
*
***********************************************************************************************************/

int main(int argc, char** argv)
{

    svlInitialize();

    // Creating SVL objects
    svlStreamManager stream(2); // number of threads per stream
    svlFilterSourceDummy source;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterSplitter splitter;
    svlFilterImageUnsharpMask filtering;
    svlFilterImageWindow window2;
    svlFilterImageRectifier* rectifier = new svlFilterImageRectifier();
    svlFilterImageCameraCalibrationOpenCV* svlCCObject;

    // Setup dummy video source
    source.SetTargetFrequency(30.0);

    // Setup image resizer
    // (Tip: enable OpenCV in CMake for higher performance)
    resizer.SetOutputRatio(0.5, 0.5, SVL_LEFT);
    resizer.SetOutputRatio(0.5, 0.5, SVL_RIGHT);
    resizer.SetInterpolation(true);

    // Setup window
    window.SetTitle("Rectified");

    // Setup unsharp masking
    // (Tip: enable OpenCV in CMake for higher performance)
    filtering.SetAmount(200);
    filtering.SetRadius(3);

    bool ok = false;
    bool runHandEye = true;
    int ch;
    unsigned int index = 1;

    string imageDirectory = "./Images/SD/";
    string imagePrefix = "image";
    string imageType = "png";
    int startIndex = 0;
    int stopIndex = 9;
    int boardWidth = 18;
    int boardHeight = 16;
    float squareSize = 2.0;

    if (argc == 3)
    {
        imageDirectory = argv[1];
        imagePrefix = argv[2];
    }
    else if(argc == 6)
    {
        imageDirectory = argv[1];
        imagePrefix = argv[2];
        imageType = argv[3];
        startIndex = atoi(argv[4]);
        stopIndex = atoi(argv[5]);
    }
    else
    {
        cout << endl << "svlExCameraCalibration - cisstStereoVision example by Wen P. Liu" << endl;
        cout << "Command line format:" << endl;
        cout << "     svlExCameraCalibration imageDirectory imagePrefix " << endl;
        cout << "     ex: images should be in format image00X.png" << endl;
        cout << "     OPTIONAL [imageType startIndex stopIndex]" << endl;
        cout << "     (defaults [png 0 9])" << endl;
        cout << "Examples:" << endl;
        cout << "     svlExCameraCalibration /ImageDirectory/ imagePrefix " << endl;
        cout << "     svlExCameraCalibration ../cisst/trunk/cisst/cisstStereoVision/examples/cameraCalibration/ image png 0 6 " << endl;
        goto labError;
    }

    svlCCObject = new svlFilterImageCameraCalibrationOpenCV();
    svlCCObject->SetBoardSize(boardWidth,boardHeight);
    svlCCObject->SetSquareSize(squareSize);
    cout << "Calling svlExCameraCalibration " << imageDirectory << " "<< imagePrefix << " " << imageType << " ";
    cout << startIndex << " " << stopIndex << " " << boardWidth << " " << boardHeight << endl;

    //SD arguments
    //./Images/SD/ image png 0 9 18 16
    //HD arguments
    //D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/Calibration_20110508/HD/run0/png/ image

    ok = svlCCObject->ProcessImages(imageDirectory,imagePrefix,imageType,startIndex,stopIndex);

    if(ok)
    {
          source.SetImage(svlCCObject->GetImages().front());
    }else
        goto labError;

    // Setup branch window
    window2.SetTitle("Original");

    // Add new output to splitter
    splitter.AddOutput("output2");

    printf("Using rectification\n");
    stream.SetSourceFilter(&source);
    source.GetOutput()->Connect(splitter.GetInput());
    splitter.GetOutput()->Connect(filtering.GetInput());
    filtering.GetOutput()->Connect(rectifier->GetInput());
    svlCCObject->GetOutput()->Connect(rectifier->GetInput());
    rectifier->GetOutput()->Connect(window.GetInput());

    svlCCObject->GetOutput("calibration")->Connect(rectifier->GetInput("calibration"));

    splitter.GetOutput("output2")->Connect(window2.GetInput());

    if(ok)
    {
          svlCCObject->RunCameraCalibration(runHandEye);
          svlCCObject->PrintCalibrationParameters();
          rectifier->GetInput("calibration")->PushSample(svlCCObject->GetCameraGeometry());
          svlCCObject->WriteToFileCalibrationParameters(imageDirectory);
    }else
        goto labError;


    cout << "Streaming is just about to start." << endl;
    cout << "Press any key to toggle images..." << endl;
    cout << "Press 'q' to stop stream..." << endl;
    cout << "Showing Image# " << index << endl;

    // Initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

    // Wait for user input
    do
    {

        ch = cmnGetChar();
        //options 'r' to add more images and recalibrate
        if(ch == 'r')
        {
            ok = svlCCObject->ProcessImage(imageDirectory,imagePrefix,imageType,index+1);
            ok = svlCCObject->RunCameraCalibration(runHandEye);
            svlCCObject->PrintCalibrationParameters();
            if(ok && svlCCObject->GetImages().size() > 0)
            {
                svlCCObject->PrintCalibrationParameters();
                rectifier->GetInput("calibration")->PushSample(svlCCObject->GetCameraGeometry());
                svlCCObject->WriteToFileCalibrationParameters(imageDirectory);
                source.SetImage(svlCCObject->GetImages().front());
            }
        }
        source.SetImage(svlCCObject->GetImages().at(index));
        index++;
        cout << "Showing Image# " << index << endl;
        if(index > svlCCObject->GetImages().size()-1)
            index = 0;
    }while (ch != 'q');

    // Safely stopping and deconstructing stream before de-allocation
    stream.Stop();
    stream.Release();

    cout << "Success... Quitting." << endl;
    return 1;

    labError:
    cout << "Error occured... Quitting." << endl;

    return 1;
}
