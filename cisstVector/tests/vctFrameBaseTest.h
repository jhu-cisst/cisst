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


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctFrameBase.h>
#include <cisstVector/vctMatrixRotation3Base.h>
#include <cisstVector/vctQuaternionRotation3Base.h>


class vctFrameBaseTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctFrameBaseTest);

    CPPUNIT_TEST(TestConstructorsDouble3);
    CPPUNIT_TEST(TestConstructorsFloat3);

    CPPUNIT_TEST(TestApplyToDouble3);
    CPPUNIT_TEST(TestApplyToFloat3);

    CPPUNIT_TEST(TestInverseDouble3);
    CPPUNIT_TEST(TestInverseFloat3);

    CPPUNIT_TEST(TestApplyMethodsOperatorsDouble3);
    CPPUNIT_TEST(TestApplyMethodsOperatorsFloat3);

    CPPUNIT_TEST(TestConstructorsDouble2);
    CPPUNIT_TEST(TestConstructorsFloat2);

    CPPUNIT_TEST(TestInverseDouble2);
    CPPUNIT_TEST(TestInverseFloat2);

    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test the constructors for 3D */
    template<class _elementType> void TestConstructors3(void);
    void TestConstructorsDouble3(void);
    void TestConstructorsFloat3(void);

    /*! Test the product methods for 3D */
    template<class _rotationType> void TestApplyTo3(void);
    void TestApplyToDouble3(void);
    void TestApplyToFloat3(void);

    /*! Test the inverse methods for 3D */
    template<class _elementType> void TestInverse3(void);
    void TestInverseDouble3(void);
    void TestInverseFloat3(void);

    /*! Test Apply methods (existence and consistency) for 3D */
    template <class _elementType> void TestApplyMethodsOperators3(void);
    void TestApplyMethodsOperatorsDouble3(void);
    void TestApplyMethodsOperatorsFloat3(void);

    /*! Test the constructors for 2D */
    template<class _elementType> void TestConstructors2(void);
    void TestConstructorsDouble2(void);
    void TestConstructorsFloat2(void);

    /*! Test the inverse methods for 2D */
    template<class _elementType> void TestInverse2(void);
    void TestInverseDouble2(void);
    void TestInverseFloat2(void);

};


