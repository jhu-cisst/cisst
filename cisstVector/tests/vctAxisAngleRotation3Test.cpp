/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-11-14
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctAxisAngleRotation3Test.h"
#include "vctGenericRotationTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandomTransformations.h>



template <class _elementType>
void vctAxisAngleRotation3Test::TestDefaultConstructor(void) {
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctGenericRotationTest::TestDefaultConstructor(axisAngleRotation);
}

void vctAxisAngleRotation3Test::TestDefaultConstructorDouble(void) {
    TestDefaultConstructor<double>();
}

void vctAxisAngleRotation3Test::TestDefaultConstructorFloat(void) {
    TestDefaultConstructor<float>();
}



template <class _elementType>
void vctAxisAngleRotation3Test::TestNormalize(void)
{
    vctAxisAngleRotation3<_elementType> defaultRotation;
    CPPUNIT_ASSERT(defaultRotation.IsNormalized());
    vctAxisAngleRotation3<_elementType> normalizedRotation;
    normalizedRotation.NormalizedOf(defaultRotation);
    CPPUNIT_ASSERT(normalizedRotation.IsNormalized());

    vctAxisAngleRotation3<_elementType> rotation1, rotation2;
    vctRandom(rotation1);
    CPPUNIT_ASSERT(rotation1.IsNormalized());

    rotation2.Assign(rotation1);
    rotation2.Angle() += 2 * cmnPI;
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEqual(normalizedRotation));
                   
    rotation2.Assign(rotation1);
    rotation2.Angle() += 4 * cmnPI;
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEqual(normalizedRotation));

    rotation2.Assign(rotation1);
    rotation2.Angle() -= 4 * cmnPI;
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEqual(normalizedRotation));
    
    rotation2.Assign(rotation1);
    rotation2.Angle() -= 2 * cmnPI;
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEqual(normalizedRotation));
}

void vctAxisAngleRotation3Test::TestNormalizeDouble(void) {
    TestNormalize<double>();
}

void vctAxisAngleRotation3Test::TestNormalizeFloat(void) {
    TestNormalize<float>();
}




template <class _elementType>
void vctAxisAngleRotation3Test::TestConversionQuaternion(void) {
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctRandom(axisAngleRotation);
    vctGenericRotationTest::TestConversion(axisAngleRotation, quaternionRotation);
}

void vctAxisAngleRotation3Test::TestConversionQuaternionDouble(void) {
    TestConversionQuaternion<double>();
}

void vctAxisAngleRotation3Test::TestConversionQuaternionFloat(void) {
    TestConversionQuaternion<float>();
}



template <class _elementType>
void vctAxisAngleRotation3Test::TestConversionMatrix(void) {
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctMatrixRotation3<_elementType> matrixRotation;
    vctRandom(axisAngleRotation);
    vctGenericRotationTest::TestConversion(axisAngleRotation, matrixRotation);
}

void vctAxisAngleRotation3Test::TestConversionMatrixDouble(void) {
    TestConversionMatrix<double>();
}

void vctAxisAngleRotation3Test::TestConversionMatrixFloat(void) {
    TestConversionMatrix<float>();
}



template <class _elementType>
void vctAxisAngleRotation3Test::TestConversionRodriguez(void) {
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctRandom(axisAngleRotation);
    vctGenericRotationTest::TestConversion(axisAngleRotation, rodriguezRotation);
}

void vctAxisAngleRotation3Test::TestConversionRodriguezDouble(void) {
    TestConversionRodriguez<double>();
}

void vctAxisAngleRotation3Test::TestConversionRodriguezFloat(void) {
    TestConversionRodriguez<float>();
}




template <class _elementType>
void vctAxisAngleRotation3Test::TestFromSignaturesQuaternion(void) {
    vctAxisAngleRotation3<_elementType> toRotation;
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(_elementType(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctAxisAngleRotation3Test::TestFromSignaturesQuaternionDouble(void) {
    TestFromSignaturesQuaternion<double>();
}

void vctAxisAngleRotation3Test::TestFromSignaturesQuaternionFloat(void) {
    TestFromSignaturesQuaternion<float>();
}



template <class _elementType>
void vctAxisAngleRotation3Test::TestFromSignaturesMatrix(void) {
    vctAxisAngleRotation3<_elementType> toRotation;
    vctMatrixRotation3Base<vctFixedSizeMatrix<_elementType, 3, 3>  > fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(_elementType(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctAxisAngleRotation3Test::TestFromSignaturesMatrixDouble(void) {
    TestFromSignaturesMatrix<double>();
}

void vctAxisAngleRotation3Test::TestFromSignaturesMatrixFloat(void) {
    TestFromSignaturesMatrix<float>();
}



template <class _elementType>
void vctAxisAngleRotation3Test::TestFromSignaturesRodriguez(void) {
    vctAxisAngleRotation3<_elementType> toRotation;
    vctRodriguezRotation3<_elementType> fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(_elementType(20.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized,
                                               true);
}

void vctAxisAngleRotation3Test::TestFromSignaturesRodriguezDouble(void) {
    TestFromSignaturesRodriguez<double>();
}

void vctAxisAngleRotation3Test::TestFromSignaturesRodriguezFloat(void) {
    TestFromSignaturesRodriguez<float>();
}


template <class _elementType>
void vctAxisAngleRotation3Test::TestRandom(void) {
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctRandom(axisAngleRotation);
    vctGenericRotationTest::TestRandom(axisAngleRotation);
}

void vctAxisAngleRotation3Test::TestRandomDouble(void) {
    TestRandom<double>();
}

void vctAxisAngleRotation3Test::TestRandomFloat(void) {
    TestRandom<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctAxisAngleRotation3Test);

