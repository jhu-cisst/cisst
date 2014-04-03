/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2004-02-11

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctFrameBaseTest.h"
#include "vctGenericRotationTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctRandomTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicVector.h>
#include <cisstVector/vctRandomDynamicMatrix.h>


template <class _elementType>
void vctFrameBaseTest::TestConstructors3(void)
{
    typedef vctFixedSizeVector<_elementType, 3> vectorType;

    vectorType vector(1.0, 3.0, -2.0);
    vector.Divide(_elementType(vector.Norm()));
    vectorType noTranslation(0.0, 0.0, 0.0);
    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();

    typedef vctMatrixRotation3<_elementType> MatRotType;
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    vctFrameBase<MatRotType> matrixFrame;
    vctFrameBase<QuatRotType> quaternionFrame;

    vctGenericRotationTest::TestDefaultConstructor(matrixFrame);
    vctGenericRotationTest::TestDefaultConstructor(quaternionFrame);

    CPPUNIT_ASSERT((matrixFrame.Rotation() - MatRotType::Identity() ).LinfNorm() == 0);
    CPPUNIT_ASSERT((matrixFrame.Translation() - noTranslation).LinfNorm() < tolerance);

    CPPUNIT_ASSERT((quaternionFrame.Rotation() - QuatRotType::Identity() ).LinfNorm() < tolerance);
    CPPUNIT_ASSERT((quaternionFrame.Translation() - noTranslation).LinfNorm() < tolerance);

    MatRotType matrixRotation(vctAxisAngleRotation3<_elementType>(vector, _elementType(cmnPI_2)));
    QuatRotType quaternionRotation(vctAxisAngleRotation3<_elementType>(vector, _elementType(cmnPI_2)));

    vctFrameBase<MatRotType> matrixFrameB(matrixRotation, vector);
    vctFrameBase<QuatRotType> quaternionFrameB(quaternionRotation, vector);

    CPPUNIT_ASSERT((matrixFrameB.Rotation() - matrixRotation).LinfNorm() < tolerance);
    CPPUNIT_ASSERT((matrixFrameB.Translation() - vector).LinfNorm() < tolerance);

    CPPUNIT_ASSERT((quaternionFrameB.Rotation() - quaternionRotation).LinfNorm() < tolerance);
    CPPUNIT_ASSERT((quaternionFrameB.Translation() - vector).LinfNorm() < tolerance);
}


void vctFrameBaseTest::TestConstructorsDouble3(void)
{
    TestConstructors3<double>();
}

void vctFrameBaseTest::TestConstructorsFloat3(void)
{
    TestConstructors3<float>();
}


template <class _rotationType>
void vctFrameBaseTest::TestApplyTo3(void)
{
    typedef typename _rotationType::value_type value_type;
    value_type tolerance = cmnTypeTraits<value_type>::Tolerance();

    vctFixedSizeVector<value_type, 3> x(1.0, 0.0, 0.0);
    vctFixedSizeVector<value_type, 3> y(0.0, 1.0, 0.0);
    vctFixedSizeVector<value_type, 3> z(0.0, 0.0, 1.0);
    vctFixedSizeVector<value_type, 3> result;
    vctFrameBase<_rotationType> composed;

    vctFrameBase<_rotationType> testFrame;
    testFrame.Rotation().From(vctAxisAngleRotation3<value_type>(x, value_type(cmnPI_2)));
    testFrame.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).Norm() < tolerance);
    testFrame.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).Norm() < tolerance);
    testFrame.ApplyTo(vctFrameBase<_rotationType>::Identity(), composed);
    composed.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).Norm() < tolerance);
    composed.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).Norm() < tolerance);

    testFrame.Rotation().From(vctAxisAngleRotation3<value_type>(y, value_type(cmnPI_2)));
    testFrame.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).Norm() < tolerance);
    testFrame.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).Norm() < tolerance);
    testFrame.ApplyTo(vctFrameBase<_rotationType>::Identity(), composed);
    testFrame.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).Norm() < tolerance);
    testFrame.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).Norm() < tolerance);

    testFrame.Rotation().From(vctAxisAngleRotation3<value_type>(z, value_type(cmnPI_2)));
    testFrame.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).Norm() < tolerance);
    testFrame.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).Norm() < tolerance);
    testFrame.ApplyTo(vctFrameBase<_rotationType>::Identity(), composed);
    testFrame.Rotation().From(vctAxisAngleRotation3<value_type>(z, value_type(cmnPI_2)));
    testFrame.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).Norm() < tolerance);
    testFrame.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).Norm() < tolerance);
}


