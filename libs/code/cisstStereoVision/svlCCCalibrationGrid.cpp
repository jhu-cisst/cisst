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
#include <cisstStereoVision/svlCCCalibrationGrid.h>

svlCCCalibrationGrid::svlCCCalibrationGrid(IplImage* iplImage, cv::Size boardSize, float gridSize)
{
    this->iplImage = iplImage;
    this->gridSize = gridSize;
    this->boardSize = boardSize;
    this->valid = false;
    this->cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    this->distCoeffs  = cv::Mat::zeros(5, 1, CV_64F);
    this->rvec = cv::Mat::zeros(3,1,CV_64F);
    this->tvec = cv::Mat::zeros(3,1,CV_64F);
    this->calibrationError = std::numeric_limits<double>::max( );;
    this->minGridPoints = 10;
    this->refineThreshold = 2;
    debug = false;
}

bool svlCCCalibrationGrid::isHighDefinition()
{
    if(iplImage == NULL)
        return false;
    return (std::max(iplImage->height,iplImage->width) > 1000);
}

/**************************************************************************************************
* create2DChessboardCorners()					
* This function creates the 2D points of your chessboard in its own coordinate system
*
* Input:
*	visible		bool						- indicator whether to make initial corners visible or not
*	
* Output:
*	void							
*
***********************************************************************************************************/
void svlCCCalibrationGrid::create2DChessboardCorners(bool visible)
{
    int width = boardSize.width;
    int height = boardSize.height;
    calibrationGridPoints = new cv::Point2f*[width];
    imagePoints = new cv::Point2f*[width];
    visibility = new bool*[width];

    for(int i=0;i<width;i++)
    {
        *(calibrationGridPoints+i)=new cv::Point2f[height];
        *(imagePoints+i)=new cv::Point2f[height];
        *(visibility+i)=new bool[height];
    }

    for( int j = 0; j < height; j++ )
    {
        for( int i = 0; i < width; i++ )
        {
            calibrationGridPoints[i][j] = cv::Point2f(calibrationGridOrigin.x+(i-width/2)*gridSizePixel,calibrationGridOrigin.y+(j-height/2)*gridSizePixel);
            imagePoints[i][j] = cv::Point2f(0,0);
            visibility[i][j] = visible;
            if(debug && j<5 && i<5)
            {
                std::cout << "create2DChessboardCorners: at " << i << "," << j << ": (" << calibrationGridOrigin.x+(i-width/2)*gridSizePixel << ",";
                std::cout << calibrationGridOrigin.y+(j-height/2)*gridSizePixel << ")" << std::endl;
            }

        }
    }
}

float svlCCCalibrationGrid::distanceBetweenTwoPoints (float x1, float y1, float x2, float y2)
{
    return sqrt( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) ) ;
}

cv::Point2f svlCCCalibrationGrid::extrapolateFromTwoPoints(float x1, float y1, float x2, float y2)
{
    return cv::Point2f(x2+(x2-x1),y2+(y2-y1));
}

cv::Point2f svlCCCalibrationGrid::midPointBetweenTwoPoints(float x1, float y1, float x2, float y2)
{
    return cv::Point2f((x1+x2)/2,(y1+y2)/2);
}

float svlCCCalibrationGrid::nearestCorner(cv::Point2f targetPoint, cv::Point2f* corner, float distanceThreshold, bool checkNormalized)
{
    float currentMinDistance, minDistance, normDistance;
    cv::Point2f currentTargetCorner = targetPoint;
    minDistance = std::numeric_limits<float>::max( );
    cv::Point2f* cornerNorm = new cv::Point2f();

    for(int i = 0; i < corners.size(); i++ )
    {
        currentMinDistance = distanceBetweenTwoPoints(targetPoint.x,targetPoint.y, corners.at(i).x, corners.at(i).y);
        if(currentMinDistance < minDistance)
        {
            currentTargetCorner = (cv::Point2f) corners.at(i);
            minDistance = currentMinDistance;
        }
    }

    //NOT USED
    //if(minDistance > distanceThreshold && checkNormalized)
    //{
    //	normDistance = nearestCornerNorm(targetPoint,cornerNorm,distanceThreshold);
    //	if(normDistance < minDistance && normDistance < 2)
    //	{
    //		currentTargetCorner.x = cornerNorm->x;
    //		currentTargetCorner.y = cornerNorm->y;
    //	}
    //}

    corner->x = currentTargetCorner.x;
    corner->y = currentTargetCorner.y;

    if(debug)
    {
        if(distanceThreshold > 4 &&  homographyInlierLevel == 1 && minDistance < gridSizePixel/2 )
        {
            cvCircle( iplImage, targetPoint, 5, cvScalar(0,255,0), 1, 8, 0 );
            cvLine(iplImage, targetPoint, cv::Point2f(corner->x, corner->y) ,cvScalar(255,0,255));
            cvCircle( iplImage, cv::Point2f(corner->x, corner->y), 5, cvScalar(0,0,255), 1, 8, 0 );
            //cout << "Target point (" << targetPoint.x << "," << targetPoint.y << ") & corner (" <<  corner->x << "," << corner->y << ")" <<endl;
            //cout << "Homography Level: " << homographyInlierLevel << " distance: " <<  minDistance << " grid size: " <<gridSizePixel<< endl;
        }
    }

    return minDistance;
}

float svlCCCalibrationGrid::nearestCornerNorm(cv::Point2f targetPoint, cv::Point2f* corner, float distanceThreshold)
{
    float currentMinDistance, minDistance;
    cv::Point2f currentTargetCorner = targetPoint;
    minDistance = std::numeric_limits<float>::max( );

    for(int i = 0; i < normCorners.size(); i++ )
    {
        currentMinDistance = distanceBetweenTwoPoints(targetPoint.x,targetPoint.y, normCorners.at(i).x, normCorners.at(i).y);
        if(currentMinDistance < minDistance)
        {
            currentTargetCorner = (cv::Point2f) normCorners.at(i);
            minDistance = currentMinDistance;
        }
    }

    corner->x = currentTargetCorner.x;
    corner->y = currentTargetCorner.y;

    if(debug)
    {
        if(distanceThreshold > 4 &&  homographyInlierLevel == 1 && minDistance < gridSizePixel/2 )
        {
            cvCircle( iplImage, targetPoint, 5, cvScalar(0,255,0), 1, 8, 0 );
            cvLine(iplImage, targetPoint, cv::Point2f(corner->x, corner->y) ,cvScalar(255,0,255));
            cvCircle( iplImage, cv::Point2f(corner->x, corner->y), 5, cvScalar(0,0,255), 1, 8, 0 );
            //cout << "Target point (" << targetPoint.x << "," << targetPoint.y << ") & corner (" <<  corner->x << "," << corner->y << ")" <<endl;
            //cout << "Homography Level: " << homographyInlierLevel << " distance: " <<  minDistance << " grid size: " <<gridSizePixel<< endl;
        }
    }

    return minDistance;
}

int svlCCCalibrationGrid::findGridPointIndex(cv::Point3f point)
{
    int val = -1;
    for(int i=0;i<groundTruthCalibrationGridPoints.size();i++)
    {
        if(groundTruthCalibrationGridPoints.at(i).x == point.x && groundTruthCalibrationGridPoints.at(i).y == point.y)
        {
            return i;
        }
    }
    return val;
}

