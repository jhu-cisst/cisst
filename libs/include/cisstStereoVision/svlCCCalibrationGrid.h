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
#ifndef _svlCCCalibrationGrid_h
#define _svlCCCalibrationGrid_h
#ifndef _highgui_h
#include <highgui.h>
#endif
#ifndef _cv_h
#include <cv.h>
#endif
#include <math.h>
#include <iostream>
#include "svlCCOriginDetector.h"
#include "svlCCCornerDetector.h"

class svlCCCalibrationGrid
{
	public:
		svlCCCalibrationGrid(IplImage* iplImage, cv::Size boardSize, float size);
		void correlate(svlCCOriginDetector* originDetector, svlCCCornerDetector* cornerDetector);
		double refine(const cv::Mat& localRvec, const cv::Mat& localTvec, const cv::Mat& localCameraMatrix, const cv::Mat& localDistCoeffs, float threshold, bool runHomography, bool checkNormalized);
		std::vector<cv::Point2f> getGoodImagePoints();
		std::vector<cv::Point3f> getGoodCalibrationGridPoints3D();
		std::vector<cv::Point2f> getGoodProjectedImagePoints();
		void optimizeCalibration();
		void compareGroundTruth();
		void printCalibrationParameters();

		////////// Parameters //////////
		cv::Point2f** calibrationGridPoints;
		cv::Point2f** imagePoints;
		bool** visibility;
		float gridSize;
		cv::Size boardSize;
		float gridSizePixel;
		std::vector<cv::Point2f> groundTruthImagePoints;
		std::vector<cv::Point3f> groundTruthCalibrationGridPoints;
		std::vector<cv::Point2f> intersectionImagePoints;
		std::vector<cv::Point3f> intersectionCalibrationGridPoints;
		cv::Mat cameraMatrix;
		CvMat* groundTruthCameraTransformation;
		CvMat* worldToTCP;
		cv::Mat distCoeffs;
		cv::Mat rvec;
		cv::Mat tvec;
		cv::Mat rmatrix;
		bool valid;
		int refineThreshold;
		std::vector<cv::Point2f> goodImagePoints;

	private:
		int findGridPointIndex(cv::Point3f point);
		void create2DChessboardCorners(bool visible);
		void homographyCorrelation(double threshold);
		int applyHomography(double homography[], float threshold);
		bool updateHomography(float threshold);
		float distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2);
		cv::Point2f extrapolateFromTwoPoints(float x1, float y1, float x2, float y2);
		cv::Point2f midPointBetweenTwoPoints(float x1, float y1, float x2, float y2);
		void findInitialCornerHelper(CvMat* coordsSrc, CvMat* coordsDst, bool initial);
		void findInitialCorners(CvMat* coordsSrc, CvMat* coordsDst);
		float nearestCorner(cv::Point2f targetPoint, cv::Point2f* corner, float distanceThreshold, bool checkNormalized);
		float nearestCornerNorm(cv::Point2f targetPoint, cv::Point2f* corner, float distanceThreshold);
		std::vector<cv::Point3f> getAllCalibrationGridPoints3D();		
		double runCalibration();
		bool isHighDefinition();
	
		////////// Parameters //////////
		const static bool debug = false;
		cv::Point2f calibrationGridOrigin;
		cv::Point2f imageOrigin;
		int homographyInlierLevel;
		CvMat* calibrationGridColorBlobs;
		CvMat* imageColorBlobs;
		std::vector<cv::Point2f> corners;
		std::vector<cv::Point2f> normCorners;
		IplImage* iplImage;
		std::vector<cv::Point2f> colorBlobsFromDetector;
		cv::Point2f originFromDetector;
		double calibrationError;
		std::vector<cv::Point2f> projectedImagePoints;
		double projectedImagePointsCalibrationError;
		int originColorModeFlag;
		int minGridPoints;

};

#endif