void vctFrameBaseTest::TestApplyToDouble3(void)
{
    TestApplyTo3<vctDoubleMatRot3>();
    TestApplyTo3<vctDoubleQuatRot3>();
}

void vctFrameBaseTest::TestApplyToFloat3(void)
{
    TestApplyTo3<vctDoubleMatRot3>();
    TestApplyTo3<vctDoubleQuatRot3>();
}



template <class _elementType>
void vctFrameBaseTest::TestInverse3(void)
{
    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();
    typedef vctFixedSizeVector<_elementType, 3> vectorType;
    typedef vctMatrixRotation3<_elementType> MatRotType;
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<_elementType, 4> > QuatRotType;
    typedef vctFrameBase<MatRotType> matrixFrameType;
    typedef vctFrameBase<QuatRotType> quaternionFrameType;

    vectorType x(1.0, 0.0, 0.0);
    vectorType y(0.0, 1.0, 0.0);
    vectorType z(0.0, 0.0, 1.0);
    vectorType axis(_elementType(1.3), _elementType(0.3), _elementType(-1.0));
    axis.Divide(_elementType(axis.Norm()));

    vectorType result, temp;
    matrixFrameType mfTest, mfProduct /*, mfResult*/;
    quaternionFrameType qfTest, qfProduct /*, qfResult*/;

    _elementType angle = 0.0;

    int counter = 0;
    // try different angles and axis
    while (angle <= _elementType(2.0 * cmnPI)) {
        angle = _elementType(counter * cmnPI / 10.0);
        axis[counter % 3] += _elementType(counter / 10.0);
        counter++;

        mfTest.Rotation() = MatRotType(vctAxisAngleRotation3<_elementType>(axis / _elementType(axis.Norm()),
                                                                           angle,
                                                                           VCT_NORMALIZE));
        mfTest.Translation() = axis;
        // test for x
        CPPUNIT_ASSERT(((mfTest.Inverse() * (mfTest * x)) - x).LinfNorm() < tolerance);
        mfTest.ApplyTo(x, temp);
        mfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - x).LinfNorm() < tolerance);
        // test for y
        CPPUNIT_ASSERT(((mfTest.Inverse() * (mfTest * y)) - y).LinfNorm() < tolerance);
        mfTest.ApplyTo(y, temp);
        mfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - y).LinfNorm() < tolerance);
        // test for z
        CPPUNIT_ASSERT(((mfTest.Inverse() * (mfTest * z)) - z).LinfNorm() < tolerance);
        mfTest.ApplyTo(x, temp);
        mfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - x).LinfNorm() < tolerance);
        // test composition
        mfProduct = mfTest * mfTest.Inverse();
        CPPUNIT_ASSERT(mfProduct.Translation().LinfNorm() < tolerance);
        CPPUNIT_ASSERT((mfProduct.Rotation() - matrixFrameType::RotationType::Identity()).LinfNorm() < tolerance);
        qfTest.Rotation() = QuatRotType(vctAxisAngleRotation3<_elementType>(axis / _elementType(axis.Norm()),
                                                                            angle,
                                                                            VCT_NORMALIZE));
        qfTest.Translation() = axis;
        // test for x
        CPPUNIT_ASSERT(((qfTest.Inverse() * (qfTest * x)) - x).LinfNorm() < tolerance);
        qfTest.ApplyTo(x, temp);
        qfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - x).LinfNorm() < tolerance);
        // test for y
        CPPUNIT_ASSERT(((qfTest.Inverse() * (qfTest * y)) - y).LinfNorm() < tolerance);
        qfTest.ApplyTo(y, temp);
        qfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - y).LinfNorm() < tolerance);
        // test for z
        CPPUNIT_ASSERT(((qfTest.Inverse() * (qfTest * z)) - z).LinfNorm() < tolerance);
        qfTest.ApplyTo(z, temp);
        qfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - z).LinfNorm() < tolerance);
        // test composition
        qfProduct = qfTest * qfTest.Inverse();
        CPPUNIT_ASSERT(qfProduct.Translation().LinfNorm() < tolerance);
        CPPUNIT_ASSERT((qfProduct.Rotation() - quaternionFrameType::RotationType::Identity()).LinfNorm() < tolerance);
    }
}

