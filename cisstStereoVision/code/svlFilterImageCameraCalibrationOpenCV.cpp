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

#include <cisstStereoVision/svlFilterImageCameraCalibrationOpenCV.h>
#include "svlImageProcessingHelper.h"
#include <sstream>

const static int MINCORNERTHRESHOLD = 5;
const static int MAXCALIBRATIONITERATION = 10;
const static int MAXNUMBEROFGRIDS = 50;
const static bool DEBUG = false;

/***************************************************/
/*** svlFilterImageCameraCalibrationOpenCV class ***/
/***************************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageCameraCalibrationOpenCV, svlFilterBase)

        svlFilterImageCameraCalibrationOpenCV::svlFilterImageCameraCalibrationOpenCV() :
        svlFilterBase()
{
    AddInput("input", true);
    AddOutput("output", true);
    AddOutput("calibration", false);
    SetOutputType("calibration", svlTypeCameraGeometry);

    Visibility = new int[MAXNUMBEROFGRIDS];
    CameraGeometry = new svlSampleCameraGeometry();
    MinHandEyeAvgError = std::numeric_limits<double>::max( );
    CameraCalibrationError = std::numeric_limits<double>::max( );
}

svlFilterImageCameraCalibrationOpenCV::~svlFilterImageCameraCalibrationOpenCV()
{
    if(Visibility) delete Visibility;
    if(CameraGeometry) delete CameraGeometry;

}

void svlFilterImageCameraCalibrationOpenCV::Reset()
{
    Images.clear();
    CalibrationGrids.clear();
    Rvecs.clear();
    Tvecs.clear();
    MinHandEyeAvgError = std::numeric_limits<double>::max( );
    CameraCalibrationError = std::numeric_limits<double>::max( );
    CameraGeometry->Empty();
}

int svlFilterImageCameraCalibrationOpenCV::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterImageCameraCalibrationOpenCV::Process(svlProcInfo* CMN_UNUSED(procInfo), svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

bool svlFilterImageCameraCalibrationOpenCV::ProcessImage(std::string imageDirectory, std::string imagePrefix, std::string imageType, int index, vctDynamicVector<vctInt2> originIndicators)
{
    std::stringstream path;
    std::string currentFileName;

    svlCCCalibrationGrid* calibrationGrid;
    svlCCOriginDetector* calOriginDetector;
    svlCCCornerDetector* calCornerDetector;
    calCornerDetector = new svlCCCornerDetector(BoardSize.width,BoardSize.height);
    calOriginDetector = new svlCCOriginDetector();
    vctDynamicVector<vctDynamicVector<vctInt2> > localOriginIndicators;
    unsigned int ok = 0;

    // image file
    path << imageDirectory;
    path << imagePrefix;
    path.fill('0');
    path << std::setw(3) << index << std::setw(1);
    path << "." << imageType;

    std::cout << "Attempting to load image: " << path.str() << std::endl;

    ok = svlImageIO::Read(image, 0, path.str());
    if(ok != SVL_OK)
    {
        std::cout << "ERROR: svl Failed to load image: " << path.str() << std::endl;
        return false;
    }

    cv::Mat matImage(image.IplImageRef());
    if(!(matImage.data))
    {
        std::cout << "ERROR: cv::Mat Failed to convert image: " << path.str() << std::endl;
        return false;
    }
    if(originIndicators.empty())
    {
        // tracker coords file
        path.str(std::string());
        path << imageDirectory;
        path << imagePrefix;
        path.fill('0');
        path << std::setw(3) << index << std::setw(1);
        path << ".colorpts";
        currentFileName = path.str();

        if(DEBUG)
            std::cout << "Reading origin indicators for " << currentFileName << std::endl;
        localOriginIndicators.resize(1);
        ImportOriginsFile(path.str(),localOriginIndicators);
    }

    // find origin must preceed corners, additional draws throws off threshold
    if(!localOriginIndicators.empty())
    {
        std::cout << "using origin indicators: " << localOriginIndicators << std::endl;
        calOriginDetector->detectOrigin(image.IplImageRef(),localOriginIndicators[0]);
    }else
    {
        calOriginDetector->detectOrigin(image.IplImageRef(),originIndicators);
    }
    // find corners
    calCornerDetector->detectCorners(matImage,image.IplImageRef());

    // find corner correlation
    calibrationGrid = new svlCCCalibrationGrid(image.IplImageRef(), BoardSize,SquareSize);
    calibrationGrid->correlate(calOriginDetector, calCornerDetector);

    // tracker coords file
    path.str(std::string());
    path << imageDirectory;
    path << imagePrefix;
    path.fill('0');
    path << std::setw(3) << index << std::setw(1);
    path << ".coords";
    currentFileName = path.str();

    if(DEBUG)
        std::cout << "Reading coords for " << currentFileName << std::endl;

    svlCCTrackerCoordsFileIO coordsFileIO(currentFileName.c_str());
    ok = coordsFileIO.parseFile();
    if(ok)
    {
        coordsFileIO.repackData();
        calibrationGrid->worldToTCP = coordsFileIO.worldToTCP;
        calibrationGrid->hasTracking = true;
    }else{
        calibrationGrid->hasTracking = false;
    }

    //save images and calibration grids
    Images.push_back(image);

    if(calibrationGrid->valid)
    {
        CalibrationGrids.push_back(calibrationGrid);
        ImageSize =  matImage.size();
        return true;
    }
    else
        return false;
}

/**************************************************************************************************
* ProcessImages
*	Process images individually for calibration
*
* Input:
*	imageDirectory                  string						- Directory where images are
*	imagePrefix			string						- Common prefix for images
*	imageType			string						- Commen appendix for images
*	startIndex			int							- Image index to start
*	stopIndex			int							- Image index to end
*
* Output:
*	bool											- Success indicator
*
***********************************************************************************************************/
bool svlFilterImageCameraCalibrationOpenCV::ProcessImages(std::string imageDirectory, std::string imagePrefix, std::string imageType, int startIndex, int stopIndex, bool loadOrigins)
{
    bool valid = false;
    vctDynamicVector<vctDynamicVector<vctInt2> > origins;

    if(loadOrigins)
    {
        std::stringstream path;
        // image file
        path << imageDirectory;
        path << "origins.txt";
        origins.resize(stopIndex-startIndex+1);
        loadOrigins = ImportOriginsFile(path.str(),origins);
        std::cout << "use origins file: "<< loadOrigins << " " << path.str() << std::endl;
    }

    for(int i=startIndex;i<stopIndex+1;i++){
        if(loadOrigins)
            valid = ProcessImage(imageDirectory, imagePrefix, imageType, i, origins[i]) || valid;
        else
            valid = ProcessImage(imageDirectory, imagePrefix, imageType, i) || valid;
    }

    if (!valid)
    {
        std::cout << "svlFilterImageCameraCalibrationOpenCV.process() - NO VALID IMAGES! Please acquire more images and try again! " << std::endl;
    }

    return valid;
}

bool svlFilterImageCameraCalibrationOpenCV::ImportOriginsFile(const std::string & inputFile, vctDynamicVector<vctDynamicVector<vctInt2> >& origins)
{
    vct3 positionFromFile;

    std::string tempLine = "aaaa";
    std::vector <std::string> token;
    std::ifstream inf(inputFile.c_str());
    int i=0;
    while(1) {
        tempLine = "aaaa";
        std::vector <std::string> token;
        std::getline(inf, tempLine);
        Tokenize(tempLine, token, ",");
        if (inf.eof() || token.size() <= 0)
            break;
        std::copy(token.begin(), token.end(), std::ostream_iterator<std::string>(std::cerr, " "));
        // std::cerr << token << std::endl;
        if (token.at(0).compare(0,1,"#")) {
            //assume format is name,point1X,point1Y,point2X,point2Y,point3X,point3Y
            if (token.size() < 7)
            {
                std::cout << "ERROR: ImportOriginsfiles: ";
                std::copy(token.begin(), token.end(), std::ostream_iterator<std::string>(std::cout, " "));
                std::cout << std::endl;
                return false;
            }
            //assume 3 points
            origins[i].resize(3);
            origins[i][0] = vctInt2((int)strtod(token.at(1).c_str(), NULL),(int)strtod(token.at(2).c_str(), NULL));
            origins[i][1] = vctInt2((int)strtod(token.at(3).c_str(), NULL),(int)strtod(token.at(4).c_str(), NULL));
            origins[i][2] = vctInt2((int)strtod(token.at(5).c_str(), NULL),(int)strtod(token.at(6).c_str(), NULL));
            i++;
        }
        token.clear();
    }
    return true;
}


