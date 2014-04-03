/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2010-02-27
  
  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstMultiTask/mtsFixedSizeVectorTypes.h>

class mtsFixedSizeVectorTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(mtsFixedSizeVectorTest);

    CPPUNIT_TEST(TestConversionDouble3);
    CPPUNIT_TEST(TestConversionDouble6);
    CPPUNIT_TEST(TestConversionInt3);
    CPPUNIT_TEST(TestConversionInt6);

    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp(void) {}
    
    void tearDown(void) {}

    template <class _elementType, vct::size_type _size>
    void TestConversion(void);
    void TestConversionDouble3(void);
    void TestConversionDouble6(void);
    void TestConversionInt3(void);
    void TestConversionInt6(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(mtsFixedSizeVectorTest);

