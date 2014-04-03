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

#ifndef _svlCCFileIO_h
#define _svlCCFileIO_h
#include <cisstStereoVision/svlTypes.h>

class svlCCFileIO
{
public:
    enum formatEnum {ORIGINAL, IMPROVED}; // For subclass of svlCCFileIO for the DLR .pts type
    // Constructor takes filename string of file to read in
    svlCCFileIO(const char* filename): END(-1)
    {
        myFileName = filename;

    }
    // Parses file into lines, and passes each line to ParseLine()
    bool parseFile()
    {
        char buffer[200];
        int lineNum = 0;
        int sectionNum = 0;

        std::ifstream& fileStream = openFile(myFileName);
        if(fileStream.bad())
            return false;

        try{
            while (fileStream.good() && !fileStream.eof() && !fileStream.fail())
            {
                if(sections[sectionNum]->lineNum < lineNum)
                {
                    if(sections[sectionNum]->lineNum != END)
                        sectionNum++;
                }


                fileStream.getline(buffer,200,'\n');
                svlCCFileIO::parseLine(buffer,sections[sectionNum]->fmt,lineNum);
                lineNum++;

                if (fileStream.fail() && !fileStream.eof())
                {
                    std::cout << "fileStream FAILED" << std::endl;
                    return false;
                }
            }
        }catch(std::ios_base::failure)
        {
            std::cout << myFileName << " failed." << std::endl;
            return false;
        }

        return true;
    }
    // Virtual method to repack the file data into file-specific data structures
    // implemented in detail in each subclass
    virtual void repackData(){};

protected:
    // Object to keep track of file formats for different sections of files
    class SectionFormat
    {
    public:
        SectionFormat(int lnNum, const char* fmtarray)
        {
            lineNum = lnNum;
            fmt = fmtarray;
        }
        int lineNum;
        const char* fmt;
    };
    // Creates an ifstream object for the specified file
    std::ifstream& openFile(const char* filename)
    {
        // from C++ for dummies
        std::ifstream* pFileStream = new std::ifstream(filename);

        if (pFileStream->good())
        {
            std::cerr << "Sucessfully opened " << filename << std::endl;
        }
        else
        {
            std::cerr << "Could not open " << filename << std::endl;
        }
        return *pFileStream;
    }

    //Parses a line from a file according to a format string
    void parseLine(const char line[], const char format[], int lineNum)
    {
        char buffer[200];
        int i = 0;
        int numsRead = 0;
        std::istringstream inp(line);
        double d;

        while(format[i] != '\n')
        {
            if(format[i] == '#')
            {
                i++;
                char delim = format[i];
                inp.getline(buffer,200,delim);
                std::istringstream inp2(buffer);
                while(inp2 >> d) {
                    data[lineNum][numsRead] = (float)d;
                    numsRead++;
                }
            }
            else
            {
                inp.getline(buffer,200,format[i]);
            }
            i++;
        }
    }


    // parameters
    const static bool debug = false;
    const char* myFileName;
    int fileFormat;
    SectionFormat* sections[12];
    float data [500][12];
    const int END;

};

//================== Subclass of svlCCFileIO for the DLR .pts type ==================//
class svlCCPointsFileIO : public svlCCFileIO
{
public:
    svlCCPointsFileIO(const char* filename, int fileFormat);
    void repackData();
    void repackData(IplImage* iplImage);
    float errorCheck(vctDynamicVector<vctDynamicVector<vct3> > Dc, vctDynamicVector<vctDynamicVector<vct3> > Ac, vctDynamicVector<vctDynamicVector<vct3> > Cc);
    void showData();

    std::vector<cv::Point2f> imagePoints;
    std::vector<cv::Point3f> calibrationGridPoints;
    CvMat* worldToTCP;

protected:
    int pointsCount;
    cv::Size imageSize;

};

//================== Subclass of svlCCFileIO for the DLR cameraCalibration.m type ==================//
class svlCCDLRCalibrationFileIO : public svlCCFileIO
{
public:
    svlCCDLRCalibrationFileIO(const char* filename);
    void repackData(int numImages);

    cv::Mat tcpToCamera, cameraMatrix,distCoeffs;
    std::vector<CvMat*> cameraMatrices;
    cv::Point2f focalLength;
    cv::Point2f principalPoint;
    float alpha;
    int numImages;


protected:
    const static bool debug = false;
    void printCameraMatrix();

};

//================== Subclass of svlCCFileIO for the tracking .coords type ==================//
class svlCCTrackerCoordsFileIO : public svlCCFileIO
{
public:
    svlCCTrackerCoordsFileIO(const char* filename);
    void repackData();

    CvMat* worldToTCP;


protected:
    const static bool debug = false;

};

#endif