/**************************************************************************************************
* compareGroundTruth()					
*	Compares calibration results with ground truth, currently read from DLR outputs.
*
* Looks for max and rms distance of corners and deltas of rotation and translation matrices
*
***********************************************************************************************************/
void svlCCCalibrationGrid::compareGroundTruth()
{
    int index;
    float distance, averageDistance = 0, maxDistance = -1;
    cv::Point3f maxPoint;
    int count = 0;
    std::vector<cv::Point2f> intersectionImagePoints;
    std::vector<cv::Point3f> intersectionCalibrationGridPoints;
    std::vector<cv::Point3f> grid = getGoodCalibrationGridPoints3D();
    std::vector<cv::Point2f> image = getGoodImagePoints();

    for(int i=0;i<grid.size();i++)
    {
        index = findGridPointIndex(grid.at(i));
        if(index != -1)
        {
            distance = distanceBetweenTwoPoints(groundTruthImagePoints.at(index).x,groundTruthImagePoints.at(index).y,image.at(i).x,image.at(i).y);
            if(distance > maxDistance)
            {
                maxDistance = distance;
                maxPoint = groundTruthCalibrationGridPoints.at(index);
            }
            //if(distance < 10){
            count++;
            averageDistance += distance;
            intersectionCalibrationGridPoints.push_back(grid.at(i));
            intersectionImagePoints.push_back(image.at(i));
            //intersectionCalibrationGridPoints.push_back(groundTruthCalibrationGridPoints.at(index));
            //intersectionImagePoints.push_back(groundTruthImagePoints.at(index));
            //}else{
            //	cout << "Point (" << grid.at(i).x << "," << grid.at(i).y << ") index: " << i << " too far! " << distance << endl;
            //}
        }else{
            //cout << "Point (" << grid.at(i).x << "," << grid.at(i).y << ") index: " << i << " not found " << endl;
        }
    }
    averageDistance /= count;
    std::cout << "Comparing ground truth max distance " << maxDistance <<  " Point (" << maxPoint.x << "," << maxPoint.y << ")" << std::endl;
    std::cout <<	" avg distance: " << averageDistance << " from " << count << " out of " << grid.size() << std::endl;

    std::cout << "groundTruthRvec: " << groundTruthRvec.at<double>(0,0) <<","<< groundTruthRvec.at<double>(0,1) <<","<< groundTruthRvec.at<double>(0,2) <<","<< std::endl;
    std::cout << "rvect: " << rvec.at<double>(0,0) <<","<< rvec.at<double>(0,1) <<","<< rvec.at<double>(0,2) <<","<< std::endl;
    std::cout << "groundTruthTvec: " << groundTruthTvec.at<double>(0,0) <<","<< groundTruthTvec.at<double>(0,1) <<","<< groundTruthTvec.at<double>(0,2) <<","<< std::endl;
    std::cout << "tvect: " << tvec.at<double>(0,0) <<","<< tvec.at<double>(0,1) <<","<< tvec.at<double>(0,2) <<","<< std::endl;

    std::cout << "world_T_TCP: " << worldToTCP->data.fl[0] <<","<< worldToTCP->data.fl[1]<<","<< worldToTCP->data.fl[2] <<","<< worldToTCP->data.fl[3] << std::endl;
    std::cout << "world_T_TCP: " << worldToTCP->data.fl[4] <<","<< worldToTCP->data.fl[5]<<","<< worldToTCP->data.fl[6] <<","<< worldToTCP->data.fl[7] << std::endl;
    std::cout << "world_T_TCP: " << worldToTCP->data.fl[8] <<","<< worldToTCP->data.fl[9]<<","<< worldToTCP->data.fl[10] <<","<< worldToTCP->data.fl[11] <<std::endl;
    std::cout << "world_T_TCP: " << worldToTCP->data.fl[12] <<","<< worldToTCP->data.fl[13]<<","<< worldToTCP->data.fl[14] <<","<< worldToTCP->data.fl[15] <<std::endl;
}

void svlCCCalibrationGrid::setGroundTruthTransformation(CvMat* groundTruthCameraTransformation)
{
    groundTruthRmatrix = cvCreateMat(3,3,CV_64FC1);
    groundTruthRmatrix->data.db[0] = groundTruthCameraTransformation->data.fl[0];
    groundTruthRmatrix->data.db[1] = groundTruthCameraTransformation->data.fl[1];
    groundTruthRmatrix->data.db[2] = groundTruthCameraTransformation->data.fl[2];
    groundTruthRmatrix->data.db[3] = groundTruthCameraTransformation->data.fl[4];
    groundTruthRmatrix->data.db[4] = groundTruthCameraTransformation->data.fl[5];
    groundTruthRmatrix->data.db[5] = groundTruthCameraTransformation->data.fl[6];
    groundTruthRmatrix->data.db[6] = groundTruthCameraTransformation->data.fl[8];
    groundTruthRmatrix->data.db[7] = groundTruthCameraTransformation->data.fl[9];
    groundTruthRmatrix->data.db[8] = groundTruthCameraTransformation->data.fl[10];
    CvMat* rodriguesRvec = cvCreateMat(3,1,CV_64FC1);
    cvRodrigues2(groundTruthRmatrix,rodriguesRvec);
    this->groundTruthRvec = (cv::Mat_<double>(3,1) << rodriguesRvec->data.db[0],rodriguesRvec->data.db[1],rodriguesRvec->data.db[2]);
    this->groundTruthTvec = (cv::Mat_<double>(3,1) << groundTruthCameraTransformation->data.fl[3],groundTruthCameraTransformation->data.fl[7],groundTruthCameraTransformation->data.fl[11]);
}

