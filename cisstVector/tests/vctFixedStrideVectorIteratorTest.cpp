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


#include "vctFixedStrideVectorIteratorTest.h"

#include <algorithm>


void vctFixedStrideVectorIteratorTest::TestIncrementDecrementForVector(void) {
    VectorType::const_iterator iterator = MyVector.begin();
    VectorType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyVector.size());
    iterator += position;
    VectorIteratorTest::TestIncrementDecrement(iterator);
}


void vctFixedStrideVectorIteratorTest::TestIncrementDecrementForSubsequence1(void) {
    Subsequence1Type::const_iterator iterator = Subsequence1.begin();
    Subsequence1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Subsequence1.size());
    iterator += position;
    Subsequence1Test::TestIncrementDecrement(iterator);
}


void vctFixedStrideVectorIteratorTest::TestDecrementIncrementForVector(void) {
    VectorType::const_iterator iterator = MyVector.begin();
    VectorType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyVector.size());
    iterator += position;
    VectorIteratorTest::TestDecrementIncrement(iterator);
}


void vctFixedStrideVectorIteratorTest::TestDecrementIncrementForSubsequence1(void) {
    Subsequence1Type::const_iterator iterator = Subsequence1.begin();
    Subsequence1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Subsequence1.size());
    iterator += position;
    Subsequence1Test::TestDecrementIncrement(iterator);
}


void vctFixedStrideVectorIteratorTest::TestPreAndPostIncrementAndDecrementForVector(void) {
    VectorType::const_iterator iterator = MyVector.begin();
    VectorType::size_type position = RandomGenerator.ExtractRandomSizeT(2, MyVector.size() - 2);
    iterator += position;
    VectorIteratorTest::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctFixedStrideVectorIteratorTest::TestPreAndPostIncrementAndDecrementForSubsequence1(void) {
    Subsequence1Type::const_iterator iterator = Subsequence1.begin();
    Subsequence1Type::size_type position = RandomGenerator.ExtractRandomSizeT(2, Subsequence1.size() - 2);
    iterator += position;
    Subsequence1Test::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForIncrementVector(void) {
    VectorIteratorTest::TestContainerSizeForIncrement(MyVector);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForDecrementVector(void) {
    VectorIteratorTest::TestContainerSizeForIncrement(MyVector);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForReverseIncrementVector(void) {
    VectorIteratorTest::TestContainerSizeForReverseIncrement(MyVector);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForReverseDecrementVector(void) {
    VectorIteratorTest::TestContainerSizeForReverseIncrement(MyVector);
}


void vctFixedStrideVectorIteratorTest::TestAdditionAndSubtractionSymmetryForVector(void) {
    VectorType::const_iterator iterator = MyVector.begin();
    VectorType::difference_type position = RandomGenerator.ExtractRandomPtrdiffT(0, MyVector.size());
    iterator += position;
    VectorType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, MyVector.size() - position);
    VectorIteratorTest::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctFixedStrideVectorIteratorTest::TestAdditionAndIncrementEquivalenceForVector(void) {
    VectorType::const_iterator iterator = MyVector.begin();
    VectorType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyVector.size());
    iterator += position;
	VectorType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyVector.size() - position) );
    VectorIteratorTest::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctFixedStrideVectorIteratorTest::TestOrderingForVector(void) {
    VectorType::const_iterator iterator = MyVector.begin();
    VectorType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyVector.size());
    iterator += position;
	VectorType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyVector.size() - position) );
    VectorIteratorTest::TestOrdering(iterator, difference);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForIncrementSubsequence1(void) {
    Subsequence1Test::TestContainerSizeForIncrement(Subsequence1);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForDecrementSubsequence1(void) {
    Subsequence1Test::TestContainerSizeForIncrement(Subsequence1);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForReverseIncrementSubsequence1(void) {
    Subsequence1Test::TestContainerSizeForReverseIncrement(Subsequence1);
}


void vctFixedStrideVectorIteratorTest::TestContainerSizeForReverseDecrementSubsequence1(void) {
    Subsequence1Test::TestContainerSizeForReverseIncrement(Subsequence1);
}


void vctFixedStrideVectorIteratorTest::TestAdditionAndSubtractionSymmetryForSubsequence1(void) {
    Subsequence1Type::const_iterator iterator = Subsequence1.begin();
    Subsequence1Type::difference_type position = RandomGenerator.ExtractRandomPtrdiffT(0, Subsequence1.size());
    iterator += position;
    Subsequence1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, Subsequence1.size() - position);
    Subsequence1Test::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctFixedStrideVectorIteratorTest::TestAdditionAndIncrementEquivalenceForSubsequence1(void) {
    Subsequence1Type::const_iterator iterator = Subsequence1.begin();
    Subsequence1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Subsequence1.size());
    iterator += position;
	Subsequence1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, Subsequence1.size() - position) );
    Subsequence1Test::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctFixedStrideVectorIteratorTest::TestOrderingForSubsequence1(void) {
    Subsequence1Type::const_iterator iterator = Subsequence1.begin();
    Subsequence1Type::size_type position = RandomGenerator.ExtractRandomSizeT(0, Subsequence1.size());
    iterator += position;
	Subsequence1Type::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, Subsequence1.size() - position) );
    Subsequence1Test::TestOrdering(iterator, difference);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctFixedStrideVectorIteratorTest);
