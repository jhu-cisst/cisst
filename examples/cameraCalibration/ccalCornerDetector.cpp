#include "ccalCornerDetector.h"
#include "ccalOriginDetector.h"
#include <limits>

using namespace cv;
using namespace std;

ccalCornerDetector::ccalCornerDetector(int width, int height)
{
	checkerBoardWidth = width;
	checkerBoardHeight = height;

	// Tune these parameters for goodFeaturesToTrack, cornerSubPix
	quality_level = 0.3;
	min_distance = 5;
	eig_block_size = 5;
	use_harris = false;
	maxCorners = 200;
	subPixTermEpsilon = 0.001;
	
	reset();
}

void ccalCornerDetector::reset()
{
	cornerDetectionFlag = NO_CORNERS;
	chessboardCorners.clear();
}

void ccalCornerDetector::findChessboard(Mat matImage, IplImage* iplImage)
{
	// Convert to grayscale
	IplImage* iplGray = cvCreateImage(matImage.size(), IPL_DEPTH_8U, 1);
	cvCvtColor(iplImage, iplGray, CV_BGR2GRAY);
	Mat matGray;
	cvtColor(matImage, matGray, CV_BGR2GRAY);
	cv::Size innerBoardSize = cv::Size(checkerBoardWidth-1,checkerBoardHeight-1);

	// Check for chess board
	int checkCheckerBoard = cvCheckChessboard(iplGray, innerBoardSize);
	if(debug)	
		cout << "Image is checkerBoard: " << checkCheckerBoard << endl;	

	if(checkCheckerBoard == 1)
	{
		// Find checker board corners
		int chessboardCornersCount, chessboardCornersFound;
		chessboardCornersFound = findChessboardCorners(matGray, innerBoardSize, chessboardCorners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

       // Improve the found corners' coordinate accuracy
	   if(chessboardCornersFound) 
		   cornerSubPix( iplGray, chessboardCorners, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

		if(debug)
			cout << "found: " << chessboardCornersFound << " # corners: "<< chessboardCorners.size() << endl;

		if((chessboardCorners.size() > 0)&&(chessboardCornersFound == 1)){
			cornerDetectionFlag = OPENCV_CHESSBOARD;
		}
	}

	// Free memory
	cvReleaseImage(&iplGray);
	matGray.~Mat();

}

void ccalCornerDetector::findGoodFeaturesNormalized(cv::Mat matImage, cv::Mat matGraySmooth)
{

	////////////////////Features from normalized histogram/////////////////
	// Perform histogram equalization
	Mat matNormalizedR = Mat(matImage.rows,matImage.cols,CV_8UC1);
	Mat matNormalizedG = Mat(matImage.rows,matImage.cols,CV_8UC1);
	Mat matNormalizedB = Mat(matImage.rows,matImage.cols,CV_8UC1);
	Mat matNormalized = Mat(matImage.rows,matImage.cols,CV_8UC3);
	vector<Mat> planes;
	cv::split(matImage, planes);
	planes.at(0).convertTo(planes.at(0),CV_8UC1);
	planes.at(1).convertTo(planes.at(1),CV_8UC1);
	planes.at(2).convertTo(planes.at(2),CV_8UC1);
	cv::equalizeHist(planes.at(0),matNormalizedR);
	cv::equalizeHist(planes.at(1),matNormalizedG);
	cv::equalizeHist(planes.at(2),matNormalizedB);
	planes.clear();
	planes.push_back(matNormalizedR);
	planes.push_back(matNormalizedG);
	planes.push_back(matNormalizedB);
	cv::merge(planes,matNormalized);
	matNormalized.convertTo(matNormalized,CV_32FC1);

	// Convert to grayscale
	Mat matGrayNorm = Mat(matImage.rows,matImage.cols,CV_32FC1);
	cvtColor(matNormalized, matGrayNorm, CV_BGR2GRAY);

	// Gaussian blur to eliminate specular noise
	Mat matGrayNormSmooth = Mat(matImage.rows,matImage.cols,CV_32FC1);
	cv::boxFilter(matGrayNorm,matGrayNormSmooth,matImage.depth(),cv::Size(min_distance,min_distance));
	matGrayNormSmooth.convertTo(matGrayNormSmooth,CV_32FC1);

	// Get Features
	cv::goodFeaturesToTrack(matGrayNormSmooth,chessboardCornersNorm,maxCorners,quality_level,min_distance,Mat(),eig_block_size,use_harris);
	matGrayNormSmooth.convertTo(matGrayNormSmooth,CV_8UC1);
	if(chessboardCornersNorm.size() > 0)
	{
		cv::cornerSubPix(matGrayNormSmooth,chessboardCornersNorm,cv::Size(min_distance,min_distance), cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS, 0, subPixTermEpsilon));
		cv::cornerSubPix(matGraySmooth,chessboardCornersNorm,cv::Size(min_distance,min_distance), cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS,0, subPixTermEpsilon));
	}

	// free memory
	matNormalizedR.~Mat();
	matNormalizedG.~Mat();
	matNormalizedB.~Mat();
	matNormalized.~Mat();
	matGrayNorm.~Mat();
	matGrayNormSmooth.~Mat();
}

