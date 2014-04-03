/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-11-16
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctRodriguezRotation3Test.h"
#include "vctGenericRotationTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandomTransformations.h>


template <class _elementType>
void vctRodriguezRotation3Test::TestDefaultConstructor(void) {
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctGenericRotationTest::TestDefaultConstructor(rodriguezRotation);
}

void vctRodriguezRotation3Test::TestDefaultConstructorDouble(void) {
    TestDefaultConstructor<double>();
}

void vctRodriguezRotation3Test::TestDefaultConstructorFloat(void) {
    TestDefaultConstructor<float>();
}


template <class _elementType>
void vctRodriguezRotation3Test::TestNormalize(void)
{
    vctRodriguezRotation3<_elementType> defaultRotation;
    CPPUNIT_ASSERT(defaultRotation.IsNormalized());
    vctRodriguezRotation3<_elementType> normalizedRotation;
    normalizedRotation.NormalizedOf(defaultRotation);
    CPPUNIT_ASSERT(normalizedRotation.IsNormalized());

    vctRodriguezRotation3<_elementType> rotation1, rotation2;
    vctRandom(rotation1);
    CPPUNIT_ASSERT(rotation1.IsNormalized());

    rotation2.Assign(rotation1);
    CPPUNIT_ASSERT(rotation2.AlmostEquivalent(rotation1));
    rotation2.Multiply(_elementType(1.0 + (2.0 * cmnPI) / rotation2.Norm()));
    CPPUNIT_ASSERT(!rotation2.AlmostEqual(rotation1));
    CPPUNIT_ASSERT(rotation2.AlmostEquivalent(rotation1));
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEquivalent(normalizedRotation));
    CPPUNIT_ASSERT(normalizedRotation.IsNormalized());
                   
    rotation2.Assign(rotation1);
    rotation2.Multiply(_elementType(1.0 + (4.0 * cmnPI) / rotation2.Norm()));
    CPPUNIT_ASSERT(!rotation2.AlmostEqual(rotation1));
    CPPUNIT_ASSERT(rotation2.AlmostEquivalent(rotation1));
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEquivalent(normalizedRotation));
    CPPUNIT_ASSERT(normalizedRotation.IsNormalized());

    rotation2.Assign(rotation1);
    rotation2.Multiply(_elementType(1.0 - (2.0 * cmnPI) / rotation2.Norm()));
    CPPUNIT_ASSERT(!rotation2.AlmostEqual(rotation1));
    CPPUNIT_ASSERT(rotation2.AlmostEquivalent(rotation1));
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEquivalent(normalizedRotation));
    CPPUNIT_ASSERT(normalizedRotation.IsNormalized());
    
    rotation2.Assign(rotation1);
    rotation2.Multiply(_elementType(1.0 - (4.0 * cmnPI) / rotation2.Norm()));
    CPPUNIT_ASSERT(!rotation2.AlmostEqual(rotation1));
    CPPUNIT_ASSERT(rotation2.AlmostEquivalent(rotation1));
    normalizedRotation.NormalizedOf(rotation2);
    CPPUNIT_ASSERT(rotation1.AlmostEquivalent(normalizedRotation));
    CPPUNIT_ASSERT(normalizedRotation.IsNormalized());
}

void vctRodriguezRotation3Test::TestNormalizeDouble(void) {
    TestNormalize<double>();
}

void vctRodriguezRotation3Test::TestNormalizeFloat(void) {
    TestNormalize<float>();
}




template <class _elementType>
void vctRodriguezRotation3Test::TestConversionQuaternion(void) {
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > quaternionRotation;
    vctRandom(rodriguezRotation);
    vctGenericRotationTest::TestConversion(rodriguezRotation, quaternionRotation);
}

void vctRodriguezRotation3Test::TestConversionQuaternionDouble(void) {
    TestConversionQuaternion<double>();
}

void vctRodriguezRotation3Test::TestConversionQuaternionFloat(void) {
    TestConversionQuaternion<float>();
}



template <class _elementType>
void vctRodriguezRotation3Test::TestConversionMatrix(void) {
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctMatrixRotation3<_elementType> matrixRotation;
    vctRandom(rodriguezRotation);
    vctGenericRotationTest::TestConversion(rodriguezRotation, matrixRotation);
}

void vctRodriguezRotation3Test::TestConversionMatrixDouble(void) {
    TestConversionMatrix<double>();
}

void vctRodriguezRotation3Test::TestConversionMatrixFloat(void) {
    TestConversionMatrix<float>();
}



template <class _elementType>
void vctRodriguezRotation3Test::TestConversionAxisAngle(void) {
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctRandom(rodriguezRotation);
    vctGenericRotationTest::TestConversion(rodriguezRotation, axisAngleRotation);
}

void vctRodriguezRotation3Test::TestConversionAxisAngleDouble(void) {
    TestConversionAxisAngle<double>();
}

void vctRodriguezRotation3Test::TestConversionAxisAngleFloat(void) {
    TestConversionAxisAngle<float>();
}




template <class _elementType>
void vctRodriguezRotation3Test::TestFromSignaturesQuaternion(void) {
    vctRodriguezRotation3<_elementType> toRotation;
    vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(_elementType(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctRodriguezRotation3Test::TestFromSignaturesQuaternionDouble(void) {
    TestFromSignaturesQuaternion<double>();
}

void vctRodriguezRotation3Test::TestFromSignaturesQuaternionFloat(void) {
    TestFromSignaturesQuaternion<float>();
}



template <class _elementType>
void vctRodriguezRotation3Test::TestFromSignaturesMatrix(void) {
    vctRodriguezRotation3<_elementType> toRotation;
    vctMatrixRotation3Base<vctFixedSizeMatrix<_elementType, 3, 3> > fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(_elementType(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctRodriguezRotation3Test::TestFromSignaturesMatrixDouble(void) {
    TestFromSignaturesMatrix<double>();
}

void vctRodriguezRotation3Test::TestFromSignaturesMatrixFloat(void) {
    TestFromSignaturesMatrix<float>();
}



template <class _elementType>
void vctRodriguezRotation3Test::TestFromSignaturesAxisAngle(void) {
    vctRodriguezRotation3<_elementType> toRotation;
    vctAxisAngleRotation3<_elementType> fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Axis().Add(_elementType(20.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctRodriguezRotation3Test::TestFromSignaturesAxisAngleDouble(void) {
    TestFromSignaturesAxisAngle<double>();
}

void vctRodriguezRotation3Test::TestFromSignaturesAxisAngleFloat(void) {
    TestFromSignaturesAxisAngle<float>();
}


template <class _elementType>
void vctRodriguezRotation3Test::TestRandom(void) {
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctRandom(rodriguezRotation);
    vctGenericRotationTest::TestRandom(rodriguezRotation);
}

void vctRodriguezRotation3Test::TestRandomDouble(void) {
    TestRandom<double>();
}

void vctRodriguezRotation3Test::TestRandomFloat(void) {
    TestRandom<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctRodriguezRotation3Test);