void svlFilterImageCameraCalibrationOpenCV::Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


bool svlFilterImageCameraCalibrationOpenCV::RunCameraCalibration(bool runHandEye)
{
    CameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    DistCoeffs  = cv::Mat::zeros(5, 1, CV_64F);

    ObjectPoints.clear();
    ProjectedObjectPoints.clear();
    ImagePoints.clear();
    ProjectedImagePoints.clear();
    PointFiles.clear();

    this->RunHandEye = runHandEye;
    if(RunHandEye)
    {
        CalHandEye = new svlCCHandEyeCalibration(CalibrationGrids);
    }

    double rms = OptimizeCalibration();

    if(rms < std::numeric_limits<double>::max( ))
    {
        UpdateCameraGeometry();
        return true;
    }

    return false;
}

/**************************************************************************************************
* OptimizeCalibration()
*	Optimize on calibration by projection and refining each grid
*
* Output:
*	void
*
***********************************************************************************************************/
double svlFilterImageCameraCalibrationOpenCV::OptimizeCalibration()
{
    double rms = std::numeric_limits<double>::max( );
    double prevRMS = std::numeric_limits<double>::max( );

    int prevPointsCount = 0;
    int iteration = 0;
    cv::Mat pPrevCameraMatrix;
    cv::Mat pPrevDistCoeffs;
    std::vector<cv::Mat> pPrevRvecs;
    std::vector<cv::Mat> pPrevTvecs;
    cv::Mat prevCameraMatrix;
    cv::Mat prevDistCoeffs;
    std::vector<cv::Mat> prevRvecs;
    std::vector<cv::Mat> prevTvecs;
    std::vector<cv::Mat> originalCameraMatrix;
    std::vector<cv::Mat> originalDistCoeffs;
    std::vector<cv::Mat> originalRvecs;
    std::vector<cv::Mat> originalTvecs;
    float* originalThresholds = new float[MAXNUMBEROFGRIDS];
    int* prevVisibility = new int[MAXNUMBEROFGRIDS];
    int* pPrevVisibility = new int[MAXNUMBEROFGRIDS];
    float prevThreshold, pPrevThreshold;
    int validIndex = 0;
    int maxPointsCount = 0;
    float refineThreshold = 2.0;
    int rootMeanSquaredThreshold = 1;
    int pointsCount;

    //if(max(imageSize.height,imageSize.width) > 1000)
    //	refineThreshold = 4;

    //save original
    for(int i=0;i<(int)CalibrationGrids.size();i++)
    {
        pPrevVisibility[i] = CalibrationGrids.at(i)->valid;
        if(CalibrationGrids.at(i)->valid)
        {
            originalCameraMatrix.push_back(CalibrationGrids.at(i)->cameraMatrix);
            originalDistCoeffs.push_back(CalibrationGrids.at(i)->distCoeffs);
            originalRvecs.push_back(CalibrationGrids.at(i)->rvec);
            originalTvecs.push_back(CalibrationGrids.at(i)->tvec);
            originalThresholds[i] = CalibrationGrids.at(i)->refineThreshold;
        }
    }

    //refine
    rms = Calibrate(false,false,&pointsCount);
    prevCameraMatrix = CameraMatrix;
    prevDistCoeffs = DistCoeffs;
    prevRvecs =Rvecs;
    prevTvecs = Tvecs;
    prevThreshold = refineThreshold;
    pPrevCameraMatrix = prevCameraMatrix;
    pPrevDistCoeffs = prevDistCoeffs;
    pPrevRvecs = prevRvecs;
    pPrevTvecs = prevTvecs;
    pPrevThreshold = prevThreshold;

    for(int i=0;i<(int)CalibrationGrids.size();i++)
    {
        if(DEBUG)
            std::cout << "Grid " << i << " valid: " << Visibility[i] << std::endl;
        if(CalibrationGrids.at(i)->valid)
        {
            Visibility[i] = 1;
        }
        else
            Visibility[i] = 0;
        prevVisibility[i] = Visibility[i];
    }
    maxPointsCount = std::max(pointsCount,maxPointsCount);
    int pointIncreaseIteration = 0;

    // check for bad calibration
    if(rms == std::numeric_limits<double>::max( ))
        return rms;

    while((rms < std::numeric_limits<double>::max( )) && (rms > rootMeanSquaredThreshold)&& (iteration < MAXCALIBRATIONITERATION))
    {
         // Lower threshold for higher iteration of optimization
        if(iteration > 1)
            refineThreshold = 2;

        if(rms > prevRMS &&
           pointsCount > static_cast<int>(maxPointsCount + MINCORNERTHRESHOLD*CalibrationGrids.size()))
        {
            refineThreshold = 1;
            pointIncreaseIteration++;
        }

        if(rms < prevRMS ||
           pointsCount > static_cast<int>(maxPointsCount + pointIncreaseIteration*MINCORNERTHRESHOLD*CalibrationGrids.size()))
        {
            std::cout << "Iteration: " << iteration << " rms delta: " << prevRMS-rms << " count delta: " <<  pointsCount-maxPointsCount << " pointIteration " << pointIncreaseIteration <<std::endl;
            pPrevCameraMatrix = prevCameraMatrix;
            pPrevDistCoeffs = prevDistCoeffs;
            pPrevRvecs = prevRvecs;
            pPrevTvecs = prevTvecs;
            pPrevThreshold = prevThreshold;
            prevCameraMatrix = CameraMatrix;
            prevDistCoeffs = DistCoeffs;
            prevRvecs =Rvecs;
            prevTvecs = Tvecs;
            prevRMS = rms;
            prevPointsCount = pointsCount;
            prevThreshold = refineThreshold;
            for(int i=0;i<(int)CalibrationGrids.size();i++)
            {
                pPrevVisibility[i] = prevVisibility[i];
                if(CalibrationGrids.at(i)->valid)
                {
                    Visibility[i] = 1;
                }
                else
                    Visibility[i] = 0;
                prevVisibility[i] = Visibility[i];
            }
            maxPointsCount = std::max(pointsCount,maxPointsCount);
            RefineGrids(refineThreshold);
            rms = Calibrate(false,false,&pointsCount);

        }else
        {
            break;
        }
        iteration++;
    }

    //if(DEBUG)
    std::cout <<std::endl << "==========Optimize Calibration stopped at " << iteration << " iterations=========" <<std::endl;
    if(DEBUG)
        PrintCalibrationParameters();

    if(iteration > 0)
    {
        CameraMatrix = pPrevCameraMatrix;
        DistCoeffs = pPrevDistCoeffs;
        Rvecs = pPrevRvecs;
        Tvecs = pPrevTvecs;
        refineThreshold = pPrevThreshold;

        if(DEBUG)
            PrintCalibrationParameters();

        for(int i=0;i<(int)CalibrationGrids.size();i++)
        {
            Visibility[i] = pPrevVisibility[i];
        }
        RefineGrids(refineThreshold);
        rms = Calibrate(false, false, &pointsCount);
    }else{
        //No iteration, set calibration grids back to original
        for(int i=0;i<(int)CalibrationGrids.size();i++)
        {
            if(pPrevVisibility[i]){
                CalibrationGrids.at(i)->refine(originalRvecs.at(validIndex),originalTvecs.at(validIndex),originalCameraMatrix.at(validIndex),originalDistCoeffs.at(validIndex),originalThresholds[i],false);
                validIndex++;
            }
        }

        if(DEBUG)
            PrintCalibrationParameters();
        rms = Calibrate(false, false,&pointsCount);
    }


    return rms;
}

