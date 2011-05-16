/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: simplestream.cpp 2088 2010-11-29 16:22:59Z bvagvol1 $

  Author(s):  Balazs Vagvolgyi
  Created on: 2006

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
#include "svlImageProcessingHelper.h"
#include "ccalOriginDetector.h"
#include "ccalCornerDetector.h"
#include "ccalHandEyeCalibration.h"
#include "ccalFileIO.h"
#include <limits>

using namespace std;
using namespace cv;

#pragma region cameraCalibration

cv::Mat cameraMatrix, distCoeffs;
std::vector<cv::Mat> rvecs;
std::vector<cv::Mat> tvecs;
int flags;
std::vector<cv::Point3f> objectImagePoints;
std::vector<std::vector<cv::Point3f>> objectPoints;
std::vector<std::vector<cv::Point3f>> projectedObjectPoints;
std::vector<std::vector<cv::Point2f>> imagePoints;
std::vector<std::vector<cv::Point2f>> projectedImagePoints;
cv::Size imageSize;
ccalOriginDetector* calOriginDetector; 
ccalCornerDetector* calCornerDetector;
std::vector<svlSampleImageRGB> images;
std::vector<ccalCalibrationGrid*> calibrationGrids;
std::vector<ccalPointsFileIO*> pointFiles;
bool debug;
int minCornerThreshold = 5;
double rootMeanSquaredThreshold = 1;
int maxCalibrationIteration = 10;
int visibility[20];
int refineThreshold = 2;
int pointsCount;

void printCalibrationParameters()
{
	for(int i=0;i<cameraMatrix.rows;i++)
	{
		for(int j=0;j<cameraMatrix.cols;j++)
		{
			std::cout << "Camera matrix: " << cameraMatrix.at<double>(i,j) << std::endl;
		}
	}
	
	for(int i=0;i<distCoeffs.rows;i++)
	{
		for(int j=0;j<distCoeffs.cols;j++)
		{
			std::cout << "Distortion _coefficients: " << distCoeffs.at<double>(i,j) << std::endl;
		}
	}

	//for(int i=0;i<rvecs.size();i++)
	//{
	//	cout << "rvect: " << i << ": " << rvecs.at(i).at<double>(0,0) <<","<< rvecs.at(i).at<double>(0,1) <<","<< rvecs.at(i).at<double>(0,2) <<","<< endl;
	//	cout << "tvect: " << i << ": " << tvecs.at(i).at<double>(0,0) <<","<< tvecs.at(i).at<double>(0,1) <<","<< tvecs.at(i).at<double>(0,2) <<","<< endl;
	//}
}

/**************************************************************************************************
* computeReprojectionErrors()					
*	Compute the average L1 reprojection error
*	
* Input
*	objectPoints			const vector<vector<Point3f> >&				- 3D object points of chessboard plane
*	imagePoints				const vector<vector<Point3f> >&				- 2D image points
*	rvecs					const vector<Mat>&							- camera rotation vectors
*	tvecs					const vector<Mat>& 							- camera translation vectors
*	cameraMatrix			const Mat&									- camera intrinsic matrix
*	distCoeffs				const Mat&									- camera distortion coefficients
*	perViewErrors			vector<float>&								- average error per camera
*	projected				bool										- Indicator whether or not to use projected or standard points
*
* Output:
*	double																- Average L1 reprojection error 					
*
***********************************************************************************************************/
double computeReprojectionErrors(
	        const vector<vector<Point3f> >& objectPoints,
	        const vector<vector<Point2f> >& imagePoints,
	        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
	        const Mat& cameraMatrix, const Mat& distCoeffs,
	        vector<float>& perViewErrors, bool projected )
{
    vector<Point2f> imagePoints2, projectedImgPoints;
    vector<Point3f> projectedObjPoints;
    int i, totalPoints = 0;
    double totalErr = 0, err;
	int validIndex = 0;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      cameraMatrix, distCoeffs, imagePoints2);
		projectedImagePoints.push_back(imagePoints2);
		projectedObjectPoints = objectPoints;
		if(projected)
		{				
			while(!calibrationGrids.at(validIndex)->valid)
				validIndex++;
			
			for(int j=0;j<(int)imagePoints2.size();j++)
			{
				//orange, original projected points
				//cvCircle( images.at(validIndex).IplImageRef(), imagePoints.at(i).at(j), 3, cvScalar(55,0,255,0), 1, 8, 0 );
				//pink, projected final projected points
				cvCircle( images.at(validIndex).IplImageRef(), imagePoints2.at(j), 3, cvScalar(255,0,255,0), 1, 8, 0 );
			}
			validIndex++;
		}
		err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L1 );
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)(err/n);
        totalErr += err;
        totalPoints += n;
    }

    return totalErr/totalPoints;
}

