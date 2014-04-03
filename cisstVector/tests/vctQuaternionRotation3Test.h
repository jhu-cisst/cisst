/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-01-15
  
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

#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctMatrixRotation3.h>


class vctQuaternionRotation3Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctQuaternionRotation3Test);

    CPPUNIT_TEST(TestConstructorsDouble);
    CPPUNIT_TEST(TestConstructorsFloat);

    CPPUNIT_TEST(TestConversionMatrixDouble);
    CPPUNIT_TEST(TestConversionMatrixFloat);

    CPPUNIT_TEST(TestConversionAxisAngleDouble);
    CPPUNIT_TEST(TestConversionAxisAngleFloat);

    CPPUNIT_TEST(TestConversionRodriguezDouble);
    CPPUNIT_TEST(TestConversionRodriguezFloat);

    CPPUNIT_TEST(TestDefaultConstructorDouble);
    CPPUNIT_TEST(TestDefaultConstructorFloat);

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

    /*! Test the default constructor */
    template <class _elementType>
        void TestDefaultConstructor(void);
    void TestDefaultConstructorDouble(void);
    void TestDefaultConstructorFloat(void);

};


