/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-01-09

  (C) Copyright 2004-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctMatrixRotation3Test.h"
#include "vctGenericRotationTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandomTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>


template <class _elementType>
void vctMatrixRotation3Test::TestConstructors(void) {

    unsigned int row, column;

    vctFixedSizeVector<_elementType, 3> x(1.0, 0.0, 0.0);
    vctFixedSizeVector<_elementType, 3> y(0.0, 1.0, 0.0);
    vctFixedSizeVector<_elementType, 3> z(0.0, 0.0, 1.0);
    vctFixedSizeVector<_elementType, 3> difference;

    typedef vctMatrixRotation3<_elementType> MatRotType;
    MatRotType testRotation1;

    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();

    CPPUNIT_ASSERT(testRotation1.rows() == 3);
    CPPUNIT_ASSERT(testRotation1.cols() == 3);

    for (row = 0; row < 3; row++) {
        for (column = 0; column < 3; column++) {
            if (row == column) {
                CPPUNIT_ASSERT(testRotation1[row][column] == 1.0);
            } else {
                CPPUNIT_ASSERT(testRotation1[row][column] == 0.0);
            }
        }
    }

    CPPUNIT_ASSERT(testRotation1 == MatRotType::Identity());
    testRotation1[0][0] = 0.0;
    CPPUNIT_ASSERT(testRotation1 != MatRotType::Identity());
    testRotation1.Assign(1.0, 0.0, 0.0,
                         0.0, 1.0, 0.0,
                         0.0, 0.0, 1.0);
    CPPUNIT_ASSERT(testRotation1 == MatRotType::Identity());

    MatRotType testRotation2(vctAxisAngleRotation3<_elementType>(x, _elementType(cmnPI_2)));
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

    MatRotType testRotation3;
    difference = testRotation3 * x - x;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = testRotation3 * y - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
    difference = testRotation3 * y - y;
    CPPUNIT_ASSERT(difference.LinfNorm() < tolerance);
}

void vctMatrixRotation3Test::TestConstructorsDouble(void) {
    TestConstructors<double>();
}

