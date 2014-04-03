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
#ifndef _svlCCOriginDetector_h
#define _svlCCOriginDetector_h

#include <math.h>
#include <iostream>

#include <cisstStereoVision/svlTypes.h>
class svlCCOriginDetector
{
public:
    enum originDetectionEnum { NO_ORIGIN, COLOR};
    enum colorIndexEnum {RED_INDEX, GREEN_INDEX, BLUE_INDEX, YELLOW_INDEX};
    enum colorModeEnum {RGB, RGY};

    svlCCOriginDetector(int colorModeFlag=RGY);
    void detectOrigin(IplImage* iplImage, vctDynamicVector<vctInt2> inputBlobs = vctDynamicVector<vctInt2>());
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
