/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-02-23
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctMatrixRotation2Test.h"
#include "vctGenericRotationTest.h"

#include <cisstVector/vctRandomTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctMatrixRotation2.h>
#include <cisstVector/vctAngleRotation2.h>


template <class _elementType>
void vctMatrixRotation2Test::TestConstructors(void) {

    unsigned int row, column;

    vctFixedSizeVector<_elementType, 2> x(1.0, 0.0);
    vctFixedSizeVector<_elementType, 2> y(0.0, 1.0);
    vctFixedSizeVector<_elementType, 2> difference;

    typedef vctMatrixRotation2<_elementType> RotationType;
    RotationType testRotation1;
    
    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();
    
    CPPUNIT_ASSERT(testRotation1.rows() == 2);
    CPPUNIT_ASSERT(testRotation1.cols() == 2);
    
    for (row = 0; row < 2; row++) {
        for (column = 0; column < 2; column++) {
            if (row == column) {
                CPPUNIT_ASSERT(testRotation1[row][column] == 1.0);
            } else {
                CPPUNIT_ASSERT(testRotation1[row][column] == 0.0);
            }
        }
    }

    CPPUNIT_ASSERT(testRotation1 == RotationType::Identity());
    testRotation1[0][0] = 0.0;
    CPPUNIT_ASSERT(testRotation1 != RotationType::Identity());
    testRotation1.Assign(1.0, 0.0,
                         0.0, 1.0);
    CPPUNIT_ASSERT(testRotation1 == RotationType::Identity());
  
    RotationType testRotation2((vctAngleRotation2)(cmnPI_2));
    difference = testRotation2 * y - x;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = -(testRotation2 * x) - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    
    RotationType testRotation3;
    difference = testRotation3 * x - x;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = testRotation3 * y - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
}

void vctMatrixRotation2Test::TestConstructorsDouble(void) {
    TestConstructors<double>();
}

void vctMatrixRotation2Test::TestConstructorsFloat(void) {
    TestConstructors<float>();
}



template <class _elementType>
void vctMatrixRotation2Test::TestConstructorFromVectors(void) {
    typedef vctMatrixRotation2<_elementType> RotationType;

    RotationType matrix;
    vctRandom(matrix);
    
    RotationType matrixByColumn(matrix.Column(0), matrix.Column(1));
    CPPUNIT_ASSERT(matrix == matrixByColumn);
    
    RotationType matrixByRow(matrix.Row(0), matrix.Row(1), false);
    CPPUNIT_ASSERT(matrix == matrixByRow);
    CPPUNIT_ASSERT(matrixByColumn == matrixByRow);
}

void vctMatrixRotation2Test::TestConstructorFromVectorsDouble(void) {
    TestConstructorFromVectors<double>();
}

void vctMatrixRotation2Test::TestConstructorFromVectorsFloat(void) {
    TestConstructorFromVectors<float>();
}



template <class _elementType>
void vctMatrixRotation2Test::TestConversionAngle(void) {
    typedef vctMatrixRotation2<_elementType> RotationType;
    RotationType matrixRotation;
    vctAngleRotation2 angleRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestConversion(matrixRotation, angleRotation);
}

void vctMatrixRotation2Test::TestConversionAngleDouble(void) {
    TestConversionAngle<double>();
}

void vctMatrixRotation2Test::TestConversionAngleFloat(void) {
    TestConversionAngle<float>();
}



template <class _elementType>
void vctMatrixRotation2Test::TestDefaultConstructor(void) {
    typedef vctMatrixRotation2<_elementType> RotationType;
    RotationType matrixRotation;
    vctGenericRotationTest::TestDefaultConstructor(matrixRotation);
}

void vctMatrixRotation2Test::TestDefaultConstructorDouble(void) {
    TestDefaultConstructor<double>();
}

void vctMatrixRotation2Test::TestDefaultConstructorFloat(void) {
    TestDefaultConstructor<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctMatrixRotation2Test);

