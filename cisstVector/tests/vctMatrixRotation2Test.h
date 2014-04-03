/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-02-23
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctMatrixRotation2.h>

class vctMatrixRotation2Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctMatrixRotation2Test);

    CPPUNIT_TEST(TestConstructorsDouble);
    CPPUNIT_TEST(TestConstructorsFloat);

    CPPUNIT_TEST(TestConstructorFromVectorsDouble);
    CPPUNIT_TEST(TestConstructorFromVectorsFloat);

    CPPUNIT_TEST(TestConversionAngleDouble);
    CPPUNIT_TEST(TestConversionAngleFloat);

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

    /*! Test the from vector methods */
    template<class _elementType>
        void TestConstructorFromVectors(void);
    void TestConstructorFromVectorsDouble(void);
    void TestConstructorFromVectorsFloat(void);

    /*! Test the conversion from and to angle rotation */
    template<class _elementType>
        void TestConversionAngle(void);
    void TestConversionAngleDouble(void);
    void TestConversionAngleFloat(void);

    /*! Test default constructor */
    template <class _elementType>
        void TestDefaultConstructor(void);
    void TestDefaultConstructorDouble(void);
    void TestDefaultConstructorFloat(void);

};

