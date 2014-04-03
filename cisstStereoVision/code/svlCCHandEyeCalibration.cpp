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


#include "cisstStereoVision/svlCCHandEyeCalibration.h"
#include <sstream>

svlCCHandEyeCalibration::svlCCHandEyeCalibration(std::vector<svlCCCalibrationGrid*> calibrationGrids)
{
    handEyeMethodFlag = DUAL_QUATERNION;
    this->calibrationGrids = calibrationGrids;
    cameraToTCP = cvCreateMat(4,4,CV_64FC1);
    debug = false;
}

double svlCCHandEyeCalibration::calibrate()
{
    double error;
    cameraMatrix.clear();
    worldToTCPMatrix.clear();
    
    std::cout << std::endl << "Starting hand eye calibration" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    //process calibration grid images
    cv::Mat rmatrix, tvec;
    CvMat* worldToTCP;
    CvMat *cmatrix, *tcpMatrix; //, *tcpMatrixTemp;
    for(unsigned int i=0;i<calibrationGrids.size();i++)
    {
        if(calibrationGrids.at(i)->valid)
        {
            if(debug)
                std::cout << "Processing Grid# " << i << " of " << calibrationGrids.size() << std::endl;
            cmatrix = cvCreateMat(4,4,CV_64FC1);
            //camera to grid transformation
            if(debug)
            {
                rmatrix = calibrationGrids.at(i)->groundTruthRmatrix;
                tvec = calibrationGrids.at(i)->groundTruthTvec;
            }else
            {
                rmatrix = calibrationGrids.at(i)->rmatrix;
                tvec = calibrationGrids.at(i)->tvec;
            }
            cmatrix->data.db[0] = rmatrix.at<double>(0,0);
            cmatrix->data.db[1] = rmatrix.at<double>(0,1);
            cmatrix->data.db[2] = rmatrix.at<double>(0,2);
            cmatrix->data.db[3] = tvec.at<double>(0,0);
            cmatrix->data.db[4] = rmatrix.at<double>(1,0);
            cmatrix->data.db[5] = rmatrix.at<double>(1,1);
            cmatrix->data.db[6] = rmatrix.at<double>(1,2);
            cmatrix->data.db[7] = tvec.at<double>(0,1);
            cmatrix->data.db[8] = rmatrix.at<double>(2,0);
            cmatrix->data.db[9] = rmatrix.at<double>(2,1);
            cmatrix->data.db[10] = rmatrix.at<double>(2,2);
            cmatrix->data.db[11] = tvec.at<double>(0,2);
            cmatrix->data.db[12] = 0;
            cmatrix->data.db[13] = 0;
            cmatrix->data.db[14] = 0;
            cmatrix->data.db[15] = 1;
            
            cameraMatrix.push_back(cmatrix);
            
            //world to tcp
            worldToTCP = calibrationGrids.at(i)->worldToTCP;
            tcpMatrix = cvCreateMat(4,4,CV_64FC1);
            tcpMatrix->data.db[0] = worldToTCP->data.fl[0];
            tcpMatrix->data.db[1] = worldToTCP->data.fl[1];
            tcpMatrix->data.db[2] = worldToTCP->data.fl[2];
            tcpMatrix->data.db[3] = worldToTCP->data.fl[3];
            tcpMatrix->data.db[4] = worldToTCP->data.fl[4];
            tcpMatrix->data.db[5] = worldToTCP->data.fl[5];
            tcpMatrix->data.db[6] = worldToTCP->data.fl[6];
            tcpMatrix->data.db[7] = worldToTCP->data.fl[7];
            tcpMatrix->data.db[8] = worldToTCP->data.fl[8];
            tcpMatrix->data.db[9] = worldToTCP->data.fl[9];
            tcpMatrix->data.db[10] = worldToTCP->data.fl[10];
            tcpMatrix->data.db[11] = worldToTCP->data.fl[11];
            tcpMatrix->data.db[12] = 0;
            tcpMatrix->data.db[13] = 0;
            tcpMatrix->data.db[14] = 0;
            tcpMatrix->data.db[15] = 1;
            worldToTCPMatrix.push_back(tcpMatrix);
        }
    }
    
    valid = new int*[calibrationGrids.size()];
    for(unsigned int i=0;i<calibrationGrids.size();i++)
    {
        *(valid+i)=new int[calibrationGrids.size()];
    }
    
    for(unsigned int i=0;i<calibrationGrids.size();i++)
    {
        for(unsigned int j=0;j<calibrationGrids.size();j++)
        {
            if(i!=j && calibrationGrids.at(i)->hasTracking && calibrationGrids.at(j)->hasTracking &&
               ((!debug && calibrationGrids.at(i)->valid && calibrationGrids.at(j)->valid)||
                (debug && calibrationGrids.at(i)->validGroundTruth && calibrationGrids.at(j)->validGroundTruth)))
            {
                valid[i][j] = 1;
            }
            else
                valid[i][j] = 0;
        }
    }
    
    //calibrate
    switch(handEyeMethodFlag)
    {
    case DUAL_QUATERNION:
        error = optimizeDualQuaternionMethod();
        break;
    default:
        std::cout << "Unknown hand eye method: " <<  handEyeMethodFlag << std::endl;
        break;
    }
    
    //if(debug)
    //printData();
    
    // Free memory
    //rmatrix.~Mat();
    //tvec.~Mat();
    //if(worldToTCP && cmatrix && tcpMatrix)
    //{
    //cvReleaseMat(&worldToTCP);
    //cvReleaseMat(&cmatrix);
    //cvReleaseMat(&tcpMatrix);
    //}
    return error;
}

double svlCCHandEyeCalibration::optimizeDualQuaternionMethod()
{
    double minHandEyeAvgError = std::numeric_limits<double>::max( );
    double handEyeError = std::numeric_limits<double>::max( );
    double initialHandEyeError;
    vct4x4 minTcpTCamera;
    int* myIndicies = new int[cameraMatrix.size()];
    for(unsigned int j=0;j<cameraMatrix.size();j++)
        myIndicies[j] = j;
    
    int* myPermutationIndicies = new int[cameraMatrix.size()];
    for(unsigned int j=0;j<cameraMatrix.size();j++)
        myPermutationIndicies[j] = j;
    
    int setIteration = 0;
    int totalIteration = 0;
    double maxThreshold = 0.3;
    double minThreshold = 0.2;
    
    int maxIteration = std::min<int>((int)cameraMatrix.size()*cameraMatrix.size()*cameraMatrix.size()*cameraMatrix.size(),(int)10000);
    int maxSetIteration = std::min<int>((int)cameraMatrix.size()*cameraMatrix.size()*cameraMatrix.size(),(int)1000);
    if(cameraMatrix.size() < 10)
        maxThreshold = 1.0;
    
    for(unsigned int i=cameraMatrix.size();i>std::max(cameraMatrix.size()/2,(cameraMatrix.size()-5));i--)
    {
        //combinations
        do
        {
            initialHandEyeError = dualQuaternionMethod(myIndicies,i,false)*cameraMatrix.size()/i;
            if(initialHandEyeError > maxThreshold)
                continue;
            if(debug)
            {
                std::cout << "   ||" << cameraMatrix.size() << " choose " << i << "||";            
                for(unsigned int k=0;k<i;k++)
                    std::cout << myIndicies[k];
                std::cout<< "||";
                std::cout << "initialHandEyeError - " << initialHandEyeError << "||"  << std::endl;
            }
            for(unsigned int j=0;j<i;j++)
                myPermutationIndicies[j] = myIndicies[j]; 
            setIteration = 0;
            maxSetIteration = std::min<int>((int)cameraMatrix.size()*cameraMatrix.size()*cameraMatrix.size(),(int)1000);
            
            //permutations
            do
            {
                handEyeError = dualQuaternionMethod(myPermutationIndicies,i,false)*cameraMatrix.size()/i;
                if(handEyeError < minHandEyeAvgError)
                {
                    
                    minHandEyeAvgError = handEyeError;
                    minTcpTCamera = tcp_T_camera;
                    
                    if(debug)
                    {
                        std::cout << "      Iteration: " << totalIteration << " of " <<maxIteration << " perm: " << handEyeError << " using "<<i ;
                        std::cout << " Indicies: ";
                        for(unsigned int k=0;k<i;k++)
                            std::cout << myPermutationIndicies[k];
                        std::cout << std::endl;
                    }
                    
                    if(minHandEyeAvgError < minThreshold)
                        maxSetIteration = maxIteration;
                }
                setIteration++;
                totalIteration++;
            } while (std::next_permutation(myPermutationIndicies,myPermutationIndicies+i) && setIteration < maxSetIteration && totalIteration <maxIteration);
            if(debug)
                std::cout << "   set ends at iteration " << totalIteration << " of max set " << maxSetIteration << "||" << std::endl;
        } while (next_combination(myIndicies,myIndicies+i,myIndicies+calibrationGrids.size())&& totalIteration <maxIteration);
    }
    if(debug)
        std::cout << "call ends at iteration " << totalIteration << " of max " << maxIteration << "||" << std::endl;
    
    handEyeAvgError = minHandEyeAvgError;
    tcp_T_camera = minTcpTCamera;
    std::cout << "HandEye Avg Error " << handEyeAvgError << std::endl << std::endl;
    std::cout << tcp_T_camera << std::endl;
    return handEyeAvgError;
}