/**************************************************************************************************
* calibrate()
*	Calibrate with current points
*
* Input
*	projected		bool				- Indicator whether or not to use the projected points from last
*										calibration, with known improvement or standard calibration
*										with current points
*
* Output:
*	void
*
***********************************************************************************************************/
double svlFilterImageCameraCalibrationOpenCV::Calibrate(bool projected, bool groundTruthTest, int *pointsCount)
{
    // empty points vectors
    ImagePoints.clear();
    ObjectPoints.clear();
    *pointsCount = 0;
    std::vector<cv::Point2f> points2D;
    std::vector<cv::Point3f> points3D;

    if(!projected)
    {
        //empty projected points
        ProjectedImagePoints.clear();
        ProjectedObjectPoints.clear();

        //get Points
        for(int i=0;i<(int)CalibrationGrids.size();i++)
        {
            Visibility[i] = 0;
            if((!groundTruthTest && CalibrationGrids.at(i)->valid) || (groundTruthTest && CalibrationGrids.at(i)->validGroundTruth)){
                points2D.clear();
                points3D.clear();
                if(groundTruthTest)
                {
                    points2D = CalibrationGrids.at(i)->groundTruthImagePoints;
                    points3D = CalibrationGrids.at(i)->groundTruthCalibrationGridPoints;
                }
                else
                {
                    points2D = CalibrationGrids.at(i)->getGoodImagePoints();
                    points3D = CalibrationGrids.at(i)->getGoodCalibrationGridPoints3D();
                }
                if (static_cast<int>(points2D.size()) > CalibrationGrids.at(i)->minGridPoints &&
                    static_cast<int>(points3D.size()) > CalibrationGrids.at(i)->minGridPoints &&
                    points2D.size() == points3D.size())
                {
                    std::cout << "image " << i << " using " << points2D.size() <<" points." << std::endl;
                    ImagePoints.push_back(points2D);
                    ObjectPoints.push_back(points3D);
                    *pointsCount += points2D.size();
                    Visibility[i] = 1;
                }
            }
        }
    }

    double rms = std::numeric_limits<double>::max( );
    bool check = false;
    double handEyeAvgError = std::numeric_limits<double>::max( );

    if(projected)
    {
        if(ProjectedObjectPoints.size() <= 0)
            return rms;
    }
    else
    {
        if(ObjectPoints.size() <= 0)
            return rms;
    }

    if(!projected)
        std::cout << "Calibrating using " << *pointsCount <<" points." << std::endl;

    rms = RunOpenCVCalibration(projected);
    check = CheckCalibration(projected);
    UpdateCalibrationGrids();
    if(this->RunHandEye)
    {
        handEyeAvgError = CalHandEye->calibrate();
        std::cout << "Returned HandEyeError " << handEyeAvgError <<" min so far " << MinHandEyeAvgError << std::endl;
        if(handEyeAvgError > 0 && handEyeAvgError < MinHandEyeAvgError)
        {
            MinHandEyeAvgError = handEyeAvgError;
            TcpTCamera = CalHandEye->tcp_T_camera;
        }
    }

    if(!check)
        return std::numeric_limits<double>::max( );
    else
        return rms;
}

/**************************************************************************************************
* checkCalibration()
*	Check for reprojection error from current calibration
*
* Input
*	projected		bool				- Indicator whether or not to use the projected points from last
*										calibration, with known improvement or standard calibration
*										with current points
*
* Output:
*	bool								- Indicator of success
*
***********************************************************************************************************/
bool svlFilterImageCameraCalibrationOpenCV::CheckCalibration(bool projected)
{
    bool ok = false;
    std::vector<float> reprojErrs;
    ok = checkRange(CameraMatrix) && checkRange(DistCoeffs);

    if(projected)
    {
        CameraCalibrationError = ComputeReprojectionErrors(ProjectedObjectPoints, ProjectedImagePoints,
            Rvecs, Tvecs, CameraMatrix, DistCoeffs, reprojErrs, projected);
    }else
    {
        CameraCalibrationError = ComputeReprojectionErrors(ObjectPoints, ImagePoints,
            Rvecs, Tvecs, CameraMatrix, DistCoeffs, reprojErrs, projected);
    }
    std::cout << "Range check " << ok << ", average L1 Norm error: " << CameraCalibrationError <<std::endl;

    if(ok && DEBUG)
    {
        PrintCalibrationParameters();
    }

    return ok;
}

/**************************************************************************************************
* runOpenCVCalibration()
*	Calibration with OpenCV function
*
* Input
*	projected		bool				- Indicator whether or not to use the projected points from last
*										calibration, with known improvement or standard calibration
*										with current points
*
* Output:
*	double								- RMS error
*
***********************************************************************************************************/
double svlFilterImageCameraCalibrationOpenCV::RunOpenCVCalibration(bool projected)
{
    int flags = 0;
    double rms;
    Rvecs.clear();
    Tvecs.clear();

    if(projected)
    {
        printf("============calibrateCamera: running projected============\n");
        rms = calibrateCamera(ProjectedObjectPoints, ProjectedImagePoints, ImageSize, CameraMatrix,
                              DistCoeffs, Rvecs, Tvecs, flags);//cv::CALIB_FIX_K1|cv::CALIB_FIX_K2|cv::CALIB_FIX_K3);
    }
    else
    {
        printf("============calibrateCamera: running standard============\n");
        rms = calibrateCamera(ObjectPoints, ImagePoints, ImageSize, CameraMatrix,
                              DistCoeffs, Rvecs, Tvecs, flags);//cv::CALIB_FIX_K1|cv::CALIB_FIX_K2|cv::CALIB_FIX_K3);
    }
    //if(DEBUG)
    printf("RMS error reported by calibrateCamera: %g\n", rms);

    return rms;
}

