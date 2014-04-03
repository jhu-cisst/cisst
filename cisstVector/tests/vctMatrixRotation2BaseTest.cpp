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


#include "vctMatrixRotation2BaseTest.h"
#include "vctGenericRotationTest.h"

#include <cisstVector/vctRandomTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomDynamicVector.h>
#include <cisstVector/vctMatrixRotation2Base.h>
#include <cisstVector/vctAngleRotation2.h>


template <class _rotationType, class _vectorType>
void vctMatrixRotation2BaseTest::TestConstructors(const _vectorType & aVector) {

    typedef _rotationType RotationType;
    typedef _vectorType VectorType;
    typedef typename RotationType::value_type value_type;

    unsigned int row, column;

    // for all vector, use copy constructor just to ensure dynamic
    // allocation if needed.
    VectorType x(aVector);  x.Assign(1.0, 0.0);
    VectorType y(aVector);  y.Assign(0.0, 1.0);
    VectorType difference(aVector);

    RotationType testRotation1;
    
    value_type tolerance = cmnTypeTraits<value_type>::Tolerance();
    
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

void vctMatrixRotation2BaseTest::TestConstructorsFixedSizeDouble(void) {
    vctFixedSizeVector<double, 2> aVector;
    TestConstructors<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestConstructorsFixedSizeFloat(void) {
    vctFixedSizeVector<float, 2> aVector;
    TestConstructors<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestConstructorsDynamicDouble(void) {
    vctDynamicVector<double> aVector(2);
    TestConstructors<vctMatrixRotation2Base<vctDynamicMatrix<double> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestConstructorsDynamicFloat(void) {
    vctDynamicVector<float> aVector(2);
    TestConstructors<vctMatrixRotation2Base<vctDynamicMatrix<float> > >(aVector);
}




template <class _rotationType, class _vectorType>
void vctMatrixRotation2BaseTest::TestApplyTo(const _vectorType & aVector) {
    typedef _rotationType RotationType;
    typedef _vectorType VectorType;
    typedef typename RotationType::value_type value_type;

    value_type tolerance = cmnTypeTraits<value_type>::Tolerance();

    VectorType x(aVector); x.Assign(1.0, 0.0);
    VectorType y(aVector); y.Assign(0.0, 1.0);
    VectorType result(aVector);

    RotationType composed;

    RotationType testRotation((vctAngleRotation2)(cmnPI_2));
    testRotation.ApplyTo(y, result);
    CPPUNIT_ASSERT((x - result).LinfNorm() < tolerance);
    testRotation.ApplyTo(x, result);
    CPPUNIT_ASSERT((y + result).LinfNorm() < tolerance);
    testRotation.ApplyTo(RotationType::Identity(), composed);
    composed.ApplyTo(y, result);
    CPPUNIT_ASSERT((x - result).LinfNorm() < tolerance);
    composed.ApplyTo(x, result);
    CPPUNIT_ASSERT((y + result).LinfNorm() < tolerance);

    testRotation.ApplyTo(testRotation.Inverse(), composed);
    CPPUNIT_ASSERT((composed - RotationType::Identity()).LinfNorm() < tolerance);
}

void vctMatrixRotation2BaseTest::TestApplyToFixedSizeDouble(void) {
    vctFixedSizeVector<double, 2> aVector;
    TestApplyTo<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestApplyToFixedSizeFloat(void) {
    vctFixedSizeVector<float, 2> aVector;
    TestApplyTo<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestApplyToDynamicDouble(void) {
    vctDynamicVector<double> aVector(2);
    TestApplyTo<vctMatrixRotation2Base<vctDynamicMatrix<double> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestApplyToDynamicFloat(void) {
    vctDynamicVector<float> aVector(2);
    TestApplyTo<vctMatrixRotation2Base<vctDynamicMatrix<float> > >(aVector);
}



template <class _rotationType>
void vctMatrixRotation2BaseTest::TestNormalize(void) {
    // this test proves that the matrix is a valid rotation matrix
    // after normalization.  It doesn't test that the matrix is the
    // best normalized one.

    typedef _rotationType RotationType;
    typedef typename RotationType::value_type value_type;
    RotationType m1;
    RotationType m2;
    
    value_type tolerance = cmnTypeTraits<value_type>::Tolerance();
    unsigned int column, row;

    CPPUNIT_ASSERT(m1.IsNormalized());

    m1[0][1] = static_cast<value_type>(5);
    CPPUNIT_ASSERT(!m1.IsNormalized());

    m1 = RotationType::Identity();
    CPPUNIT_ASSERT(m1.IsNormalized());

    m1[0][0] += static_cast<value_type>(100 * tolerance);
    m1[1][0] += static_cast<value_type>(100 * tolerance);
    CPPUNIT_ASSERT(!m1.IsNormalized());
    
    m2.NormalizedOf(m1);
    CPPUNIT_ASSERT(m2.IsNormalized());
    
    m1.NormalizedSelf();
    for (row = 0; row < 2; row++) {
        for (column = 0; column < 2; column++) {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(m1[row][column], m2[row][column], tolerance);
        }
    }
}

void vctMatrixRotation2BaseTest::TestNormalizeFixedSizeDouble(void) {
    TestNormalize<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestNormalizeFixedSizeFloat(void) {
    TestNormalize<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestNormalizeDynamicDouble(void) {
    TestNormalize<vctMatrixRotation2Base<vctDynamicMatrix<double> > >();
}

void vctMatrixRotation2BaseTest::TestNormalizeDynamicFloat(void) {
    TestNormalize<vctMatrixRotation2Base<vctDynamicMatrix<float> > >();
}



template <class _rotationType>
void vctMatrixRotation2BaseTest::TestFromVectors(void) {
    typedef _rotationType RotationType;

    RotationType matrix;
    vctRandom(matrix);
    
    RotationType matrixByColumn(matrix.Column(0), matrix.Column(1));
    CPPUNIT_ASSERT(matrix == matrixByColumn);
    
    matrixByColumn.From(matrix.Column(0), matrix.Column(1));
    CPPUNIT_ASSERT(matrix == matrixByColumn);
    
    RotationType matrixByRow(matrix.Row(0), matrix.Row(1), false);
    CPPUNIT_ASSERT(matrix == matrixByRow);
    CPPUNIT_ASSERT(matrixByColumn == matrixByRow);
    
    matrixByRow.From(matrix.Row(0), matrix.Row(1), false);
    CPPUNIT_ASSERT(matrix == matrixByRow);
    CPPUNIT_ASSERT(matrixByColumn == matrixByRow);
}

void vctMatrixRotation2BaseTest::TestFromVectorsFixedSizeDouble(void) {
    TestFromVectors<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestFromVectorsFixedSizeFloat(void) {
    TestFromVectors<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestFromVectorsDynamicDouble(void) {
    TestFromVectors<vctMatrixRotation2Base<vctDynamicMatrix<double> > >();
}

void vctMatrixRotation2BaseTest::TestFromVectorsDynamicFloat(void) {
    TestFromVectors<vctMatrixRotation2Base<vctDynamicMatrix<float> > >();
}



template <class _rotationType>
void vctMatrixRotation2BaseTest::TestConversionAngle(void) {
    typedef _rotationType RotationType;
    RotationType matrixRotation;
    vctAngleRotation2 angleRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestConversion(matrixRotation, angleRotation);
}

void vctMatrixRotation2BaseTest::TestConversionAngleFixedSizeDouble(void) {
    TestConversionAngle<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestConversionAngleFixedSizeFloat(void) {
    TestConversionAngle<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestConversionAngleDynamicDouble(void) {
    TestConversionAngle<vctMatrixRotation2Base<vctDynamicMatrix<double> > >();
}

void vctMatrixRotation2BaseTest::TestConversionAngleDynamicFloat(void) {
    TestConversionAngle<vctMatrixRotation2Base<vctDynamicMatrix<float> > >();
}



template <class _rotationType, class _vectorType>
void vctMatrixRotation2BaseTest::TestIdentity(const _vectorType & aVector) {
    typedef _rotationType RotationType;
    typedef _vectorType VectorType;
    typedef typename RotationType::value_type value_type;
    RotationType matrixRotation;
    VectorType inputVector(aVector);
    VectorType outputVector(aVector);
    vctRandom(matrixRotation);
    vctRandom(inputVector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestIdentity(matrixRotation, inputVector, outputVector);
}

void vctMatrixRotation2BaseTest::TestIdentityFixedSizeDouble(void) {
    vctFixedSizeVector<double, 2> aVector;
    TestIdentity<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestIdentityFixedSizeFloat(void) {
    vctFixedSizeVector<float, 2> aVector;
    TestIdentity<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestIdentityDynamicDouble(void) {
    vctDynamicVector<double> aVector(2);
    TestIdentity<vctMatrixRotation2Base<vctDynamicMatrix<double> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestIdentityDynamicFloat(void) {
    vctDynamicVector<float> aVector(2);
    TestIdentity<vctMatrixRotation2Base<vctDynamicMatrix<float> > >(aVector);
}



template <class _rotationType>
void vctMatrixRotation2BaseTest::TestDefaultConstructor(void) {
    typedef _rotationType RotationType;
    RotationType matrixRotation;
    vctGenericRotationTest::TestDefaultConstructor(matrixRotation);
}

void vctMatrixRotation2BaseTest::TestDefaultConstructorFixedSizeDouble(void) {
    TestDefaultConstructor<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestDefaultConstructorFixedSizeFloat(void) {
    TestDefaultConstructor<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestDefaultConstructorDynamicDouble(void) {
    TestDefaultConstructor<vctMatrixRotation2Base<vctDynamicMatrix<double> > >();
}

void vctMatrixRotation2BaseTest::TestDefaultConstructorDynamicFloat(void) {
    TestDefaultConstructor<vctMatrixRotation2Base<vctDynamicMatrix<float> > >();
}



template <class _rotationType>
void vctMatrixRotation2BaseTest::TestInverse(void) {
    typedef _rotationType RotationType;
    RotationType matrixRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestInverse(matrixRotation);
    
}

void vctMatrixRotation2BaseTest::TestInverseFixedSizeDouble(void) {
    TestInverse<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestInverseFixedSizeFloat(void) {
    TestInverse<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestInverseDynamicDouble(void) {
    TestInverse<vctMatrixRotation2Base<vctDynamicMatrix<double> > >();
}

void vctMatrixRotation2BaseTest::TestInverseDynamicFloat(void) {
    TestInverse<vctMatrixRotation2Base<vctDynamicMatrix<float> > >();
}



template <class _rotationType, class _vectorType>
void vctMatrixRotation2BaseTest::TestComposition(const _vectorType & aVector) {
    typedef _rotationType RotationType;
    typedef _vectorType VectorType;
    typedef typename RotationType::value_type value_type;
    RotationType matrixRotation1;
    RotationType matrixRotation2;
    VectorType inputVector(aVector);
    vctRandom(matrixRotation1);
    vctRandom(matrixRotation2);
    vctRandom(inputVector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestComposition(matrixRotation1, matrixRotation2, inputVector);
}

void vctMatrixRotation2BaseTest::TestCompositionFixedSizeDouble(void) {
    vctFixedSizeVector<double, 2> aVector;
    TestComposition<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestCompositionFixedSizeFloat(void) {
    vctFixedSizeVector<float, 2> aVector;
    TestComposition<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestCompositionDynamicDouble(void) {
    vctDynamicVector<double> aVector(2);
    TestComposition<vctMatrixRotation2Base<vctDynamicMatrix<double> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestCompositionDynamicFloat(void) {
    vctDynamicVector<float> aVector(2);
    TestComposition<vctMatrixRotation2Base<vctDynamicMatrix<float> > >(aVector);
}



template <class _rotationType>
void vctMatrixRotation2BaseTest::TestRandom(void) {
    typedef _rotationType RotationType;
    RotationType matrixRotation;
    vctRandom(matrixRotation);
    vctGenericRotationTest::TestRandom(matrixRotation);
}

void vctMatrixRotation2BaseTest::TestRandomFixedSizeDouble(void) {
    TestRandom<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestRandomFixedSizeFloat(void) {
    TestRandom<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >();
}

void vctMatrixRotation2BaseTest::TestRandomDynamicDouble(void) {
    TestRandom<vctMatrixRotation2Base<vctDynamicMatrix<double> > >();
}

void vctMatrixRotation2BaseTest::TestRandomDynamicFloat(void) {
    TestRandom<vctMatrixRotation2Base<vctDynamicMatrix<float> > >();
}



template <class _rotationType, class _vectorType>
void vctMatrixRotation2BaseTest::TestRigidity(const _vectorType & aVector) {
    typedef _rotationType RotationType;
    typedef _vectorType VectorType;
    typedef typename RotationType::value_type value_type;
    RotationType matrixRotation;
    VectorType vector1(aVector);
    VectorType vector2(aVector);
    vctRandom(matrixRotation);
    vctRandom(vector1, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctRandom(vector2, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestRigidity2D(matrixRotation, vector1, vector2);
}

void vctMatrixRotation2BaseTest::TestRigidityFixedSizeDouble(void) {
    vctFixedSizeVector<double, 2> aVector;
    TestRigidity<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestRigidityFixedSizeFloat(void) {
    vctFixedSizeVector<float, 2> aVector;
    TestRigidity<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestRigidityDynamicDouble(void) {
    vctDynamicVector<double> aVector(2);
    TestRigidity<vctMatrixRotation2Base<vctDynamicMatrix<double> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestRigidityDynamicFloat(void) {
    vctDynamicVector<float> aVector(2);
    TestRigidity<vctMatrixRotation2Base<vctDynamicMatrix<float> > >(aVector);
}



template <class _rotationType, class _vectorType>
void vctMatrixRotation2BaseTest::TestApplyMethodsOperators(const _vectorType & aVector) {
    typedef _rotationType RotationType;
    typedef _vectorType VectorType;
    typedef typename RotationType::value_type value_type;

    RotationType matrixRotation;
    vctRandom(matrixRotation);
    
    VectorType vector(aVector);
    vctRandom(vector, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    vctGenericRotationTest::TestApplyMethodsOperatorsObject(matrixRotation, vector);

    RotationType rotation;
    vctRandom(rotation);
    vctGenericRotationTest::TestApplyMethodsOperatorsXform(matrixRotation, rotation);
}

void vctMatrixRotation2BaseTest::TestApplyMethodsOperatorsFixedSizeDouble(void) {
    vctFixedSizeVector<double, 2> aVector;
    TestApplyMethodsOperators<vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > >(aVector);
}

void vctMatrixRotation2BaseTest::TestApplyMethodsOperatorsFixedSizeFloat(void) {
    vctFixedSizeVector<float, 2> aVector;
    TestApplyMethodsOperators<vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > >(aVector);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctMatrixRotation2BaseTest);