/**************************************************************************************************
*This computes the hand-eye calibration using the method described in 
*"Hand-Eye Calibration Using Dual Quaternions" from 
*Konstantinos Daniilidis

**************************************************************************************************/
double svlCCHandEyeCalibration::dualQuaternionMethod(int* indicies, int indiciesSize, bool runCombination)
{
    
    CvMat *A, *B, *temp0, *temp1, *mulResult, *invResult, *q, *qPrime, *sTemp, *a3x1, *b3x1, *aPrime3x1, *bPrime3x1, *T;
    std::vector<CvMat*> aQ;
    std::vector<CvMat*> aQPrime;
    std::vector<CvMat*> bQ;
    std::vector<CvMat*> bQPrime;
    std::vector<CvMat*> S;
    std::vector<CvMat*> aMatrix;
    std::vector<CvMat*> bMatrix;
    int aIndex,bIndex;
    handEyeAvgError = 0;
    
    //cameraMatrix,worldToTCPMatrix;
    int j = 0;
    for(int i=0;i<(int)(indiciesSize)*(indiciesSize);i++)
    {
        if(runCombination)
        {
            j++;
            if (j >= indiciesSize)
                j=0;
        }
        else
        {
            j = i+1;
            if (j >= indiciesSize)
                break;
        }
        aIndex = indicies[i%indiciesSize];
        bIndex = indicies[j];
        
        if(valid[aIndex][bIndex])
        {
            if(debug)
                std::cout << "Processing camera matrix " << aIndex << " and " << bIndex << "  of " << indiciesSize<<std::endl;
            A = cvCreateMat(4,4,CV_64FC1);
            B = cvCreateMat(4,4,CV_64FC1);
            temp0 = cvCreateMat(4,4,CV_64FC1);
            temp1 = cvCreateMat(4,4,CV_64FC1);
            mulResult = cvCreateMat(4,4,CV_64FC1);
            invResult = cvCreateMat(4,4,CV_64FC1);
            a3x1 = cvCreateMat(3,1,CV_64FC1);
            aPrime3x1 = cvCreateMat(3,1,CV_64FC1);
            b3x1 = cvCreateMat(3,1,CV_64FC1);
            bPrime3x1 = cvCreateMat(3,1,CV_64FC1);
            
            // A = inv(Hmarker2world(:,:,i+1))*Hmarker2world(:,:,i);
            temp0 = worldToTCPMatrix[aIndex];
            temp1 = worldToTCPMatrix[bIndex];
            cvInvert(temp1, invResult, CV_LU);
            cvMatMul(invResult, temp0, A);
            aMatrix.push_back(A);
            
            //B = Hgrid2cam(:,:,i+1)*inv(Hgrid2cam(:,:,i));
            temp0 = cameraMatrix[aIndex];
            temp1 = cameraMatrix[bIndex];
            cvInvert(temp0,invResult,CV_LU);
            cvMatMul(temp1,invResult,B);
            bMatrix.push_back(B);
            
            //[q,qprime] = getDualQuaternion(A(1:3,1:3),A(1:3,4));
            q = cvCreateMat(4,1,CV_64FC1);
            qPrime = cvCreateMat(4,1,CV_64FC1);
            getDualQuaternion(A, q, qPrime);
            aQ.push_back(q);
            aQPrime.push_back(qPrime);
            
            //[q,qprime] = getDualQuaternion(B(1:3,1:3),B(1:3,4));
            q = cvCreateMat(4,1,CV_64FC1);
            qPrime = cvCreateMat(4,1,CV_64FC1);
            getDualQuaternion(B, q, qPrime);
            bQ.push_back(q);
            bQPrime.push_back(qPrime);
        }
    }
    
    if(aMatrix.size() < 1)
    {
        if(debug)
            std::cout << std::endl << "Cannot process hand eye calibration, valid pairs images total: "<< aMatrix.size() << std::endl ;
        return std::numeric_limits<double>::max( );
    }
    
    //The dual quaternion is (Q.q + epsilon*Q.prime)
    //a = Qa.q, a' = Qa.prime  idem for b
    if(aQ.size()>0)
        T = cvCreateMat(6*aQ.size(),8,CV_64FC1);
    else
        T = cvCreateMat(6,8,CV_64FC1);
    for(unsigned int i=0;i<aQ.size();i++)
    {
        a3x1->data.db[0] = aQ[i]->data.db[0];
        a3x1->data.db[1] = aQ[i]->data.db[1];
        a3x1->data.db[2] = aQ[i]->data.db[2];
        b3x1->data.db[0] = bQ[i]->data.db[0];
        b3x1->data.db[1] = bQ[i]->data.db[1];
        b3x1->data.db[2] = bQ[i]->data.db[2];
        aPrime3x1->data.db[0] = aQPrime[i]->data.db[0];
        aPrime3x1->data.db[1] = aQPrime[i]->data.db[1];
        aPrime3x1->data.db[2] = aQPrime[i]->data.db[2];
        bPrime3x1->data.db[0] = bQPrime[i]->data.db[0];
        bPrime3x1->data.db[1] = bQPrime[i]->data.db[1];
        bPrime3x1->data.db[2] = bQPrime[i]->data.db[2];
        sTemp = cvCreateMat(6,8,CV_64FC1);
        cvSetZero(sTemp);
        populateComplexMatrixST(a3x1,b3x1,aPrime3x1,bPrime3x1,sTemp, T, i);
        S.push_back(sTemp);
    }
    
    CvMat* U  = cvCreateMat(T->rows,T->rows,CV_64FC1);
    CvMat* uTranspose  = cvCreateMat(T->rows,T->rows,CV_64FC1);
    CvMat* D  = cvCreateMat(T->rows,T->cols,CV_64FC1);
    CvMat* V  = cvCreateMat(T->cols,T->cols,CV_64FC1);
    CvMat* vTranspose = cvCreateMat(T->cols,T->cols,CV_64FC1);
    //The flags cause U and V to be returned transposed (does not work well without the transpose flags).
    //cvSVD(T, D, U, V); // A = U D V^T
    cvSVD(T, D, uTranspose, vTranspose, CV_SVD_U_T|CV_SVD_V_T); // A = U D V^T
    cvTranspose(uTranspose,U);
    cvTranspose(vTranspose,V);
    
    CvMat *u1, *u1Transpose, *u2, *u2Transpose, *v1, *v2;
    u1 = cvCreateMat(4,1,CV_64FC1);
    u1Transpose = cvCreateMat(1,4,CV_64FC1);
    u2 = cvCreateMat(4,1,CV_64FC1);
    u2Transpose = cvCreateMat(1,4,CV_64FC1);
    v1 = cvCreateMat(4,1,CV_64FC1);
    v2 = cvCreateMat(4,1,CV_64FC1);
    
    u1->data.db[0] = V->data.db[6];
    u1->data.db[1] = V->data.db[14];
    u1->data.db[2] = V->data.db[22];
    u1->data.db[3] = V->data.db[30];
    cvTranspose(u1,u1Transpose);
    v1->data.db[0] = V->data.db[38];
    v1->data.db[1] = V->data.db[46];
    v1->data.db[2] = V->data.db[54];
    v1->data.db[3] = V->data.db[62];
    
    u2->data.db[0] = V->data.db[7];
    u2->data.db[1] = V->data.db[15];
    u2->data.db[2] = V->data.db[23];
    u2->data.db[3] = V->data.db[31];
    cvTranspose(u2,u2Transpose);
    v2->data.db[0] = V->data.db[39];
    v2->data.db[1] = V->data.db[47];
    v2->data.db[2] = V->data.db[55];
    v2->data.db[3] = V->data.db[63];
    
    
    CvMat *aTemp, *bTemp, *cTemp, *poly, *roots, *temp0Mul1x1, *temp1Mul1x1, *temp2Mul1x1, *val0, *val1;
    CvMat *qFinal, *qConj, *rMatrix, *tMatrix;
    double root, val, lambda0, lambda1;
    aTemp = cvCreateMat(1,1,CV_64FC1);
    bTemp = cvCreateMat(1,1,CV_64FC1);
    cTemp = cvCreateMat(1,1,CV_64FC1);
    temp0Mul1x1 = cvCreateMat(1,1,CV_64FC1);
    temp1Mul1x1 = cvCreateMat(1,1,CV_64FC1);
    temp2Mul1x1 = cvCreateMat(1,1,CV_64FC1);
    poly = cvCreateMat(3,1,CV_64FC1);
    roots = cvCreateMat(2,1,CV_64FC1);
    val0 = cvCreateMat(1,1,CV_64FC1);
    val1 = cvCreateMat(1,1,CV_64FC1);
    qFinal = cvCreateMat(8,1,CV_64FC1);
    qConj = cvCreateMat(4,1,CV_64FC1);
    rMatrix = cvCreateMat(3,3,CV_64FC1);
    tMatrix = cvCreateMat(4,1,CV_64FC1);
    
    cvMatMul(u1Transpose,v1,aTemp);
    cvMatMul(u1Transpose,v2,temp0Mul1x1);
    cvMatMul(u2Transpose,v1,temp1Mul1x1);
    cvAdd(temp0Mul1x1,temp1Mul1x1,bTemp);
    cvMatMul(u2Transpose,v2,cTemp);
    poly->data.db[0] = aTemp->data.db[0];
    poly->data.db[1] = bTemp->data.db[0];
    poly->data.db[2] = cTemp->data.db[0];
    //cvSolvePoly keeps returning NaN, writing own quadratic solver
    solveQuadratic(poly->data.db[0],poly->data.db[1],poly->data.db[2],roots);
    
    cvMatMul(u1Transpose,u1,temp0Mul1x1);
    temp0Mul1x1->data.db[0] = temp0Mul1x1->data.db[0]*roots->data.db[0]*roots->data.db[0];
    cvMatMul(u1Transpose,u2,temp1Mul1x1);
    temp1Mul1x1->data.db[0] = 2*roots->data.db[0]*temp1Mul1x1->data.db[0];
    cvAdd(temp0Mul1x1,temp1Mul1x1,temp2Mul1x1);
    cvMatMul(u2Transpose,u2,temp0Mul1x1);
    cvAdd(temp2Mul1x1,temp0Mul1x1,val0);
    
    cvMatMul(u1Transpose,u1,temp0Mul1x1);
    temp0Mul1x1->data.db[0] = temp0Mul1x1->data.db[0]*roots->data.db[1]*roots->data.db[1];
    cvMatMul(u1Transpose,u2,temp1Mul1x1);
    temp1Mul1x1->data.db[0] = 2*roots->data.db[1]*temp1Mul1x1->data.db[0];
    cvAdd(temp0Mul1x1,temp1Mul1x1,temp2Mul1x1);
    cvMatMul(u2Transpose,u2,temp0Mul1x1);
    cvAdd(temp2Mul1x1,temp0Mul1x1,val1);
    
    if(val0->data.db[0]>val1->data.db[0])
    {
        root = roots->data.db[0];
        val = val0->data.db[0];
    }else
    {
        root = roots->data.db[1];
        val = val1->data.db[0];
    }
    
    lambda1 = sqrt(1/val);
    lambda0 = root*lambda1;
    
    qFinal->data.db[0] = lambda0*u1->data.db[0]+lambda1*u2->data.db[0];
    qFinal->data.db[1] = lambda0*u1->data.db[1]+lambda1*u2->data.db[1];
    qFinal->data.db[2] = lambda0*u1->data.db[2]+lambda1*u2->data.db[2];
    qFinal->data.db[3] = lambda0*u1->data.db[3]+lambda1*u2->data.db[3];
    qFinal->data.db[4] = lambda0*v1->data.db[0]+lambda1*v2->data.db[0];
    qFinal->data.db[5] = lambda0*v1->data.db[1]+lambda1*v2->data.db[1];
    qFinal->data.db[6] = lambda0*v1->data.db[2]+lambda1*v2->data.db[2];
    qFinal->data.db[7] = lambda0*v1->data.db[3]+lambda1*v2->data.db[3];
    
    q = cvCreateMat(4,1,CV_64FC1);
    qPrime = cvCreateMat(4,1,CV_64FC1);
    q->data.db[0] = qFinal->data.db[1];
    q->data.db[1] = qFinal->data.db[2];
    q->data.db[2] = qFinal->data.db[3];
    q->data.db[3] = qFinal->data.db[0];
    qPrime->data.db[0] = qFinal->data.db[5];
    qPrime->data.db[1] = qFinal->data.db[6];
    qPrime->data.db[2] = qFinal->data.db[7];
    qPrime->data.db[3] = qFinal->data.db[4];
    qConj->data.db[0] = -q->data.db[0];
    qConj->data.db[1] = -q->data.db[1];
    qConj->data.db[2] = -q->data.db[2];
    qConj->data.db[3] = q->data.db[3];
    
    quaternionToRMatrix(rMatrix,q);
    quaternionMul(qPrime,qConj,tMatrix);
    cvSetReal2D(tMatrix,0,0,2*tMatrix->data.db[0]);
    cvSetReal2D(tMatrix,1,0,2*tMatrix->data.db[1]);
    cvSetReal2D(tMatrix,2,0,2*tMatrix->data.db[2]);
    cvSetReal2D(tMatrix,3,0,2*tMatrix->data.db[3]);
    
    CvMat *tMatrix3x1, *rMatrixNeg, *invCameraToTCP, *tempMul3x1;
    tMatrix3x1 = cvCreateMat(3,1,CV_64FC1);
    rMatrixNeg = cvCreateMat(3,3,CV_64FC1);
    tempMul3x1 = cvCreateMat(3,1,CV_64FC1);
    invCameraToTCP = cvCreateMat(4,4,CV_64FC1);
    
    cvSetReal2D(rMatrixNeg,0,0,-rMatrix->data.db[0]);
    cvSetReal2D(rMatrixNeg,0,1,-rMatrix->data.db[1]);
    cvSetReal2D(rMatrixNeg,0,2,-rMatrix->data.db[2]);
    cvSetReal2D(rMatrixNeg,1,0,-rMatrix->data.db[3]);
    cvSetReal2D(rMatrixNeg,1,1,-rMatrix->data.db[4]);
    cvSetReal2D(rMatrixNeg,1,2,-rMatrix->data.db[5]);
    cvSetReal2D(rMatrixNeg,2,0,-rMatrix->data.db[6]);
    cvSetReal2D(rMatrixNeg,2,1,-rMatrix->data.db[7]);
    cvSetReal2D(rMatrixNeg,2,2,-rMatrix->data.db[8]);
    
    cvSetReal2D(tMatrix3x1,0,0,tMatrix->data.db[0]);
    cvSetReal2D(tMatrix3x1,1,0,tMatrix->data.db[1]);
    cvSetReal2D(tMatrix3x1,2,0,tMatrix->data.db[2]);
    
    cvMatMul(rMatrixNeg,tMatrix3x1,tempMul3x1);
    cvSetReal2D(invCameraToTCP,0,0,rMatrix->data.db[0]);
    cvSetReal2D(invCameraToTCP,0,1,rMatrix->data.db[1]);
    cvSetReal2D(invCameraToTCP,0,2,rMatrix->data.db[2]);
    cvSetReal2D(invCameraToTCP,0,3,tempMul3x1->data.db[0]);
    cvSetReal2D(invCameraToTCP,1,0,rMatrix->data.db[3]);
    cvSetReal2D(invCameraToTCP,1,1,rMatrix->data.db[4]);
    cvSetReal2D(invCameraToTCP,1,2,rMatrix->data.db[5]);
    cvSetReal2D(invCameraToTCP,1,3,tempMul3x1->data.db[1]);
    cvSetReal2D(invCameraToTCP,2,0,rMatrix->data.db[6]);
    cvSetReal2D(invCameraToTCP,2,1,rMatrix->data.db[7]);
    cvSetReal2D(invCameraToTCP,2,2,rMatrix->data.db[8]);
    cvSetReal2D(invCameraToTCP,2,3,tempMul3x1->data.db[2]);
    cvSetReal2D(invCameraToTCP,3,0,0);
    cvSetReal2D(invCameraToTCP,3,1,0);
    cvSetReal2D(invCameraToTCP,3,2,0);
    cvSetReal2D(invCameraToTCP,3,3,1);
    
    cvInvert(invCameraToTCP,cameraToTCP);
    cameraToTCP->data.db[12] = 0.0;
    cameraToTCP->data.db[13] = 0.0;
    cameraToTCP->data.db[14] = 0.0;
    cameraToTCP->data.db[15] = 1.0;
    tcp_T_camera = vct4x4(cameraToTCP->data.db[0],cameraToTCP->data.db[1],cameraToTCP->data.db[2],cameraToTCP->data.db[3],
                          cameraToTCP->data.db[4],cameraToTCP->data.db[5],cameraToTCP->data.db[6],cameraToTCP->data.db[7],
                          cameraToTCP->data.db[8],cameraToTCP->data.db[9],cameraToTCP->data.db[10],cameraToTCP->data.db[11],
                          cameraToTCP->data.db[12],cameraToTCP->data.db[13],cameraToTCP->data.db[14],cameraToTCP->data.db[15]);
    
    if(debug)
    {
        std::cout << "===============T===============" << std::endl;
        printCvMatDouble(T);
        //std::cout <<"===============U===============" << std::endl;
        //printCvMatDouble(U);
        std::cout <<"===============D===============" << std::endl;
        printCvMatDouble(D);
        std::cout <<"===============V Transposed===============" << std::endl;
        printCvMatDouble(vTranspose);
        std::cout <<"===============V===============" << std::endl;
        printCvMatDouble(V);
        std::cout <<"===============V7===============" << std::endl;
        for(int i=0;i<(int)V->rows;i++)
        {
            std::cout << V->data.db[i*(V->cols)+6] << std::endl;
        }
        std::cout<<std::endl;
        std::cout <<"===============V8===============" << std::endl;
        for(int i=0;i<(int)V->rows;i++)
        {
            std::cout << V->data.db[i*(V->cols)+7] << std::endl;
        }
        std::cout<<std::endl;
        std::cout <<"===============u1===============" << std::endl;
        printCvMatDouble(u1);
        std::cout <<"===============u2===============" << std::endl;
        printCvMatDouble(u2);
        std::cout <<"===============v1===============" << std::endl;
        printCvMatDouble(v1);
        std::cout <<"===============v2===============" << std::endl;
        printCvMatDouble(v2);
        std::cout <<"===============poly [a b c]===============" << std::endl;
        printCvMatDouble(poly);
        std::cout <<"===============roots===============" << std::endl;
        printCvMatDouble(roots);
        std::cout <<"===============val1===============" << std::endl;
        printCvMatDouble(val0);
        std::cout <<"===============val2===============" << std::endl;
        printCvMatDouble(val1);
        std::cout <<"===============s===============" << std::endl;
        std::cout << root << std::endl;
        std::cout <<"===============val===============" << std::endl;
        std::cout << val << std::endl;
        std::cout <<"===============lambda1, lambda 2===============" << std::endl;
        std::cout << lambda0 << std::endl;
        std::cout << lambda1 << std::endl;
        std::cout <<"===============qFinal===============" << std::endl;
        printCvMatDouble(qFinal);
        std::cout <<"===============qPrime===============" << std::endl;
        printCvMatDouble(qPrime);
        std::cout <<"===============qConj===============" << std::endl;
        printCvMatDouble(qConj);
        std::cout <<"===============R===============" << std::endl;
        printCvMatDouble(rMatrix);
        std::cout <<"===============t===============" << std::endl;
        printCvMatDouble(tMatrix);
    }
    
    //std::cout <<"===============TCP_T_Camera===============" << std::endl;
    //printCvMatDouble(cameraToTCP);
    
    //free memory
    //CvMat *A, *B, *temp0, *temp1, *mulResult, *invResult, *q, *qPrime, *sTemp, *a3x1, *b3x1, *aPrime3x1, *bPrime3x1, *T;
    //cvReleaseMat(&A);
    //cvReleaseMat(&B);
    
    // oddly releasing temp0,temp1,mulResult,invResult causes errors
    //cvReleaseMat(&temp0);
    //cvReleaseMat(&temp1);
    //cvReleaseMat(&mulResult);
    //cvReleaseMat(&invResult);
    
    if(q && qPrime && sTemp && a3x1 && b3x1 && aPrime3x1 && bPrime3x1 && T)
    {
        cvReleaseMat(&q);
        cvReleaseMat(&qPrime);
        cvReleaseMat(&sTemp);
        cvReleaseMat(&a3x1);
        cvReleaseMat(&b3x1);
        cvReleaseMat(&aPrime3x1);
        cvReleaseMat(&bPrime3x1);
        cvReleaseMat(&T);
    }
    
    if(U && uTranspose && D && V && vTranspose)
    {
        cvReleaseMat(&U);
        cvReleaseMat(&uTranspose);
        cvReleaseMat(&D);
        cvReleaseMat(&V);
        cvReleaseMat(&vTranspose);
    }
    
    //CvMat *aTemp, *bTemp, *cTemp, *poly, *roots, *temp0Mul1x1, *temp1Mul1x1, *temp2Mul1x1, *val0, *val1;
    //CvMat *qFinal, *qConj, *rMatrix, *tMatrix;
    if(aTemp && bTemp && cTemp && poly && roots && temp0Mul1x1 && temp1Mul1x1 && temp2Mul1x1 && val0 && val1 && qFinal && qConj && rMatrix && tMatrix)
    {
        cvReleaseMat(&aTemp);
        cvReleaseMat(&bTemp);
        cvReleaseMat(&cTemp);
        cvReleaseMat(&poly);
        cvReleaseMat(&roots);
        cvReleaseMat(&temp0Mul1x1);
        cvReleaseMat(&temp1Mul1x1);
        cvReleaseMat(&temp2Mul1x1);
        cvReleaseMat(&val0);
        cvReleaseMat(&val1);
        cvReleaseMat(&qFinal);
        cvReleaseMat(&qConj);
        cvReleaseMat(&rMatrix);
        cvReleaseMat(&tMatrix);
    }
    
    //CvMat *tMatrix3x1, *rMatrixNeg, *invCameraToTCP, *tempMul3x1;
    if(tMatrix3x1 && rMatrixNeg && invCameraToTCP && tempMul3x1)
    {
        cvReleaseMat(&tMatrix3x1);
        cvReleaseMat(&rMatrixNeg);
        cvReleaseMat(&invCameraToTCP);
        cvReleaseMat(&tempMul3x1);
    }
    
    return getAvgHandEyeError(aMatrix, bMatrix);
}

