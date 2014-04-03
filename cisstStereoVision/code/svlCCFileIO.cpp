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

// include what is needed from cisst

// system includes
#include <iostream>

#include <cisstStereoVision/svlCCFileIO.h>

//================== Subclass of svlCCFileIO for the points file (DRL .pts) type ==================//
svlCCPointsFileIO::svlCCPointsFileIO(const char* filename, int fileFormat)
    :svlCCFileIO(filename)
{
    this->fileFormat = fileFormat;
    worldToTCP = cvCreateMat(4,4,CV_64F);

    switch(fileFormat)
    {
    case ORIGINAL:
        sections[0] = new SectionFormat(2,"######\n");
        sections[1] = new SectionFormat(7,"##\n");
        sections[2] = new SectionFormat(10,"#\n");
        sections[3] = new SectionFormat(END,"#,#,#\n");
        break;
    case IMPROVED:
        sections[0] = new SectionFormat(3,"######\n");
        sections[1] = new SectionFormat(6,"##\n");
        sections[2] = new SectionFormat(9,"#\n");
        sections[3] = new SectionFormat(END,"#,#,#\n");
        break;
    default:
        printf("Unknown poitns file format: %d\n", fileFormat);
        break;
    }

}

// repack data
void svlCCPointsFileIO::repackData(IplImage* iplImage)
{
    int offset, worldToTCPOffset;
    static CvScalar colors[] =
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}},
        {{255,255,255}}
    };

    switch(fileFormat)
    {
    case ORIGINAL:
        pointsCount = (int)data[10][0];
        cv::Size((int)data[7][0],(int)data[7][1]);
        worldToTCPOffset = 2;
        offset = 13;
        break;
		case IMPROVED:
        pointsCount = (int)data[9][0];
        imageSize = cv::Size((int)data[6][0],(int)data[6][1]);
        worldToTCPOffset = 3;
        offset = 12;
        break;
		default:
        printf("Unknown points file format: %d\n", fileFormat);
        return;
    }

    //World to TCP
    float worldToTCPRVector[1][3] = { {data[worldToTCPOffset][0],data[worldToTCPOffset][1],data[worldToTCPOffset][2]} };
    float worldToTCPTVector[1][3] = { {data[worldToTCPOffset][3],data[worldToTCPOffset][4],data[worldToTCPOffset][5]} };
    cv::Mat rvect(1,3,CV_64F,worldToTCPRVector);
    cv::Mat rmatrix;
    cv::Rodrigues(rvect,rmatrix);
    float dataMatrix[4][4] = {{(float)rmatrix.at<double>(0,0),(float)rmatrix.at<double>(0,1),(float)rmatrix.at<double>(0,2),worldToTCPTVector[0][0]},
                              {(float)rmatrix.at<double>(1,0),(float)rmatrix.at<double>(1,1),(float)rmatrix.at<double>(1,2),worldToTCPTVector[0][1]},
                              {(float)rmatrix.at<double>(2,0),(float)rmatrix.at<double>(2,1),(float)rmatrix.at<double>(2,2),worldToTCPTVector[0][2]},
                              {0,0,0,1}};
    worldToTCP = cvCreateMat(4,4,CV_64F);
    worldToTCP->data.fl[0] = dataMatrix[0][0];
    worldToTCP->data.fl[1] = dataMatrix[0][1];
    worldToTCP->data.fl[2] = dataMatrix[0][2];
    worldToTCP->data.fl[3] = dataMatrix[0][3];
    worldToTCP->data.fl[4] = dataMatrix[1][0];
    worldToTCP->data.fl[5] = dataMatrix[1][1];
    worldToTCP->data.fl[6] = dataMatrix[1][2];
    worldToTCP->data.fl[7] = dataMatrix[1][3];
    worldToTCP->data.fl[8] = dataMatrix[2][0];
    worldToTCP->data.fl[9] = dataMatrix[2][1];
    worldToTCP->data.fl[10] = dataMatrix[2][2];
    worldToTCP->data.fl[11] = dataMatrix[2][3];
    worldToTCP->data.fl[12] = dataMatrix[3][0];
    worldToTCP->data.fl[13] = dataMatrix[3][1];
    worldToTCP->data.fl[14] = dataMatrix[3][2];
    worldToTCP->data.fl[15] = dataMatrix[3][3];

    //imagePoints;
    //calibrationGridPoints;
    int index;
    for(int i=0;i<(int)pointsCount;i++)
    {
        index = i + offset;
        //calibrationGridPoints.push_back(cv::Point3f(imageSize.width/2+data[index][0]/20*30,imageSize.height/2+data[index][1]/20*30,data[index][2]));
        calibrationGridPoints.push_back(cv::Point3f(data[index][0],data[index][1],data[index][2]));
        imagePoints.push_back(cv::Point2f(data[index][3],data[index][4]));

        if(debug)
        {
            //cvCircle( iplImage, cv::Point2f(imageSize.width/2+data[index][0]/20*30,imageSize.height/2+data[index][1]/20*30), 5, colors[1], 1, 8, 0 );
            cvCircle( iplImage, cv::Point2f(data[index][0],data[index][1]), 5, colors[1], 1, 8, 0 );
            cvCircle( iplImage, cv::Point2f(data[index][3],data[index][4]), 5, colors[6], 1, 8, 0 );
            std::cout << "repackData at point: " << i << " (" << data[index][0] << ",";
            std::cout << data[index][1] << ")" << " : (" << data[index][3] << ",";
            std::cout << data[index][4] << ")" << std::endl;
        }

    }
    if(debug)
        std::cout << "read " << pointsCount << " points" <<std::endl;

    // Free memory
    rvect.~Mat();
    rmatrix.~Mat();
}

// repack data
void svlCCPointsFileIO::repackData()
{
    //see overloaded repackData(IplImage* iplImage);
}

void svlCCPointsFileIO::showData()
{
    if(debug)
    {
        std::cout << "pointsCount = " << pointsCount << std::endl;
        std::cout << "Image size: " << imageSize.width << "," << imageSize.height << std::endl;
    }
}


//================== Subclass of svlCCFileIO for the DLR cameraCalibration.m type ==================//
svlCCDLRCalibrationFileIO::svlCCDLRCalibrationFileIO(const char* filename)
    :svlCCFileIO(filename)
{

    sections[0] = new SectionFormat(5,"[##\n");
    sections[1] = new SectionFormat(8,"[##\n");
    sections[2] = new SectionFormat(11,"[#\n");
    sections[3] = new SectionFormat(14,"[###\n");
    sections[4] = new SectionFormat(15,"[###\n");
    sections[5] = new SectionFormat(16,"[###\n");
    sections[6] = new SectionFormat(20,"####\n");
    sections[7] = new SectionFormat(END,"#,#,#\n");

}

void svlCCDLRCalibrationFileIO::repackData(int numImages)
{
    focalLength = cv::Point2f(data[5][0],data[5][1]);
    principalPoint = cv::Point2f(data[8][0],data[8][1]);
    alpha = data[11][0];
    distCoeffs = (cv::Mat_<double>(5,1) << data[14][0],data[14][1],data[14][2], 0.0,0.0);
    tcpToCamera = (cv::Mat_<double>(3,3) << data[20][0], data[20][1],data[20][2], data[20][3],data[21][0], data[21][1],data[21][2], data[21][3],data[22][0], data[22][1],data[22][2], data[22][3]);
    cameraMatrix = (cv::Mat_<double>(3,3) << data[5][0], 0.0, data[8][0],0.0, data[5][1],data[8][1],0.0,0.0,1);

    this->numImages = numImages;

    //cameraMatrices;
    std::cout<<"focal length: " << focalLength.x<<","<<focalLength.y << std::endl;
    std::cout<<"pricipal point: " << principalPoint.x<<","<<principalPoint.y << std::endl;
    std::cout<<"alpha " << alpha << std::endl;
    std::cout<<"distortion coeffs " << distCoeffs.at<double>(0,0) <<","<< distCoeffs.at<double>(1,0) <<","<< distCoeffs.at<double>(2,0) << std::endl;
    std::cout<<"data 15: " << data[15][0] <<","<< data[15][1] <<","<< data[15][2] << std::endl;
    std::cout<<"data 16: " << data[16][0] <<","<< data[16][1] <<","<< data[16][2] << std::endl;

    std::cout<<"tcpToCamera1 " << tcpToCamera.at<double>(0,0) <<","<< tcpToCamera.at<double>(0,1) <<","<< tcpToCamera.at<double>(0,2) <<","<< tcpToCamera.at<double>(0,3)<< std::endl;
    std::cout<<"tcpToCamera1 " << tcpToCamera.at<double>(1,0) <<","<< tcpToCamera.at<double>(1,1) <<","<< tcpToCamera.at<double>(1,2) <<","<< tcpToCamera.at<double>(1,3)<< std::endl;
    std::cout<<"tcpToCamera1 " << tcpToCamera.at<double>(2,0) <<","<< tcpToCamera.at<double>(2,1) <<","<< tcpToCamera.at<double>(2,2) <<","<< tcpToCamera.at<double>(2,3)<< std::endl;

    int index = 28;
    for(int i=0;i<(int)numImages;i++)
    {
        float cameraParameters[3][4] = {{data[index][0], data[index][1],data[index][2], data[index][3]},
                                        {data[index+1][0], data[index+1][1],data[index+1][2], data[index+1][3]},
                                        {data[index+2][0], data[index+2][1],data[index+2][2], data[index+2][3]}};

        CvMat* myCameraMatrix = cvCreateMat(3,4,CV_64F);
        myCameraMatrix->data.fl[0] = cameraParameters[0][0];
        myCameraMatrix->data.fl[1] = cameraParameters[0][1];
        myCameraMatrix->data.fl[2] = cameraParameters[0][2];
        myCameraMatrix->data.fl[3] = cameraParameters[0][3];
        myCameraMatrix->data.fl[4] = cameraParameters[1][0];
        myCameraMatrix->data.fl[5] = cameraParameters[1][1];
        myCameraMatrix->data.fl[6] = cameraParameters[1][2];
        myCameraMatrix->data.fl[7] = cameraParameters[1][3];
        myCameraMatrix->data.fl[8] = cameraParameters[2][0];
        myCameraMatrix->data.fl[9] = cameraParameters[2][1];
        myCameraMatrix->data.fl[10] = cameraParameters[2][2];
        myCameraMatrix->data.fl[11] = cameraParameters[2][3];


        cameraMatrices.push_back(myCameraMatrix);

        index+=5;
    }

    printCameraMatrix();

}

void svlCCDLRCalibrationFileIO::printCameraMatrix()
{
    for(int i=0;i<(int)numImages;i++)
    {
        CvMat* m = (CvMat*) cameraMatrices[i];
        std::cout<<"CameraMatrix # " << i << " " << m->data.fl[0]<<","<< m->data.fl[1] <<","<< m->data.fl[2] <<","<< m->data.fl[3]<< std::endl;
        std::cout<<"CameraMatrix # " << i << " " << m->data.fl[4]<<","<< m->data.fl[5] <<","<< m->data.fl[6] <<","<< m->data.fl[7]<< std::endl;
        std::cout<<"CameraMatrix # " << i << " " << m->data.fl[8]<<","<< m->data.fl[9] <<","<< m->data.fl[10] <<","<< m->data.fl[11]<<std::endl<<std::endl;
    }
}

//================== Subclass of svlCCFileIO for the tracking .coords type ==================//
svlCCTrackerCoordsFileIO::svlCCTrackerCoordsFileIO(const char *filename)
    :svlCCFileIO(filename)
{
    sections[0] = new SectionFormat(0,"############\n");
    sections[1] = new SectionFormat(END,"#,#,#\n");
}

void svlCCTrackerCoordsFileIO::repackData()
{

    worldToTCP = cvCreateMat(4,4,CV_64F);

    worldToTCP->data.fl[0] = data[0][0];
    worldToTCP->data.fl[1] = data[0][1];
    worldToTCP->data.fl[2] = data[0][2];
    worldToTCP->data.fl[3] = data[0][3];
    worldToTCP->data.fl[4] = data[0][4];
    worldToTCP->data.fl[5] = data[0][5];
    worldToTCP->data.fl[6] = data[0][6];
    worldToTCP->data.fl[7] = data[0][7];
    worldToTCP->data.fl[8] = data[0][8];
    worldToTCP->data.fl[9] = data[0][9];
    worldToTCP->data.fl[10] = data[0][10];
    worldToTCP->data.fl[11] = data[0][11];
    worldToTCP->data.fl[12] = 0;
    worldToTCP->data.fl[13] = 0;
    worldToTCP->data.fl[14] = 0;
    worldToTCP->data.fl[15] = 1;
}
