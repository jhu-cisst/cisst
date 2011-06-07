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
#ifndef _svlCCCameraCalibration_h
#define _svlCCCameraCalibration_h
#ifndef _cv_h
#include <cv.h>
#endif
#include <cisstStereoVision.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlFilterImageRectifier.h>
#include <cisstStereoVision/svlCCCalibrationGrid.h>
#include <cisstStereoVision/svlCCOriginDetector.h>
#include <cisstStereoVision/svlCCCornerDetector.h>
#include <cisstStereoVision/svlCCFileIO.h>
#include <limits>

//there aren't directives in OpenCV v.5, but they exist in OpenCV v.4
#undef CV_MIN
#undef CV_MAX
#define  CV_MIN(a, b)   ((a) <= (b) ? (a) : (b)) 
#define  CV_MAX(a, b)   ((a) >= (b) ? (a) : (b))


// Always include last!
#include <cisstStereoVision/svlExport.h>

//there aren't directives in OpenCV v.5, but they exist in OpenCV v.4
#undef CV_MIN
#undef CV_MAX
#define  CV_MIN(a, b)   ((a) <= (b) ? (a) : (b)) 
#define  CV_MAX(a, b)   ((a) >= (b) ? (a) : (b))

class CISST_EXPORT svlCCCameraCalibration
{

	public:
		svlCCCameraCalibration();
		bool processImages(std::string imageDirectory, std::string imagePrefix, std::string imageType, int startIndex, int stopIndex, int boardWidth, int boardHeight, int originDetectorColorModeFlag);
		svlFilterImageRectifier* getRectifier();
        void printCalibrationParameters();
		cv::Mat cameraMatrix, distCoeffs;
		std::vector<svlSampleImageRGB> images;
		cv::Size imageSize;


	private:
	double computeReprojectionErrors(
		const std::vector<std::vector<cv::Point3f> >& objectPoints,
		const std::vector<std::vector<cv::Point2f> >& imagePoints,
		const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
			const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
	        std::vector<float>& perViewErrors, bool projected );
		void updateCalibrationGrids();
		double runCalibration(bool projected);
		bool checkCalibration(bool projected);
		double calibrate(bool projected);
		void refineGrids(int localThreshold);
		void optimizeCalibration();
		bool calibration(bool groundTruthTest);

		////////// Parameters //////////
		std::vector<svlCCCalibrationGrid*> calibrationGrids;
		std::vector<cv::Mat> rvecs;
		std::vector<cv::Mat> tvecs;
		int flags;
		std::vector<cv::Point3f> objectImagePoints;
		std::vector<std::vector<cv::Point3f> > objectPoints;
		std::vector<std::vector<cv::Point3f> > projectedObjectPoints;
		std::vector<std::vector<cv::Point2f> > imagePoints;
		std::vector<std::vector<cv::Point2f> > projectedImagePoints;
		svlCCOriginDetector* calOriginDetector; 
		svlCCCornerDetector* calCornerDetector;

		std::vector<svlCCPointsFileIO*> pointFiles;
		// const static 
		bool debug;
		int minCornerThreshold;
		double rootMeanSquaredThreshold;
		int maxCalibrationIteration;
		//Hard coded to 25 for now
		int visibility[25];
		int refineThreshold;
		int pointsCount;

};

#endif