/**************************************************************************************************
* findInitialCornerHelper()					
*	THIS IS UGLY BRUTE FORCE INITIAL CORNER FINDING HELPER BY EXTRAPOLATION FROM ORIGIN AND COLOR BLOBS...CHANGE ME IF YOU CAN!!!
*
* Input:
*	coordsSrc		CvMat*						- Coordinates of virtual planar chessboard
*	coordsDst		CvMat*						- Coordinates of points on image
*	initial			bool						- inidicator for initial call or improved
*	
* Output:
*	void							
*
***********************************************************************************************************/
void svlCCCalibrationGrid::findInitialCornerHelper(CvMat* coordsSrc, CvMat* coordsDst, bool initial)
{
    float distanceFromOriginToRed = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::RED_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::RED_INDEX).y);
    float distanceFromOriginToGreen = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::GREEN_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::GREEN_INDEX).y);
    float distanceFromOriginToBlue = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::BLUE_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::BLUE_INDEX).y);
    float distanceFromOriginToYellow = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::YELLOW_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::YELLOW_INDEX).y);

    cv::Point2f extrapolatedCornerSrc;
    cv::Point2f extrapolatedCornerDst;
    int scale = 4;
    cv::Point2f* corner = new cv::Point2f();
    float sizePixel = 0;
    int numDetectorColorBlobs = 3;
    int index;

    for(int i=0;i<colorBlobsFromDetector.size();i++)
    {
        if(((originColorModeFlag == svlCCOriginDetector::RGB) && (i== svlCCOriginDetector::YELLOW_INDEX))||
           ((originColorModeFlag == svlCCOriginDetector::RGY) && (i== svlCCOriginDetector::BLUE_INDEX)))
            continue;
        index = i;
        if((originColorModeFlag == svlCCOriginDetector::RGY) && (i== svlCCOriginDetector::YELLOW_INDEX))
            index = i-1;
        coordsDst->data.fl[scale*index] = colorBlobsFromDetector.at(i).x;
        coordsDst->data.fl[scale*index+1] = colorBlobsFromDetector.at(i).y;
        if(initial)
        {
            imageColorBlobs->data.fl[2*index] = colorBlobsFromDetector.at(i).x;
            imageColorBlobs->data.fl[2*index+1] = colorBlobsFromDetector.at(i).y;
        }
        extrapolatedCornerDst = extrapolateFromTwoPoints(imageOrigin.x,imageOrigin.y,colorBlobsFromDetector.at(i).x,colorBlobsFromDetector.at(i).y);
        if(nearestCorner(extrapolatedCornerDst, corner, distanceFromOriginToRed/2, false) >= distanceFromOriginToRed/2)
        {
            corner->x = extrapolatedCornerDst.x;
            corner->y = extrapolatedCornerDst.y;
        }
        coordsDst->data.fl[scale*index+2] = corner->x;
        coordsDst->data.fl[scale*index+3] = corner->y;

        switch(i)
        {
        case (int)svlCCOriginDetector::RED_INDEX:
            extrapolatedCornerSrc = cv::Point2f(calibrationGridOrigin.x+gridSizePixel/2,calibrationGridOrigin.y-gridSizePixel/2);
            break;
        case (int)svlCCOriginDetector::GREEN_INDEX:
            extrapolatedCornerSrc = cv::Point2f(calibrationGridOrigin.x-gridSizePixel/2,calibrationGridOrigin.y+gridSizePixel/2);
            break;
        case (int)svlCCOriginDetector::BLUE_INDEX:
            if(originColorModeFlag == svlCCOriginDetector::RGB)
                extrapolatedCornerSrc = cv::Point2f(calibrationGridOrigin.x-gridSizePixel/2,calibrationGridOrigin.y-gridSizePixel/2);
            break;
        case (int)svlCCOriginDetector::YELLOW_INDEX:
            if(originColorModeFlag == svlCCOriginDetector::RGY)
                extrapolatedCornerSrc = cv::Point2f(calibrationGridOrigin.x-gridSizePixel/2,calibrationGridOrigin.y-gridSizePixel/2);
            break;
        default:
            std::cout << "Unrecognized color index "<< i << std::endl;
            break;
        }

        if(((originColorModeFlag == svlCCOriginDetector::RGB) && (i!= svlCCOriginDetector::YELLOW_INDEX))||
           ((originColorModeFlag == svlCCOriginDetector::RGY) && (i!= svlCCOriginDetector::BLUE_INDEX)))
        {
            coordsSrc->data.fl[scale*index] = extrapolatedCornerSrc.x;
            coordsSrc->data.fl[scale*index+1] = extrapolatedCornerSrc.y;
            if(!initial)
            {
                calibrationGridColorBlobs->data.fl[2*index] = extrapolatedCornerSrc.x;
                calibrationGridColorBlobs->data.fl[2*index+1] = extrapolatedCornerSrc.y;
            }
            extrapolatedCornerSrc = extrapolateFromTwoPoints(calibrationGridOrigin.x,calibrationGridOrigin.y,extrapolatedCornerSrc.x,extrapolatedCornerSrc.y);
            coordsSrc->data.fl[scale*index+2] = extrapolatedCornerSrc.x;
            coordsSrc->data.fl[scale*index+3] = extrapolatedCornerSrc.y;

            if(debug)
            {
                std::cout << "Dest blob points: " << coordsDst->data.fl[2*index] << "," << coordsDst->data.fl[2*index+1] << std::endl;
                std::cout << "Src grid points: " << coordsSrc->data.fl[2*index] << "," << coordsSrc->data.fl[2*index+1] << std::endl;
            }
        }
    }

    //non-colored corner
    //if(originColorModeFlag == svlCCOriginDetector::RGB)
    extrapolatedCornerSrc = extrapolateFromTwoPoints(coordsSrc->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+2],coordsSrc->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+3], calibrationGridOrigin.x,calibrationGridOrigin.y);
    //else if(originColorModeFlag == svlCCOriginDetector::RGY)
    //	extrapolatedCornerSrc = extrapolateFromTwoPoints(coordsSrc->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+2],coordsSrc->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+3], calibrationGridOrigin.x,calibrationGridOrigin.y);
    coordsSrc->data.fl[numDetectorColorBlobs*scale] = extrapolatedCornerSrc.x;
    coordsSrc->data.fl[numDetectorColorBlobs*scale+1] = extrapolatedCornerSrc.y;

    if(initial)
    {
        //if(originColorModeFlag == svlCCOriginDetector::RGB)
        extrapolatedCornerDst = extrapolateFromTwoPoints(coordsDst->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+2],coordsDst->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+3], imageOrigin.x,imageOrigin.y);
        //if(originColorModeFlag == svlCCOriginDetector::RGY)
        //	extrapolatedCornerDst = extrapolateFromTwoPoints(coordsDst->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+2],coordsDst->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+3], imageOrigin.x,imageOrigin.y);

        //nearestCorner(extrapolatedCornerDst, corner, distanceFromOriginToRed/2, false);
        if(nearestCorner(extrapolatedCornerDst, corner, distanceFromOriginToRed/2, false) >= distanceFromOriginToRed/2)
        {
            corner->x = extrapolatedCornerDst.x;
            corner->y = extrapolatedCornerDst.y;
        }
        coordsDst->data.fl[numDetectorColorBlobs*scale] = corner->x;
        coordsDst->data.fl[numDetectorColorBlobs*scale+1] = corner->y;

        //origin
        coordsDst->data.fl[numDetectorColorBlobs*scale+2] = imageOrigin.x;
        coordsDst->data.fl[numDetectorColorBlobs*scale+3] = imageOrigin.y;
        coordsSrc->data.fl[numDetectorColorBlobs*scale+2] = calibrationGridOrigin.x;
        coordsSrc->data.fl[numDetectorColorBlobs*scale+3] = calibrationGridOrigin.y;
    }

    //midpoint corners
    for(int i=0;i<colorBlobsFromDetector.size();i++)
    {
        if(((originColorModeFlag == svlCCOriginDetector::RGB) && (i== svlCCOriginDetector::YELLOW_INDEX))||
           ((originColorModeFlag == svlCCOriginDetector::RGY) && (i== svlCCOriginDetector::BLUE_INDEX)))
            continue;
        index = i;
        if((originColorModeFlag == svlCCOriginDetector::RGY) && (i== svlCCOriginDetector::YELLOW_INDEX))
            index = i-1;
        switch(i)
        {
        case (int)svlCCOriginDetector::RED_INDEX:
            extrapolatedCornerSrc = cv::Point2f(coordsSrc->data.fl[numDetectorColorBlobs*scale],coordsSrc->data.fl[numDetectorColorBlobs*scale+1]);
            if(initial)
                extrapolatedCornerDst = cv::Point2f(coordsDst->data.fl[numDetectorColorBlobs*scale],coordsDst->data.fl[numDetectorColorBlobs*scale+1]);
            break;
        case (int)svlCCOriginDetector::GREEN_INDEX:
            //if(originColorModeFlag == svlCCOriginDetector::RGB)
            //{
            extrapolatedCornerSrc = cv::Point2f(coordsSrc->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+2],coordsSrc->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+3]);
            if(initial)
                extrapolatedCornerDst = cv::Point2f(coordsDst->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+2],coordsDst->data.fl[scale*svlCCOriginDetector::BLUE_INDEX+3]);
            //}else if(originColorModeFlag == svlCCOriginDetector::RGY)
            //{
            //	extrapolatedCornerSrc = cv::Point2f(coordsSrc->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+2],coordsSrc->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+3]);
            //	if(initial)
            //		extrapolatedCornerDst = cv::Point2f(coordsDst->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+2],coordsDst->data.fl[scale*svlCCOriginDetector::YELLOW_INDEX+3]);
            //}
            break;
        case (int)svlCCOriginDetector::BLUE_INDEX:
            if(originColorModeFlag == svlCCOriginDetector::RGB)
            {
                extrapolatedCornerSrc = cv::Point2f(coordsSrc->data.fl[scale*svlCCOriginDetector::RED_INDEX+2],coordsSrc->data.fl[scale*svlCCOriginDetector::RED_INDEX+3]);
                if(initial)
                    extrapolatedCornerDst = cv::Point2f(coordsDst->data.fl[scale*svlCCOriginDetector::RED_INDEX+2],coordsDst->data.fl[scale*svlCCOriginDetector::RED_INDEX+3]);
            }
            break;
		case (int)svlCCOriginDetector::YELLOW_INDEX:
            if(originColorModeFlag == svlCCOriginDetector::RGY)
            {
                extrapolatedCornerSrc = cv::Point2f(coordsSrc->data.fl[scale*svlCCOriginDetector::RED_INDEX+2],coordsSrc->data.fl[scale*svlCCOriginDetector::RED_INDEX+3]);
                if(initial)
                    extrapolatedCornerDst = cv::Point2f(coordsDst->data.fl[scale*svlCCOriginDetector::RED_INDEX+2],coordsDst->data.fl[scale*svlCCOriginDetector::RED_INDEX+3]);
            }
            break;
		default:
            //printf("Unrecognized color index %d\n", i);
            break;
        }

        if(((originColorModeFlag == svlCCOriginDetector::RGB) && (i!= svlCCOriginDetector::YELLOW_INDEX))||
           ((originColorModeFlag == svlCCOriginDetector::RGY) && (i!= svlCCOriginDetector::BLUE_INDEX)))
        {
            extrapolatedCornerSrc = midPointBetweenTwoPoints(coordsSrc->data.fl[scale*index+2],coordsSrc->data.fl[scale*index+3],extrapolatedCornerSrc.x,extrapolatedCornerSrc.y);
            coordsSrc->data.fl[(numDetectorColorBlobs+1)*scale+index*2] = extrapolatedCornerSrc.x;
            coordsSrc->data.fl[(numDetectorColorBlobs+1)*scale+index*2+1] = extrapolatedCornerSrc.y;

            if(initial)
            {
                extrapolatedCornerDst = midPointBetweenTwoPoints(coordsDst->data.fl[scale*index+2],coordsDst->data.fl[scale*index+3],extrapolatedCornerDst.x,extrapolatedCornerDst.y);
                if(nearestCorner(extrapolatedCornerDst, corner, distanceFromOriginToRed/2, false) >= distanceFromOriginToRed/2)
                {
                    corner->x = extrapolatedCornerDst.x;
                    corner->y = extrapolatedCornerDst.y;
                }
                coordsDst->data.fl[(numDetectorColorBlobs+1)*scale+index*2] = corner->x;
                coordsDst->data.fl[(numDetectorColorBlobs+1)*scale+index*2+1] = corner->y;
                sizePixel += distanceBetweenTwoPoints(corner->x,corner->y,imageOrigin.x,imageOrigin.y);
            }
        }
    }
    //non-colored corner
    extrapolatedCornerSrc = cv::Point2f(coordsSrc->data.fl[scale*svlCCOriginDetector::GREEN_INDEX+2],coordsSrc->data.fl[scale*svlCCOriginDetector::GREEN_INDEX+3]);
    extrapolatedCornerSrc = midPointBetweenTwoPoints(coordsSrc->data.fl[numDetectorColorBlobs*scale],coordsSrc->data.fl[numDetectorColorBlobs*scale+1],extrapolatedCornerSrc.x,extrapolatedCornerSrc.y);
    coordsSrc->data.fl[(numDetectorColorBlobs+1)*scale+6] = extrapolatedCornerSrc.x;
    coordsSrc->data.fl[(numDetectorColorBlobs+1)*scale+7] = extrapolatedCornerSrc.y;

    if(initial)
    {
        extrapolatedCornerDst = cv::Point2f(coordsDst->data.fl[scale*svlCCOriginDetector::GREEN_INDEX+2],coordsDst->data.fl[scale*svlCCOriginDetector::GREEN_INDEX+3]);
        extrapolatedCornerDst = midPointBetweenTwoPoints(coordsDst->data.fl[numDetectorColorBlobs*scale],coordsDst->data.fl[numDetectorColorBlobs*scale+1],extrapolatedCornerDst.x,extrapolatedCornerDst.y);
        if(nearestCorner(extrapolatedCornerDst, corner, distanceFromOriginToRed/2, false) >= distanceFromOriginToRed/2)
        {
            corner->x = extrapolatedCornerDst.x;
            corner->y = extrapolatedCornerDst.y;
        }
        coordsDst->data.fl[(numDetectorColorBlobs+1)*scale+6] = corner->x;
        coordsDst->data.fl[(numDetectorColorBlobs+1)*scale+7] = corner->y;
        sizePixel += distanceBetweenTwoPoints(corner->x,corner->y,imageOrigin.x,imageOrigin.y);

        gridSizePixel = sizePixel/(numDetectorColorBlobs+1);
    }
}

/**************************************************************************************************
* findInitialCorners()					
*	Find initial corners from color blobs with basic geometry
*
* Input:
*	coordsSrc		CvMat*						- Coordinates of virtual planar chessboard
*	coordsDst		CvMat*						- Coordinates of points on image
*	
* Output:
*	void							
*
*
***********************************************************************************************************/
void svlCCCalibrationGrid::findInitialCorners(CvMat* coordsSrc, CvMat* coordsDst)
{
    calibrationGridColorBlobs = cvCreateMat(3,2, CV_32F);
    imageColorBlobs = cvCreateMat(3,2, CV_32F);

    float distanceFromOriginToRed = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::RED_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::RED_INDEX).y);
    float distanceFromOriginToGreen = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::GREEN_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::GREEN_INDEX).y);
    float distanceFromOriginToBlue = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::BLUE_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::BLUE_INDEX).y);
    float distanceFromOriginToYellow = distanceBetweenTwoPoints(originFromDetector.x,originFromDetector.y,colorBlobsFromDetector.at(svlCCOriginDetector::YELLOW_INDEX).x,colorBlobsFromDetector.at(svlCCOriginDetector::YELLOW_INDEX).y);
    float sizePixel = 0;

    if(originColorModeFlag == svlCCOriginDetector::RGB)
        gridSizePixel = 2*(distanceFromOriginToRed+distanceFromOriginToGreen+distanceFromOriginToBlue)/3;
    else if(originColorModeFlag == svlCCOriginDetector::RGY)
        gridSizePixel = 2*(distanceFromOriginToRed+distanceFromOriginToGreen+distanceFromOriginToYellow)/3;


    //find nearest corner to origin
    cv::Point2f* corner = new cv::Point2f();
    if(nearestCorner(originFromDetector, corner, distanceFromOriginToRed/2, false) >= distanceFromOriginToRed/2)
    {
        corner->x = originFromDetector.x;
        corner->y = originFromDetector.y;
    }
    imageOrigin = cv::Point2f(corner->x,corner->y);
    calibrationGridOrigin = cv::Point2f(iplImage->width/2,iplImage->height/2);

    //initial scale
    findInitialCornerHelper(coordsSrc, coordsDst, true);

    //rescale
    findInitialCornerHelper(coordsSrc, coordsDst, false);

    //if(debug)
    std::cout << "size of grid in pixels: " << gridSizePixel << std::endl;
}

void svlCCCalibrationGrid::homographyCorrelation(double threshold)
{
    // we call create2DChessboardCorners at homographyInlierLevel=1 after finding initial corners
    for(homographyInlierLevel = 1; homographyInlierLevel < std::max(boardSize.width/2,boardSize.height/2); homographyInlierLevel++)
    {
        updateHomography(threshold);
    }
}

/**************************************************************************************************
* correlate()					
*	Given origin with orientation and corners, correlate planar chessboard corners with image corners
*	for calibration
*
* Input:
*	originDetector	svlCCOriginDetector*				- origin detector object
*	cornerDetector	svlCCCornerDetector*				- corner detector object
*	
* Output:
*	void							
*
***********************************************************************************************************/
void svlCCCalibrationGrid::correlate(svlCCOriginDetector* originDetector, svlCCCornerDetector* cornerDetector)
{
    int cornerDetectionFlag = cornerDetector->getCornerDetectionFlag();
    int originDetectionFlag = originDetector->getOriginDetectionFlag();
    originColorModeFlag = originDetector->getOriginColorModeFlag();
    colorBlobsFromDetector = originDetector->getColorBlobs();
    originFromDetector = originDetector->getOrigin();
    double threshold = 15;

    valid = false;
    bool homographyCheck = false;

    //check for orientation detection
    switch(originDetectionFlag)
    {
    case (int)svlCCOriginDetector::COLOR:
        valid = true;
        break;
    default:
        valid = false;
        break;
    }

    if(!valid)
        return;

    //check for corner detection
    switch(cornerDetectionFlag)
    {
    case (int)svlCCCornerDetector::FEATURES:
        valid = true;
        corners = cornerDetector->getChessboardCorners();
        normCorners = cornerDetector->getChessboardCornersNormalized();

        //iterative homography
        homographyCorrelation(threshold);

        //optimize
        optimizeCalibration();

        if(debug)
            std::cout << "Average grid size in pixels " << gridSizePixel << std::endl;
        break;
    default:
        valid = false;
        create2DChessboardCorners(false);
        break;
    }

}

/**************************************************************************************************
* optimizeCalibration()					
*	Optimize calibration by reprojection until no improvement or maximum iteration is reached	
*	
* Output:
*	void							
*
***********************************************************************************************************/
void svlCCCalibrationGrid::optimizeCalibration()
{
    double prevRMS = std::numeric_limits<double>::max( );
    double rootMeanSquaredThreshold = 1;
    double rms;
    int maxCalibrationIteration = 5;
    int iteration = 0;
    cv::Mat pPrevCameraMatrix;
    cv::Mat pPrevDistCoeffs;
    cv::Mat pPrevRvec;
    cv::Mat pPrevTvec;
    cv::Mat prevCameraMatrix;
    cv::Mat prevDistCoeffs;
    cv::Mat prevRvec;
    cv::Mat prevTvec;
    int prevThreshold, pPrevThreshold;
    int numberGoodPointsBeforeOpt, numberGoodPointsAfterOpt;
    initialImagePoints = getGoodImagePoints();
    initialCalibrationGridPoints3D = getGoodCalibrationGridPoints3D();
    numberGoodPointsBeforeOpt = initialImagePoints.size();


    //if(max(imageSize.height,imageSize.width) > 1000)
    //	refineThreshold = 4;

    //refine
    prevRMS = runCalibration();
    prevCameraMatrix = cameraMatrix;
    prevDistCoeffs = distCoeffs;
    prevRvec = rvec;
    prevTvec = tvec;
    prevThreshold = refineThreshold;
    pPrevCameraMatrix = prevCameraMatrix;
    pPrevDistCoeffs = prevDistCoeffs;
    pPrevRvec = prevRvec;
    pPrevTvec = prevTvec;
    pPrevThreshold = prevThreshold;

    // check for bad calibration
    if(prevRMS == std::numeric_limits<double>::max( ))
        return;

    rms = refine(rvec, tvec, cameraMatrix, distCoeffs, refineThreshold, false,true);

    while((rms < std::numeric_limits<double>::max( )) && (rms > rootMeanSquaredThreshold)&& (iteration < maxCalibrationIteration))
    {
        // Lower threshold for higher iteration of optimization
        //if(iteration > 1)
        //	refineThreshold = 2;

        if(rms < prevRMS)
        {
            pPrevCameraMatrix = prevCameraMatrix;
            pPrevDistCoeffs = prevDistCoeffs;
            pPrevRvec = prevRvec;
            pPrevTvec = prevTvec;
            pPrevThreshold = prevThreshold;
            prevCameraMatrix = cameraMatrix;
            prevDistCoeffs = distCoeffs;
            prevRvec = rvec;
            prevTvec = tvec;
            prevRMS = rms;
            prevThreshold = refineThreshold;
            rms = refine(rvec, tvec, cameraMatrix, distCoeffs, refineThreshold, false,true);
        }else
        {
            break;
        }
        iteration++;
    }

    rms = refine(pPrevRvec, pPrevTvec, pPrevCameraMatrix, pPrevDistCoeffs,pPrevThreshold,false,true);
    this->cameraMatrix = pPrevCameraMatrix;
    this->distCoeffs = pPrevDistCoeffs;
    this->rvec = pPrevRvec;
    this->tvec = pPrevTvec;
    this->refineThreshold = pPrevThreshold;
    if(debug)
        printCalibrationParameters();

    numberGoodPointsAfterOpt = getGoodImagePoints().size();

    //if(debug)
    std::cout << "Count before refinement: " <<numberGoodPointsBeforeOpt<< " after: " <<numberGoodPointsAfterOpt <<" valid: " << valid <<std::endl;
}

/**************************************************************************************************
* refine()					
*	Refine the number of valid corners used in calibration by reprojection with given camera
*	parameters
*	
* Input:
*	localRvec			const cv::Mat&					- Vector representation of rotation
*	localTvec			const cv::Mat&					- Vector representation of translation
*	localCameraMatrix	const cv::Mat&					- Matrix representation of camera intrinsics
*	localTvec			const cv::Mat&					- Matrix representation of camera distortion coefficients
*	threshold			double							- Threshold to consider within range for nearest corner
*	runHomography		const cv::Mat&					- Indicator whether or not to update homography
*	checkNormalized		const cv::Mat&					- Indicator whether or not to check normalized corners
*	
* Output:
*	void							
*
***********************************************************************************************************/
double svlCCCalibrationGrid::refine(const cv::Mat& localRvec, const cv::Mat& localTvec, const cv::Mat& localCameraMatrix, const cv::Mat& localDistCoeffs, float threshold, bool runHomography, bool checkNormalized)
{
    projectedImagePoints.clear();
    projectPoints(cv::Mat(getAllCalibrationGridPoints3D()), localRvec, localTvec, localCameraMatrix, localDistCoeffs, projectedImagePoints);
    cv::Point2f* corner = new cv::Point2f();
    cv::Point2f foundCorner;
    float distance;
    int index;

    if(debug)
        std::cout << "Number of good points before refinement: " << getGoodImagePoints().size() << std::endl;

    for( int j = 0; j < (boardSize.height); j++ )
        for( int i = 0; i < (boardSize.width); i++ )
        {
        index = j*boardSize.width+i;
        if(nearestCorner(projectedImagePoints.at(index),corner,threshold, checkNormalized && threshold<=2 && std::max(iplImage->height,iplImage->width)>1000) < threshold)
        {
            foundCorner = cv::Point2f(corner->x,corner->y);
            imagePoints[i][j] = foundCorner;
            visibility[i][j] = true;
        }else{
            visibility[i][j] = false;
        }
    }

    if(runHomography)
        updateHomography(threshold);

    if(debug)
        std::cout << "Number of good points after refinement: " << getGoodImagePoints().size() << std::endl;

    return runCalibration();
}

