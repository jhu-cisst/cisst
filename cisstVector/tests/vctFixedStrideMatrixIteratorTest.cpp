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


#include "vctFixedStrideMatrixIteratorTest.h"

#include <algorithm>


void vctFixedStrideMatrixIteratorTest::TestIncrementDecrementForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
    MatrixIteratorTest::TestIncrementDecrement(iterator);
}


void vctFixedStrideMatrixIteratorTest::TestIncrementDecrementForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
    Submatrix1Test::TestIncrementDecrement(iterator);
}


void vctFixedStrideMatrixIteratorTest::TestDecrementIncrementForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
    MatrixIteratorTest::TestDecrementIncrement(iterator);
}


void vctFixedStrideMatrixIteratorTest::TestDecrementIncrementForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
    Submatrix1Test::TestDecrementIncrement(iterator);
}


void vctFixedStrideMatrixIteratorTest::TestPreAndPostIncrementAndDecrementForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(2, MyMatrix.size() - 2);
    iterator += position;
    MatrixIteratorTest::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctFixedStrideMatrixIteratorTest::TestPreAndPostIncrementAndDecrementForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(2, Submatrix1.size() - 2);
    iterator += position;
    Submatrix1Test::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForIncrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForIncrement(MyMatrix);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForDecrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForIncrement(MyMatrix);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForReverseIncrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForReverseIncrement(MyMatrix);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForReverseDecrementMatrix(void) {
    MatrixIteratorTest::TestContainerSizeForReverseIncrement(MyMatrix);
}


void vctFixedStrideMatrixIteratorTest::TestAdditionAndSubtractionSymmetryForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::difference_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
    MatrixType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, MyMatrix.size() - position);
    MatrixIteratorTest::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctFixedStrideMatrixIteratorTest::TestAdditionAndIncrementEquivalenceForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
	MatrixType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyMatrix.size() - position) );
    MatrixIteratorTest::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctFixedStrideMatrixIteratorTest::TestOrderingForMatrix(void) {
    MatrixType::const_iterator iterator = MyMatrix.begin();
    MatrixType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyMatrix.size());
    iterator += position;
	MatrixType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyMatrix.size() - position) );
    MatrixIteratorTest::TestOrdering(iterator, difference);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForIncrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForIncrement(Submatrix1);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForDecrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForIncrement(Submatrix1);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForReverseIncrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForReverseIncrement(Submatrix1);
}


void vctFixedStrideMatrixIteratorTest::TestContainerSizeForReverseDecrementSubmatrix1(void) {
    Submatrix1Test::TestContainerSizeForReverseIncrement(Submatrix1);
}


void vctFixedStrideMatrixIteratorTest::TestAdditionAndSubtractionSymmetryForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::difference_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
    Submatrix1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, Submatrix1.size() - position);
    Submatrix1Test::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctFixedStrideMatrixIteratorTest::TestAdditionAndIncrementEquivalenceForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
	Submatrix1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, Submatrix1.size() - position) );
    Submatrix1Test::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctFixedStrideMatrixIteratorTest::TestOrderingForSubmatrix1(void) {
    Submatrix1Type::const_iterator iterator = Submatrix1.begin();
    Submatrix1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Submatrix1.size());
    iterator += position;
	Submatrix1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, Submatrix1.size() - position) );
    Submatrix1Test::TestOrdering(iterator, difference);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctFixedStrideMatrixIteratorTest);
