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
#ifndef _svlCCCalibrationGrid_h
#define _svlCCCalibrationGrid_h

#include <math.h>
#include <iostream>
#include <limits>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlCCOriginDetector.h>
#include <cisstStereoVision/svlCCCornerDetector.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class svlCCCalibrationGrid
{
public:
    svlCCCalibrationGrid(IplImage* iplImage, cv::Size boardSize, float size);
    void correlate(svlCCOriginDetector* originDetector, svlCCCornerDetector* cornerDetector);
    double refine(const cv::Mat& localRvec, const cv::Mat& localTvec, const cv::Mat& localCameraMatrix, const cv::Mat& localDistCoeffs, float threshold, bool runHomography);
    std::vector<cv::Point2f> getGoodImagePoints();
    std::vector<cv::Point3f> getGoodCalibrationGridPoints3D();
    std::vector<cv::Point2f> getGoodProjectedImagePoints();
    void optimizeCalibration();
    void setGroundTruthTransformation(CvMat* groundTruthCameraTransformation);
    void compareGroundTruth();
    void printCalibrationParameters();
    svlSampleCameraGeometry* GetCameraGeometry();

    ////////// Parameters //////////
    cv::Point2f** calibrationGridPoints;
    cv::Point2f** imagePoints;
    bool** visibility;
    float gridSize;
    cv::Size boardSize;
    float gridSizePixel;
    std::vector<cv::Point2f> groundTruthImagePoints;
    std::vector<cv::Point3f> groundTruthCalibrationGridPoints;
    cv::Mat cameraMatrix;
    cv::Mat groundTruthRvec;
    CvMat* groundTruthRmatrix;
    cv::Mat groundTruthTvec;
    CvMat* worldToTCP;
    cv::Mat distCoeffs;
    cv::Mat rvec;
    cv::Mat tvec;
    cv::Mat rmatrix;
    bool valid;
    bool validGroundTruth;
    bool hasTracking;
    float refineThreshold;
    int minGridPoints;

private:
    int findGridPointIndex(cv::Point3f point);
    void create2DChessboardCorners(bool visible);
    void homographyCorrelation(float threshold);
    int applyHomography(double homography[], float threshold);
    bool updateHomography(float threshold);
    float distanceBetweenTwoPoints ( float x1, float y1, float x2, float y2);
    cv::Point2f extrapolateFromTwoPoints(float x1, float y1, float x2, float y2);
    cv::Point2f midPointBetweenTwoPoints(float x1, float y1, float x2, float y2);
    void findInitialCornerHelper(CvMat* coordsSrc, CvMat* coordsDst, bool initial);
    void findInitialCorners(CvMat* coordsSrc, CvMat* coordsDst);
    float nearestCorner(cv::Point2f targetPoint, cv::Point2f* corner, float distanceThreshold);
    std::vector<cv::Point3f> getAllCalibrationGridPoints3D();
    double runCalibration();
    bool isHighDefinition();

    ////////// Parameters //////////
    bool debug;
    cv::Point2f calibrationGridOrigin;
    cv::Point2f imageOrigin;
    int homographyInlierLevel;
    CvMat* calibrationGridColorBlobs;
    CvMat* imageColorBlobs;
    std::vector<cv::Point2f> corners;
    IplImage* iplImage;
    std::vector<cv::Point2f> colorBlobsFromDetector;
    cv::Point2f originFromDetector;
    double calibrationError;
    std::vector<cv::Point2f> projectedImagePoints;
    double projectedImagePointsCalibrationError;
    int originColorModeFlag;

};

#endif
