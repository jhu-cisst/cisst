/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2007-09-13

  (C) Copyright 2007-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctFrame4x4Test.h"
#include "vctGenericRotationTest.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctFrame4x4.h>
#include <cisstVector/vctRandomTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>


template <class _elementType, bool _rowMajor>
void vctFrame4x4Test::TestConstructors(void) {
    typedef vctFrame4x4<_elementType, _rowMajor> FrameType;
    typedef typename FrameType::value_type value_type;
    typedef typename FrameType::RotationRefType RotationRefType;

    /* test default constructor */
    FrameType frame;
    vctGenericRotationTest::TestDefaultConstructor(frame);

    /* test that references are set correctly */
    CPPUNIT_ASSERT(frame.Rotation().Equal(RotationRefType::Identity()));
    CPPUNIT_ASSERT(frame.Translation().Equal(static_cast<value_type>(0.0)));
    CPPUNIT_ASSERT(frame.Perspective().Equal(static_cast<value_type>(0.0)));
    CPPUNIT_ASSERT(frame[3][3] == static_cast<value_type>(1.0));

    unsigned int i, j;
    for (i = 0; i < 3; i++) {
        CPPUNIT_ASSERT(&(frame.Translation().Element(i)) == &(frame.Element(i, 3)));
        CPPUNIT_ASSERT(&(frame.Perspective().Element(i)) == &(frame.Element(3, i)));
        for (j = 0; j < 3; j++) {
            CPPUNIT_ASSERT(&(frame.Rotation().Element(i, j)) == &(frame.Element(i, j)));
        }
    }

    /* test constructor from rotation matrix and translation */
    vctMatrixRotation3<value_type> rotation;
    vctFixedSizeVector<value_type, 3> translation;
    translation.Zeros();
    FrameType frameRT(rotation, translation);
    vctGenericRotationTest::TestDefaultConstructor(frameRT);

    /* test default constructor + assign identity to rotation and or translation */
    frameRT.Assign(FrameType::Identity());
    frameRT.Rotation().Assign(rotation);
    vctGenericRotationTest::TestDefaultConstructor(frameRT);

    frameRT.Assign(FrameType::Identity());
    frameRT.Translation().Assign(translation);
    vctGenericRotationTest::TestDefaultConstructor(frameRT);
}

void vctFrame4x4Test::TestConstructorsDouble(void) {
    TestConstructors<double, VCT_ROW_MAJOR>();
    TestConstructors<double, VCT_COL_MAJOR>();
}

void vctFrame4x4Test::TestConstructorsFloat(void) {
    TestConstructors<float, VCT_ROW_MAJOR>();
    TestConstructors<float, VCT_COL_MAJOR>();
}



template <class _elementType, bool _rowMajor>
void vctFrame4x4Test::TestApplyTo(void) {
    typedef vctFrame4x4<_elementType, _rowMajor> FrameType;
    typedef typename FrameType::value_type value_type;

    value_type tolerance = cmnTypeTraits<value_type>::Tolerance();
    vctFixedSizeVector<value_type, 3> noTranslation(0.0, 0.0, 0.0);
    vctFixedSizeVector<value_type, 3> x(1.0, 0.0, 0.0);
    vctFixedSizeVector<value_type, 3> y(0.0, 1.0, 0.0);
    vctFixedSizeVector<value_type, 3> z(0.0, 0.0, 1.0);
    vctFixedSizeVector<value_type, 3> result;
    FrameType composed;

    FrameType testRotation(vctAxisAngleRotation3<value_type>(x, value_type(cmnPI_2)), noTranslation);
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).Norm() < tolerance);
    testRotation.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).Norm() < tolerance);
    testRotation.ApplyTo(FrameType::Identity(), composed);
    composed.ApplyTo(y, result);
    CPPUNIT_ASSERT((z - result).Norm() < tolerance);
    composed.ApplyTo(z, result);
    CPPUNIT_ASSERT((y + result).Norm() < tolerance);

    testRotation.Rotation().From(vctAxisAngleRotation3<value_type>(y, value_type(cmnPI_2)));
    testRotation.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).Norm() < tolerance);
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).Norm() < tolerance);
    testRotation.ApplyTo(FrameType::Identity(), composed);
    testRotation.ApplyTo(z, result);
    CPPUNIT_ASSERT((x - result).Norm() < tolerance);
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((z + result).Norm() < tolerance);

    testRotation.Rotation().From(vctAxisAngleRotation3<value_type>(z, value_type(cmnPI_2)));
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).Norm() < tolerance);
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).Norm() < tolerance);
    testRotation.ApplyTo(FrameType::Identity(), composed);
    testRotation.From(vctAxisAngleRotation3<value_type>(z, value_type(cmnPI_2)), noTranslation);
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((y - result).Norm() < tolerance);
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((x + result).Norm() < tolerance);
}