double svlCCHandEyeCalibration::getAvgHandEyeError(std::vector<CvMat*> aMatrix, std::vector<CvMat*> bMatrix)
{
    double avgError = 0;
    double error = 0;
    double totalError = 0;
    //int aIndex, bIndex;
    unsigned int k;

    for(k=0;k<aMatrix.size();k++)
    {
        error = checkAXXB(aMatrix.at(k),bMatrix.at(k));
        totalError+= error;
    }
    
    avgError = totalError / k;
    
    return avgError;
}

bool svlCCHandEyeCalibration::getDualQuaternion(CvMat* matrix, CvMat* q, CvMat* qPrime)
{
    CvMat *rvec, *tvec, *rmatrix, *rvecNorm, *l, *transpose, *tempArith1x1, *tempArith3x1, *tempMul3x1, *c;
    rvec = cvCreateMat(3,1,CV_64FC1);
    tvec = cvCreateMat(3,1,CV_64FC1);
    rvecNorm = cvCreateMat(3,1,CV_64FC1);
    cvSetZero(rvecNorm);
    l = cvCreateMat(3,1,CV_64FC1);
    transpose = cvCreateMat(1,3,CV_64FC1);
    tempArith1x1 = cvCreateMat(1,1,CV_64FC1);
    tempArith3x1 = cvCreateMat(3,1,CV_64FC1);
    tempMul3x1 = cvCreateMat(3,1,CV_64FC1);
    c = cvCreateMat(3,1,CV_64FC1);
    
    double rvecNormDouble, tempArithDouble;
    
    //Conversion from R,t to the screw representation [d,theta,l,m]
    rmatrix = cvCreateMat(3,3,CV_64FC1);
    rmatrix->data.db[0] = matrix->data.db[0];
    rmatrix->data.db[1] = matrix->data.db[1];
    rmatrix->data.db[2] = matrix->data.db[2];
    rmatrix->data.db[3] = matrix->data.db[4];
    rmatrix->data.db[4] = matrix->data.db[5];
    rmatrix->data.db[5] = matrix->data.db[6];
    rmatrix->data.db[6] = matrix->data.db[8];
    rmatrix->data.db[7] = matrix->data.db[9];
    rmatrix->data.db[8] = matrix->data.db[10];
    tvec->data.db[0] = matrix->data.db[3];
    tvec->data.db[1] = matrix->data.db[7];
    tvec->data.db[2] = matrix->data.db[11];
    
    //r = rodrigues(R);
    cvRodrigues2(rmatrix,rvec);
    //theta = norm(r);
    rvecNormDouble = cvNorm(rvec);
    cvAddS(rvecNorm,cvScalar(rvecNormDouble),rvecNorm);
    
    //l = r/norm(theta);
    cvDiv(rvec,rvecNorm,l);
    
    //d = l'*t;
    cvTranspose(l,transpose);
    cvMatMul(transpose,tvec,tempArith1x1);
    
    //c = .5*(t-d*l)+cot(theta/2)*cross(l,t);
    //cvScaleAdd
    tempMul3x1->data.db[0] = tempArith1x1->data.db[0]*l->data.db[0];
    tempMul3x1->data.db[1] = tempArith1x1->data.db[0]*l->data.db[1];
    tempMul3x1->data.db[2] = tempArith1x1->data.db[0]*l->data.db[2];
    cvSub(tvec,tempMul3x1,tempArith3x1);
    tempArith3x1->data.db[0] = tempArith3x1->data.db[0]/2;
    tempArith3x1->data.db[1] = tempArith3x1->data.db[1]/2;
    tempArith3x1->data.db[2] = tempArith3x1->data.db[2]/2;
    tempArithDouble = 1/tan(rvecNormDouble/2);
    cvCrossProduct(l,tvec,tempMul3x1);
    tempMul3x1->data.db[0] = tempMul3x1->data.db[0]*tempArithDouble;
    tempMul3x1->data.db[1] = tempMul3x1->data.db[1]*tempArithDouble;
    tempMul3x1->data.db[2] = tempMul3x1->data.db[2]*tempArithDouble;
    cvAdd(tempArith3x1,tempMul3x1,c);
    
    //  q = [sin(theta/2)*l; cos(theta/2)];
    q->data.db[0] = l->data.db[0]*sin(rvecNormDouble/2);
    q->data.db[1] = l->data.db[1]*sin(rvecNormDouble/2);
    q->data.db[2] = l->data.db[2]*sin(rvecNormDouble/2);
    q->data.db[3] = cos(rvecNormDouble/2);
    
    //qprime = [.5*(q(4)*t+cross(t,q(1:3)));-.5*q(1:3)'*t];
    tempArith3x1->data.db[0] = q->data.db[0];
    tempArith3x1->data.db[1] = q->data.db[1];
    tempArith3x1->data.db[2] = q->data.db[2];
    cvCrossProduct(tvec,tempArith3x1,tempMul3x1);
    cvTranspose(tempArith3x1,transpose);
    
    tempArith3x1->data.db[0] = tvec->data.db[0]*q->data.db[3];
    tempArith3x1->data.db[1] = tvec->data.db[1]*q->data.db[3];
    tempArith3x1->data.db[2] = tvec->data.db[2]*q->data.db[3];
    
    qPrime->data.db[0] = (tempArith3x1->data.db[0]+tempMul3x1->data.db[0])/2;
    qPrime->data.db[1] = (tempArith3x1->data.db[1]+tempMul3x1->data.db[1])/2;
    qPrime->data.db[2] = (tempArith3x1->data.db[2]+tempMul3x1->data.db[2])/2;
    cvMatMul(transpose,tvec,tempArith1x1);
    qPrime->data.db[3] = -tempArith1x1->data.db[0]/2;
    
    //free memory
    //*rvec, *tvec, *rmatrix, *rvecNorm, *l, *transpose, *tempArith1x1, *tempArith3x1, *tempMul3x1, *c;
    if(rvec && tvec && rmatrix && rvecNorm && l && transpose && tempArith1x1 && tempArith3x1 && tempMul3x1 &&c)
    {
        cvReleaseMat(&rvec);
        cvReleaseMat(&tvec);
        cvReleaseMat(&rmatrix);
        cvReleaseMat(&rvecNorm);
        cvReleaseMat(&l);
        cvReleaseMat(&transpose);
        cvReleaseMat(&tempArith1x1);
        cvReleaseMat(&tempArith3x1);
        cvReleaseMat(&tempMul3x1);
        cvReleaseMat(&c);
    }
    //WLIU TODO: Check for bad dual quaternion creation
    return true;
}

