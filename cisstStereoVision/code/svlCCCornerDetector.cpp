/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisstStereoVision/svlCCCornerDetector.h>
#include <cisstStereoVision/svlCCOriginDetector.h>
#include <limits>

svlCCCornerDetector::svlCCCornerDetector(int CMN_UNUSED(width), int CMN_UNUSED(height))
{
    reset();
}

void svlCCCornerDetector::reset()
{
    cornerDetectionFlag = NO_CORNERS;
    chessboardCorners.clear();
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
void svlCCCornerDetector::findGoodFeatures(cv::Mat matImage)
{

    // Tune these parameters for goodFeaturesToTrack, cornerSubPix
    double quality_level = 0.3;
    int min_distance = 5;
    int eig_block_size = 5;
    bool use_harris = false;
    int maxCorners = 200;
    double subPixTermEpsilon = 0.0001;

    //check for HD
    if(std::max(matImage.rows,matImage.cols) > 1000)
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
    cv::Mat matGray = cv::Mat(matImage.rows,matImage.cols,CV_32FC1);
    cvtColor(matImage, matGray, CV_BGR2GRAY);

    // Gaussian blur to eliminate specular noise
    cv::Mat matGraySmooth = cv::Mat(matImage.rows,matImage.cols,CV_32FC1);
    cv::boxFilter(matGray,matGraySmooth,matImage.depth(),cv::Size(min_distance,min_distance));
    matGraySmooth.convertTo(matGraySmooth,CV_32FC1);

    // Get features
    cv::goodFeaturesToTrack(matGraySmooth,chessboardCorners,maxCorners,quality_level,min_distance,cv::Mat(),eig_block_size,use_harris);

    // Refine features
    matGraySmooth.convertTo(matGraySmooth,CV_8UC1);
    if(chessboardCorners.size() > 0)
        cv::cornerSubPix(matGraySmooth,chessboardCorners,cv::Size(min_distance,min_distance), cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS, 0, subPixTermEpsilon));

    if(chessboardCorners.size() > 0)
    {
        cornerDetectionFlag = FEATURES;
    }else
    {
        cornerDetectionFlag = NO_CORNERS;
    }

    //if(debug)
    std::cout << "Number of good features: " << chessboardCorners.size() << std::endl;

    // Free Memory
    matGray.~Mat();
    matGraySmooth.~Mat();
}

float svlCCCornerDetector::distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2)
{
    return sqrt( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) ) ;
}

bool svlCCCornerDetector::nearestCorner(IplImage* iplImage, cv::Point2f targetPoint,cv::Point2f* corner, float distanceThreshold, bool draw)
{
    float currentMinDistance, minDistance;
    cv::Point2f currentTargetCorner = targetPoint;
    minDistance = std::numeric_limits<float>::max( );

    switch(cornerDetectionFlag)
    {
    case FEATURES:
        for(unsigned int i = 0; i < chessboardCorners.size(); i++ )
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
        std::cout << "Unrecognized flag or no corners found: " << cornerDetectionFlag << std::endl;

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
        std::cout << "Given target point (" << targetPoint.x << "," << targetPoint.y << ") found corner at (" <<  corner->x << "," << corner->y << ") distance: " <<  minDistance << std::endl;
    }

    if(draw && minDistance <= distanceThreshold && debug)
    {
        cvCircle( iplImage, targetPoint, 5, cvScalar(0,255,0), 1, 8, 0 );
        cvLine(iplImage, targetPoint, cv::Point2f(corner->x, corner->y) ,cvScalar(255,0,255));
        cvCircle( iplImage, cv::Point2f(corner->x, corner->y), 5, cvScalar(0,0,255), 1, 8, 0 );
    }
    return (minDistance <= distanceThreshold);
}

void svlCCCornerDetector::drawCorners(IplImage* iplImage)
{
    if(debug)
        std::cout << "Drawing corners with flag: " << cornerDetectionFlag << std::endl;

    switch(cornerDetectionFlag)
    {
    case FEATURES:
        for(unsigned int i = 0; i < chessboardCorners.size(); i++ )
        {
            cvCircle( iplImage, cvPoint(cvRound(chessboardCorners.at(i).x),cvRound(chessboardCorners.at(i).y)), 3, cvScalar(255,255,0), 1, 8, 0 );
        }
        break;
		default:
        std::cout << "Unrecognized flag or no corners found: " << cornerDetectionFlag << std::endl;

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
void svlCCCornerDetector::detectCorners(cv::Mat matImage,IplImage* iplImage)
{
    reset();

    if(cornerDetectionFlag == NO_CORNERS)
    {
        findGoodFeatures(matImage);
    }

    // Draw detected corners
    //if(debug)
    drawCorners(iplImage);

}



