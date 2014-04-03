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
#ifndef _svlCCHandEyeCalibration_h
#define _svlCCHandEyeCalibration_h

#include <cisstStereoVision/svlTypes.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlCCCalibrationGrid.h>
#include <limits>

class svlCCHandEyeCalibration
{
public:
    enum handEyeMethodEnum {DUAL_QUATERNION};
    svlCCHandEyeCalibration(std::vector<svlCCCalibrationGrid*> calibrationGrids);
    double calibrate();
    vct4x4 tcp_T_camera;

private:
    std::vector<svlCCCalibrationGrid*> calibrationGrids;
    double optimizeDualQuaternionMethod();
    double getAvgHandEyeError(std::vector<CvMat*> aMatrix, std::vector<CvMat*> bMatrix);
    double dualQuaternionMethod(int* indicies, int indiciesSize, bool runCombination);
    bool getDualQuaternion(CvMat* matrix, CvMat* q, CvMat* qPrime);
    void populateComplexMatrixST(CvMat* a, CvMat* b, CvMat* aPrime, CvMat* bPrime, CvMat* s, CvMat* T, int index);
    void quaternionMul(CvMat* q1, CvMat* q2, CvMat* result);
    void quaternionToRMatrix(CvMat* rMatrix, CvMat* quaternion);
    void solveQuadratic(double a, double b, double c, CvMat* roots);
    void printData();
    void printCvMatDouble(CvMat* matrix);
    double checkAXXB(CvMat* A, CvMat* B);
    template <typename Iterator> bool next_combination(const Iterator first, Iterator k, const Iterator last);

    //parameters
    bool debug;
    int handEyeMethodFlag;
    std::vector<CvMat*> cameraMatrix;
    std::vector<CvMat*> worldToTCPMatrix;
    CvMat* cameraToTCP;
    double handEyeAvgError;
    int** valid;
};

#endif
