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


#include "vctVarStrideMatrixIteratorTest.h"

#include <algorithm>


void vctVarStrideMatrixIteratorTest::TestIncrementDecrementForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
    MatrixIteratorTest::TestIncrementDecrement(iterator);
}


void vctVarStrideMatrixIteratorTest::TestIncrementDecrementForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
    Submatrix1Test::TestIncrementDecrement(iterator);
}


void vctVarStrideMatrixIteratorTest::TestDecrementIncrementForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
    MatrixIteratorTest::TestDecrementIncrement(iterator);
}


void vctVarStrideMatrixIteratorTest::TestDecrementIncrementForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
    Submatrix1Test::TestDecrementIncrement(iterator);
}


void vctVarStrideMatrixIteratorTest::TestPreAndPostIncrementAndDecrementForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(2, MyMatrix.size() - 2);
    iterator += position;
    MatrixIteratorTest::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctVarStrideMatrixIteratorTest::TestPreAndPostIncrementAndDecrementForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(2, Submatrix1.size() - 2);
    iterator += position;
    Submatrix1Test::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForIncrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForIncrement(MyMatrix);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForDecrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForIncrement(MyMatrix);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForReverseIncrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForReverseIncrement(MyMatrix);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForReverseDecrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForReverseIncrement(MyMatrix);
}


void vctVarStrideMatrixIteratorTest::TestAdditionAndSubtractionSymmetryForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::difference_type position = RandomGenerator.ExtractRandomPtrdiffT(0, MyMatrix.size());
    iterator += position;
    MatrixType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, MyMatrix.size() - position);
    MatrixIteratorTest::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctVarStrideMatrixIteratorTest::TestAdditionAndIncrementEquivalenceForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
	MatrixType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyMatrix.size() - position) );
    MatrixIteratorTest::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctVarStrideMatrixIteratorTest::TestOrderingForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
	MatrixType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyMatrix.size() - position) );
    MatrixIteratorTest::TestOrdering(iterator, difference);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForIncrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForIncrement(Submatrix1);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForDecrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForIncrement(Submatrix1);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForReverseIncrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForReverseIncrement(Submatrix1);
}


void vctVarStrideMatrixIteratorTest::TestContainerSizeForReverseDecrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForReverseIncrement(Submatrix1);
}


void vctVarStrideMatrixIteratorTest::TestAdditionAndSubtractionSymmetryForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::difference_type position = RandomGenerator.ExtractRandomPtrdiffT(0, Submatrix1.size());
    iterator += position;
    Submatrix1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, Submatrix1.size() - position);
    Submatrix1Test::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctVarStrideMatrixIteratorTest::TestAdditionAndIncrementEquivalenceForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
	Submatrix1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, Submatrix1.size() - position) );
    Submatrix1Test::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctVarStrideMatrixIteratorTest::TestOrderingForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
	Submatrix1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, Submatrix1.size() - position) );
    Submatrix1Test::TestOrdering(iterator, difference);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctVarStrideMatrixIteratorTest);

