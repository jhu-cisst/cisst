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


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctMatrixRotation2Base.h>

class vctMatrixRotation2BaseTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctMatrixRotation2BaseTest);

    CPPUNIT_TEST(TestConstructorsFixedSizeDouble);
    CPPUNIT_TEST(TestConstructorsFixedSizeFloat);
    CPPUNIT_TEST(TestConstructorsDynamicDouble);
    CPPUNIT_TEST(TestConstructorsDynamicFloat);

    CPPUNIT_TEST(TestApplyToFixedSizeDouble);
    CPPUNIT_TEST(TestApplyToFixedSizeFloat);
    CPPUNIT_TEST(TestApplyToDynamicDouble);
    CPPUNIT_TEST(TestApplyToDynamicFloat);

    CPPUNIT_TEST(TestNormalizeFixedSizeDouble);
    CPPUNIT_TEST(TestNormalizeFixedSizeFloat);
    CPPUNIT_TEST(TestNormalizeDynamicDouble);
    CPPUNIT_TEST(TestNormalizeDynamicFloat);

    CPPUNIT_TEST(TestFromVectorsFixedSizeDouble);
    CPPUNIT_TEST(TestFromVectorsFixedSizeFloat);
    CPPUNIT_TEST(TestFromVectorsDynamicDouble);
    CPPUNIT_TEST(TestFromVectorsDynamicFloat);

    CPPUNIT_TEST(TestConversionAngleFixedSizeDouble);
    CPPUNIT_TEST(TestConversionAngleFixedSizeFloat);
    CPPUNIT_TEST(TestConversionAngleDynamicDouble);
    CPPUNIT_TEST(TestConversionAngleDynamicFloat);

    CPPUNIT_TEST(TestIdentityFixedSizeDouble);
    CPPUNIT_TEST(TestIdentityFixedSizeFloat);
    CPPUNIT_TEST(TestIdentityDynamicDouble);
    CPPUNIT_TEST(TestIdentityDynamicFloat);

    CPPUNIT_TEST(TestDefaultConstructorFixedSizeDouble);
    CPPUNIT_TEST(TestDefaultConstructorFixedSizeFloat);
    CPPUNIT_TEST(TestDefaultConstructorDynamicDouble);
    CPPUNIT_TEST(TestDefaultConstructorDynamicFloat);

    CPPUNIT_TEST(TestInverseFixedSizeDouble);
    CPPUNIT_TEST(TestInverseFixedSizeFloat);
    CPPUNIT_TEST(TestInverseDynamicDouble);
    CPPUNIT_TEST(TestInverseDynamicFloat);

    CPPUNIT_TEST(TestCompositionFixedSizeDouble);
    CPPUNIT_TEST(TestCompositionFixedSizeFloat);
    CPPUNIT_TEST(TestCompositionDynamicDouble);
    CPPUNIT_TEST(TestCompositionDynamicFloat);

    CPPUNIT_TEST(TestRandomFixedSizeDouble);
    CPPUNIT_TEST(TestRandomFixedSizeFloat);
    CPPUNIT_TEST(TestRandomDynamicDouble);
    CPPUNIT_TEST(TestRandomDynamicFloat);

    CPPUNIT_TEST(TestRigidityFixedSizeDouble);
    CPPUNIT_TEST(TestRigidityFixedSizeFloat);
    CPPUNIT_TEST(TestRigidityDynamicDouble);
    CPPUNIT_TEST(TestRigidityDynamicFloat);

    CPPUNIT_TEST(TestApplyMethodsOperatorsFixedSizeDouble);
    CPPUNIT_TEST(TestApplyMethodsOperatorsFixedSizeFloat);

    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test the constructors. */
    template <class _rotationType, class _vectorType> void TestConstructors(const _vectorType & aVector);
    void TestConstructorsFixedSizeDouble(void);
    void TestConstructorsFixedSizeFloat(void);
    void TestConstructorsDynamicDouble(void);
    void TestConstructorsDynamicFloat(void);

    /*! Test the product methods */
    template <class _rotationType, class _vectorType> void TestApplyTo(const _vectorType & aVector);
    void TestApplyToFixedSizeDouble(void);
    void TestApplyToFixedSizeFloat(void);
    void TestApplyToDynamicDouble(void);
    void TestApplyToDynamicFloat(void);

    /*! Test the normalize methods */
    template <class _rotationType> void TestNormalize(void);
    void TestNormalizeFixedSizeDouble(void);
    void TestNormalizeFixedSizeFloat(void);
    void TestNormalizeDynamicDouble(void);
    void TestNormalizeDynamicFloat(void);

    /*! Test the from vector methods */
    template <class _rotationType> void TestFromVectors(void);
    void TestFromVectorsFixedSizeDouble(void);
    void TestFromVectorsFixedSizeFloat(void);
    void TestFromVectorsDynamicDouble(void);
    void TestFromVectorsDynamicFloat(void);

    /*! Test the conversion from and to angle rotation */
    template <class _elementType> void TestConversionAngle(void);
    void TestConversionAngleFixedSizeDouble(void);
    void TestConversionAngleFixedSizeFloat(void);
    void TestConversionAngleDynamicDouble(void);
    void TestConversionAngleDynamicFloat(void);

    /*! Test Identity */
    template <class _rotationType, class _vectorType> void TestIdentity(const _vectorType & aVector);
    void TestIdentityFixedSizeDouble(void);
    void TestIdentityFixedSizeFloat(void);
    void TestIdentityDynamicDouble(void);
    void TestIdentityDynamicFloat(void);

    /*! Test default constructor */
    template <class _rotationType> void TestDefaultConstructor(void);
    void TestDefaultConstructorFixedSizeDouble(void);
    void TestDefaultConstructorFixedSizeFloat(void);
    void TestDefaultConstructorDynamicDouble(void);
    void TestDefaultConstructorDynamicFloat(void);

    /*! Test the inverse methods */
    template<class _rotationType> void TestInverse(void);
    void TestInverseFixedSizeDouble(void);
    void TestInverseFixedSizeFloat(void);
    void TestInverseDynamicDouble(void);
    void TestInverseDynamicFloat(void);
    
    /*! Test Composition */
    template <class _rotationType, class _vectorType> void TestComposition(const _vectorType & aVector);
    void TestCompositionFixedSizeDouble(void);
    void TestCompositionFixedSizeFloat(void);
    void TestCompositionDynamicDouble(void);
    void TestCompositionDynamicFloat(void);

    /*! Test Random */
    template <class _rotationType> void TestRandom(void);
    void TestRandomFixedSizeDouble(void);
    void TestRandomFixedSizeFloat(void);
    void TestRandomDynamicDouble(void);
    void TestRandomDynamicFloat(void);

    /*! Test Rigidity */
    template <class _rotationType, class _vectorType> void TestRigidity(const _vectorType & aVector);
    void TestRigidityFixedSizeDouble(void);
    void TestRigidityFixedSizeFloat(void);
    void TestRigidityDynamicDouble(void);
    void TestRigidityDynamicFloat(void);

    /*! Test Apply methods (existence and consistency) */
    template <class _rotationType, class _vectorType> void TestApplyMethodsOperators(const _vectorType & aVector);
    void TestApplyMethodsOperatorsFixedSizeDouble(void);
    void TestApplyMethodsOperatorsFixedSizeFloat(void);
};

