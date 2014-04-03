/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-01-15
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctQuaternionRotation3Test.h"
#include "vctGenericRotationTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandomTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>


template <class _elementType>
void vctQuaternionRotation3Test::TestConstructors(void) {

    vctFixedSizeVector<_elementType, 3> x(1.0, 0.0, 0.0);
    vctFixedSizeVector<_elementType, 3> y(0.0, 1.0, 0.0);
    vctFixedSizeVector<_elementType, 3> z(0.0, 0.0, 1.0);
    vctFixedSizeVector<_elementType, 3> difference;

    typedef vctQuaternionRotation3<_elementType> QuatRotType;
    QuatRotType testRotation1;

    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();

    CPPUNIT_ASSERT(testRotation1.size() == 4);
    CPPUNIT_ASSERT(testRotation1.X() == 0.0);
    CPPUNIT_ASSERT(testRotation1.Y() == 0.0);
    CPPUNIT_ASSERT(testRotation1.Z() == 0.0);
    CPPUNIT_ASSERT(testRotation1.R() == 1.0);

    CPPUNIT_ASSERT(testRotation1 == QuatRotType::Identity());
    testRotation1.X() = 1.0;
    CPPUNIT_ASSERT(testRotation1 != QuatRotType::Identity());
    testRotation1.Assign(0.0, 0.0, 0.0, 1.0);
    CPPUNIT_ASSERT(testRotation1 == QuatRotType::Identity());

    QuatRotType testRotation2(vctAxisAngleRotation3<_elementType>(x, _elementType(cmnPI_2)));
    difference = testRotation2 * y - z;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = -(testRotation2 * z) - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);

    testRotation2.From(vctAxisAngleRotation3<_elementType>(y, _elementType(cmnPI_2)));
    difference = testRotation2 * z - x;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = -(testRotation2 * x) - z;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);

    testRotation2.From(vctAxisAngleRotation3<_elementType>(z, _elementType(cmnPI_2)));
    difference = testRotation2 * x - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = -(testRotation2 * y) - x;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);

    QuatRotType testRotation3;
    difference = testRotation3 * x - x;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = testRotation3 * y - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = testRotation3 * y - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
}


void vctQuaternionRotation3Test::TestConstructorsDouble(void) {
    TestConstructors<double>();
}

void vctQuaternionRotation3Test::TestConstructorsFloat(void) {
    TestConstructors<float>();
}



template <class _elementType>
void vctQuaternionRotation3Test::TestConversionMatrix(void) {
    typedef vctQuaternionRotation3<_elementType> QuatRotType;
    QuatRotType quaternionRotation;
    QuatRotType rotationQuaternion;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestConversion(quaternionRotation, rotationQuaternion);
}

void vctQuaternionRotation3Test::TestConversionMatrixDouble(void) {
    TestConversionMatrix<double>();
}

void vctQuaternionRotation3Test::TestConversionMatrixFloat(void) {
    TestConversionMatrix<float>();
}



template <class _elementType>
void vctQuaternionRotation3Test::TestConversionAxisAngle(void) {
    vctQuaternionRotation3<_elementType> quaternionRotation;
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestConversion(quaternionRotation, axisAngleRotation);
}

void vctQuaternionRotation3Test::TestConversionAxisAngleDouble(void) {
    TestConversionAxisAngle<double>();
}

void vctQuaternionRotation3Test::TestConversionAxisAngleFloat(void) {
    TestConversionAxisAngle<float>();
}



template <class _elementType>
void vctQuaternionRotation3Test::TestConversionRodriguez(void) {
    vctQuaternionRotation3<_elementType> quaternionRotation;
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestConversion(quaternionRotation, rodriguezRotation);
}

void vctQuaternionRotation3Test::TestConversionRodriguezDouble(void) {
    TestConversionRodriguez<double>();
}

void vctQuaternionRotation3Test::TestConversionRodriguezFloat(void) {
    TestConversionRodriguez<float>();
}



template <class _elementType>
void vctQuaternionRotation3Test::TestDefaultConstructor(void) {
    vctQuaternionRotation3<_elementType> quaternionRotation;
    vctGenericRotationTest::TestDefaultConstructor(quaternionRotation);
}

void vctQuaternionRotation3Test::TestDefaultConstructorDouble(void) {
    TestDefaultConstructor<double>();
}

void vctQuaternionRotation3Test::TestDefaultConstructorFloat(void) {
    TestDefaultConstructor<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctQuaternionRotation3Test);