void updateCalibrationGrids()
{
	int validIndex = 0;
	for(int i=0;i<(int)calibrationGrids.size();i++)
	{
		if(visibility[i]){
			//cout << "Updating grid " << i << " with parameters " << validIndex <<endl; 
			//cout << "rvect: " << i << ": " << rvecs.at(validIndex).at<double>(0,0) <<","<< rvecs.at(validIndex).at<double>(0,1) <<","<< rvecs.at(validIndex).at<double>(0,2) <<","<< endl;
			//cout << "tvect: " << i << ": " << tvecs.at(validIndex).at<double>(0,0) <<","<< tvecs.at(validIndex).at<double>(0,1) <<","<< tvecs.at(validIndex).at<double>(0,2) <<","<< endl;
			calibrationGrids.at(i)->cameraMatrix = cameraMatrix;
			calibrationGrids.at(i)->distCoeffs = distCoeffs;
			calibrationGrids.at(i)->rvec = rvecs[validIndex];
			calibrationGrids.at(i)->tvec = tvecs[validIndex];
			validIndex++;
		}
	}
}

/**************************************************************************************************
* runCalibration()					
*	Calibration with OpenCV function
*	
* Input
*	projected		bool				- Indicator whether or not to use the projected points from last
*										calibration, with known improvement or standard calibration
*										with current points
*
* Output:
*	double								- RMS error 					
*
***********************************************************************************************************/
double runCalibration(bool projected)
{
	flags = 0;
	double rms;
	rvecs.clear();
	tvecs.clear();
	
	if(projected)
	{
		printf("============calibrateCamera: running projected============\n");
		rms = calibrateCamera(projectedObjectPoints, projectedImagePoints, imageSize, cameraMatrix,
		distCoeffs, rvecs, tvecs, flags);//|cv::CALIB_FIX_K1|cv::CALIB_FIX_K2|cv::CALIB_FIX_K3);
	}
	else
	{
		printf("============calibrateCamera: running standard============\n");
		rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
		distCoeffs, rvecs, tvecs, flags);//|cv::CALIB_FIX_K1|cv::CALIB_FIX_K2|cv::CALIB_FIX_K3);
	}
    //if(debug)
		printf("RMS error reported by calibrateCamera: %g\n", rms);



    return rms;
}

/**************************************************************************************************
* checkCalibration()					
*	Check for reprojection error from current calibration
*	
* Input
*	projected		bool				- Indicator whether or not to use the projected points from last
*										calibration, with known improvement or standard calibration
*										with current points
*
* Output:
*	bool								- Indicator of success 					
*
***********************************************************************************************************/
bool checkCalibration(bool projected)
{
	bool ok = false;
	vector<float> reprojErrs;
	ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
    double totalAvgErr;
	
	if(projected)
	{
		totalAvgErr = computeReprojectionErrors(projectedObjectPoints, projectedImagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs, projected);
	}else
	{
		totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs, projected);
	}
	std::cout << "Range check " << ok << " Total Avg error: " << totalAvgErr <<std::endl;

	if(ok && debug)
	{
		printCalibrationParameters();
	}

	return ok;
}