void svlFilterImageCameraCalibrationOpenCV::RefineGrids(float localThreshold)
{
    //refine
    int validIndex = 0;
    for(int i=0;i<(int)CalibrationGrids.size();i++)
    {
        if(Visibility[i])
        {
            CalibrationGrids.at(i)->refine(Rvecs[validIndex],Tvecs[validIndex],CameraMatrix,DistCoeffs,localThreshold,false);
            validIndex++;
        }
    }
}

/**************************************************************************************************
* computeReprojectionErrors()
*	Compute the average L1 reprojection error
*
* Input
*	objectPoints			const vector<vector<Point3f> >&				- 3D object points of chessboard plane
*	imagePoints				const vector<vector<Point3f> >&				- 2D image points
*	rvecs					const vector<Mat>&							- camera rotation vectors
*	tvecs					const vector<Mat>& 							- camera translation vectors
*	CameraMatrix			const Mat&									- camera intrinsic matrix
*	DistCoeffs				const Mat&									- camera distortion coefficients
*	perViewErrors			vector<float>&								- average error per camera
*	projected				bool										- Indicator whether or not to use projected or standard points
*
* Output:
*	double																- Average L1 reprojection error
*
***********************************************************************************************************/
double svlFilterImageCameraCalibrationOpenCV::ComputeReprojectionErrors(
    const std::vector<std::vector<cv::Point3f> >& objectPoints,
    const std::vector<std::vector<cv::Point2f> >& imagePoints,
    const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
    const cv::Mat& CameraMatrix, const cv::Mat& DistCoeffs,
    std::vector<float>& perViewErrors, bool projected )
{
    std::vector<cv::Point2f> imagePoints2, projectedImgPoints;
    std::vector<cv::Point3f> projectedObjPoints;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    int validIndex = 0;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
        projectPoints(cv::Mat(objectPoints[i]), rvecs[i], tvecs[i],
            CameraMatrix, DistCoeffs, imagePoints2);
        ProjectedImagePoints.push_back(imagePoints2);
        ProjectedObjectPoints = objectPoints;
        if(projected)
        {
            while(!CalibrationGrids.at(validIndex)->valid)
                validIndex++;

            for(int j=0;j<(int)imagePoints2.size();j++)
            {
                //orange, original projected points
                //cvCircle( images.at(validIndex).IplImageRef(), imagePoints.at(i).at(j), 3, cvScalar(55,0,255,0), 1, 8, 0 );
                //pink, projected final projected points
                cvCircle(Images.at(validIndex).IplImageRef(), imagePoints2.at(j), 3, cvScalar(255,0,255,0), 1, 8, 0 );
            }
            validIndex++;
        }
        err = norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), CV_L1 );
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)(err/n);
        totalErr += err;
        totalPoints += n;
    }

    return totalErr/totalPoints;
}

