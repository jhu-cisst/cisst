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

#include <cisstVector/vctQuaternionBase.h>


class vctQuaternionBaseTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctQuaternionBaseTest);
    CPPUNIT_TEST(TestConstructorsDouble);
    CPPUNIT_TEST(TestConstructorsFloat);
    CPPUNIT_TEST(TestProductDouble);
    CPPUNIT_TEST(TestProductFloat);
    CPPUNIT_TEST(TestConjugateDouble);
    CPPUNIT_TEST(TestConjugateFloat);
    CPPUNIT_TEST(TestNormalizeDouble);
    CPPUNIT_TEST(TestNormalizeFloat);
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
        void TestProduct(void);
    void TestProductDouble(void);
    void TestProductFloat(void);
    
    /*! Test the conjugate methods */
    template<class _elementType>
        void TestConjugate(void);
    void TestConjugateDouble(void);
    void TestConjugateFloat(void);
    
    /*! Test the normalize methods */
    template<class _elementType>
        void TestNormalize(void);
    void TestNormalizeDouble(void);
    void TestNormalizeFloat(void);
};

