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

#include <cisstStereoVision.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterImageRectifier.h>
#include <cisstStereoVision/svlCCCalibrationGrid.h>
#include <cisstStereoVision/svlCCOriginDetector.h>
#include <cisstStereoVision/svlCCCornerDetector.h>
#include <cisstStereoVision/svlCCHandEyeCalibration.h>
#include <cisstStereoVision/svlCCFileIO.h>

#include <limits>

//there aren't directives in OpenCV v.5, but they exist in OpenCV v.4
#undef CV_MIN
#undef CV_MAX
#define  CV_MIN(a, b)   ((a) <= (b) ? (a) : (b)) 
#define  CV_MAX(a, b)   ((a) >= (b) ? (a) : (b))

// Always include last!
#include <cisstStereoVision/svlExport.h>

typedef vctDynamicNArrayRef<double,1> NumpyNVctType;

//there aren't directives in OpenCV v.5, but they exist in OpenCV v.4
#undef CV_MIN
#undef CV_MAX
#define  CV_MIN(a, b)   ((a) <= (b) ? (a) : (b)) 
#define  CV_MAX(a, b)   ((a) >= (b) ? (a) : (b))

class CISST_EXPORT svlCCCameraCalibration
{

public:
    svlCCCameraCalibration();
    bool process(std::string imageDirectory, std::string imagePrefix, std::string imageType, int startIndex, int stopIndex, int boardWidth, int boardHeight, int originDetectorColorModeFlag);
    int setRectifier(svlFilterImageRectifier* rectifier);
    bool processImage(std::string imageDirectory, std::string imagePrefix, std::string imageType, int index);
    bool runCameraCalibration();
    vct4x4 runHandEyeCalibration();
    int setImageVisibility(int index, int visible);
    std::vector<svlCCCalibrationGrid*> getCalibrationGrids(){return calibrationGrids;};
    int setFilterSourceDummy(svlFilterSourceDummy* source, int index);
    void printCalibrationParameters();
    std::vector<svlSampleImageRGB> images;
    cv::Size imageSize;
    svlSampleCameraGeometry* cameraGeometry;

private:
    double computeReprojectionErrors(
            const std::vector<std::vector<cv::Point3f> >& objectPoints,
            const std::vector<std::vector<cv::Point2f> >& imagePoints,
            const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
            const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
            std::vector<float>& perViewErrors, bool projected );
    void updateCalibrationGrids();
    double runOpenCVCalibration(bool projected);
    bool checkCalibration(bool projected);
    double calibrate(bool projected, bool groundTruthTest);
    void refineGrids(int localThreshold);
    void optimizeCalibration();
    bool calibration();
    void updateCameraGeometry();
    void reset();
    void runTest();
    vct2 getFocii(){ return f;};
    vct2 getCameraCenter(){ return c;};
    vctFixedSizeVector<double,7> getDistortionCoefficients(){return k;};

    ////////// Parameters //////////
    //camera parameters
    cv::Mat cameraMatrix, distCoeffs;
    cv::Mat groundTruthCameraMatrix, groundTruthDistCoeffs;
    vct2 f;
    vct2 c;
    vctFixedSizeVector<double,7> k;
    //vector of calibration grids
    std::vector<svlCCCalibrationGrid*> calibrationGrids;
    //vector of rotation matrices
    std::vector<cv::Mat> rvecs;
    //vector of translation matrices
    std::vector<cv::Mat> tvecs;
    //flag for OpenCV camera calibration, default to 0
    int flags;
    //size of calibration grid
    cv::Size boardSize;
    //vector of 3D calibration grid points
    std::vector<std::vector<cv::Point3f> > objectPoints;
    //vector of projected calibration grid points
    std::vector<std::vector<cv::Point3f> > projectedObjectPoints;
    //vector of 2D image points of calibration grid
    std::vector<std::vector<cv::Point2f> > imagePoints;
    //vector of 2D projected image points of calibration grid
    std::vector<std::vector<cv::Point2f> > projectedImagePoints;
    svlCCOriginDetector* calOriginDetector;
    svlCCCornerDetector* calCornerDetector;
    svlCCHandEyeCalibration* calHandEye;

    //vector of track point files (.coords)
    std::vector<svlCCPointsFileIO*> pointFiles;
    //DLR results for ground truth testing
    svlCCDLRCalibrationFileIO* dlrFileIO;
    int minCornerThreshold;
    double rootMeanSquaredThreshold;
    int maxCalibrationIteration;
    int maxNumberOfGrids;
    int* visibility;
    int refineThreshold;
    //number of points used  for calibration during optimization
    int pointsCount;
    svlSampleImageRGB image;

    bool debug;
    //compare with DLR
    bool groundTruthTest;
};

#endif