void svlFilterImageCameraCalibrationOpenCV::UpdateCalibrationGrids()
{
    int validIndex = 0;
    for(int i=0;i<(int)CalibrationGrids.size();i++)
    {
        if(Visibility[i]){
            CalibrationGrids.at(i)->cameraMatrix = CameraMatrix;
            CalibrationGrids.at(i)->distCoeffs = DistCoeffs;
            CalibrationGrids.at(i)->rvec = Rvecs[validIndex];
            CalibrationGrids.at(i)->tvec = Tvecs[validIndex];
            CalibrationGrids.at(i)->valid = 1;
            validIndex++;
        }else{
            CalibrationGrids.at(i)->valid = 0;
        }
    }
}

void svlFilterImageCameraCalibrationOpenCV::UpdateCameraGeometry()
{
    double alpha = 0.0;//assumed to be square pixels
    vct2 f = vct2(CameraMatrix.at<double>(0,0),CameraMatrix.at<double>(1,1));
    vct2 c = vct2(CameraMatrix.at<double>(0,2),CameraMatrix.at<double>(1,2));
    //reduced camera model set kc(5)=0; Zhang sets last 3 to zero
    vctFixedSizeVector<double,7> k = vctFixedSizeVector<double,7>(DistCoeffs.at<double>(0,0),DistCoeffs.at<double>(1,0),DistCoeffs.at<double>(2,0),DistCoeffs.at<double>(3,0),0.0,0.0,0.0);
    //vctFixedSizeVector<double,7> k = vctFixedSizeVector<double,7>(DistCoeffs.at<double>(0,0),DistCoeffs.at<double>(1,0),DistCoeffs.at<double>(2,0),DistCoeffs.at<double>(3,0),DistCoeffs.at<double>(4,0),0.0,0.0);
    CameraGeometry->SetIntrinsics(f,c,alpha,k);

//    std::cout << "==========setRectifier==============" << std::endl;
//    svlFilterOutput* output = GetOutput("calibration");
//    if (!output)
//        std::cout << "no calibration output" << std::endl;
//
//    if(!output->IsConnected())
//        std::cout << "calibration not conected" << std::endl;
//
//    svlFilterBase* filter = output->GetFilter();
//    if(!filter->IsInitialized())
//        std::cout << "calibration filter not initialized" << std::endl;
//    output->PushSample(CameraGeometry);

    //int result= rectifier->GetInput("calibration")->PushSample(CameraGeometry);
    //SHOULD NOT BE USING SetTableFromCameraCalibration() DIRECTLY
    //int result = rectifier->SetTableFromCameraCalibration(imageSize.height,imageSize.width, vct3x3::Eye(),f,c,k,0,0);
}