/**************************************************************************************************
* findGoodFeatures()					
*	Find eigenvalue based features from chessboard images using OpenCV with these steps:
*	1). Conver to grayscale
*	2). Gaussian blur to remove specular noise
*	3). cv::goodFeaturesToTrack
*	4). cv::cornerSubPix
*
* Input:
*	matImage	cv::Mat						- Matrix representation of the OpenCV image
*	
* Output:
*	void							
*
*	Not Used:
*	 call to finding features with histogram normalized thresholds, does find more features, 
*	but from observation does not improve calibration
*
***********************************************************************************************************/
void ccalCornerDetector::findGoodFeatures(cv::Mat matImage)
{
	//check for HD
	if(max(matImage.rows,matImage.cols) > 1000)
	{
		min_distance = 10;
		maxCorners = 400;
	//check for SD
	}else
	{
		min_distance = 5;
		maxCorners = 200;
	}

	////////// Features from Image //////////
	// Convert to grayscale
	Mat matGray = Mat(matImage.rows,matImage.cols,CV_32FC1);
	cvtColor(matImage, matGray, CV_BGR2GRAY);

	// Gaussian blur to eliminate specular noise
	Mat matGraySmooth = Mat(matImage.rows,matImage.cols,CV_32FC1);
	cv::boxFilter(matGray,matGraySmooth,matImage.depth(),cv::Size(min_distance,min_distance));
	matGraySmooth.convertTo(matGraySmooth,CV_32FC1);
	
	// Get features
	cv::goodFeaturesToTrack(matGraySmooth,chessboardCorners,maxCorners,quality_level,min_distance,Mat(),eig_block_size,use_harris);

	// Refine features
	matGraySmooth.convertTo(matGraySmooth,CV_8UC1);
	if(chessboardCorners.size() > 0)
		cv::cornerSubPix(matGraySmooth,chessboardCorners,cv::Size(min_distance,min_distance), cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS, 0, subPixTermEpsilon));

	findGoodFeaturesNormalized(matImage,matGraySmooth);

	if(chessboardCorners.size() > 0)
	{	
		cornerDetectionFlag = FEATURES;
	}else
	{
		cornerDetectionFlag = NO_CORNERS;
	}

	//if(debug)
		printf("Number of good features: %d\n", chessboardCorners.size());	

	// Free Memory
	matGray.~Mat();
	matGraySmooth.~Mat();
}

float ccalCornerDetector::distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2)
{
	return sqrt( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) ) ;
}

