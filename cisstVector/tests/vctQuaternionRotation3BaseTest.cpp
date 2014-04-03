/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2007-02-05

  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctQuaternionRotation3BaseTest.h"
#include "vctGenericRotationTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandomTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>


template <class _elementType>
void vctQuaternionRotation3BaseTest::TestConstructors(void) {

    vctFixedSizeVector<_elementType, 3> x(1.0, 0.0, 0.0);
    vctFixedSizeVector<_elementType, 3> y(0.0, 1.0, 0.0);
    vctFixedSizeVector<_elementType, 3> z(0.0, 0.0, 1.0);
    vctFixedSizeVector<_elementType, 3> difference;

    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
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


void vctQuaternionRotation3BaseTest::TestConstructorsDouble(void) {
    TestConstructors<double>();
}

void vctQuaternionRotation3BaseTest::TestConstructorsFloat(void) {
    TestConstructors<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestApplyTo(void) {

    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();

    vctFixedSizeVector<_elementType, 3> x(1.0, 0.0, 0.0);
    vctFixedSizeVector<_elementType, 3> y(0.0, 1.0, 0.0);
    vctFixedSizeVector<_elementType, 3> z(0.0, 0.0, 1.0);
    vctFixedSizeVector<_elementType, 3> result;
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    QuatRotType composed;

    QuatRotType testRotation(vctAxisAngleRotation3<_elementType>(x, _elementType(cmnPI_2)));
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).LinfNorm() < tolerance);
    testRotation.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).LinfNorm() < tolerance);
    testRotation.ApplyTo(QuatRotType::Identity(), composed);
    composed.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).LinfNorm() < tolerance);
    composed.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).LinfNorm() < tolerance);

    testRotation.From(vctAxisAngleRotation3<_elementType>(y, _elementType(cmnPI_2)));
    testRotation.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).LinfNorm() < tolerance);
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).LinfNorm() < tolerance);
    testRotation.ApplyTo(QuatRotType::Identity(), composed);
    composed.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).LinfNorm() < tolerance);
    composed.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).LinfNorm() < tolerance);

    testRotation.From(vctAxisAngleRotation3<_elementType>(z, _elementType(cmnPI_2)));
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).LinfNorm() < tolerance);
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).LinfNorm() < tolerance);
    testRotation.ApplyTo(QuatRotType::Identity(), composed);
    composed.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).LinfNorm() < tolerance);
    composed.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).LinfNorm() < tolerance);
}


void vctQuaternionRotation3BaseTest::TestApplyToDouble(void) {
    TestApplyTo<double>();
}