void svlCCHandEyeCalibration::populateComplexMatrixST(CvMat* a, CvMat* b, CvMat* aPrime, CvMat* bPrime, CvMat* s, CvMat* T, int index)
{
    CvMat* tempArith3x1 = cvCreateMat(3,1,CV_64FC1);
    
    //S(:,:,i) = [Qa(i).q(1:3)-Qb(i).q(1:3)   crossprod(Qa(i).q(1:3)+Qb(i).q(1:3)) zeros(3,1) zeros(3,3);...
    //            Qa(i).qprime(1:3)-Qb(i).qprime(1:3)   crossprod(Qa(i).qprime(1:3)+Qb(i).qprime(1:3)) Qa(i).q(1:3)-Qb(i).q(1:3)   crossprod(Qa(i).q(1:3)+Qb(i).q(1:3))];
    
    cvSub(a,b,tempArith3x1);
    s->data.db[0] = tempArith3x1->data.db[0];
    s->data.db[8] = tempArith3x1->data.db[1];
    s->data.db[16] = tempArith3x1->data.db[2];
    
    cvAdd(a,b,tempArith3x1);
    s->data.db[1] = 0;
    s->data.db[2] = -tempArith3x1->data.db[2];
    s->data.db[3] = tempArith3x1->data.db[1];
    s->data.db[9] = tempArith3x1->data.db[2];
    s->data.db[10] = 0;
    s->data.db[11] = -tempArith3x1->data.db[0];
    s->data.db[17] = -tempArith3x1->data.db[1];
    s->data.db[18] = tempArith3x1->data.db[0];
    s->data.db[19] = 0;
    
    cvSub(aPrime,bPrime,tempArith3x1);
    s->data.db[24] = tempArith3x1->data.db[0];
    s->data.db[32] = tempArith3x1->data.db[1];
    s->data.db[40] = tempArith3x1->data.db[2];
    
    cvAdd(aPrime,bPrime,tempArith3x1);
    s->data.db[25] = 0;
    s->data.db[26] = -tempArith3x1->data.db[2];
    s->data.db[27] = tempArith3x1->data.db[1];
    s->data.db[33] = tempArith3x1->data.db[2];
    s->data.db[34] = 0;
    s->data.db[35] = -tempArith3x1->data.db[0];
    s->data.db[41] = -tempArith3x1->data.db[1];
    s->data.db[42] = tempArith3x1->data.db[0];
    s->data.db[43] = 0;
    
    cvSub(a,b,tempArith3x1);
    s->data.db[28] = tempArith3x1->data.db[0];
    s->data.db[36] = tempArith3x1->data.db[1];
    s->data.db[44] = tempArith3x1->data.db[2];
    
    cvAdd(a,b,tempArith3x1);
    s->data.db[29] = 0;
    s->data.db[30] = -tempArith3x1->data.db[2];
    s->data.db[31] = tempArith3x1->data.db[1];
    s->data.db[37] = tempArith3x1->data.db[2];
    s->data.db[38] = 0;
    s->data.db[39] = -tempArith3x1->data.db[0];
    s->data.db[45] = -tempArith3x1->data.db[1];
    s->data.db[46] = tempArith3x1->data.db[0];
    s->data.db[47] = 0;
    
    //copy to T
    int tIndex = index*s->rows*s->cols;
    for(int i=0;i<(int)s->rows*s->cols;i++)
    {
        T->data.db[tIndex+i] = s->data.db[i];
    }
    
    //free memory
    if(tempArith3x1)
        cvReleaseMat(&tempArith3x1);
}

