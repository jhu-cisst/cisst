/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ofri Sadowsky
  Created on:	2004-04-20
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctDeterminantTest.h"
#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnTypeTraits.h>

vctDeterminantTest::vctDeterminantTest()
: Vec2D(0)
, Mat2x2(0)
, Vec3D( vctFixedSizeVector<ElementType, 3>(0) )
, RandomSequence( cmnRandomSequence::GetInstance() )
{}

void vctDeterminantTest::setUp()
{
    const ElementType tolerance = cmnTypeTraits<ElementType>::Tolerance();
    const ElementType minRandom(-10);
    const ElementType maxRandom(10);

    while (Vec2D.Norm() < tolerance) {
        RandomSequence.ExtractRandomValueArray(minRandom, maxRandom, Vec2D.Pointer(), 2);
    }

    while (Mat2x2.Norm() < tolerance) {
        RandomSequence.ExtractRandomValueArray(minRandom, maxRandom, Mat2x2.Pointer(), 4);
    }

    while (Vec3D[0].Norm() < tolerance) {
        RandomSequence.ExtractRandomValueArray(minRandom, maxRandom, Vec3D[0].Pointer(), 3);
    }

    while (Vec3D[1].Norm() < tolerance) {
        RandomSequence.ExtractRandomValueArray(minRandom, maxRandom, Vec3D[1].Pointer(), 3);
    }
}


void vctDeterminantTest::TestDeterminant2x2ByInverse(const vctFixedSizeMatrix<ElementType, 2, 2> & inputMatrix)
{
    const ElementType determinant = vctDeterminant<2>::Compute(inputMatrix);

    const ElementType tolerance = cmnTypeTraits<ElementType>::Tolerance();

    if (fabs(determinant) < tolerance) {
        vctFixedSizeVector<ElementType, 2> rowRatio;
        rowRatio.ElementwiseRatioOf( inputMatrix.Row(0), inputMatrix.Row(1) );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( rowRatio[0], rowRatio[1], tolerance );

        vctFixedSizeVector<ElementType, 2> columnRatio;
        columnRatio.ElementwiseRatioOf( inputMatrix.Column(0), inputMatrix.Column(1) );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( columnRatio[0], columnRatio[1], tolerance );

        return;
    }

    const vctFixedSizeMatrix<ElementType, 2, 2> inverse(
        inputMatrix[1][1] / determinant, -inputMatrix[0][1] / determinant,
        -inputMatrix[1][0] / determinant, inputMatrix[0][0] / determinant
        );

    /* gcc (Ubuntu 4.4.3-4ubuntu5) 4.4.3 crashes with internal error
       on ubuntu 64 - Ubuntu 10.04.1 LTS with const */
    /* const */ vctFixedSizeMatrix<ElementType, 2, 2> product( inputMatrix * inverse );
    const vctFixedSizeMatrix<ElementType, 2, 2> identity( ElementType(1), ElementType(0), ElementType(0), ElementType(1) );
    const vctFixedSizeMatrix<ElementType, 2, 2> difference = product - identity;
    ElementType diffNorm = difference.Norm();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, diffNorm, tolerance );
}


void vctDeterminantTest::TestDeterminant2x2ByOrthogonal(const vctFixedSizeVector<ElementType, 2> & inputVector)
{
    const ElementType tolerance = cmnTypeTraits<ElementType>::Tolerance();
    const vctFixedSizeVector<ElementType, 2> normalizedInput = inputVector / inputVector.Norm();
    const vctFixedSizeMatrix<ElementType, 2, 2> orthogonalMatrix(
        normalizedInput[0], normalizedInput[1],
        -normalizedInput[1], normalizedInput[0]);

    const ElementType determinant = vctDeterminant<2>::Compute(orthogonalMatrix);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, determinant, tolerance );

    TestDeterminant2x2ByInverse( orthogonalMatrix );
}


void vctDeterminantTest::TestDeterminant2x2ByLinearCombination(const vctFixedSizeVector<ElementType, 2> & row0)
{
    const ElementType tolerance = cmnTypeTraits<ElementType>::Tolerance();

    const vctFixedSizeVector<ElementType, 2> row1 = ElementType(2) * row0;
    const vctFixedSizeMatrix<ElementType, 2, 2> matrix2x2(
        row0[0], row0[1], row1[0], row1[1] );

    const ElementType determinant = vctDeterminant<2>::Compute(matrix2x2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0, determinant, tolerance);

    TestDeterminant2x2ByInverse( matrix2x2 );
}


void vctDeterminantTest::TestDeterminant3x3ByCrossProduct(const vctFixedSizeVector<ElementType, 3> & row0,
                                                          const vctFixedSizeVector<ElementType, 3> & row1)
{
    const ElementType tolerance = cmnTypeTraits<ElementType>::Tolerance();

    vctFixedSizeVector<ElementType, 3> row2;
    row2.CrossProductOf( row0, row1 );

    const vctFixedSizeMatrix<ElementType, 3, 3> matrix3x3(
        row0[0], row0[1], row0[2],
        row1[0], row1[1], row1[2],
        row2[0], row2[1], row2[2] );

    const ElementType determinant = vctDeterminant<3>::Compute(matrix3x3);

    if (row2.Norm() < tolerance) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, determinant, tolerance);
    }
    else {
        CPPUNIT_ASSERT( fabs(determinant) > tolerance );
    }
}


void vctDeterminantTest::TestDeterminant3x3ByLinearCombination(const vctFixedSizeVector<ElementType, 3> & row0,
                                                               const vctFixedSizeVector<ElementType, 3> & row1)
{
    const ElementType tolerance = cmnTypeTraits<ElementType>::Tolerance();

    const vctFixedSizeVector<ElementType, 3> row2 = row0 + row1;

    const vctFixedSizeMatrix<ElementType, 3, 3> matrix3x3(
        row0[0], row0[1], row0[2],
        row1[0], row1[1], row1[2],
        row2[0], row2[1], row2[2] );

    const ElementType determinant = vctDeterminant<3>::Compute(matrix3x3);

    CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, determinant, tolerance );
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDeterminantTest);