/**************************************************************************************************
* calibrate()					
*	Calibrate with current points
*	
* Input
*	projected		bool				- Indicator whether or not to use the projected points from last
*										calibration, with known improvement or standard calibration
*										with current points
*
* Output:
*	void 					
*
***********************************************************************************************************/
double calibrate(bool projected)
{
	// empty points vectors
	imagePoints.clear();
	objectPoints.clear();
	pointsCount = 0;

	if(!projected)	
	{
		//empty projected points
		projectedImagePoints.clear();
		projectedObjectPoints.clear();

		//get Points
		for(int i=0;i<(int)calibrationGrids.size();i++)
		{
			if(calibrationGrids.at(i)->valid){
				visibility[i] = 1;
				std::vector<cv::Point2f> gPoints = calibrationGrids.at(i)->getGoodImagePoints();
				cout << "image " << i << " using " << gPoints.size() <<" points." << endl;
				imagePoints.push_back(calibrationGrids.at(i)->getGoodImagePoints());
				objectPoints.push_back(calibrationGrids.at(i)->getGoodCalibrationGridPoints3D());
				pointsCount += calibrationGrids.at(i)->goodImagePoints.size();
			}else
				visibility[i] = 0;
		}
	}

	double rms = numeric_limits<double>::max( );
	bool check = false;

	if(projected)
	{
		if(projectedObjectPoints.size() <= 0)
			return rms;
	}
	else
	{
		if(objectPoints.size() <= 0)
			return rms;
	}

	if(!projected)
		cout << "Calibrating using " << pointsCount <<" points." << endl;
	rms = runCalibration(projected);
	check = checkCalibration(projected);

	if(!check)
		return numeric_limits<double>::max( );
	else
		return rms;
}

void refineGrids(int localThreshold)
{
	//refine
	int validIndex = 0;
	for(int i=0;i<(int)calibrationGrids.size();i++)
	{
		if(visibility[i])
		{
			//cout << "refining grid " << i << " with parameters " << validIndex <<endl; 
			//cout << "rvect: " << i << ": " << rvecs.at(validIndex).at<double>(0,0) <<","<< rvecs.at(validIndex).at<double>(0,1) <<","<< rvecs.at(validIndex).at<double>(0,2) <<","<< endl;
			//cout << "tvect: " << i << ": " << tvecs.at(validIndex).at<double>(0,0) <<","<< tvecs.at(validIndex).at<double>(0,1) <<","<< tvecs.at(validIndex).at<double>(0,2) <<","<< endl;
			//calibrationGrids.at(i)->refine(rvecs[validIndex],tvecs[validIndex],cameraMatrix,distCoeffs,threshold,true,false);
			calibrationGrids.at(i)->refine(rvecs[validIndex],tvecs[validIndex],cameraMatrix,distCoeffs,localThreshold,false,true);
			validIndex++;
		}
	}
}

