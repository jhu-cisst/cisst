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

#ifndef _svlFilterImageCameraCalibrationOpenCV_h
#define _svlFilterImageCameraCalibrationOpenCV_h

#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterImageRectifier.h>
#include <cisstStereoVision/svlCCCalibrationGrid.h>
#include <cisstStereoVision/svlCCOriginDetector.h>
#include <cisstStereoVision/svlCCCornerDetector.h>
#include <cisstStereoVision/svlCCHandEyeCalibration.h>
#include <cisstStereoVision/svlCCFileIO.h>
#include <cisstStereoVision/svlImageProcessing.h>
#include <limits>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterImageCameraCalibrationOpenCV : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterImageCameraCalibrationOpenCV();
    virtual ~svlFilterImageCameraCalibrationOpenCV();

    bool ProcessImages(std::string imageDirectory, std::string imagePrefix, std::string imageType, int startIndex, int stopIndex, bool loadOrigins=false);
    bool ProcessImage(std::string imageDirectory, std::string imagePrefix, std::string imageType, int index, vctDynamicVector<vctInt2> originIndicators = vctDynamicVector<vctInt2>());
    void Reset();
    bool RunCameraCalibration(bool runHandEye);
    void SetBoardSize(int width, int height){ BoardSize = cv::Size(width,height);};
    void SetSquareSize(float size){ SquareSize = size;};
    std::vector<svlSampleImageRGB> GetImages(){return Images;};
    svlSampleCameraGeometry* GetCameraGeometry(int index=-1);
    void SetCameraGeometry(vct2 f, vct2 c, double alpha, vctFixedSizeVector<double,7> k) { CameraGeometry->SetIntrinsics(f,c,alpha,k);};
    void PrintCalibrationParameters();
    void WriteToFileCalibrationParameters(std::string directory);
    double GetCameraCalibrationReprojectionError(void){return CameraCalibrationError;};
    double GetHandEyeCalibrationError(void) { return MinHandEyeAvgError;};

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

private:
    double OptimizeCalibration();
    void RefineGrids(float localThreshold);
    double Calibrate(bool projected, bool groundTruthTest, int *pointsCount);
    bool CheckCalibration(bool projected);
    double RunOpenCVCalibration(bool projected);
    double ComputeReprojectionErrors(
        const std::vector<std::vector<cv::Point3f> >& objectPoints,
        const std::vector<std::vector<cv::Point2f> >& imagePoints,
        const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
        const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
        std::vector<float>& perViewErrors, bool projected );
    void UpdateCalibrationGrids();
    void UpdateCameraGeometry();
    bool ImportOriginsFile(const std::string & inputFile, vctDynamicVector<vctDynamicVector<vctInt2> >& origins);
    void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);

    //Camera Calibration
    std::vector<svlSampleImageRGB> Images;
    //vector of calibration grids
    std::vector<svlCCCalibrationGrid*> CalibrationGrids;
    cv::Size ImageSize;
    //size of calibration grid
    cv::Size BoardSize;
    //size of squares in mm
    float SquareSize;
    svlSampleCameraGeometry* CameraGeometry;
    cv::Mat CameraMatrix, DistCoeffs;
    //vector of rotation matrices
    std::vector<cv::Mat> Rvecs;
    //vector of translation matrices
    std::vector<cv::Mat> Tvecs;
    //vector of 3D calibration grid points
    std::vector<std::vector<cv::Point3f> > ObjectPoints;
    //vector of projected calibration grid points
    std::vector<std::vector<cv::Point3f> > ProjectedObjectPoints;
    //vector of 2D image points of calibration grid
    std::vector<std::vector<cv::Point2f> > ImagePoints;
    //vector of 2D projected image points of calibration grid
    std::vector<std::vector<cv::Point2f> > ProjectedImagePoints;
    //vector of track point files (.coords)
    std::vector<svlCCPointsFileIO*> PointFiles;
    int* Visibility;
    double CameraCalibrationError;

    //Hand Eye Calibration
    svlCCHandEyeCalibration* CalHandEye;
    bool RunHandEye;
    double MinHandEyeAvgError;
    vct4x4 TcpTCamera;

    svlSampleImageRGB image;

};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterImageCameraCalibrationOpenCV)

#endif // svlFilterImageCameraCalibrationOpenCV_h