void vctFrame4x4Test::TestApplyToDouble(void) {
    TestApplyTo<double, VCT_ROW_MAJOR>();
    TestApplyTo<double, VCT_COL_MAJOR>();
}

void vctFrame4x4Test::TestApplyToFloat(void) {
    TestApplyTo<float, VCT_ROW_MAJOR>();
    TestApplyTo<float, VCT_COL_MAJOR>();
}



template <class _elementType, bool _rowMajor>
void vctFrame4x4Test::TestInverse(void) {
    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();
    typedef vctFixedSizeVector<_elementType, 3> vectorType;
    typedef vctFrame4x4<_elementType, _rowMajor> FrameType;

    vectorType x(1.0, 0.0, 0.0);
    vectorType y(0.0, 1.0, 0.0);
    vectorType z(0.0, 0.0, 1.0);
    vectorType axis(_elementType(1.3), _elementType(0.3), _elementType(-1.0));
    axis.Divide(_elementType(axis.Norm()));

    vectorType result, temp;
    FrameType test, product;

    _elementType angle = 0.0;

    int counter = 0;
    // try different angles and axis
    while (angle <= _elementType(2.0 * cmnPI)) {
        angle = _elementType(counter * cmnPI / 10.0);
        axis[counter % 3] += _elementType(counter / 10.0);
        counter++;

        test.Rotation().From(vctAxisAngleRotation3<_elementType>(axis / _elementType(axis.Norm()),
                                                                   angle,
                                                                   VCT_NORMALIZE));
        test.Translation() = axis;
        // test for x
        CPPUNIT_ASSERT(((test.Inverse() * (test * x)) - x).LinfNorm() < tolerance);
        test.ApplyTo(x, temp);
        test.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - x).LinfNorm() < tolerance);
        // test for y
        CPPUNIT_ASSERT(((test.Inverse() * (test * y)) - y).LinfNorm() < tolerance);
        test.ApplyTo(y, temp);
        test.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - y).LinfNorm() < tolerance);
        // test for z
        CPPUNIT_ASSERT(((test.Inverse() * (test * z)) - z).LinfNorm() < tolerance);
        test.ApplyTo(x, temp);
        test.ApplyInverseTo(temp, result);
        CPPUNIT_ASSERT((result - x).LinfNorm() < tolerance);
        // test composition
        product = test * test.Inverse();
        CPPUNIT_ASSERT(product.Translation().LinfNorm() < tolerance);
        CPPUNIT_ASSERT((product.Rotation() - FrameType::RotationRefType::Identity()).LinfNorm() < tolerance);
    }
}

void vctFrame4x4Test::TestInverseDouble(void) {
    TestInverse<double, VCT_ROW_MAJOR>();
    TestInverse<double, VCT_COL_MAJOR>();
}

void vctFrame4x4Test::TestInverseFloat(void) {
    TestInverse<float, VCT_ROW_MAJOR>();
    TestInverse<float, VCT_COL_MAJOR>();
}