/**************************************************************************************************
* optimizeCalibration()					
*	Optimize on calibration by projection and refining each grid
*
* Output:
*	void 					
*
***********************************************************************************************************/
void optimizeCalibration()
{
	double rms; 
	double prevRMS = numeric_limits<double>::max( );
	int iteration = 0;
	cv::Mat pPrevCameraMatrix;
	cv::Mat pPrevDistCoeffs;
	std::vector<cv::Mat> pPrevRvecs;
	std::vector<cv::Mat> pPrevTvecs;
	cv::Mat prevCameraMatrix;
	cv::Mat prevDistCoeffs;
	std::vector<cv::Mat> prevRvecs;
	std::vector<cv::Mat> prevTvecs;
	std::vector<cv::Mat> originalCameraMatrix;
	std::vector<cv::Mat> originalDistCoeffs;
	std::vector<cv::Mat> originalRvecs;
	std::vector<cv::Mat> originalTvecs;
	int originalThresholds[20];
	int prevVisibility[20];
	int pPrevVisibility[20];
	int prevThreshold, pPrevThreshold;
	int validIndex = 0;
	int maxPointsCount = 0;
	
	//if(max(imageSize.height,imageSize.width) > 1000)
	//	refineThreshold = 4;

	//save original
	for(int i=0;i<(int)calibrationGrids.size();i++)
	{
		pPrevVisibility[i] = calibrationGrids.at(i)->valid;
		if(calibrationGrids.at(i)->valid)
		{
			originalCameraMatrix.push_back(calibrationGrids.at(i)->cameraMatrix);
			originalDistCoeffs.push_back(calibrationGrids.at(i)->distCoeffs);
			originalRvecs.push_back(calibrationGrids.at(i)->rvec);
			originalTvecs.push_back(calibrationGrids.at(i)->tvec);
			originalThresholds[i] = calibrationGrids.at(i)->refineThreshold;
		}
	}

	//refine
	rms = calibrate(false);
	prevCameraMatrix = cameraMatrix;
	prevDistCoeffs = distCoeffs;
	prevRvecs = rvecs;
	prevTvecs = tvecs;
	prevThreshold = refineThreshold;
	pPrevCameraMatrix = prevCameraMatrix;
	pPrevDistCoeffs = prevDistCoeffs;
	pPrevRvecs = prevRvecs;
	pPrevTvecs = prevTvecs;
	pPrevThreshold = prevThreshold;	
	for(int i=0;i<(int)calibrationGrids.size();i++)
	{
		if(calibrationGrids.at(i)->valid)
		{
			visibility[i] = 1;
		}
		else
			visibility[i] = 0;
		prevVisibility[i] = visibility[i];
	}
	maxPointsCount = max(pointsCount,maxPointsCount);
	int pointIncreaseIteration = 0;

	// check for bad calibration
	if(rms == numeric_limits<double>::max( ))
		return;
	
	while((rms < numeric_limits<double>::max( )) && (rms > rootMeanSquaredThreshold)&& (iteration < maxCalibrationIteration))
	{
		// Lower threshold for higher iteration of optimization
		if(iteration > 1)
			refineThreshold = 2;

		if(rms > prevRMS && pointsCount > (maxPointsCount + minCornerThreshold*calibrationGrids.size()))
		{
			refineThreshold = 1;
			pointIncreaseIteration++;
		}

		if(rms < prevRMS || (pointsCount > (maxPointsCount + pointIncreaseIteration*minCornerThreshold*calibrationGrids.size())))
		{
			cout << "Iteration: " << iteration << " rms delta: " << prevRMS-rms << " count delta: " <<  pointsCount-maxPointsCount << " pointIteration " << pointIncreaseIteration <<endl;
			pPrevCameraMatrix = prevCameraMatrix;
			pPrevDistCoeffs = prevDistCoeffs;
			pPrevRvecs = prevRvecs;
			pPrevTvecs = prevTvecs;
			pPrevThreshold = prevThreshold;
			prevCameraMatrix = cameraMatrix;
			prevDistCoeffs = distCoeffs;
			prevRvecs = rvecs;
			prevTvecs = tvecs;
			prevRMS = rms;
			prevThreshold = refineThreshold;
			for(int i=0;i<(int)calibrationGrids.size();i++)
			{
				pPrevVisibility[i] = prevVisibility[i];
				if(calibrationGrids.at(i)->valid)
				{
					visibility[i] = 1;
				}
				else
					visibility[i] = 0;
				prevVisibility[i] = visibility[i];
			}
			//updateCalibrationGrids();
			maxPointsCount = max(pointsCount,maxPointsCount);
			refineGrids(refineThreshold);
			rms = calibrate(false);

		}else
		{
			break;
		}
		iteration++;
	}

	//if(debug)
		cout <<endl << "==========Optimize Calibration stopped at " << iteration << " iterations=========" <<endl;
	if(debug)
		printCalibrationParameters();

	if(iteration > 0)
	{
		cameraMatrix = pPrevCameraMatrix;
		distCoeffs = pPrevDistCoeffs;
		rvecs = pPrevRvecs;
		tvecs = pPrevTvecs;
		refineThreshold = pPrevThreshold;

		if(debug)
			printCalibrationParameters();

		for(int i=0;i<(int)calibrationGrids.size();i++)
		{
			visibility[i] = pPrevVisibility[i];
		}
		refineGrids(refineThreshold);
		rms = calibrate(false);
	}else{
		//No iteration, set calibration grids back to original
		for(int i=0;i<(int)calibrationGrids.size();i++)
		{
			if(pPrevVisibility[i]){
				calibrationGrids.at(i)->refine(originalRvecs.at(validIndex),originalTvecs.at(validIndex),originalCameraMatrix.at(validIndex),originalDistCoeffs.at(validIndex),originalThresholds[i],false,false);
				validIndex++;
			}
		}

		if(debug)
			printCalibrationParameters();
		rms = calibrate(false);
	}

}

