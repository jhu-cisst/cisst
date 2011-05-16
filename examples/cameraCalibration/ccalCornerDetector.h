#ifndef _ccalCornerDetector_h
#define _ccalCornerDetector_h
#include <highgui.h>
#include <cv.h>
#include <math.h>
#include <iostream>

//there aren't directives in OpenCV v.5, but they exist in OpenCV v.4
#undef CV_MIN
#undef CV_MAX
#define  CV_MIN(a, b)   ((a) <= (b) ? (a) : (b)) 
#define  CV_MAX(a, b)   ((a) >= (b) ? (a) : (b))

class ccalCornerDetector
{
	public:
		enum cornerDetectionEnum { NO_CORNERS, OPENCV_CHESSBOARD, FEATURES};
		ccalCornerDetector(int width, int height);
		void detectCorners(cv::Mat matImage, IplImage* iplImage);
		int getCornerDetectionFlag(){return cornerDetectionFlag;};
		std::vector<cv::Point2f> getChessboardCorners(){return chessboardCorners;};
		std::vector<cv::Point2f> getChessboardCornersNormalized(){return chessboardCornersNorm;};
		bool nearestCorner(IplImage* iplImage, cv::Point2f targetPoint,cv::Point2f* corner, float distanceThreshold, bool draw);

	private:
		void findChessboard(cv::Mat matImage, IplImage* iplImage);
		void findGoodFeatures(cv::Mat matImage);
		void findGoodFeaturesNormalized(cv::Mat matImage,cv::Mat matGraySmooth);
		
		void drawCorners(IplImage* iplImage);
		float distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2);
		void reset();

		////////// Parameters //////////
		const static bool debug = false;
		int cornerDetectionFlag;
		int checkerBoardWidth;
		int checkerBoardHeight;	
		double quality_level;
		double min_distance;
		int eig_block_size;
		int use_harris;
		int maxCorners;
		double subPixTermEpsilon;

		//chessboard corners
		std::vector<cv::Point2f> chessboardCorners;
		std::vector<cv::Point2f> chessboardCornersNorm;

};


#endif