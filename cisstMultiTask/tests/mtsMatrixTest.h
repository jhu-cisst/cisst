/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-04-29
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstMultiTask/mtsMatrix.h>

class mtsMatrixTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(mtsMatrixTest);

    CPPUNIT_TEST(TestSetSizeFromDouble);
    CPPUNIT_TEST(TestSetSizeFromInt);

    CPPUNIT_TEST(TestConversionDouble);
    CPPUNIT_TEST(TestConversionInt);

    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp(void) {}
    
    void tearDown(void) {}
    
    /*! Test the SetSizeFrom method */
    template <class _elementType> void TestSetSizeFrom(void);
    void TestSetSizeFromDouble(void);
    void TestSetSizeFromInt(void);

    /*! Test conversion methods (ctor, =, down cast, ...) */
    template <class _elementType> void TestConversion(void);
    void TestConversionDouble(void);
    void TestConversionInt(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(mtsMatrixTest);