bool calibration(bool groundTruthTest)
{
	double rms;

	///////////////////////optimize
	optimizeCalibration();

	///////////////////////projected
	rms = calibrate(true);
	updateCalibrationGrids();
	//refineGrids(refineThreshold);

	//////////////////////compare to ground truth
	if(groundTruthTest)
	{
		projectedObjectPoints.clear();
		projectedImagePoints.clear();

		for(int i=0;i<(int)calibrationGrids.size();i++)
		{
			if(calibrationGrids.at(i)->valid)
				calibrationGrids.at(i)->compareGroundTruth();
		}
	}

	return rms < numeric_limits<double>::max( );
}

/**************************************************************************************************
* processImages()					
*	Process images individually for calibration
*	
* Input:
*	imageDirectory		string						- Directory where images are
*	imagePrefix			string						- Common prefix for images
*	imageType			string						- Commen appendix for images
*	startIndex			int							- Image index to start
*	stopIndex			int							- Image index to end
*	boardSize			cv::Size					- Size of chessboard
*	colorModeFlag		int							- Color blobs for origin/orientation detection
*
* Output:
*	bool											- Success indicator						
*
***********************************************************************************************************/
bool processImages(string imageDirectory, string imagePrefix, string imageType, int startIndex, int stopIndex, cv::Size boardSize, int colorModeFlag)
{

	cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	distCoeffs  = cv::Mat::zeros(5, 1, CV_64F);
	bool ok = false;
	bool groundTruthTest = true;

	string currentFileName;
	string currentImagePrefix;
	int successCorners = 0;
	int successOrigins = 0;
	std::vector<cv::Point2f> chessboardCorners;
	int cornerDetectionFlag;
	int originDetectionFlag;
    svlSampleImageRGB image;
	cv::Mat matImage;

	ccalCornerDetector* calCornerDetector;
	vector<float> reprojErrs;
	
	calCornerDetector = new ccalCornerDetector(boardSize.width,boardSize.height);
	char buffer[100];
	CvMemStorage* storage = cvCreateMemStorage(0); 
	calOriginDetector = new ccalOriginDetector(colorModeFlag);
	float squareSize = 2.f;
	ccalCalibrationGrid* calibrationGrid;
	ccalDLRCalibrationFileIO* dlrFileIO;

	// DLR calibration
	if(groundTruthTest)
	{
		currentFileName = imageDirectory;
		currentImagePrefix = "camera_calibration_callab_matlab";
		currentFileName.append(currentImagePrefix.append(".m"));
		if(debug)
			cout << "Reading DLR calibration info for " << currentFileName << endl;

		dlrFileIO = new ccalDLRCalibrationFileIO(currentFileName.c_str());
		dlrFileIO->parseFile();
		dlrFileIO->repackData(10);
	}

	for(int i=startIndex;i<stopIndex+1;i++){
		// image file
		itoa(i,buffer,10);
		currentFileName = imageDirectory;
		currentImagePrefix = imagePrefix;
		currentFileName.append(currentImagePrefix.append(buffer).append(imageType));
		if(!(matImage=cv::imread(currentFileName, 1)).data)
			break;
		svlImageIO::Read(image, 0, currentFileName);
		imageSize =  matImage.size();

		// find origin (must preceed corners, additional draws throws off threshold)
		calOriginDetector->detectOrigin(image.IplImageRef());
		originDetectionFlag = calOriginDetector->getOriginDetectionFlag();

		// find corners
		calCornerDetector->detectCorners(matImage,image.IplImageRef());
		cornerDetectionFlag = calCornerDetector->getCornerDetectionFlag();

		// find corner correlation
		calibrationGrid = new ccalCalibrationGrid(image.IplImageRef(), boardSize,squareSize);
		calibrationGrid->correlate(calOriginDetector, calCornerDetector);

		// tracker coords file
		itoa(i,buffer,10);
		currentFileName = imageDirectory;
		currentImagePrefix = imagePrefix;
		currentFileName.append(currentImagePrefix.append(buffer).append(".coords"));
		if(debug)
			cout << "Reading coords for " << currentFileName << endl;

		ccalTrackerCoordsFileIO coordsFileIO(currentFileName.c_str());
		coordsFileIO.parseFile();
		coordsFileIO.repackData();
		calibrationGrid->worldToTCP = coordsFileIO.worldToTCP;

		if(groundTruthTest)
		{
			// points file
			itoa(i,buffer,10);
			currentFileName = imageDirectory;
			currentImagePrefix = "shot";
			currentFileName.append(currentImagePrefix.append(buffer).append(".pts"));
			if(debug)
				cout << "Reading points for " << currentFileName << endl;

			ccalPointsFileIO pointsFileIO(currentFileName.c_str(),ccalFileIO::formatEnum::IMPROVED);
			pointsFileIO.parseFile();
			pointsFileIO.repackData(image.IplImageRef());

			//save for calibration grid
			calibrationGrid->groundTruthImagePoints = pointsFileIO.imagePoints;
			calibrationGrid->groundTruthCalibrationGridPoints = pointsFileIO.calibrationGridPoints;
			if(dlrFileIO->cameraMatrix.size() > i)
				calibrationGrid->groundTruthCameraTransformation = dlrFileIO->cameraMatrix[i];
		}

		//save images and calibration grids
		images.push_back(image);
		calibrationGrids.push_back(calibrationGrid);
	}

	ok = calibration(groundTruthTest);

	return ok;

}

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
		printCalibrationParameters();
	//}

	bool debug = true;
	svlFilterImageRectifier *rectifier = (svlFilterImageRectifier *)new svlFilterImageRectifier();
	vct3x3 R = vct3x3::Eye();

	vct2 f(cameraMatrix.at<double>(0,0),cameraMatrix.at<double>(1,1));
	vct2 c(cameraMatrix.at<double>(0,2),cameraMatrix.at<double>(1,2));
	vctFixedSizeVector<double,5> k(distCoeffs.at<double>(0,0),distCoeffs.at<double>(1,0),distCoeffs.at<double>(2,0),distCoeffs.at<double>(3,0),distCoeffs.at<double>(4,0));//-0.36,0.1234,0.0,0,0);//

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
    svlImageProcessingHelper::RectificationInternals* table;

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

