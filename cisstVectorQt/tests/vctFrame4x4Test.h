/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2007-09-13

  (C) Copyright 2007-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class vctFrame4x4Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctFrame4x4Test);
    {
        CPPUNIT_TEST(TestConstructorsDouble);
        CPPUNIT_TEST(TestConstructorsFloat);

        CPPUNIT_TEST(TestApplyToDouble);
        CPPUNIT_TEST(TestApplyToFloat);

        CPPUNIT_TEST(TestInverseDouble);
        CPPUNIT_TEST(TestInverseFloat);

        CPPUNIT_TEST(TestCompositionDouble);
        CPPUNIT_TEST(TestCompositionFloat);

        CPPUNIT_TEST(TestRigidityDouble);
        CPPUNIT_TEST(TestRigidityFloat);

        CPPUNIT_TEST(TestIdentityDouble);
        CPPUNIT_TEST(TestIdentityFloat);

        CPPUNIT_TEST(TestApplyMethodsOperatorsDouble);
        CPPUNIT_TEST(TestApplyMethodsOperatorsFloat);

        CPPUNIT_TEST(TestConversionFrame3MatrixDouble);
        CPPUNIT_TEST(TestConversionFrame3MatrixFloat);
    }
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    /*! Test the constructors. */
    template <class _elementType, bool _rowMajor> void TestConstructors(void);
    void TestConstructorsDouble(void);
    void TestConstructorsFloat(void);

    /*! Test the product methods */
    template <class _elementType, bool _rowMajor> void TestApplyTo(void);
    void TestApplyToDouble(void);
    void TestApplyToFloat(void);

    /*! Test the inverse methods */
    template <class _elementType, bool _rowMajor> void TestInverse(void);
    void TestInverseDouble(void);
    void TestInverseFloat(void);

    /*! Test composition methods */
    template <class _elementType, bool _rowMajor> void TestComposition(void);
    void TestCompositionDouble(void);
    void TestCompositionFloat(void);

    /*! Test rigidity */
    template <class _elementType, bool _rowMajor> void TestRigidity(void);
    void TestRigidityDouble(void);
    void TestRigidityFloat(void);

    /*! Test identity */
    template <class _elementType, bool _rowMajor> void TestIdentity(void);
    void TestIdentityDouble(void);
    void TestIdentityFloat(void);

    /*! Test Apply methods (existence and consistency) */
    template <class _elementType, bool _rowMajor> void TestApplyMethodsOperators(void);
    void TestApplyMethodsOperatorsDouble(void);
    void TestApplyMethodsOperatorsFloat(void);

    /*! Test conversion methods from a vctFrm3 (translation + rotation) */
    template <class _elementType> void TestConversionFrame3Matrix(void);
    void TestConversionFrame3MatrixDouble(void);
    void TestConversionFrame3MatrixFloat(void);
};


