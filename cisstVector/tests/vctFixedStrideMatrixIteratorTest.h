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


#ifndef _vctFixedStrideMatrixIteratorTest_h
#define _vctFixedStrideMatrixIteratorTest_h

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFixedSizeMatrixRef.h>

#include "vctGenericIteratorTest.h"

/*! A concrete implementation of the generic iterator tests for the case of
  dynamic vectors and subsequences.  The test use randomly-initialized iterators
  on a fixed-size vector of size 60.
  \sa vctGenericIteratorTest
*/
class vctFixedStrideMatrixIteratorTest : public CppUnit::TestCase
{
 public:
    enum {ROWS = 6, COLS = 7, ROWS1 = 4, COLS1 = 3};
    
    typedef int MatrixElementType;
    typedef vctFixedSizeMatrix<MatrixElementType, ROWS, COLS> MatrixType;
    typedef MatrixType::Submatrix<ROWS1, COLS1>::Type Submatrix1Type;
    
    typedef GenericIteratorTest<MatrixType> MatrixIteratorTest;
    typedef GenericIteratorTest<Submatrix1Type> Submatrix1Test;
    
 protected:
    MatrixType MyMatrix;
    cmnRandomSequence & RandomGenerator;
    Submatrix1Type Submatrix1;
    
 public:
    vctFixedStrideMatrixIteratorTest():
        RandomGenerator(cmnRandomSequence::GetInstance()),
        Submatrix1(MyMatrix.Pointer())
    {}
    
    virtual void setUp() {
        unsigned int i;
        for (i = 0; i < MyMatrix.size(); ++i) {
            MyMatrix.at(i) = i;
        }
    }
    
    void TestIncrementDecrementForMatrix(void);
    void TestDecrementIncrementForMatrix(void);
    void TestPreAndPostIncrementAndDecrementForMatrix(void);
    void TestContainerSizeForIncrementMatrix(void);
    void TestContainerSizeForDecrementMatrix(void);
    void TestContainerSizeForReverseIncrementMatrix(void);
    void TestContainerSizeForReverseDecrementMatrix(void);
    void TestAdditionAndSubtractionSymmetryForMatrix(void);
    void TestAdditionAndIncrementEquivalenceForMatrix(void);
    void TestOrderingForMatrix(void);
    
    void TestIncrementDecrementForSubmatrix1(void);
    void TestDecrementIncrementForSubmatrix1(void);
    void TestPreAndPostIncrementAndDecrementForSubmatrix1(void);
    void TestContainerSizeForIncrementSubmatrix1(void);
    void TestContainerSizeForDecrementSubmatrix1(void);
    void TestContainerSizeForReverseIncrementSubmatrix1(void);
    void TestContainerSizeForReverseDecrementSubmatrix1(void);
    void TestAdditionAndSubtractionSymmetryForSubmatrix1(void);
    void TestAdditionAndIncrementEquivalenceForSubmatrix1(void);
    void TestOrderingForSubmatrix1(void);
    
    
    
    CPPUNIT_TEST_SUITE(vctFixedStrideMatrixIteratorTest);
    
    CPPUNIT_TEST(TestIncrementDecrementForMatrix);
    CPPUNIT_TEST(TestDecrementIncrementForMatrix);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForMatrix);
    CPPUNIT_TEST(TestContainerSizeForIncrementMatrix);
    CPPUNIT_TEST(TestContainerSizeForDecrementMatrix);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementMatrix);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementMatrix);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForMatrix);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForMatrix);
    CPPUNIT_TEST(TestOrderingForMatrix);
    CPPUNIT_TEST(TestIncrementDecrementForSubmatrix1);
    CPPUNIT_TEST(TestDecrementIncrementForSubmatrix1);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForSubmatrix1);
    CPPUNIT_TEST(TestContainerSizeForIncrementSubmatrix1);
    CPPUNIT_TEST(TestContainerSizeForDecrementSubmatrix1);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementSubmatrix1);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementSubmatrix1);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForSubmatrix1);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForSubmatrix1);
    CPPUNIT_TEST(TestOrderingForSubmatrix1);

    CPPUNIT_TEST_SUITE_END();
};


#endif  // _vctFixedStrideMatrixIteratorTest_h