void svlCCHandEyeCalibration::quaternionMul(CvMat* q1, CvMat* q2, CvMat* result)
{
    double x1=q1->data.db[0];
    double y1=q1->data.db[1];
    double z1=q1->data.db[2];
    double w1=q1->data.db[3];
    double x2=q2->data.db[0];
    double y2=q2->data.db[1];
    double z2=q2->data.db[2];
    double w2=q2->data.db[3];
    
    result->data.db[0] = ( x1*w2 + y1*z2 - z1*y2 + w1*x2);
    result->data.db[1] = (-x1*z2 + y1*w2 + z1*x2 + w1*y2);
    result->data.db[2] = ( x1*y2 - y1*x2 + z1*w2 + w1*z2);
    result->data.db[3] = (-x1*x2 - y1*y2 - z1*z2 + w1*w2);
}

void svlCCHandEyeCalibration::quaternionToRMatrix(CvMat* rMatrix, CvMat* quaternion)
{
    double x = quaternion->data.db[0];
    double y = quaternion->data.db[1];
    double z = quaternion->data.db[2];
    double w = quaternion->data.db[3];
    double x2 = x * x;
    double y2 = y * y;
    double z2 = z * z;
    double w2 = w * w;
    double xy = x * y;
    double xz = x * z;
    double yz = y * z;
    double xw = x * w;
    double yw = y * w;
    double zw = z * w;
    
    cvSetReal2D(rMatrix,0,0, (x2 - y2 - z2 + w2));
    cvSetReal2D(rMatrix,0,1, (2.0 * (xy + zw)));
    cvSetReal2D(rMatrix,0,2, (2.0 * (xz - yw)));
    cvSetReal2D(rMatrix,1,0, (2.0 * (xy - zw)));
    cvSetReal2D(rMatrix,1,1, (-x2 +y2 - z2 + w2));
    cvSetReal2D(rMatrix,1,2, (2.0 * (yz + xw)));
    cvSetReal2D(rMatrix,2,0, (2.0 * (xz + yw)));
    cvSetReal2D(rMatrix,2,1, (2.0 * (yz - xw)));
    cvSetReal2D(rMatrix,2,2, (-x2 - y2 + z2 + w2));
}

