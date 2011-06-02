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
cv::Size imageSize;
cv::Mat cameraMatrix, distCoeffs;

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
svlFilterImageRectifier* getRectifier(cv::Mat& cameraMatrix, cv::Mat& distCoeffs)
{
	//if(debug)
	//{
		cout << "==========GetRectifier==============" << endl;
	//}

	bool debug = true;
	svlFilterImageRectifier *rectifier = (svlFilterImageRectifier *)new svlFilterImageRectifier();
	vct3x3 R = vct3x3::Eye();

	vct2 f(cameraMatrix.at<double>(0,0),cameraMatrix.at<double>(1,1));
	vct2 c(cameraMatrix.at<double>(0,2),cameraMatrix.at<double>(1,2));
	vctFixedSizeVector<double,7> k(distCoeffs.at<double>(0,0),distCoeffs.at<double>(1,0),distCoeffs.at<double>(2,0),distCoeffs.at<double>(3,0),distCoeffs.at<double>(4,0),0.0,0.0);//-0.36,0.1234,0.0,0,0);//

	double alpha = 0.0;//assumed to be square pixels
	vct3x3 KK_new = vct3x3::Eye();
	unsigned int videoch = 0;

	KK_new.at(0,0) = cameraMatrix.at<double>(0,0);
	KK_new.at(0,1) = 0;
	KK_new.at(0,2) = cameraMatrix.at<double>(0,2);
	KK_new.at(1,0) = 0;
	KK_new.at(1,1) = cameraMatrix.at<double>(1,1);
	KK_new.at(1,2) = cameraMatrix.at<double>(1,2);
	KK_new.at(2,0) = 0;
	KK_new.at(2,1) = 0;
	KK_new.at(2,2) = 1;

	//unsigned int height,unsigned int width,vct3x3 R,vct2 f, vct2 c, vctFixedSizeVector<double,5> k, double alpha, vct3x3 KK_new,unsigned int videoch)
	int result = rectifier->SetTableFromCameraCalibration(imageSize.height,imageSize.width, R,f,c,k,alpha,KK_new,videoch);

	if(result == SVL_OK)
	{
		//vctFixedSizeVector<svlImageProcessing::Internals, SVL_MAX_CHANNELS> Tables = rectifier->GetTables();
		//table = dynamic_cast<svlImageProcessingHelper::RectificationInternals*>(Tables[videoch].Get());
        if (debug) {
			printf("svlFilterImageRectifier SetTableFromCameraCalibration: success! %d\n", result);
		}
		return rectifier;
	}else{
		return NULL;
	}
}

//#pragma region handeye
//
//void runHandEye()
//{
//	svlCCHandEyeCalibration* handEyeCalibration = new svlCCHandEyeCalibration(calibrationGrids);
//	handEyeCalibration->calibrate();
//}
//
//#pragma endregion handeye


svlFilterImageRectifier* setupCalibration()
{

	double cameraParameters[3][3] = {{815,0,905.76},{0,813.87,554.29},{0,0,1}};//{386.488, 0, 367.874}, {0, 352.521, 231.203}, {0, 0, 1}};
	double distortionParameters[5][1] = { -0.360706, 0.1309,0,0,0};//-0.351604, 0.113210, 0.000000 , 0.0 , 0.000000000000000};
	cameraMatrix = cv::Mat(3, 3, CV_64F, cameraParameters);
	distCoeffs  = cv::Mat(5, 1, CV_64F, distortionParameters);
	return getRectifier(cameraMatrix, distCoeffs);
}

int main(int argc, char** argv)
{

    svlInitialize();

    // Creating SVL objects
    svlStreamManager stream(2); // number of threads per stream
    svlSampleImageRGB image;
    svlFilterSourceDummy source;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterSplitter splitter;
    svlFilterImageUnsharpMask filtering;
    svlFilterImageWindow window2;
	svlFilterImageRectifier* rectifier;
	svlCCCameraCalibration* svlCCObject = new svlCCCameraCalibration();

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

	string imageDirectory = "./Images/SD/";
	string imagePrefix = "image0";
	string imageType = ".png";
	int startIndex = 0;
	int stopIndex = 9;
	cv::Size boardSize = cv::Size(18,16);
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
		boardSize = cv::Size(atoi(argv[6]),atoi(argv[7]));
	}
    else 
	{
        cout << endl << "svlExCameraCalibration - cisstStereoVision example by Wen P. Liu" << endl;
        cout << "Command line format:" << endl;
        cout << "     svlExCameraCalibration imageDirectory imagePrefix imageType" << endl;
        cout << "     OPTIONAL [startIndex stopIndex boardSizeWidth boardSizeHeight]" << endl;
        cout << "     (defaults [0 9 .png 18 16])" << endl;
        cout << "Examples:" << endl;
        cout << "     svlExExposureCorrection ./Images/SD/ image0 " << endl;
        cout << "     svlExExposureCorrection ./Images/SD/ image0 .png 0 9 18 16" << endl;
		goto labError;
    }

    cout << "Calling svlExCameraCalibration " << imageDirectory << " "<< imagePrefix << " " << imageType << " ";
	cout << startIndex << " " << stopIndex << " " << boardSize.width << " " << boardSize.height << endl; 

	//SD arguments
	//./Images/SD/ image0 .png 0 9 18 16
	//HD arguments
	//D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/Calibration_20110508/HD/run0/png/ image0

	ok = svlCCObject->processImages(imageDirectory,imagePrefix,imageType,startIndex,stopIndex,boardSize,originDetectorColorModeFlag);

	if(ok && svlCCObject->images.size() > 0)
	{
		imageSize = svlCCObject->imageSize;
		svlCCObject->printCalibrationParameters();
		rectifier = getRectifier(svlCCObject->cameraMatrix, svlCCObject->distCoeffs);
		source.SetImage(svlCCObject->images.back());
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
    cout << "Press any key to stop stream..." << endl;

    // Initialize and start stream
    if (stream.Play() != SVL_OK) goto labError;

	// hand-eye calibration
	//if(ok)
	//	runHandEye();

    // Wait for user input
    cmnGetChar();

    // Safely stopping and deconstructing stream before de-allocation
    stream.Release();

    cout << "Success... Quitting." << endl;
    return 1;

labError:
    cout << "Error occured... Quitting." << endl;

    return 1;
}