void vctFrameBaseTest::TestInverseDouble3(void)
{
    TestInverse3<double>();
}

void vctFrameBaseTest::TestInverseFloat3(void)
{
    TestInverse3<float>();
}



template <class _elementType>
void vctFrameBaseTest::TestApplyMethodsOperators3(void)
{
    typedef _elementType value_type;
    typedef vctMatrixRotation3Base<vctFixedSizeMatrix<value_type, 3, 3> > MatRotType;
    typedef vctQuaternionRotation3Base<vctFixedSizeVector<value_type, 4> > QuatRotType;
    typedef vctFrameBase<MatRotType> matrixFrameType;
    typedef vctFrameBase<QuatRotType> quaternionFrameType;

    vctFixedSizeVector<value_type, matrixFrameType::DIMENSION> fixedVector;
    vctRandom(fixedVector, value_type(-1.0), value_type(1.0));
    vctDynamicVector<_elementType> dynamicVector(matrixFrameType::DIMENSION);
    vctRandom(dynamicVector, value_type(-1.0), value_type(1.0));

    const unsigned int dynamicMatrixWidth = cmnRandomSequence::GetInstance().ExtractRandomInt(1, 21);
    vctDynamicMatrix<value_type> dynamicMatrixRowMajor(3, dynamicMatrixWidth, VCT_ROW_MAJOR);
    vctDynamicMatrix<value_type> dynamicMatrixColMajor(3, dynamicMatrixWidth, VCT_COL_MAJOR);
    vctRandom(dynamicMatrixRowMajor, value_type(-4.0), value_type(4.0));
    vctRandom(dynamicMatrixColMajor, value_type(-4.0), value_type(4.0));

    enum {FIXED_MATRIX_WIDTH = 6};
    vctFixedSizeMatrix<value_type, 3, FIXED_MATRIX_WIDTH, VCT_ROW_MAJOR> fixedMatrixRowMajor;
    vctFixedSizeMatrix<value_type, 3, FIXED_MATRIX_WIDTH, VCT_COL_MAJOR> fixedMatrixColMajor;
    vctRandom(fixedMatrixRowMajor, value_type(-4.0), value_type(4.0));
    vctRandom(fixedMatrixColMajor, value_type(-4.0), value_type(4.0));

    matrixFrameType matrixFrame1;
    vctRandom(matrixFrame1.Rotation());
    vctRandom(matrixFrame1.Translation(), value_type(-1.0), value_type(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(matrixFrame1, fixedVector);
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(matrixFrame1,
                                                          fixedMatrixRowMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(matrixFrame1,
                                                          fixedMatrixColMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(matrixFrame1, dynamicVector);
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(matrixFrame1,
                                                          dynamicMatrixRowMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(matrixFrame1,
                                                          dynamicMatrixColMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());

    matrixFrameType matrixFrame2;
    vctRandom(matrixFrame2.Rotation());
    vctRandom(matrixFrame2.Translation(), value_type(-1.0), value_type(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsXform(matrixFrame1, matrixFrame2);

    quaternionFrameType quaternionFrame1;
    vctRandom(quaternionFrame1.Rotation());
    vctRandom(quaternionFrame1.Translation(), value_type(-1.0), value_type(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(quaternionFrame1, fixedVector);
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(quaternionFrame1,
                                                          fixedMatrixRowMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(quaternionFrame1,
                                                          fixedMatrixColMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(quaternionFrame1, dynamicVector);
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(quaternionFrame1,
                                                          dynamicMatrixRowMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());
    vctGenericRotationTest::TestApplyMethodsNoReturnValue(quaternionFrame1,
                                                          dynamicMatrixColMajor,
                                                          cmnTypeTraits<value_type>::Tolerance());

    quaternionFrameType quaternionFrame2;
    vctRandom(quaternionFrame2.Rotation());
    vctRandom(quaternionFrame2.Translation(), value_type(-1.0), value_type(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsXform(quaternionFrame1, quaternionFrame2);
}


void vctFrameBaseTest::TestApplyMethodsOperatorsDouble3(void)
{
    TestApplyMethodsOperators3<double>();
}

void vctFrameBaseTest::TestApplyMethodsOperatorsFloat3(void)
{
    TestApplyMethodsOperators3<float>();
}



template <class _elementType>
void vctFrameBaseTest::TestConstructors2(void)
{
    typedef vctFixedSizeVector<_elementType, 2> vectorType;

    vectorType vector(1.0, -2.0);
    vector.Divide(_elementType(vector.Norm()));
    vectorType noTranslation(0.0, 0.0);
    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();

    typedef vctMatrixRotation2<_elementType> MatRotType;
    vctFrameBase<MatRotType> matrixFrame;

    vctGenericRotationTest::TestDefaultConstructor(matrixFrame);

    CPPUNIT_ASSERT((matrixFrame.Rotation() - MatRotType::Identity() ).LinfNorm() == 0);
    CPPUNIT_ASSERT((matrixFrame.Translation() - noTranslation).LinfNorm() < tolerance);

    MatRotType matrixRotation;
    matrixRotation.From(vctAngleRotation2(cmnPI_2));

    vctFrameBase<MatRotType> matrixFrameB(matrixRotation, vector);

    CPPUNIT_ASSERT((matrixFrameB.Rotation() - matrixRotation).LinfNorm() < tolerance);
    CPPUNIT_ASSERT((matrixFrameB.Translation() - vector).LinfNorm() < tolerance);
}


void vctFrameBaseTest::TestConstructorsDouble2(void)
{
    TestConstructors2<double>();
}

void vctFrameBaseTest::TestConstructorsFloat2(void)
{
    TestConstructors2<float>();
}



template <class _elementType>
void vctFrameBaseTest::TestInverse2(void)
{
    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();
    typedef vctFixedSizeVector<_elementType, 2> vectorType;
    typedef vctMatrixRotation2<_elementType> MatRotType;
    typedef vctFrameBase<MatRotType> matrixFrameType;

    vectorType x(1.0, 0.0);
    vectorType y(0.0, 1.0);
    vectorType axis(_elementType(1.2), _elementType(0.2));
    axis.Divide(_elementType(axis.Norm()));

    vectorType result, temp;
    matrixFrameType mfTest, mfProduct;

    _elementType angle = 0.0;

    int counter = 0;
    // try different angles and axis
    while (angle <= _elementType(2.0 * cmnPI)) {
        angle = _elementType(counter * cmnPI / 10.0);
        axis[counter % 2] += _elementType(counter / 10.0);
        counter++;

        mfTest.Rotation() = MatRotType(vctAngleRotation2(angle, VCT_NORMALIZE));
        mfTest.Translation() = axis;
        // test for x
        CPPUNIT_ASSERT(((mfTest.Inverse() * (mfTest * x)) - x).LinfNorm() < tolerance);
        mfTest.ApplyTo(x, temp);
        mfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - x).LinfNorm() < tolerance);
        // test for y
        CPPUNIT_ASSERT(((mfTest.Inverse() * (mfTest * y)) - y).LinfNorm() < tolerance);
        mfTest.ApplyTo(y, temp);
        mfTest.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - y).LinfNorm() < tolerance);
        // test composition
        mfProduct = mfTest * mfTest.Inverse();
        CPPUNIT_ASSERT(mfProduct.Translation().LinfNorm() < tolerance);
        CPPUNIT_ASSERT((mfProduct.Rotation() - matrixFrameType::RotationType::Identity()).LinfNorm() < tolerance);
    }
}

void vctFrameBaseTest::TestInverseDouble2(void)
{
    TestInverse2<double>();
}

void vctFrameBaseTest::TestInverseFloat2(void)
{
    TestInverse2<float>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctFrameBaseTest);

