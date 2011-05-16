#ifndef _ccalCalibrationGrid_h
#define _ccalCalibrationGrid_h
#include <highgui.h>
#include <cv.h>
#include <math.h>
#include <iostream>
#include "ccalOriginDetector.h"
#include "ccalCornerDetector.h"

class ccalCalibrationGrid
{
	public:
		ccalCalibrationGrid(IplImage* iplImage, cv::Size boardSize, float size);
		void correlate(ccalOriginDetector* originDetector, ccalCornerDetector* cornerDetector);
		double refine(const cv::Mat& localRvec, const cv::Mat& localTvec, const cv::Mat& localCameraMatrix, const cv::Mat& localDistCoeffs, float threshold, bool runHomography, bool checkNormalized);
		std::vector<cv::Point2f> getGoodImagePoints();
		std::vector<cv::Point3f> getGoodCalibrationGridPoints3D();
		std::vector<cv::Point2f> ccalCalibrationGrid::getGoodProjectedImagePoints();
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