template <class _elementType, bool _rowMajor>
void vctFrame4x4Test::TestRigidity(void) {
    typedef vctFrame4x4<_elementType, _rowMajor> FrameType;
    typedef typename FrameType::value_type value_type;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    VectorType vector1;
    VectorType vector2;
    FrameType frame;
    typename FrameType::RotationRefType rotation = frame.Rotation();
    typename FrameType::TranslationRefType translation = frame.Translation();
    vctRandom(rotation);
    vctRandom(translation, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctRandom(vector1, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctRandom(vector2, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestRigidity(frame, vector1, vector2);
}

void vctFrame4x4Test::TestRigidityDouble(void) {
    TestRigidity<double, VCT_ROW_MAJOR>();
    TestRigidity<double, VCT_COL_MAJOR>();
}

void vctFrame4x4Test::TestRigidityFloat(void) {
    TestRigidity<float, VCT_ROW_MAJOR>();
    TestRigidity<float, VCT_COL_MAJOR>();
}



template <class _elementType, bool _rowMajor>
void vctFrame4x4Test::TestComposition(void) {
    typedef vctFrame4x4<_elementType, _rowMajor> FrameType;
    typedef typename FrameType::value_type value_type;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    FrameType frame1;
    FrameType frame2;
    VectorType inputVector;
    typename FrameType::RotationRefType rotation1 = frame1.Rotation();
    typename FrameType::TranslationRefType translation1 = frame1.Translation();
    vctRandom(rotation1);
    vctRandom(translation1, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    typename FrameType::RotationRefType rotation2 = frame2.Rotation();
    typename FrameType::TranslationRefType translation2 = frame2.Translation();
    vctRandom(rotation2);
    vctRandom(translation2, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctRandom(inputVector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestComposition(frame1, frame2, inputVector);
}

void vctFrame4x4Test::TestCompositionDouble(void) {
    TestComposition<double, VCT_ROW_MAJOR>();
    TestComposition<double, VCT_COL_MAJOR>();
}

void vctFrame4x4Test::TestCompositionFloat(void) {
    TestComposition<float, VCT_ROW_MAJOR>();
    TestComposition<float, VCT_COL_MAJOR>();
}



template <class _elementType, bool _rowMajor>
void vctFrame4x4Test::TestIdentity(void) {
    typedef vctFrame4x4<_elementType, _rowMajor> FrameType;
    typedef typename FrameType::value_type value_type;
    typedef vctFixedSizeVector<value_type, 3> VectorType;

    FrameType frame;
    VectorType inputVector;
    VectorType outputVector;
    typename FrameType::RotationRefType rotation = frame.Rotation();
    typename FrameType::TranslationRefType translation = frame.Translation();
    vctRandom(rotation);
    vctRandom(translation, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctRandom(inputVector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestIdentity(frame, inputVector, outputVector);
}

void vctFrame4x4Test::TestIdentityDouble(void) {
    TestIdentity<double, VCT_ROW_MAJOR>();
    TestIdentity<double, VCT_COL_MAJOR>();
}

void vctFrame4x4Test::TestIdentityFloat(void) {
    TestIdentity<float, VCT_ROW_MAJOR>();
    TestIdentity<float, VCT_COL_MAJOR>();
}



template <class _elementType, bool _rowMajor>
void vctFrame4x4Test::TestApplyMethodsOperators(void) {
    typedef vctFixedSizeVector<_elementType, 3> VectorType;
    typedef vctFrame4x4<_elementType, _rowMajor> FrameType;

    VectorType vector;
    vctRandom(vector, _elementType(-1.0), _elementType(1.0));

    FrameType frame1;
    typename FrameType::RotationRefType rotation1 = frame1.Rotation();
    typename FrameType::TranslationRefType translation1 = frame1.Translation();
    vctRandom(rotation1);
    vctRandom(translation1, _elementType(-1.0), _elementType(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(frame1, vector);

    FrameType frame2;
    typename FrameType::RotationRefType rotation2 = frame2.Rotation();
    typename FrameType::TranslationRefType translation2 = frame2.Translation();
    vctRandom(rotation2);
    vctRandom(translation2, _elementType(-1.0), _elementType(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsXform(frame1, frame2);
}

void vctFrame4x4Test::TestApplyMethodsOperatorsDouble(void) {
    TestApplyMethodsOperators<double, VCT_ROW_MAJOR>();
    TestApplyMethodsOperators<double, VCT_COL_MAJOR>();
}

void vctFrame4x4Test::TestApplyMethodsOperatorsFloat(void) {
    TestApplyMethodsOperators<float, VCT_ROW_MAJOR>();
    TestApplyMethodsOperators<float, VCT_COL_MAJOR>();
}

template <class _elementType>
void vctFrame4x4Test::TestConversionFrame3Matrix(void) {
    vctFrameBase<vctMatrixRotation3<_elementType> > frame3;
    vctFrame4x4<_elementType> frame4x4;
    vctRandom(frame3.Rotation());
    vctRandom(frame3.Translation(), static_cast<_elementType>(-1.0), static_cast<_elementType>(1.0));
    vctGenericRotationTest::TestConversion(frame3, frame4x4);
}

void vctFrame4x4Test::TestConversionFrame3MatrixDouble(void) {
    TestConversionFrame3Matrix<double>();
}

void vctFrame4x4Test::TestConversionFrame3MatrixFloat(void) {
    TestConversionFrame3Matrix<float>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctFrame4x4Test);
