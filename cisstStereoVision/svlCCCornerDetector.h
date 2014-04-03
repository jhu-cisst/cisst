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
#ifndef _svlCCCornerDetector_h
#define _svlCCCornerDetector_h

#include <math.h>
#include <iostream>
#include <cisstStereoVision/svlTypes.h>

class svlCCCornerDetector
{
public:
    enum cornerDetectionEnum { NO_CORNERS, OPENCV_CHESSBOARD, FEATURES};
    svlCCCornerDetector(int width, int height);
    void detectCorners(cv::Mat matImage, IplImage* iplImage);
    int getCornerDetectionFlag(){return cornerDetectionFlag;};
    std::vector<cv::Point2f> getChessboardCorners(){return chessboardCorners;};

private:
    bool nearestCorner(IplImage* iplImage, cv::Point2f targetPoint,cv::Point2f* corner, float distanceThreshold, bool draw);
    void findGoodFeatures(cv::Mat matImage);

    void drawCorners(IplImage* iplImage);
    float distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2);
    void reset();

    ////////// Parameters //////////
    const static bool debug = false;
    int cornerDetectionFlag;

    //chessboard corners
    std::vector<cv::Point2f> chessboardCorners;

};


#endif
