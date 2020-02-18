/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-01-12

  (C) Copyright 2004-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>

class vctMatrixRotation3Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctMatrixRotation3Test);

    CPPUNIT_TEST(TestConstructorsDouble);
    CPPUNIT_TEST(TestConstructorsFloat);

    CPPUNIT_TEST(TestConstructorFromVectorsDouble);
    CPPUNIT_TEST(TestConstructorFromVectorsFloat);

    CPPUNIT_TEST(TestFromVectorsDouble);
    CPPUNIT_TEST(TestFromVectorsFloat);

    CPPUNIT_TEST(TestConversionQuaternionDouble);
    CPPUNIT_TEST(TestConversionQuaternionFloat);

    CPPUNIT_TEST(TestConversionAxisAngleDouble);
    CPPUNIT_TEST(TestConversionAxisAngleFloat);

    CPPUNIT_TEST(TestConversionRodriguezDouble);
    CPPUNIT_TEST(TestConversionRodriguezFloat);

    CPPUNIT_TEST(TestConversionEulerZYZDouble);
    CPPUNIT_TEST(TestConversionEulerZYZFloat);

    CPPUNIT_TEST(TestConversionEulerZYXDouble);
    CPPUNIT_TEST(TestConversionEulerZYXFloat);

    CPPUNIT_TEST(TestConversionEulerZXZDouble);
    CPPUNIT_TEST(TestConversionEulerZXZFloat);

    CPPUNIT_TEST(TestConversionEulerYZXDouble);
    CPPUNIT_TEST(TestConversionEulerYZXFloat);

    CPPUNIT_TEST(TestFromSignaturesQuaternionDouble);
    CPPUNIT_TEST(TestFromSignaturesQuaternionFloat);

    CPPUNIT_TEST(TestFromSignaturesAxisAngleDouble);
    CPPUNIT_TEST(TestFromSignaturesAxisAngleFloat);

    CPPUNIT_TEST(TestFromSignaturesRodriguezDouble);
    CPPUNIT_TEST(TestFromSignaturesRodriguezFloat);

    CPPUNIT_TEST(TestDefaultConstructorDouble);
    CPPUNIT_TEST(TestDefaultConstructorFloat);

    CPPUNIT_TEST(TestNormalizeDouble);
    CPPUNIT_TEST(TestNormalizeFloat);

    CPPUNIT_TEST(TestInverseDouble);
    CPPUNIT_TEST(TestInverseFloat);

    CPPUNIT_TEST(TestCompositionDouble);
    CPPUNIT_TEST(TestCompositionFloat);

    CPPUNIT_TEST(TestRandomDouble);
    CPPUNIT_TEST(TestRandomFloat);

    CPPUNIT_TEST(TestRigidityDouble);
    CPPUNIT_TEST(TestRigidityFloat);

    CPPUNIT_TEST(TestIdentityDouble);
    CPPUNIT_TEST(TestIdentityFloat);

    CPPUNIT_TEST(TestApplyToDouble);
    CPPUNIT_TEST(TestApplyToFloat);

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

    /*! Test the from vector methods */
    template<class _elementType>
        void TestConstructorFromVectors(void);
    void TestConstructorFromVectorsDouble(void);
    void TestConstructorFromVectorsFloat(void);

    /*! Test the from vector methods */
    template<class _elementType>
        void TestFromVectors(void);
    void TestFromVectorsDouble(void);
    void TestFromVectorsFloat(void);

    /*! Test the conversion from and to quaternion rotation */
    template<class _elementType>
        void TestConversionQuaternion(void);
    void TestConversionQuaternionDouble(void);
    void TestConversionQuaternionFloat(void);

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

    /*! Test the conversion from and to Euler rotation */
    template<class _elementType, vctEulerRotation3Order::OrderType order>
        void TestConversionEuler(void);
    void TestConversionEulerZYZDouble(void);
    void TestConversionEulerZYZFloat(void);
    void TestConversionEulerZYXDouble(void);
    void TestConversionEulerZYXFloat(void);
    void TestConversionEulerZXZDouble(void);
    void TestConversionEulerZXZFloat(void);
    void TestConversionEulerYZXDouble(void);
    void TestConversionEulerYZXFloat(void);

    /*! Test signatures for From(quaternion) */
    template<class _elementType>
        void TestFromSignaturesQuaternion(void);
    void TestFromSignaturesQuaternionDouble(void);
    void TestFromSignaturesQuaternionFloat(void);

    /*! Test signatures for From(axisAngle) */
    template<class _elementType>
        void TestFromSignaturesAxisAngle(void);
    void TestFromSignaturesAxisAngleDouble(void);
    void TestFromSignaturesAxisAngleFloat(void);

    /*! Test signatures for From(rodriguez) */
    template<class _elementType>
        void TestFromSignaturesRodriguez(void);
    void TestFromSignaturesRodriguezDouble(void);
    void TestFromSignaturesRodriguezFloat(void);

    /*! Test default constructor */
    template <class _elementType>
        void TestDefaultConstructor(void);
    void TestDefaultConstructorDouble(void);
    void TestDefaultConstructorFloat(void);

    /*! Test normalization methods */
    template <class _elementType>
        void TestNormalize(void);
    void TestNormalizeDouble(void);
    void TestNormalizeFloat(void);

    /*! Test inversion methods */
    template <class _elementType>
        void TestInverse(void);
    void TestInverseDouble(void);
    void TestInverseFloat(void);

    /*! Test composition methods */
    template <class _elementType>
        void TestComposition(void);
    void TestCompositionDouble(void);
    void TestCompositionFloat(void);

    /*! Test random function */
    template <class _elementType>
        void TestRandom(void);
    void TestRandomDouble(void);
    void TestRandomFloat(void);

    /*! Test rigidity */
    template <class _elementType>
        void TestRigidity(void);
    void TestRigidityDouble(void);
    void TestRigidityFloat(void);

    /*! Test identity */
    template <class _elementType>
        void TestIdentity(void);
    void TestIdentityDouble(void);
    void TestIdentityFloat(void);

    /*! Test ApplyTo */
    template <class _elementType>
        void TestApplyTo(void);
    void TestApplyToDouble(void);
    void TestApplyToFloat(void);

    /*! Test Apply methods (existence and consistency) */
    template <class _elementType>
        void TestApplyMethodsOperators(void);
    void TestApplyMethodsOperatorsDouble(void);
    void TestApplyMethodsOperatorsFloat(void);

};