void vctMatrixRotation3Test::TestConstructorsFloat(void) {
    TestConstructors<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestConstructorFromVectors(void) {
    typedef vctMatrixRotation3<_elementType> MatRotType;

    MatRotType matrix;
    vctRandom(matrix);

    MatRotType matrixByColumn(matrix.Column(0), matrix.Column(1), matrix.Column(2));
    CPPUNIT_ASSERT(matrix == matrixByColumn);

    MatRotType matrixByRow(matrix.Row(0), matrix.Row(1), matrix.Row(2), false);
    CPPUNIT_ASSERT(matrix == matrixByRow);
    CPPUNIT_ASSERT(matrixByColumn == matrixByRow);
}

void vctMatrixRotation3Test::TestConstructorFromVectorsDouble(void) {
    TestConstructorFromVectors<double>();
}

void vctMatrixRotation3Test::TestConstructorFromVectorsFloat(void) {
    TestConstructorFromVectors<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestFromVectors(void) {
    typedef vctMatrixRotation3<_elementType> RotationType;

    RotationType matrix;
    vctRandom(matrix);

    RotationType matrixByColumn(matrix.Column(0), matrix.Column(1), matrix.Column(2));
    CPPUNIT_ASSERT(matrix == matrixByColumn);

    matrixByColumn.From(matrix.Column(0), matrix.Column(1), matrix.Column(2));
    CPPUNIT_ASSERT(matrix == matrixByColumn);

    RotationType matrixByRow(matrix.Row(0), matrix.Row(1), matrix.Row(2), false);
    CPPUNIT_ASSERT(matrix == matrixByRow);
    CPPUNIT_ASSERT(matrixByColumn == matrixByRow);

    matrixByRow.From(matrix.Row(0), matrix.Row(1), matrix.Row(2), false);
    CPPUNIT_ASSERT(matrix == matrixByRow);
    CPPUNIT_ASSERT(matrixByColumn == matrixByRow);
}

void vctMatrixRotation3Test::TestFromVectorsDouble(void) {
    TestFromVectors<double>();
}

void vctMatrixRotation3Test::TestFromVectorsFloat(void) {
    TestFromVectors<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestConversionQuaternion(void) {
    vctMatrixRotation3<_elementType> matrixRotation;
    vctQuaternionRotation3<_elementType> quaternionRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestConversion(matrixRotation, quaternionRotation);
}

void vctMatrixRotation3Test::TestConversionQuaternionDouble(void) {
    TestConversionQuaternion<double>();
}

void vctMatrixRotation3Test::TestConversionQuaternionFloat(void) {
    TestConversionQuaternion<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestConversionAxisAngle(void) {
    vctMatrixRotation3<_elementType> matrixRotation;
    vctAxisAngleRotation3<_elementType> axisAngleRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestConversion(matrixRotation, axisAngleRotation);
}

void vctMatrixRotation3Test::TestConversionAxisAngleDouble(void) {
    TestConversionAxisAngle<double>();
}

void vctMatrixRotation3Test::TestConversionAxisAngleFloat(void) {
    TestConversionAxisAngle<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestConversionRodriguez(void) {
    vctMatrixRotation3<_elementType> matrixRotation;
    vctRodriguezRotation3<_elementType> rodriguezRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestConversion(matrixRotation, rodriguezRotation);
}

void vctMatrixRotation3Test::TestConversionRodriguezDouble(void) {
    TestConversionRodriguez<double>();
}

void vctMatrixRotation3Test::TestConversionRodriguezFloat(void) {
    TestConversionRodriguez<float>();
}

template <class _elementType, vctEulerRotation3Order::OrderType _order>
void vctMatrixRotation3Test::TestConversionEuler(void) {
    vctMatrixRotation3<_elementType> matrixRotation;
    vctEulerRotation3<_order> eulerRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestConversion(matrixRotation, eulerRotation);
}

void vctMatrixRotation3Test::TestConversionEulerZYZDouble(void) {
    TestConversionEuler<double, vctEulerRotation3Order::ZYZ>();
}

void vctMatrixRotation3Test::TestConversionEulerZYZFloat(void) {
    TestConversionEuler<float, vctEulerRotation3Order::ZYZ>();
}

void vctMatrixRotation3Test::TestConversionEulerZYXDouble(void) {
    TestConversionEuler<double, vctEulerRotation3Order::ZYX>();
}

void vctMatrixRotation3Test::TestConversionEulerZYXFloat(void) {
    TestConversionEuler<float, vctEulerRotation3Order::ZYX>();
}

void vctMatrixRotation3Test::TestConversionEulerZXZDouble(void) {
    TestConversionEuler<double, vctEulerRotation3Order::ZXZ>();
}

void vctMatrixRotation3Test::TestConversionEulerZXZFloat(void) {
    TestConversionEuler<float, vctEulerRotation3Order::ZXZ>();
}

void vctMatrixRotation3Test::TestConversionEulerYZXDouble(void) {
    TestConversionEuler<double, vctEulerRotation3Order::YZX>();
}

void vctMatrixRotation3Test::TestConversionEulerYZXFloat(void) {
    TestConversionEuler<float, vctEulerRotation3Order::YZX>();
}

template <class _elementType>
void vctMatrixRotation3Test::TestFromSignaturesQuaternion(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;

    RotationType toRotation;
    vctQuaternionRotation3<value_type> fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(static_cast<value_type>(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctMatrixRotation3Test::TestFromSignaturesQuaternionDouble(void) {
    TestFromSignaturesQuaternion<double>();
}

void vctMatrixRotation3Test::TestFromSignaturesQuaternionFloat(void) {
    TestFromSignaturesQuaternion<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestFromSignaturesAxisAngle(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;

    RotationType toRotation;
    vctAxisAngleRotation3<value_type> fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Axis().Add(static_cast<value_type>(1.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized);
}

void vctMatrixRotation3Test::TestFromSignaturesAxisAngleDouble(void) {
    TestFromSignaturesAxisAngle<double>();
}

void vctMatrixRotation3Test::TestFromSignaturesAxisAngleFloat(void) {
    TestFromSignaturesAxisAngle<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestFromSignaturesRodriguez(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;

    RotationType toRotation;
    vctRodriguezRotation3<value_type> fromRotationNormalized, fromRotationNotNormalized;
    vctRandom(fromRotationNormalized);
    vctRandom(fromRotationNotNormalized);
    fromRotationNotNormalized.Add(static_cast<value_type>(20.0));
    vctGenericRotationTest::TestFromSignatures(toRotation,
                                               fromRotationNormalized,
                                               fromRotationNotNormalized,
                                               true);
}

void vctMatrixRotation3Test::TestFromSignaturesRodriguezDouble(void) {
    TestFromSignaturesRodriguez<double>();
}

void vctMatrixRotation3Test::TestFromSignaturesRodriguezFloat(void) {
    TestFromSignaturesRodriguez<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestDefaultConstructor(void) {
    vctMatrixRotation3<_elementType> matrixRotation;
    vctGenericRotationTest::TestDefaultConstructor(matrixRotation);
}

void vctMatrixRotation3Test::TestDefaultConstructorDouble(void) {
    TestDefaultConstructor<double>();
}

void vctMatrixRotation3Test::TestDefaultConstructorFloat(void) {
    TestDefaultConstructor<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestNormalize(void) {
    // this test proves that the matrix is a valid rotation matrix
    // after normalization.  It doesn't test that the matrix is the
    // best normalized one.
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;

    RotationType m1, m2;

    value_type tolerance = cmnTypeTraits<value_type>::Tolerance();
    unsigned int column, row;

    CPPUNIT_ASSERT(m1.IsNormalized());

    m1[2][1] = static_cast<value_type>(5);
    CPPUNIT_ASSERT(!m1.IsNormalized());

    m1 = RotationType::Identity();
    CPPUNIT_ASSERT(m1.IsNormalized());

    m1[0][0] += static_cast<value_type>(100 * tolerance);
    m1[1][0] += static_cast<value_type>(100 * tolerance);
    CPPUNIT_ASSERT(!m1.IsNormalized());

    m2.NormalizedOf(m1);
    CPPUNIT_ASSERT(m2.IsNormalized());

    m1.NormalizedSelf();
    for (row = 0; row < 3; row++) {
        for (column = 0; column < 3; column++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(m1[row][column], m2[row][column], tolerance);
        }
    }
}

void vctMatrixRotation3Test::TestNormalizeDouble(void) {
    TestNormalize<double>();
}

void vctMatrixRotation3Test::TestNormalizeFloat(void) {
    TestNormalize<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestInverse(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;

    RotationType matrixRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestInverse(matrixRotation);

}

void vctMatrixRotation3Test::TestInverseDouble(void) {
    TestInverse<double>();
}

void vctMatrixRotation3Test::TestInverseFloat(void) {
    TestInverse<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestComposition(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    RotationType matrixRotation1;
    RotationType matrixRotation2;
    VectorType inputVector;
    vctRandom(matrixRotation1);
    vctRandom(matrixRotation2);
    vctRandom(inputVector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestComposition(matrixRotation1, matrixRotation2, inputVector);
}

void vctMatrixRotation3Test::TestCompositionDouble(void) {
    TestComposition<double>();
}

void vctMatrixRotation3Test::TestCompositionFloat(void) {
    TestComposition<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestRandom(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;

    RotationType matrixRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestRandom(matrixRotation);
}

void vctMatrixRotation3Test::TestRandomDouble(void) {
    TestRandom<double>();
}

void vctMatrixRotation3Test::TestRandomFloat(void) {
    TestRandom<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestRigidity(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    RotationType matrixRotation;
    VectorType vector1;
    VectorType vector2;
    vctRandom(matrixRotation);
    vctRandom(vector1, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctRandom(vector2, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestRigidity(matrixRotation, vector1, vector2);
}

void vctMatrixRotation3Test::TestRigidityDouble(void) {
    TestRigidity<double>();
}

void vctMatrixRotation3Test::TestRigidityFloat(void) {
    TestRigidity<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestIdentity(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    RotationType matrixRotation;
    VectorType inputVector;
    VectorType outputVector;
    vctRandom(matrixRotation);
    vctRandom(inputVector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestIdentity(matrixRotation, inputVector, outputVector);
}

void vctMatrixRotation3Test::TestIdentityDouble(void) {
    TestIdentity<double>();
}

void vctMatrixRotation3Test::TestIdentityFloat(void) {
    TestIdentity<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestApplyTo(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    value_type tolerance = cmnTypeTraits<value_type>::Tolerance();

    VectorType x(1.0, 0.0, 0.0);
    VectorType y(0.0, 1.0, 0.0);
    VectorType z(0.0, 0.0, 1.0);
    VectorType result;
    RotationType composed;

    RotationType testRotation(vctAxisAngleRotation3<value_type>(x, static_cast<value_type>(cmnPI_2)));
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).LinfNorm() < tolerance);
    testRotation.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).LinfNorm() < tolerance);
    testRotation.ApplyTo(RotationType::Identity(), composed);
    composed.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).LinfNorm() < tolerance);
    composed.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).LinfNorm() < tolerance);

    testRotation.From(vctAxisAngleRotation3<value_type>(y, static_cast<value_type>(cmnPI_2)));
    testRotation.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).LinfNorm() < tolerance);
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).LinfNorm() < tolerance);
    testRotation.ApplyTo(RotationType::Identity(), composed);
    composed.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).LinfNorm() < tolerance);
    composed.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).LinfNorm() < tolerance);

    testRotation.From(vctAxisAngleRotation3<value_type>(z, static_cast<value_type>(cmnPI_2)));
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).LinfNorm() < tolerance);
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).LinfNorm() < tolerance);
    testRotation.ApplyTo(RotationType::Identity(), composed);
    composed.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).LinfNorm() < tolerance);
    composed.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).LinfNorm() < tolerance);

    testRotation.ApplyTo(testRotation.Inverse(), composed);
    CPPUNIT_ASSERT((composed - RotationType::Identity()).LinfNorm() < tolerance);
}

void vctMatrixRotation3Test::TestApplyToDouble(void) {
    TestApplyTo<double>();
}

void vctMatrixRotation3Test::TestApplyToFloat(void) {
    TestApplyTo<float>();
}



template <class _elementType>
void vctMatrixRotation3Test::TestApplyMethodsOperators(void) {
    typedef _elementType value_type;
    typedef vctMatrixRotation3<value_type> RotationType;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    RotationType matrixRotation;
    vctRandom(matrixRotation);

    VectorType vector;
    vctRandom(vector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(matrixRotation, vector);

    RotationType rotation;
    vctRandom(rotation);
    vctGenericRotationTest::TestApplyMethodsOperatorsXform(matrixRotation, rotation);
}

void vctMatrixRotation3Test::TestApplyMethodsOperatorsDouble(void) {
    TestApplyMethodsOperators<double>();
}

void vctMatrixRotation3Test::TestApplyMethodsOperatorsFloat(void) {
    TestApplyMethodsOperators<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctMatrixRotation3Test);