bool ccalCornerDetector::nearestCorner(IplImage* iplImage, cv::Point2f targetPoint,cv::Point2f* corner, float distanceThreshold, bool draw)
{
	float currentMinDistance, minDistance;
	cv::Point2f currentTargetCorner = targetPoint;
	minDistance = numeric_limits<float>::max( );

	switch(cornerDetectionFlag)
	{
		case OPENCV_CHESSBOARD:
		case FEATURES:
			for(int i = 0; i < chessboardCorners.size(); i++ )
			{
				currentMinDistance = distanceBetweenTwoPoints(targetPoint.x,targetPoint.y, chessboardCorners.at(i).x, chessboardCorners.at(i).y);
				if(currentMinDistance < minDistance)
				{		
					currentTargetCorner = (cv::Point2f) chessboardCorners.at(i);
					minDistance = currentMinDistance;
				}
			}	
			break;
		default:
			printf("Unrecognized flag or no corners found: %d\n", cornerDetectionFlag);	
	}

	if(minDistance > distanceThreshold)
	{
		corner->x = targetPoint.x;
		corner->y = targetPoint.y;
	}
	else
	{
		corner->x = currentTargetCorner.x;
		corner->y = currentTargetCorner.y;
	}
	if(debug)
	{
		cout << "Given target point (" << targetPoint.x << "," << targetPoint.y << ") found corner at (" <<  corner->x << "," << corner->y << ") distance: " <<  minDistance << endl;	
	}

	if(draw && minDistance <= distanceThreshold && debug)
	{
		cvCircle( iplImage, targetPoint, 5, cvScalar(0,255,0), 1, 8, 0 );
		cvLine(iplImage, targetPoint, cv::Point2f(corner->x, corner->y) ,cvScalar(255,0,255));
		cvCircle( iplImage, cv::Point2f(corner->x, corner->y), 5, cvScalar(0,0,255), 1, 8, 0 );
	}
	return (minDistance <= distanceThreshold);
}

void ccalCornerDetector::drawCorners(IplImage* iplImage)
{
	if(debug)
		printf("Drawing corners with flag: %d\n", cornerDetectionFlag);	

	switch(cornerDetectionFlag)
	{
		case OPENCV_CHESSBOARD:
		case FEATURES:
			for(int i = 0; i < chessboardCorners.size(); i++ )
			{
				cvCircle( iplImage, cvPoint(cvRound(chessboardCorners.at(i).x),cvRound(chessboardCorners.at(i).y)), 3, cvScalar(255,255,0), 1, 8, 0 );
			}	
			//for(int i = 0; i < chessboardCornersNorm.size(); i++ )
			//{
			//	cvCircle( iplImage, cvPoint(cvRound(chessboardCornersNorm.at(i).x),cvRound(chessboardCornersNorm.at(i).y)), 3, cvScalar(255,255,0), 1, 8, 0 );
			//}
			break;
		default:
			printf("Unrecognized flag or no corners found: %d\n", cornerDetectionFlag);	
	}
}

/**************************************************************************************************
* detectCorners()					
*	Detects corners from chessboard images	
*
* Input:
*	matImage	cv::Mat						- Matrix representation of the OpenCV image
*	iplImage	IplImage*					- IplImage pointer to OpenCV image
*	
* Output:
*	void							
*
*	Not Used:
*	 OpenCV version of finding chessboards, since must have static # of corners, i.e.
*	 entire board must be visible
*
*	Older versions had skeleton code, i.e. not tested for the following:
*	 1). findSquares
*	 2). findSURFFeatures
*
***********************************************************************************************************/
void ccalCornerDetector::detectCorners(cv::Mat matImage,IplImage* iplImage)
{
	reset();

	// Find corners using openCV cvCheckChessboard disabled
	//if(cornerDetectionFlag == NO_CORNERS)
	//{
	//	findChessboard(matImage, iplImage);
	//}
	if(cornerDetectionFlag == NO_CORNERS)
	{
		findGoodFeatures(matImage);
	}

	// Draw detected corners
	//if(debug)
		drawCorners(iplImage);

}