#pragma region handeye

void runHandEye()
{
	ccalHandEyeCalibration* handEyeCalibration = new ccalHandEyeCalibration(calibrationGrids);
	handEyeCalibration->calibrate();
}

#pragma endregion handeye


#pragma endregion cameraCalibration

#pragma region stream
void PrintAllRegistered()
{
    int i = 0;
    cmnClassRegister::const_iterator iter;
    for (iter = cmnClassRegister::begin(); iter != cmnClassRegister::end(); iter ++) {
        std::cout << ++ i << ") " << (*iter).first << std::endl;
    }
}

void PrintFilterList()
{
    int i = 0;
    svlFilterBase* filter;
    cmnClassRegister::const_iterator iter;
    cmnGenericObject* go;
    for (iter = cmnClassRegister::begin(); iter != cmnClassRegister::end(); iter ++) {
        go = (*iter).second->Create();
        filter = dynamic_cast<svlFilterBase*>(go);
        if (filter) {
            std::cout << ++ i << ") " << (*iter).first << std::endl;
        }
        (*iter).second->Delete(go);
    }
}

svlFilterImageRectifier* setupCalibration()
{

	double cameraParameters[3][3] = {{815,0,905.76},{0,813.87,554.29},{0,0,1}};//{386.488, 0, 367.874}, {0, 352.521, 231.203}, {0, 0, 1}};
	double distortionParameters[5][1] = { -0.360706, 0.1309,0,0,0};//-0.351604, 0.113210, 0.000000 , 0.0 , 0.000000000000000};
	cameraMatrix = cv::Mat(3, 3, CV_64F, cameraParameters);
	distCoeffs  = cv::Mat(5, 1, CV_64F, distortionParameters);
	return getRectifier(cameraMatrix, distCoeffs);
}

