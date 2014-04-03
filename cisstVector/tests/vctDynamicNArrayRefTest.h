/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Anton Deguet
  Created on:	2007-02-06
  
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

#include <cisstVector/vctForwardDeclarations.h>


class vctDynamicNArrayRefTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctDynamicNArrayRefTest);

    CPPUNIT_TEST(TestRefOnArrayDouble);
    CPPUNIT_TEST(TestRefOnArrayFloat);
    CPPUNIT_TEST(TestRefOnArrayInt);

    CPPUNIT_TEST(TestSubarrayRefDouble);
    CPPUNIT_TEST(TestSubarrayRefFloat);
    CPPUNIT_TEST(TestSubarrayRefInt);

    CPPUNIT_TEST(TestPermutationRefDouble);
    CPPUNIT_TEST(TestPermutationRefFloat);
    CPPUNIT_TEST(TestPermutationRefInt);

    CPPUNIT_TEST(TestEngines);

    CPPUNIT_TEST_SUITE_END();
    
 public:

    enum {MIN_SIZE = 3, MAX_SIZE = 8};

    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test reference on full NArray */
    template<class _elementType> void TestRefOnArray(void);
    void TestRefOnArrayDouble(void);
    void TestRefOnArrayFloat(void);
    void TestRefOnArrayInt(void);

    /*! Test subarray reference */
    template<class _elementType> void TestSubarrayRef(void);
    void TestSubarrayRefDouble(void);
    void TestSubarrayRefFloat(void);
    void TestSubarrayRefInt(void);

    /*! Test permutation reference */
    template<class _elementType> void TestPermutationRef(void);
    void TestPermutationRefDouble(void);
    void TestPermutationRefFloat(void);
    void TestPermutationRefInt(void);

    /*! Test all engines for doubles */
    void TestEngines(void);
};


