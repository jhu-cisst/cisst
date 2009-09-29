/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFrameBaseTest.h 75 2009-02-24 16:47:20Z adeguet1 $
  
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

    CPPUNIT_TEST(TestConstructorsDouble);
    CPPUNIT_TEST(TestConstructorsFloat);

    CPPUNIT_TEST(TestApplyToDouble);
    CPPUNIT_TEST(TestApplyToFloat);

    CPPUNIT_TEST(TestInverseDouble);
    CPPUNIT_TEST(TestInverseFloat);

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

    /*! Test the inverse methods */
    template<class _elementType>
        void TestInverse(void);
    void TestInverseDouble(void);
    void TestInverseFloat(void);

    /*! Test Apply methods (existence and consistency) */
    template <class _elementType>
        void TestApplyMethodsOperators(void);
    void TestApplyMethodsOperatorsDouble(void);
    void TestApplyMethodsOperatorsFloat(void);

};


