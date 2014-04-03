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

#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctMatrixRotation3Base.h>


class vctQuaternionRotation3BaseTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctQuaternionRotation3BaseTest);

    CPPUNIT_TEST(TestConstructorsDouble);
    CPPUNIT_TEST(TestConstructorsFloat);
    CPPUNIT_TEST(TestApplyToDouble);
    CPPUNIT_TEST(TestApplyToFloat);

    CPPUNIT_TEST(TestConversionMatrixDouble);
    CPPUNIT_TEST(TestConversionMatrixFloat);

    CPPUNIT_TEST(TestConversionAxisAngleDouble);
    CPPUNIT_TEST(TestConversionAxisAngleFloat);

    CPPUNIT_TEST(TestConversionRodriguezDouble);
    CPPUNIT_TEST(TestConversionRodriguezFloat);

    CPPUNIT_TEST(TestFromSignaturesMatrixDouble);
    CPPUNIT_TEST(TestFromSignaturesMatrixFloat);

    CPPUNIT_TEST(TestFromSignaturesAxisAngleDouble);
    CPPUNIT_TEST(TestFromSignaturesAxisAngleFloat);

    CPPUNIT_TEST(TestFromSignaturesRodriguezDouble);
    CPPUNIT_TEST(TestFromSignaturesRodriguezFloat);

    CPPUNIT_TEST(TestIdentityDouble);
    CPPUNIT_TEST(TestIdentityFloat);

    CPPUNIT_TEST(TestDefaultConstructorDouble);
    CPPUNIT_TEST(TestDefaultConstructorFloat);

    CPPUNIT_TEST(TestInverseDouble);
    CPPUNIT_TEST(TestInverseFloat);

    CPPUNIT_TEST(TestCompositionDouble);
    CPPUNIT_TEST(TestCompositionFloat);

    CPPUNIT_TEST(TestRandomDouble);
    CPPUNIT_TEST(TestRandomFloat);

    CPPUNIT_TEST(TestRigidityDouble);
    CPPUNIT_TEST(TestRigidityFloat);

    CPPUNIT_TEST(TestApplyMethodsOperatorsDouble);
    CPPUNIT_TEST(TestApplyMethodsOperatorsFloat);

    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test the constructors. */
    template<class _elementType>
        void TestConstructors(void);
    void TestConstructorsDouble(void);
    void TestConstructorsFloat(void);

    /*! Test the product methods */
    template<class _elementType>
        void TestApplyTo(void);
    void TestApplyToDouble(void);
    void TestApplyToFloat(void);

    /*! Test the conversion from and to matrix rotation */
    template<class _elementType>
        void TestConversionMatrix(void);
    void TestConversionMatrixDouble(void);
    void TestConversionMatrixFloat(void);

    /*! Test the conversion from and to axis angle rotation */
    template<class _elementType>
        void TestConversionAxisAngle(void);
    void TestConversionAxisAngleDouble(void);
    void TestConversionAxisAngleFloat(void);

    /*! Test the conversion from and to Rodriguez rotation */
    template<class _elementType>
        void TestConversionRodriguez(void);
    void TestConversionRodriguezDouble(void);
    void TestConversionRodriguezFloat(void);

    /*! Test the conversion from and to matrix rotation */
    template<class _elementType>
        void TestFromSignaturesMatrix(void);
    void TestFromSignaturesMatrixDouble(void);
    void TestFromSignaturesMatrixFloat(void);

    /*! Test the conversion from and to axis angle rotation */
    template<class _elementType>
        void TestFromSignaturesAxisAngle(void);
    void TestFromSignaturesAxisAngleDouble(void);
    void TestFromSignaturesAxisAngleFloat(void);

    /*! Test the conversion from and to Rodriguez rotation */
    template<class _elementType>
        void TestFromSignaturesRodriguez(void);
    void TestFromSignaturesRodriguezDouble(void);
    void TestFromSignaturesRodriguezFloat(void);

    /*! Test Identity */
    template <class _elementType>
        void TestIdentity(void);
    void TestIdentityDouble(void);
    void TestIdentityFloat(void);

    /*! Test default constructor */
    template <class _elementType>
        void TestDefaultConstructor(void);
    void TestDefaultConstructorDouble(void);
    void TestDefaultConstructorFloat(void);

    /*! Test the inverse methods */
    template<class _elementType>
        void TestInverse(void);
    void TestInverseDouble(void);
    void TestInverseFloat(void);
    
    /*! Test Composition */
    template <class _elementType>
        void TestComposition(void);
    void TestCompositionDouble(void);
    void TestCompositionFloat(void);

    /*! Test Random */
    template <class _elementType>
        void TestRandom(void);
    void TestRandomDouble(void);
    void TestRandomFloat(void);

    /*! Test Rigidity */
    template <class _elementType>
        void TestRigidity(void);
    void TestRigidityDouble(void);
    void TestRigidityFloat(void);

    /*! Test Apply methods (existence and consistency) */
    template <class _elementType>
        void TestApplyMethodsOperators(void);
    void TestApplyMethodsOperatorsDouble(void);
    void TestApplyMethodsOperatorsFloat(void);
};