/**************************************************************************************************
* applyHomography()					
*	Apply the given homography to update correlated corners
*
* Input:
*	homography			double[]					- The given Homography
*	threshold			float						- Threshold to consider within range for nearest corner
*	
* Output:
*	void							
*
***********************************************************************************************************/
int svlCCCalibrationGrid::applyHomography(double homography[], float threshold)
{
    float x,y;
    float Z;
    float X;
    float Y;
    cv::Point2f point;
    cv::Point2f* corner = new cv::Point2f();
    cv::Point2f foundCorner;

    int iStart = -1*std::min(homographyInlierLevel+1,boardSize.width/2);
    int iEnd = std::min(homographyInlierLevel+1,boardSize.width/2-1);
    int jStart = -1*std::min(homographyInlierLevel+1,boardSize.height/2);
    int jEnd = std::min(homographyInlierLevel+1,boardSize.height/2-1);

    int iStartPrevious = -1*std::min(homographyInlierLevel,boardSize.width/2);
    int iEndPrevious = std::min(homographyInlierLevel,boardSize.width/2-1);
    int jStartPrevious = -1*std::min(homographyInlierLevel,boardSize.height/2);
    int jEndPrevious = std::min(homographyInlierLevel,boardSize.height/2-1);

    int indexX, indexY;

    int countWithinThreshold, adjustment, cornerDistance, pointsCount;
    countWithinThreshold = adjustment = cornerDistance = pointsCount = 0;

    for(int i=iStart;i<iEnd+1;i++)
        for(int j=jStart;j<jEnd+1;j++)
        {
        indexX = boardSize.width/2+i;
        indexY = boardSize.height/2+j;
        point = (cv::Point2f)calibrationGridPoints[indexX][indexY];
        x = point.x;
        y = point.y;
        Z = 1./(homography[6]*x + homography[7]*y + homography[8]);
        X = (homography[0]*x + homography[1]*y + homography[2])*Z;
        Y = (homography[3]*x + homography[4]*y + homography[5])*Z;
        imagePoints[indexX][indexY] = cv::Point2f(X,Y);

        cornerDistance = nearestCorner(cv::Point2f(X,Y), corner,threshold+adjustment, false);
        if(cornerDistance < threshold + adjustment || (isHighDefinition() && homographyInlierLevel > 1 && adjustment == 0 && cornerDistance < gridSizePixel/2))
        {
            imagePoints[indexX][indexY] = cv::Point2f(corner->x,corner->y);
            if(!visibility[indexX][indexY])
                visibility[indexX][indexY] = true;
            countWithinThreshold++;
        }

        //For HD adjust threshold for leniency
        if(isHighDefinition() && homographyInlierLevel > 1 && cornerDistance > threshold + adjustment && cornerDistance < gridSizePixel/2)
        {
            pointsCount++;
            adjustment += cornerDistance-(threshold + adjustment);
            adjustment/pointsCount;
        }
    }

    if(adjustment > 0 && debug)
        std::cout << "Homography Level: " << homographyInlierLevel << " threshold: " << threshold+adjustment << " adjustment " << adjustment << std::endl;

    return countWithinThreshold;
}

/**************************************************************************************************
* updateHomography()					
*	For the current homographyInlierLevel update the points used for homography calculation
*
* Input:
*	threshold			float						- Threshold to consider within range for nearest corner
*	
* Output:
*	void							
*
***********************************************************************************************************/
bool svlCCCalibrationGrid::updateHomography(float threshold)
{

    double homography[9];
    int numMatches;
    CvMat _homography = cvMat(3, 3, CV_64F, homography);
    CvMat* coordsSrc;
    CvMat* coordsDst;
    int count = 0;
    int iStart = -1*std::min(homographyInlierLevel+1,boardSize.width/2);
    int iEnd = std::min(homographyInlierLevel+1,boardSize.width/2-1);
    int jStart = -1*std::min(homographyInlierLevel+1,boardSize.height/2);
    int jEnd = std::min(homographyInlierLevel+1,boardSize.height/2-1);
    int indexX, indexY;

    if(homographyInlierLevel == 1)
    {
        numMatches = 12; //color blobs + origin + corners
        coordsSrc = cvCreateMat(numMatches,2, CV_32F);
        coordsDst = cvCreateMat(numMatches,2, CV_32F);
        findInitialCorners(coordsSrc, coordsDst);
        create2DChessboardCorners(false);
        for(int i=iStart+1;i<iEnd;i++)
        {
            for(int j=jStart+1;j<jEnd;j++)
            {
                indexX = boardSize.width/2+i;
                indexY = boardSize.height/2+j;
                visibility[indexX][indexY] = true;
            }
        }
    }else
    {
        numMatches = (iEnd-iStart+1)*(jEnd-jStart+1)+imageColorBlobs->rows;
        coordsSrc = cvCreateMat(numMatches,2, CV_32F);
        coordsDst = cvCreateMat(numMatches,2, CV_32F);

        int scale = 2;

        for(int i=iStart;i<iEnd+1;i++)
        {
            for(int j=jStart;j<jEnd+1;j++)
            {
                indexX = boardSize.width/2+i;
                indexY = boardSize.height/2+j;

                //must add dummy points even if invisible since numMatch already calculated
                if(!visibility[indexX][indexY])
                {
                    indexX = boardSize.width/2;
                    indexY = boardSize.height/2;
                }
                coordsSrc->data.fl[scale*count] = calibrationGridPoints[indexX][indexY].x;
                coordsSrc->data.fl[scale*count+1] = calibrationGridPoints[indexX][indexY].y;
                coordsDst->data.fl[scale*count] = imagePoints[indexX][indexY].x;
                coordsDst->data.fl[scale*count+1] = imagePoints[indexX][indexY].y;
                count++;
            }
        }

        for(int i=0;i<imageColorBlobs->rows;i++)
        {
            coordsSrc->data.fl[scale*count] = calibrationGridColorBlobs->data.fl[scale*i];
            coordsSrc->data.fl[scale*count+1] = calibrationGridColorBlobs->data.fl[scale*i+1];
            coordsDst->data.fl[scale*count] = imageColorBlobs->data.fl[scale*i];
            coordsDst->data.fl[scale*count+1] = imageColorBlobs->data.fl[scale*i+1];
            count++;
        }

    }

    // homography matrix
    if(cvFindHomography(coordsSrc, coordsDst, &_homography))//, 0, 2.5, 0);
    {
        if(debug)
            std::cout << "Homography success! " << std::endl;
    }
    else
    {
        std::cout << "Homography failed! "<< std::endl;
        return false;
    }

    int countWithinThreshold = applyHomography(homography,threshold);

    if(debug && homographyInlierLevel >= std::max(boardSize.width/2,boardSize.height/2)-1)
    {
        char s[20];
        char t[20];
        CvFont font;
        cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 1.0, 0, 1, CV_AA);
        for( int i = 0; i < (boardSize.width); i++ )
            for( int j = 0; j < (boardSize.height); j++ )
            {
            //sprintf(s, "%d", i*boardSize.width+j) ;
            if(visibility[i][j]){
                //sprintf(t, "%d", j) ;
                //cvPutText(iplImage,s,cv::Point2f(imagePoints[i][j].x,imagePoints[i][j].y),&font,cvScalar(255,100,100,255));
                //cvPutText(iplImage,t,cv::Point2f(imagePoints[i][j].x+1,imagePoints[i][j].y),&font,cvScalar(255,100,100,255));
                cvCircle( iplImage, imagePoints[i][j], 5, cvScalar(255,100,100,255), 1, 8, 0 );
            }else
            {
                //cvPutText(iplImage,s,cv::Point2f(imagePoints[i][j].x,imagePoints[i][j].y),&font,cvScalar(255,200,100,255));
            }
        }
    }

    return countWithinThreshold >= (numMatches-homographyInlierLevel);
}



