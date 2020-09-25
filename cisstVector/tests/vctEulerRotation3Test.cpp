/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-12-02

  (C) Copyright 2019-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctEulerRotation3Test.h"
#include <cisstVector/vctRandomTransformations.h>

void vctEulerRotationTest::TestOrderZYX(void)
{
    vctEulerZYXRotation3 eulerRotation;
    vctRandom(eulerRotation);
    vctMatRot3 matrixBruteForce =
        vctMatRot3(vctAxAnRot3(vct3(0.0, 0.0, 1.0), eulerRotation.phi()))
        * vctMatRot3(vctAxAnRot3(vct3(0.0, 1.0, 0.0), eulerRotation.theta()))
        * vctMatRot3(vctAxAnRot3(vct3(1.0, 0.0, 0.0), eulerRotation.psi()));
    vctMatRot3 matrixComputed;
    vctEulerToMatrixRotation3(eulerRotation, matrixComputed);
    CPPUNIT_ASSERT(matrixComputed.AlmostEqual(matrixBruteForce));
}

void vctEulerRotationTest::TestOrderZXZ(void)
{
    vctEulerZXZRotation3 eulerRotation;
    vctRandom(eulerRotation);
    vctMatRot3 matrixBruteForce =
        vctMatRot3(vctAxAnRot3(vct3(0.0, 0.0, 1.0), eulerRotation.phi()))
        * vctMatRot3(vctAxAnRot3(vct3(1.0, 0.0, 0.0), eulerRotation.theta()))
        * vctMatRot3(vctAxAnRot3(vct3(0.0, 0.0, 1.0), eulerRotation.psi()));
    vctMatRot3 matrixComputed;
    vctEulerToMatrixRotation3(eulerRotation, matrixComputed);
    CPPUNIT_ASSERT(matrixComputed.AlmostEqual(matrixBruteForce));
}

void vctEulerRotationTest::TestOrderYZX(void)
{
    vctEulerYZXRotation3 eulerRotation;
    vctRandom(eulerRotation);
    vctMatRot3 matrixBruteForce =
        vctMatRot3(vctAxAnRot3(vct3(0.0, 1.0, 0.0), eulerRotation.phi()))
        * vctMatRot3(vctAxAnRot3(vct3(0.0, 0.0, 1.0), eulerRotation.theta()))
        * vctMatRot3(vctAxAnRot3(vct3(1.0, 0.0, 0.0), eulerRotation.psi()));
    vctMatRot3 matrixComputed;
    vctEulerToMatrixRotation3(eulerRotation, matrixComputed);
    CPPUNIT_ASSERT(matrixComputed.AlmostEqual(matrixBruteForce));
}

void vctEulerRotationTest::TestSingularitiesZXZ(void)
{
    vctEulerZXZRotation3 euler1, euler2;
    vctMatRot3 matrix1, matrix2;

    double angle = cmnRandomSequence::GetInstance().ExtractRandomDouble(-cmnPI, cmnPI);

    // find euler back providing initial value as guess
    euler1.Assign(0.0, 0.0, angle);
    vctEulerToMatrixRotation3(euler1, matrix1);
    euler2 = euler1;
    vctEulerFromMatrixRotation3(euler2, matrix1);
    vctEulerToMatrixRotation3(euler2, matrix2);
    CPPUNIT_ASSERT(matrix1.AlmostEqual(matrix2));
    CPPUNIT_ASSERT(euler1.AlmostEqual(euler2));

    // reset initial guess for EulerFromMatrix
    euler1.Assign(0.0, 0.0, angle);
    euler2.Assign(0.0, 0.0, 0.0);
    vctEulerToMatrixRotation3(euler1, matrix1);
    vctEulerFromMatrixRotation3(euler2, matrix1);
    vctEulerToMatrixRotation3(euler2, matrix2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(euler1.phi() + euler1.psi(),
                                 euler2.phi() + euler2.psi(),
                                 cmnTypeTraits<double>::Tolerance());
    CPPUNIT_ASSERT(matrix1.AlmostEqual(matrix2));

    // find euler back providing initial value as guess
    euler1.Assign(0.0, cmnPI, angle);
    vctEulerToMatrixRotation3(euler1, matrix1);
    euler2 = euler1;
    vctEulerFromMatrixRotation3(euler2, matrix1);
    vctEulerToMatrixRotation3(euler2, matrix2);
    CPPUNIT_ASSERT(matrix1.AlmostEqual(matrix2));
    CPPUNIT_ASSERT(euler1.AlmostEqual(euler2));

    // reset initial guess for EulerFromMatrix
    euler1.Assign(0.0, cmnPI, angle);
    euler2.Assign(0.0, 0.0, 0.0);
    vctEulerToMatrixRotation3(euler1, matrix1);
    vctEulerFromMatrixRotation3(euler2, matrix1);
    vctEulerToMatrixRotation3(euler2, matrix2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(euler1.phi() - euler1.psi(),
                                 euler2.phi() - euler2.psi(),
                                 cmnTypeTraits<double>::Tolerance());
    CPPUNIT_ASSERT(matrix1.AlmostEqual(matrix2));
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctEulerRotationTest);
