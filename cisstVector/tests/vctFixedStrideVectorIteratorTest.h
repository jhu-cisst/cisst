/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2004-11-18
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctFixedStrideVectorIteratorTest_h
#define _vctFixedStrideVectorIteratorTest_h

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeVectorRef.h>

#include "vctGenericIteratorTest.h"

/*! A concrete implementation of the generic iterator tests for the case of
  dynamic vectors and subsequences.  The test use randomly-initialized iterators
  on a fixed-size vector of size 60.
  \sa vctGenericIteratorTest
*/
class vctFixedStrideVectorIteratorTest : public CppUnit::TestCase
{
 public:
    enum {SIZE=60, STRIDE1=3, SIZE1=15};
    
    typedef int VectorElementType;
    typedef vctFixedSizeVector<VectorElementType, SIZE> VectorType;
    typedef vctFixedSizeVectorRef<VectorElementType, SIZE1, STRIDE1> Subsequence1Type;
    
    typedef GenericIteratorTest<VectorType> VectorIteratorTest;
    typedef GenericIteratorTest<Subsequence1Type> Subsequence1Test;
    
 protected:
    VectorType MyVector;
    cmnRandomSequence & RandomGenerator;
    Subsequence1Type Subsequence1;
    
 public:
    vctFixedStrideVectorIteratorTest():
        RandomGenerator(cmnRandomSequence::GetInstance()),
        Subsequence1(MyVector.Pointer())
    {}
    
    virtual void setUp() {
        unsigned int i;
        for (i = 0; i < MyVector.size(); ++i) {
            MyVector[i] = i;
        }
    }
    
    void TestIncrementDecrementForVector(void);
    void TestDecrementIncrementForVector(void);
    void TestPreAndPostIncrementAndDecrementForVector(void);
    void TestContainerSizeForIncrementVector(void);
    void TestContainerSizeForDecrementVector(void);
    void TestContainerSizeForReverseIncrementVector(void);
    void TestContainerSizeForReverseDecrementVector(void);
    void TestAdditionAndSubtractionSymmetryForVector(void);
    void TestAdditionAndIncrementEquivalenceForVector(void);
    void TestOrderingForVector(void);
    
    void TestIncrementDecrementForSubsequence1(void);
    void TestDecrementIncrementForSubsequence1(void);
    void TestPreAndPostIncrementAndDecrementForSubsequence1(void);
    void TestContainerSizeForIncrementSubsequence1(void);
    void TestContainerSizeForDecrementSubsequence1(void);
    void TestContainerSizeForReverseIncrementSubsequence1(void);
    void TestContainerSizeForReverseDecrementSubsequence1(void);
    void TestAdditionAndSubtractionSymmetryForSubsequence1(void);
    void TestAdditionAndIncrementEquivalenceForSubsequence1(void);
    void TestOrderingForSubsequence1(void);
    
    
    
    CPPUNIT_TEST_SUITE(vctFixedStrideVectorIteratorTest);
    
    CPPUNIT_TEST(TestIncrementDecrementForVector);
    CPPUNIT_TEST(TestDecrementIncrementForVector);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForVector);
    CPPUNIT_TEST(TestContainerSizeForIncrementVector);
    CPPUNIT_TEST(TestContainerSizeForDecrementVector);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementVector);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementVector);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForVector);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForVector);
    CPPUNIT_TEST(TestOrderingForVector);
    CPPUNIT_TEST(TestIncrementDecrementForSubsequence1);
    CPPUNIT_TEST(TestDecrementIncrementForSubsequence1);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForSubsequence1);
    CPPUNIT_TEST(TestContainerSizeForIncrementSubsequence1);
    CPPUNIT_TEST(TestContainerSizeForDecrementSubsequence1);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementSubsequence1);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementSubsequence1);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForSubsequence1);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForSubsequence1);
    CPPUNIT_TEST(TestOrderingForSubsequence1);

    CPPUNIT_TEST_SUITE_END();
};


#endif  // _vctFixedStrideVectorIteratorTest_h

