#ifndef _ccalOriginDetector_h
#define _ccalOriginDetector_h
#include <highgui.h>
#include <cv.h>
#include <math.h>
#include <iostream>
//there aren't directives in OpenCV v.5, but they exist in OpenCV v.4
#undef CV_MIN
#undef CV_MAX
#define  CV_MIN(a, b)   ((a) <= (b) ? (a) : (b)) 
#define  CV_MAX(a, b)   ((a) >= (b) ? (a) : (b))

class ccalOriginDetector
{
	public:
		enum originDetectionEnum { NO_ORIGIN, COLOR};
		enum colorIndexEnum {RED_INDEX, GREEN_INDEX, BLUE_INDEX, YELLOW_INDEX};
		enum colorModeEnum {RGB, RGY};

		ccalOriginDetector(int colorModeFlag);
		void detectOrigin(IplImage* iplImage);
		int getOriginDetectionFlag(){return originDetectionFlag;};
		int getOriginColorModeFlag(){return originColorModeFlag;};
		std::vector<cv::Point2f> getColorBlobs() { return colorBlobs;};
		cv::Point2f getOrigin() { return origin;};
	
	private:
		void reset();
		void findOriginByColor( IplImage* img); 
		bool findColorBlobs(IplImage* iplImage, float radius, int flags[],int thresholds[]);
		
		void drawColorBlobs(IplImage* iplImage);
		float distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2);
		cv::Point2f intersectionByColorBlobs(float point_x, float point_y, float line_x1, float line_y1, float line_x2, float line_y2);
	
		////////// Parameters //////////
		const static bool debug = false;
		int originDetectionFlag;
		int originColorModeFlag;
		std::vector<cv::Point2f> colorBlobs;
		cv::Point2f origin;

};
#endif