void vctQuaternionRotation3BaseTest::TestApplyToFloat(void) {
    TestApplyTo<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestConversionMatrix(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctMatrixRotation3<_elementType> matrixRotation;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestConversion(quaternionRotation, matrixRotation);
}

void vctQuaternionRotation3BaseTest::TestConversionMatrixDouble(void) {
    TestConversionMatrix<double>();
}

void vctQuaternionRotation3BaseTest::TestConversionMatrixFloat(void) {
    TestConversionMatrix<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestConversionAxisAngle(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestConversion(quaternionRotation, axisAngleRotation);
}

void vctQuaternionRotation3BaseTest::TestConversionAxisAngleDouble(void) {
    TestConversionAxisAngle<double>();
}

void vctQuaternionRotation3BaseTest::TestConversionAxisAngleFloat(void) {
    TestConversionAxisAngle<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestConversionRodriguez(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestConversion(quaternionRotation, rodriguezRotation);
}

void vctQuaternionRotation3BaseTest::TestConversionRodriguezDouble(void) {
    TestConversionRodriguez<double>();
}

void vctQuaternionRotation3BaseTest::TestConversionRodriguezFloat(void) {
    TestConversionRodriguez<float>();
}




template <class _elementType>
void vctQuaternionRotation3BaseTest::TestFromSignaturesMatrix(void) {
    typedef vctMatrixRotation3Base<vctFixedSizeMatrix<_elementType, 3, 3> > MatRotType;
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    QuatRotType toRotation;
    MatRotType fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(_elementType(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctQuaternionRotation3BaseTest::TestFromSignaturesMatrixDouble(void) {
    TestFromSignaturesMatrix<double>();
}

void vctQuaternionRotation3BaseTest::TestFromSignaturesMatrixFloat(void) {
    TestFromSignaturesMatrix<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestFromSignaturesAxisAngle(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > toRotation;
    vctAxisAngleRotation3<_elementType> fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Axis().Add(_elementType(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctQuaternionRotation3BaseTest::TestFromSignaturesAxisAngleDouble(void) {
    TestFromSignaturesAxisAngle<double>();
}

void vctQuaternionRotation3BaseTest::TestFromSignaturesAxisAngleFloat(void) {
    TestFromSignaturesAxisAngle<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestFromSignaturesRodriguez(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > toRotation;
    vctRodriguezRotation3<_elementType> fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(_elementType(20.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized,
                                               true);
}

void vctQuaternionRotation3BaseTest::TestFromSignaturesRodriguezDouble(void) {
    TestFromSignaturesRodriguez<double>();
}

void vctQuaternionRotation3BaseTest::TestFromSignaturesRodriguezFloat(void) {
    TestFromSignaturesRodriguez<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestIdentity(void) {
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    QuatRotType quaternionRotation;
    vctFixedSizeVector<_elementType, QuatRotType::DIMENSION> inputVector, outputVector;
    vctRandom(quaternionRotation);
    vctRandom(inputVector, _elementType(-1.0), _elementType(1.0));
    vctGenericRotationTest::TestIdentity(quaternionRotation, inputVector, outputVector);
}

void vctQuaternionRotation3BaseTest::TestIdentityDouble(void) {
    TestIdentity<double>();
}

void vctQuaternionRotation3BaseTest::TestIdentityFloat(void) {
    TestIdentity<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestDefaultConstructor(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctGenericRotationTest::TestDefaultConstructor(quaternionRotation);
}

void vctQuaternionRotation3BaseTest::TestDefaultConstructorDouble(void) {
    TestDefaultConstructor<double>();
}

void vctQuaternionRotation3BaseTest::TestDefaultConstructorFloat(void) {
    TestDefaultConstructor<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestInverse(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestInverse(quaternionRotation);

}

void vctQuaternionRotation3BaseTest::TestInverseDouble(void) {
    TestInverse<double>();
}

void vctQuaternionRotation3BaseTest::TestInverseFloat(void) {
    TestInverse<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestComposition(void) {
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    QuatRotType quaternionRotation1;
    QuatRotType quaternionRotation2;
    vctFixedSizeVector<_elementType, QuatRotType::DIMENSION> inputVector;
    vctRandom(quaternionRotation1);
    vctRandom(quaternionRotation2);
    vctRandom(inputVector, _elementType(-1.0), _elementType(1.0));
    vctGenericRotationTest::TestComposition(quaternionRotation1, quaternionRotation2, inputVector);
}

void vctQuaternionRotation3BaseTest::TestCompositionDouble(void) {
    TestComposition<double>();
}

void vctQuaternionRotation3BaseTest::TestCompositionFloat(void) {
    TestComposition<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestRandom(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctRandom(quaternionRotation);
    vctGenericRotationTest::TestRandom(quaternionRotation);
}

void vctQuaternionRotation3BaseTest::TestRandomDouble(void) {
    TestRandom<double>();
}

void vctQuaternionRotation3BaseTest::TestRandomFloat(void) {
    TestRandom<float>();
}



template <class _elementType>
void vctQuaternionRotation3BaseTest::TestRigidity(void) {
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    QuatRotType quaternionRotation;
    vctFixedSizeVector<_elementType, QuatRotType::DIMENSION> vector1;
    vctFixedSizeVector<_elementType, QuatRotType::DIMENSION> vector2;
    vctRandom(quaternionRotation);
    vctRandom(vector1, _elementType(-1.0), _elementType(1.0));
    vctRandom(vector2, _elementType(-1.0), _elementType(1.0));
    vctGenericRotationTest::TestRigidity(quaternionRotation, vector1, vector2);
}

void vctQuaternionRotation3BaseTest::TestRigidityDouble(void) {
    TestRigidity<double>();
}

void vctQuaternionRotation3BaseTest::TestRigidityFloat(void) {
    TestRigidity<float>();
}


template <class _elementType>
void vctQuaternionRotation3BaseTest::TestApplyMethodsOperators(void) {
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    QuatRotType quaternionRotation;
    vctRandom(quaternionRotation);

    vctFixedSizeVector<_elementType, QuatRotType::DIMENSION> vector;
    vctRandom(vector, _elementType(-1.0), _elementType(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(quaternionRotation, vector);

    QuatRotType rotation;
    vctRandom(rotation);
    vctGenericRotationTest::TestApplyMethodsOperatorsXform(quaternionRotation, rotation);
}


void vctQuaternionRotation3BaseTest::TestApplyMethodsOperatorsDouble(void) {
    TestApplyMethodsOperators<double>();
}

void vctQuaternionRotation3BaseTest::TestApplyMethodsOperatorsFloat(void) {
    TestApplyMethodsOperators<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctQuaternionRotation3BaseTest);