int main()
{
	bool calibrate = true;
	debug = false;
	char* filenameCornerDetector = "D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/Sinus/frame-00.jpg";//"D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/MISC/box_sketch1_checkerboard.jpg";//
	char* filename = "D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/camera_calibration_20110311/ColoredGridOriginal/jpg/image00.jpg";//"D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/MatlabEx1/calib_example/Image8.jpg";
    svlInitialize();
    PrintAllRegistered();
//    PrintFilterList();

    // Creating SVL objects
    svlStreamManager stream(2); // number of threads per stream
    svlSampleImageRGB image;
    svlFilterSourceDummy source;
    svlFilterImageResizer resizer;
    svlFilterImageWindow window;
    svlFilterSplitter splitter;
    svlFilterImageUnsharpMask filtering;
    svlFilterImageOverlay overlay;
    svlFilterImageWindow window2;
	svlFilterImageRectifier* rectifier;

    // Setup dummy video source
    source.SetTargetFrequency(30.0);

    if (calibrate) {
        //source.SetType(svlTypeImageRGB);
        //source.SetDimensions(640, 480);
        //source.EnableNoiseImage(true);
        svlImageIO::Read(image, 0, filenameCornerDetector);
        source.SetImage(image);
    }
    else {
        svlImageIO::Read(image, 0, filename);
        source.SetImage(image);
		imageSize = cv::Size(image.IplImageRef()->width,image.IplImageRef()->height);
		rectifier = setupCalibration();
    }

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

    // Setup overlays
    // Add image overlay
    overlay.AddInputImage("image");
    svlOverlayImage image_overlay(SVL_LEFT,        // background video channel
                                  true,            // visible
                                  "image",         // image input name
                                  SVL_LEFT,        // image input channel
                                  vctInt2(20, 60), // position
                                  255);            // alpha (transparency)
    overlay.AddOverlay(image_overlay);
	svlOverlayStaticEllipse ellipse_overlay1;
	svlOverlayStaticEllipse ellipse_overlay2;
	svlOverlayStaticEllipse ellipse_overlay3;

	bool ok = false;
	int fileFlag = 1; //0-SD 1-HD
	if(calibrate)
	{
		string imageDirectory = "D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/ColoredCalibrationGrids/";
		string imagePrefix = "IMG0";
		string imageType = ".jpg";
		int startIndex = 0;
		int stopIndex = 1;
		cv::Size boardSize = cv::Size(14,13);
		int colorModeFlag = ccalOriginDetector::colorModeEnum::RGB;
		if(fileFlag == 0)
		{
			// SD endoscope colored grids points
			boardSize = cv::Size(18,16);
			imageDirectory = "./Images/SD/";
			imagePrefix = "image0";
			imageType = ".png";
			startIndex = 0;
			stopIndex = 9;
			colorModeFlag = ccalOriginDetector::colorModeEnum::RGY;
		}else if(fileFlag == 1)
		{
			// HD endoscope colored grids
			boardSize = cv::Size(18,16);
			imageDirectory = "D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/Calibration_20110508/HD/run0/png/";//"D:/Users/Wen/JohnsHopkins/Images/CameraCalibration/Calibration_20110426/HD/HP-3600-CS4-thick-2mm-trackerOnCamera/png/";
			imagePrefix = "image0";
			imageType = ".png";
			startIndex = 0;
			stopIndex = 9;
			colorModeFlag = ccalOriginDetector::colorModeEnum::RGY;
		}
		ok = processImages(imageDirectory,imagePrefix,imageType,startIndex,stopIndex,boardSize,colorModeFlag);

		if(ok)
		{
			rectifier = getRectifier(cameraMatrix, distCoeffs);
			if(images.size() > 0)
				source.SetImage(images.back());
		}
	}
    

    // Add static rectangle overlay
    //svlOverlayStaticRect rect_overlay(SVL_LEFT,                  // background video channel
    //                                  true,                      // visible
    //                                  svlRect(200, 15, 274, 64), // rectangle size and position
    //                                  svlRGB(128, 64, 64),       // color
    //                                  true);                     // filled
    //overlay.AddOverlay(rect_overlay);


    //// Add static triangle overlay
    //svlOverlayStaticTriangle tri_overlay(SVL_LEFT,            // background video channel
    //                                     true,                // visible
    //                                     200, 250,            // triangle corners
    //                                     350, 231,
    //                                     254, 303,
    //                                     svlRGB(64, 64, 200), // color
    //                                     true);              // filled
    //overlay.AddOverlay(tri_overlay);

    //// Add static text overlay
    //svlOverlayStaticText text_overlay(SVL_LEFT,                   // background video channel
    //                                  true,                       // visible
    //                                  "1234567890 | gfx",         // static text
    //                                  svlRect(50, 130, 200, 146), // bounding rectangle
    //                                  14.0,                       // font size
    //                                  svlRGB(255, 255, 255),      // text color
    //                                  svlRGB(32, 32, 32));        // background color
    //overlay.AddOverlay(text_overlay);

    //// Add framerate overlay
    //svlOverlayFramerate fps_overlay(SVL_LEFT,              // background video channel
    //                                true,                  // visible
    //                                &overlay,              // filter
    //                                svlRect(4, 4, 47, 20), // bounding rectangle
    //                                14.0,                  // font size
    //                                svlRGB(255, 200, 200), // text color
    //                                svlRGB(32, 32, 32));   // background color
    //overlay.AddOverlay(fps_overlay);

    //// Add bar overlay
    //svlOverlayStaticBar bar_overlay(SVL_LEFT,                    // background video channel
    //                                true,                        // visible
    //                                vct2(-10.0, 25.0),           // range
    //                                2.8,                         // value
    //                                false,                       // vertical
    //                                svlRect(400, 200, 620, 220), // bounding rectangle
    //                                svlRGB(160, 200, 160),       // color
    //                                svlRGB(100, 128, 100),       // background color
    //                                2,                           // border width
    //                                svlRGB(64, 64, 64));         // border color
    //overlay.AddOverlay(bar_overlay);

    // Setup branch window
    window2.SetTitle("Original");

    // Add new output to splitter
    splitter.AddOutput("output2");

    // Chain filters to trunk
	if(!calibrate || !ok)
	{
		printf("Not calibrating since calibrate %d and ok %d\n",calibrate,ok);
		stream.SetSourceFilter(&source);
		source.GetOutput()->Connect(splitter.GetInput());
		splitter.GetOutput()->Connect(filtering.GetInput());
		filtering.GetOutput()->Connect(overlay.GetInput());
		overlay.GetOutput()->Connect(window.GetInput());

		splitter.GetOutput("output2")->Connect(window2.GetInput());
		//splitter.GetOutput("output2")->Connect(resizer.GetInput());
		//resizer.GetOutput()->Connect(window2.GetInput());
		window2.GetOutput()->Connect(overlay.GetInput("image"));
	}else{
		printf("Using rectification\n");
		stream.SetSourceFilter(&source);
		source.GetOutput()->Connect(splitter.GetInput());
		splitter.GetOutput()->Connect(filtering.GetInput());
		filtering.GetOutput()->Connect(rectifier->GetInput());
		rectifier->GetOutput()->Connect(window.GetInput());

		splitter.GetOutput("output2")->Connect(window2.GetInput());
		//splitter.GetOutput("output2")->Connect(resizer.GetInput());
		//resizer.GetOutput()->Connect(window2.GetInput());
		window2.GetOutput()->Connect(overlay.GetInput("image"));
	}

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

#pragma endregion stream