svlSampleCameraGeometry* svlFilterImageCameraCalibrationOpenCV::GetCameraGeometry(int index)
{
    if(index == -1)
    {
        return CameraGeometry;
    }
    else if(index < static_cast<int>(CalibrationGrids.size()-1))
    {
        return CalibrationGrids.at(index)->GetCameraGeometry();
    }

    return 0; // Balazs: return value was missing. Is this the intended behavior?
}

void svlFilterImageCameraCalibrationOpenCV::PrintCalibrationParameters()
{
    for(int i=0;i<(int)CameraMatrix.rows;i++)
    {
        for(int j=0;j<(int)CameraMatrix.cols;j++)
        {
            std::cout << "Camera matrix: " << CameraMatrix.at<double>(i,j) << std::endl;
        }
    }

    for(int i=0;i<(int)DistCoeffs.rows;i++)
    {
        for(int j=0;j<(int)DistCoeffs.cols;j++)
        {
            std::cout << "Distortion _coefficients: " << DistCoeffs.at<double>(i,j) << std::endl;
        }
    }

    std::cout << "Handeye error: "<< MinHandEyeAvgError<<std::endl;
    std::cout << TcpTCamera << std::endl;

}

void svlFilterImageCameraCalibrationOpenCV::WriteToFileCalibrationParameters(std::string directory)
{
    std::stringstream path;
    path << directory;
    path << "calibration.dat";
    vct2 f = vct2(CameraMatrix.at<double>(0,0),CameraMatrix.at<double>(1,1));
    vct2 c = vct2(CameraMatrix.at<double>(0,2),CameraMatrix.at<double>(1,2));
    //reduced camera model set kc(5)=0; Zhang sets last 3 to zero
    vctFixedSizeVector<double,7> k = vctFixedSizeVector<double,7>(DistCoeffs.at<double>(0,0),DistCoeffs.at<double>(1,0),DistCoeffs.at<double>(2,0),DistCoeffs.at<double>(3,0),0.0,0.0,0.0);
    //vctFixedSizeVector<double,7> k = vctFixedSizeVector<double,7>(DistCoeffs.at<double>(0,0),DistCoeffs.at<double>(1,0),DistCoeffs.at<double>(2,0),DistCoeffs.at<double>(3,0),DistCoeffs.at<double>(4,0),0.0,0.0);

    std::ofstream outputStream(path.str().c_str(), std::ofstream::binary);
    cmnSerializer serialization(outputStream);
    //outputStream << *cameraGeometry;

    //intrinsics
    outputStream << "<intrinsics size='" << ImageSize.width << ", " << ImageSize.height << "'\n";
    outputStream << "            f='" << f(0) << ", " << f(1) << "'\n";
    outputStream << "            c='" << c(0) << ", " << c(1) << "'\n";
    outputStream << "            k='" << k(0) << ", " << k(1) << ", " << k(2) << ", " << k(3) << ", " << k(4) << ", " << k(5) << ", " << k(6) << "' />\n";

    //extrinsics
    outputStream << "<extrinsics rotation='" << TcpTCamera(0,0) << ", " << TcpTCamera(0,1) << ", " << TcpTCamera(0,2) << ", ";
    outputStream << TcpTCamera(1,0) << ", " << TcpTCamera(1,1) << ", " << TcpTCamera(1,2) << ", ";
    outputStream << TcpTCamera(2,0) << ", " << TcpTCamera(2,1) << ", " << TcpTCamera(2,2) << "'\n";
    outputStream << "            translation='" << TcpTCamera(0,3) << ", " << TcpTCamera(1,3) << ", " << TcpTCamera(2,3) << "' />\n";

    // close the stream
    outputStream.close();
}

