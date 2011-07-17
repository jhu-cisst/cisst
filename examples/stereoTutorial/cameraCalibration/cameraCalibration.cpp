/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cameraCalibration.cpp 2426 2011-05-21 00:53:58Z wliu25 $

  Author(s):  Wen P. Liu
  Created on: 2011

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision.h>
#include <cisstCommon/cmnGetChar.h>
#include <cv.h>
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
    svlCCCameraCalibration* svlCCObject = new svlCCCameraCalibration();;

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
    int index = 1;

    string imageDirectory = "./Images/SD/";
    string imagePrefix = "image";
    string imageType = "png";
    int startIndex = 0;
    int stopIndex = 9;
    int boardWidth = 18;
    int boardHeight = 16;
    float squareSize = 2.0;
    int originDetectorColorModeFlag = svlCCOriginDetector::RGY;

    if (argc == 3)
    {
        imageDirectory = argv[1];
        imagePrefix = argv[2];
    }
    else if(argc == 8)
    {
        imageDirectory = argv[1];
        imagePrefix = argv[2];
        imageType = argv[3];
        startIndex = atoi(argv[4]);
        stopIndex = atoi(argv[5]);
        boardWidth = atoi(argv[6]);
        boardHeight = atoi(argv[7]);
    }
    else 
    {
        cout << endl << "svlExCameraCalibration - cisstStereoVision example by Wen P. Liu" << endl;
        cout << "Command line format:" << endl;
        cout << "     svlExCameraCalibration imageDirectory imagePrefix imageType" << endl;
        cout << "     images should be in format image00X.png" << endl;
        cout << "     OPTIONAL [startIndex stopIndex boardSizeWidth boardSizeHeight]" << endl;
        cout << "     (defaults [0 9 png 18 16])" << endl;
        cout << "Examples:" << endl;
        cout << "     svlExCameraCalibration ./Images/SD/ image " << endl;
        cout << "     svlExCameraCalibration ./Images/SD/ image png 0 9 18 16" << endl;
        goto labError;
    }

    cout << "Calling svlExCameraCalibration " << imageDirectory << " "<< imagePrefix << " " << imageType << " ";
    cout << startIndex << " " << stopIndex << " " << boardWidth << " " << boardHeight << endl;

    //SD arguments
    //./Images/SD/ image png 0 9 18 16
    //HD arguments
    //D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/Calibration_20110508/HD/run0/png/ image

    ok = svlCCObject->process(imageDirectory,imagePrefix,imageType,startIndex,stopIndex,boardWidth,boardHeight,squareSize,originDetectorColorModeFlag);

    if(ok && svlCCObject->images.size() > 0)
    {
        svlCCObject->printCalibrationParameters();
        svlCCObject->setRectifier(rectifier);
        source.SetImageOverwrite(svlCCObject->images.front());
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
    rectifier->GetOutput()->Connect(window.GetInput());

    splitter.GetOutput("output2")->Connect(window2.GetInput());

    cout << "Streaming is just about to start." << endl;
    cout << "Press any key to toggle images..." << endl;
    cout << "Press 'q' to stop stream..." << endl;
    cout << "Showing Image# " << index << endl;

    // Initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

    // hand-eye calibration
    if(ok && runHandEye)
    {
        svlCCObject->runHandEyeCalibration();
    }
    
    // Wait for user input
    do
    {
        ch = cmnGetChar();
        source.SetImageOverwrite(svlCCObject->images.at(index));
        index++;
        cout << "Showing Image# " << index << endl;
        if(index > svlCCObject->images.size()-1)
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
