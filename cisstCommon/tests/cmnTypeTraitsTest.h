/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2004-04-02
  
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

#include <cisstCommon/cmnTypeTraits.h>


class cmnTypeTraitsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnTypeTraitsTest);
    CPPUNIT_TEST(TestFloatingPointsDouble);
    CPPUNIT_TEST(TestFloatingPointsFloat);
    CPPUNIT_TEST(TestNonFloatingPointsInt);
    CPPUNIT_TEST(TestNonFloatingPointsChar);
    CPPUNIT_TEST(TestIsDerivedFrom);
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Tests for floating points */
    template <class _elementType>
        void TestFloatingPoints(void);
    void TestFloatingPointsDouble(void);
    void TestFloatingPointsFloat(void);

    /*! Tests for non floating points */
    template <class _elementType>
        void TestNonFloatingPoints(void);
    void TestNonFloatingPointsInt(void);
    void TestNonFloatingPointsChar(void);

    /*! Test for cmnIsDerivedFrom */
    void TestIsDerivedFrom(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(cmnTypeTraitsTest);