void svlCCHandEyeCalibration::solveQuadratic(double a, double b, double c, CvMat* roots)
{
    roots->data.db[0] = (-b+sqrt(b*b-4*a*c))/(2*a);
    roots->data.db[1] = (-b-sqrt(b*b-4*a*c))/(2*a);
}

void svlCCHandEyeCalibration::printData()
{
    for(unsigned int i=0;i<cameraMatrix.size();i++)
    {
        std::cout << "==============CameraMatrix "<<i<<" =============="<<std::endl;
        printCvMatDouble(cameraMatrix[i]);
    }
    
    for(unsigned int i=0;i<worldToTCPMatrix.size();i++)
    {
        std::cout << "==============WorldToTCP "<<i<<" =============="<<std::endl;
        printCvMatDouble(worldToTCPMatrix[i]);
    }
}


void svlCCHandEyeCalibration::printCvMatDouble(CvMat* matrix)
{
    for(int i=0;i<(int)matrix->rows*matrix->cols;i++)
    {
        std::cout << matrix->data.db[i];
        if((i+1)%(matrix->cols) == 0)
            std::cout << std::endl;
        else
            std::cout << ", ";
    }
    std::cout << std::endl;
}

double svlCCHandEyeCalibration::checkAXXB(CvMat* A, CvMat* B)
{
    CvMat *ax, *xb, *xbInv, *axxb, *identity, *result;
    ax = cvCreateMat(4,4,CV_64FC1);
    xb = cvCreateMat(4,4,CV_64FC1);
    xbInv = cvCreateMat(4,4,CV_64FC1);
    axxb = cvCreateMat(4,4,CV_64FC1);
    identity = cvCreateMat(4,4,CV_64FC1);
    result = cvCreateMat(4,4,CV_64FC1);
    cvSetIdentity(identity,cvRealScalar(1.0));
    cvMatMul(A,cameraToTCP,ax);
    cvMatMul(cameraToTCP,B,xb);
    cvInvert(xb,xbInv,CV_LU);
    cvMatMul(xbInv,ax,axxb);
    cvAbsDiff(identity,axxb,result);
    
    if(debug)
        std::cout << "Avg Error " << cvAvg(result).val[0] << std::endl;
    
    if(ax && xb && xbInv && axxb && identity)
    {
        cvReleaseMat(&ax);
        cvReleaseMat(&xb);
        cvReleaseMat(&xbInv);
        cvReleaseMat(&axxb);
        cvReleaseMat(&identity);
    }
    return cvAvg(result).val[0];
}

template <typename Iterator>
        bool svlCCHandEyeCalibration::next_combination(const Iterator first, Iterator k, const Iterator last)
{
    /* Credits: Mark Nelson http://marknelson.us */
    if ((first == last) || (first == k) || (last == k))
        return false;
    Iterator i1 = first;
    Iterator i2 = last;
    ++i1;
    if (last == i1)
        return false;
    i1 = last;
    --i1;
    i1 = k;
    --i2;
    while (first != i1)
    {
        if (*--i1 < *i2)
        {
            Iterator j = k;
            while (!(*i1 < *j)) ++j;
            std::iter_swap(i1,j);
            ++i1;
            ++j;
            i2 = k;
            std::rotate(i1,j,last);
            while (last != j)
            {
                ++j;
                ++i2;
            }
            std::rotate(k,i2,last);
            return true;
        }
    }
    std::rotate(first,k,last);
    return false;
};