std::vector<cv::Point2f> svlCCCalibrationGrid::getGoodImagePoints()
{
    std::vector<cv::Point2f> goodImagePoints;
    for( int j = 0; j < (boardSize.height); j++ )
        for( int i = 0; i < (boardSize.width); i++ )
        {
        if(visibility[i][j]){
            goodImagePoints.push_back(imagePoints[i][j]);
            if(debug)
            {
                //cout << "getGoodImagePoints: at " << i << "," << j << ": (" << imagePoints[i][j].x << ",";
                //cout << imagePoints[i][j].y << ")" << endl;
                cvCircle( iplImage, imagePoints[i][j], 5, cvScalar(255,0,255,0), 1, 8, 0 );
            }
        }
    }
    //if(valid)
    valid = goodImagePoints.size() >= minGridPoints;
    return goodImagePoints;
}

std::vector<cv::Point2f> svlCCCalibrationGrid::getAllImagePoints()
{
    std::vector<cv::Point2f> allImagePoints;
    for( int j = 0; j < (boardSize.height); j++ )
    {
        for( int i = 0; i < (boardSize.width); i++ )
        {
            allImagePoints.push_back(imagePoints[i][j]);
        }
    }
    return allImagePoints;
}

std::vector<cv::Point3f> svlCCCalibrationGrid::getGoodCalibrationGridPoints3D()
{
    std::vector<cv::Point3f> goodCalibrationGridPoints;
    cv::Point2f point;
    for( int j = 0; j < (boardSize.height); j++ )
        for( int i = 0; i < (boardSize.width); i++ )
        {
        if(visibility[i][j])
        {
            point =  cv::Point2f((i-boardSize.width/2)*gridSize,(j-boardSize.height/2)*gridSize);
            //goodCalibrationGridPoints.push_back(cv::Point3f(point.x,point.y,0));
            goodCalibrationGridPoints.push_back(cv::Point3f((i-boardSize.width/2)*gridSize,(j-boardSize.height/2)*gridSize,0));
            if(debug)
            {
                std::cout << "getGoodCalibrationGridPoints3D: at " << i << "," << j << " (" << point.x<< ",";
                std::cout << point.y << ")" << " : (" << imagePoints[i][j].x << ",";
                std::cout << imagePoints[i][j].y << ")" << std::endl;
                //cvCircle( iplImage,imagePoints[i][j], 5, cvScalar(0,128,255), 1, 8, 0 );
            }
        }else{
            //if(debug)
            //cvCircle( iplImage, imagePoints[i][j], 5, colors[2], 1, 8, 0 );
        }
    }
    //if(valid)
    valid = goodCalibrationGridPoints.size() >= minGridPoints;
    return goodCalibrationGridPoints;
}

std::vector<cv::Point3f> svlCCCalibrationGrid::getAllCalibrationGridPoints3D()
{
    std::vector<cv::Point3f> allCalibrationGridPoints;
    cv::Point2f point;
    for( int j = 0; j < (boardSize.height); j++ )
        for( int i = 0; i < (boardSize.width); i++ )
        {
        allCalibrationGridPoints.push_back(cv::Point3f((i-boardSize.width/2)*gridSize,(j-boardSize.height/2)*gridSize,0));
    }

    return allCalibrationGridPoints;
}

std::vector<cv::Point2f> svlCCCalibrationGrid::getGoodProjectedImagePoints()
{
    int index;
    std::vector<cv::Point2f> goodProjectedImagePoints;
    if(projectedImagePoints.size() > 0)
    {
        for( int j = 0; j < (boardSize.height); j++ )
            for( int i = 0; i < (boardSize.width); i++ )
            {
            if(visibility[i][j])
            {
                index = j*boardSize.width+i;
                if(projectedImagePoints.size() > index)
                    goodProjectedImagePoints.push_back(projectedImagePoints.at(index));
            }
        }
    }
    return goodProjectedImagePoints;
}

/**************************************************************************************************
* runCalibration()					
*	For the current image and grid points calibrate
*	
* Output:
*	void							
*
***********************************************************************************************************/
double svlCCCalibrationGrid::runCalibration()
{
    cv::Mat localCameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    cv::Mat localDistCoeffs = cv::Mat::zeros(5, 1, CV_64F);
    std::vector<cv::Mat> rvecs, tvecs;
    //option to fix k_3 as zero for standard fov, Zhang zeros last three
    int flags = 0;//cv::CALIB_FIX_K3|cv::CALIB_ZERO_TANGENT_DIST;
    std::vector<std::vector<cv::Point3f> > objectPoints;
    std::vector<std::vector<cv::Point2f> > imagePoints;
    std::vector<std::vector<cv::Point2f> > localProjectedImagePoints;
    double rms = std::numeric_limits<double>::max( );
    double projectedImagePointsRMS = std::numeric_limits<double>::max( );

    objectPoints.push_back(getGoodCalibrationGridPoints3D());
    imagePoints.push_back(getGoodImagePoints());
    localProjectedImagePoints.push_back(getGoodProjectedImagePoints());

    if(valid)
    {
        if(imagePoints.size() > 0)
        {
            rms = calibrateCamera(objectPoints, imagePoints, cv::Size(iplImage->width,iplImage->height), localCameraMatrix,
                                  localDistCoeffs, rvecs, tvecs, flags);
            if(localProjectedImagePoints.at(0).size() > 0)
                projectedImagePointsRMS = calibrateCamera(objectPoints, localProjectedImagePoints, cv::Size(iplImage->width,iplImage->height), localCameraMatrix,
                                                          localDistCoeffs, rvecs, tvecs, flags);

            if(debug)
                std::cout << "grid::calibrateCamera:" << imagePoints.at(0).size() << " points, RMS:"<< rms << ", improved:" << projectedImagePointsRMS << std::endl;

            if(rms < calibrationError)
            {
                this->cameraMatrix = localCameraMatrix;
                this->distCoeffs = localDistCoeffs;
                //OpenCV returns k1,k2,k3 tangential, p1,p2 radial
                //currently rectifier doesn't handle radial
                //(k_1, k_2, p_1, p_2[, k_3])
                this->rvec = rvecs[0];
                this->tvec = tvecs[0];
                cv::Rodrigues(this->rvec,this->rmatrix);
                if(debug)
                    printCalibrationParameters();
            }
        }
    }

    return rms;
}

svlSampleCameraGeometry* svlCCCalibrationGrid::GetCameraGeometry()
{
    svlSampleCameraGeometry* cameraGeometry = new svlSampleCameraGeometry();
    double alpha = 0.0;//assumed to be square pixels
    vct2 f;
    vct2 c;
    vctFixedSizeVector<double,7> k;
    f = vct2(cameraMatrix.at<double>(0,0),cameraMatrix.at<double>(1,1));
    c = vct2(cameraMatrix.at<double>(0,2),cameraMatrix.at<double>(1,2));
    //OpenCV returns k1,k2,k3 raidal, p1,p2 tangential
    //rectifier (i.e matlab) k1,k2,k5 = radial, k3,k4=tangential
    //(k_1, k_2, p_1, p_2[, k_3])
    //option to fix k_3 as zero for standard fov, Zhang zeros last three
    //-0.380948, 0.149205, 0.000035,-0.00087753,0.0, 0.0, 0.0);
    //if(fabs(distCoeffs.at<double>(0,0)) > 0.39 || fabs(distCoeffs.at<double>(1,0)) > 0.15 || fabs(distCoeffs.at<double>(3,0)) > 0.00087753)
    //    k = vctFixedSizeVector<double,7>(-0.380948, 0.149205, distCoeffs.at<double>(2,0), distCoeffs.at<double>(3,0) ,0.0, 0.0, 0.0);
    //else
    if(fabs(distCoeffs.at<double>(4,0)) > 0.1)
    {
        //use reduced model
        k = vctFixedSizeVector<double,7>(distCoeffs.at<double>(0,0),distCoeffs.at<double>(1,0),distCoeffs.at<double>(2,0),distCoeffs.at<double>(3,0),0,0.0,0.0);
    }else
    {
        k = vctFixedSizeVector<double,7>(distCoeffs.at<double>(0,0),distCoeffs.at<double>(1,0),distCoeffs.at<double>(2,0),distCoeffs.at<double>(3,0),distCoeffs.at<double>(4,0),0.0,0.0);
    }    cameraGeometry->SetIntrinsics(f,c,alpha,k);
    return cameraGeometry;
}

void svlCCCalibrationGrid::printCalibrationParameters()
{
    for(int i=0;i<cameraMatrix.rows;i++)
    {
        for(int j=0;j<cameraMatrix.cols;j++)
        {
            std::cout << "Camera matrix: " << cameraMatrix.at<double>(i,j) << std::endl;
        }
    }

    for(int i=0;i<distCoeffs.rows;i++)
    {
        for(int j=0;j<distCoeffs.cols;j++)
        {
            std::cout << "Distortion _coefficients: " << distCoeffs.at<double>(i,j) << std::endl;
        }
    }


    std::cout << "rvect: " << rvec.at<double>(0,0) <<","<< rvec.at<double>(0,1) <<","<< rvec.at<double>(0,2) <<","<< std::endl;
    std::cout << "tvect: " << tvec.at<double>(0,0) <<","<< tvec.at<double>(0,1) <<","<< tvec.at<double>(0,2) <<","<< std::endl;
}

void svlCCCalibrationGrid::PrintOriginIndicators(const char * filename)
{
    std::ofstream outputStream(filename, std::ofstream::binary);
    cmnSerializer serialization(outputStream);
    outputStream << "originIndicators";
    std::cout << "Writing origins to: " << filename <<std::endl;
    for(unsigned int i=0;i<colorBlobsFromDetector.size();i++)
    {
        if(colorBlobsFromDetector.at(i).x > 0 && colorBlobsFromDetector.at(i).y > 0)
            outputStream << ","<< colorBlobsFromDetector.at(i).x << "," << colorBlobsFromDetector.at(i).y;
    }
    outputStream.close();
}

void svlCCCalibrationGrid::PrintGoodCalibrationPoints(const char * filename)
{

/*    CIP
#World to TCP
    -145.873      1.69265     -92.3186     -237.353      1008.13      668.515
#
# LEFT CAMERA
#
# WIDTH  HEIGHT
    780         580
#
# Number of Points
    217
# GRID Coordinates            PIXEL Coordinates
# x    y    z                  x          y
    -199.845     -179.550      0.00000      756.584      540.691*/

    std::vector<cv::Point2f> points2d = getGoodImagePoints();
    std::vector<cv::Point3f> points3d = getGoodCalibrationGridPoints3D();

    if(points2d.size() != points3d.size())
    {
        std::cout << "WARNING point count in 2D != 3D"<<std::endl;
    }else if(points2d.size() > minGridPoints)
    {
        std::ofstream outputStream(filename, std::ofstream::binary);
        cmnSerializer serialization(outputStream);
        outputStream << "CIP\n";
        outputStream << "# World to TCP\n";
        outputStream << "      " << rvec.at<double>(0,0) <<"     "<< rvec.at<double>(0,1) << "     "<< rvec.at<double>(0,2);
        outputStream << "      " << tvec.at<double>(0,0) <<"     "<< tvec.at<double>(0,1) << "     "<< tvec.at<double>(0,2) <<"\n";
        outputStream << "#\n";
        outputStream << "# LEFT CAMERA\n";
        outputStream << "#\n";
        outputStream << "# WIDTH  HEIGHT\n";
        outputStream << "  " << 1920 << "         " << 1080 << "\n";
        outputStream << "#\n";
        outputStream << "# Number of Points\n";
        outputStream << "  " << points2d.size() << "\n";
        outputStream << "# GRID Coordinates            PIXEL Coordinates\n";
        outputStream << "# x    y    z                  x          y\n";
        for(unsigned int i=0; i<points2d.size(); i++)
        {
            outputStream << "      " << points3d.at(i).x << "     " << points3d.at(i).y << "     " << points3d.at(i).z << "      ";
            outputStream << "      " << points2d.at(i).x << "     " << points2d.at(i).y << "     " << "\n";
        }
        outputStream <<"\n";
        // close the stream
        outputStream.close();
    }
}

void svlCCCalibrationGrid::PrintInitialCalibrationPoints(const char * filename)
{

/*    CIP
#World to TCP
    -145.873      1.69265     -92.3186     -237.353      1008.13      668.515
#
# LEFT CAMERA
#
# WIDTH  HEIGHT
    780         580
#
# Number of Points
    217
# GRID Coordinates            PIXEL Coordinates
# x    y    z                  x          y
    -199.845     -179.550      0.00000      756.584      540.691*/

    std::vector<cv::Point2f> points2d = initialImagePoints;
    std::vector<cv::Point3f> points3d = initialCalibrationGridPoints3D;

    if(points2d.size() != points3d.size())
    {
        std::cout << "WARNING point count in 2D != 3D"<<std::endl;
    }else if(points2d.size() > minGridPoints)
    {
        std::ofstream outputStream(filename, std::ofstream::binary);
        cmnSerializer serialization(outputStream);
        outputStream << "CIP\n";
        outputStream << "# World to TCP\n";
        outputStream << "      " << rvec.at<double>(0,0) <<"     "<< rvec.at<double>(0,1) << "     "<< rvec.at<double>(0,2);
        outputStream << "      " << tvec.at<double>(0,0) <<"     "<< tvec.at<double>(0,1) << "     "<< tvec.at<double>(0,2) <<"\n";
        outputStream << "#\n";
        outputStream << "# LEFT CAMERA\n";
        outputStream << "#\n";
        outputStream << "# WIDTH  HEIGHT\n";
        outputStream << "  " << 1920 << "         " << 1080 << "\n";
        outputStream << "#\n";
        outputStream << "# Number of Points\n";
        outputStream << "  " << points2d.size() << "\n";
        outputStream << "# GRID Coordinates            PIXEL Coordinates\n";
        outputStream << "# x    y    z                  x          y\n";
        for(unsigned int i=0; i<points2d.size(); i++)
        {
            outputStream << "      " << points3d.at(i).x << "     " << points3d.at(i).y << "     " << points3d.at(i).z << "      ";
            outputStream << "      " << points2d.at(i).x << "     " << points2d.at(i).y << "     " << "\n";
        }
        outputStream <<"\n";
        // close the stream
        outputStream.close();
    }
}


