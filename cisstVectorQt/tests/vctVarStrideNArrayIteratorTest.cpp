/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Daniel Li
  Created on: 2006-07-31
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctVarStrideNArrayIteratorTest.h"

#include <algorithm>


void vctVarStrideNArrayIteratorTest::TestIncrementDecrementForNArray(void) {
    NArrayType::const_iterator iterator = MyNArray.begin();
    NArrayType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyNArray.size());
    iterator += position;
    NArrayIteratorTest::TestIncrementDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestDecrementIncrementForNArray(void) {
    NArrayType::const_iterator iterator = MyNArray.begin();
    NArrayType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyNArray.size());
    iterator += position;
    NArrayIteratorTest::TestDecrementIncrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestPreAndPostIncrementAndDecrementForNArray(void) {
    NArrayType::const_iterator iterator = MyNArray.begin();
    NArrayType::size_type position = RandomGenerator.ExtractRandomSizeT(2, MyNArray.size() - 2);
    iterator += position;
    NArrayIteratorTest::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForIncrementNArray(void) {
    NArrayIteratorTest::TestContainerSizeForIncrement(MyNArray);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForDecrementNArray(void) {
    NArrayIteratorTest::TestContainerSizeForIncrement(MyNArray);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseIncrementNArray(void) {
    NArrayIteratorTest::TestContainerSizeForReverseIncrement(MyNArray);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseDecrementNArray(void) {
    NArrayIteratorTest::TestContainerSizeForReverseIncrement(MyNArray);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndSubtractionSymmetryForNArray(void) {
    NArrayType::const_iterator iterator = MyNArray.begin();
    NArrayType::difference_type position = RandomGenerator.ExtractRandomSizeT(0, MyNArray.size());
    iterator += position;
    NArrayType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, MyNArray.size() - position);
    NArrayIteratorTest::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndIncrementEquivalenceForNArray(void) {
    NArrayType::const_iterator iterator = MyNArray.begin();
    NArrayType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyNArray.size());
    iterator += position;
	NArrayType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyNArray.size() - position) );
    NArrayIteratorTest::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestOrderingForNArray(void) {
    NArrayType::const_iterator iterator = MyNArray.begin();
    NArrayType::size_type position = RandomGenerator.ExtractRandomSizeT(0, MyNArray.size());
    iterator += position;
	NArrayType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, MyNArray.size() - position) );
    NArrayIteratorTest::TestOrdering(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestIncrementDecrementForNArrayConstRef1(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef1.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef1.size());
    iterator += position;
    NArrayConstRef1Test::TestIncrementDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestDecrementIncrementForNArrayConstRef1(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef1.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef1.size());
    iterator += position;
    NArrayConstRef1Test::TestDecrementIncrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestPreAndPostIncrementAndDecrementForNArrayConstRef1(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef1.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(2, NArrayConstRef1.size() - 2);
    iterator += position;
    NArrayConstRef1Test::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForIncrementNArrayConstRef1(void) {
    NArrayConstRef1Test::TestContainerSizeForIncrement(NArrayConstRef1);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForDecrementNArrayConstRef1(void) {
    NArrayConstRef1Test::TestContainerSizeForIncrement(NArrayConstRef1);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseIncrementNArrayConstRef1(void) {
    NArrayConstRef1Test::TestContainerSizeForReverseIncrement(NArrayConstRef1);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseDecrementNArrayConstRef1(void) {
    NArrayConstRef1Test::TestContainerSizeForReverseIncrement(NArrayConstRef1);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndSubtractionSymmetryForNArrayConstRef1(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef1.begin();
    NArrayConstRefType::difference_type position = RandomGenerator.ExtractRandomPtrdiffT(0, NArrayConstRef1.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, NArrayConstRef1.size() - position);
    NArrayConstRef1Test::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndIncrementEquivalenceForNArrayConstRef1(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef1.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef1.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, NArrayConstRef1.size() - position) );
    NArrayConstRef1Test::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestOrderingForNArrayConstRef1(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef1.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef1.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, NArrayConstRef1.size() - position) );
    NArrayConstRef1Test::TestOrdering(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestIncrementDecrementForNArrayConstRef2(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef2.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef2.size());
    iterator += position;
    NArrayConstRef2Test::TestIncrementDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestDecrementIncrementForNArrayConstRef2(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef2.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef2.size());
    iterator += position;
    NArrayConstRef2Test::TestDecrementIncrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestPreAndPostIncrementAndDecrementForNArrayConstRef2(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef2.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(2, NArrayConstRef2.size() - 2);
    iterator += position;
    NArrayConstRef2Test::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForIncrementNArrayConstRef2(void) {
    NArrayConstRef2Test::TestContainerSizeForIncrement(NArrayConstRef2);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForDecrementNArrayConstRef2(void) {
    NArrayConstRef2Test::TestContainerSizeForIncrement(NArrayConstRef2);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseIncrementNArrayConstRef2(void) {
    NArrayConstRef2Test::TestContainerSizeForReverseIncrement(NArrayConstRef2);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseDecrementNArrayConstRef2(void) {
    NArrayConstRef2Test::TestContainerSizeForReverseIncrement(NArrayConstRef2);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndSubtractionSymmetryForNArrayConstRef2(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef2.begin();
    NArrayConstRefType::difference_type position = RandomGenerator.ExtractRandomPtrdiffT(0, NArrayConstRef2.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, NArrayConstRef2.size() - position);
    NArrayConstRef2Test::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndIncrementEquivalenceForNArrayConstRef2(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef2.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef2.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, NArrayConstRef2.size() - position) );
    NArrayConstRef2Test::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestOrderingForNArrayConstRef2(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef2.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef2.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, NArrayConstRef2.size() - position) );
    NArrayConstRef2Test::TestOrdering(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestIncrementDecrementForNArrayConstRef3(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef3.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef3.size());
    iterator += position;
    NArrayConstRef3Test::TestIncrementDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestDecrementIncrementForNArrayConstRef3(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef3.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef3.size());
    iterator += position;
    NArrayConstRef3Test::TestDecrementIncrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestPreAndPostIncrementAndDecrementForNArrayConstRef3(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef3.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(2, NArrayConstRef3.size() - 2);
    iterator += position;
    NArrayConstRef3Test::TestPreAndPostIncrementAndDecrement(iterator);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForIncrementNArrayConstRef3(void) {
    NArrayConstRef3Test::TestContainerSizeForIncrement(NArrayConstRef3);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForDecrementNArrayConstRef3(void) {
    NArrayConstRef3Test::TestContainerSizeForIncrement(NArrayConstRef3);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseIncrementNArrayConstRef3(void) {
    NArrayConstRef3Test::TestContainerSizeForReverseIncrement(NArrayConstRef3);
}


void vctVarStrideNArrayIteratorTest::TestContainerSizeForReverseDecrementNArrayConstRef3(void) {
    NArrayConstRef3Test::TestContainerSizeForReverseIncrement(NArrayConstRef3);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndSubtractionSymmetryForNArrayConstRef3(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef3.begin();
    NArrayConstRefType::difference_type position = RandomGenerator.ExtractRandomPtrdiffT(0, NArrayConstRef3.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(-position, NArrayConstRef3.size() - position);
    NArrayConstRef3Test::TestAdditionAndSubtractionSymmetry(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestAdditionAndIncrementEquivalenceForNArrayConstRef3(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef3.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef3.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, NArrayConstRef3.size() - position) );
    NArrayConstRef3Test::TestAdditionAndIncrementEquivalence(iterator, difference);
}


void vctVarStrideNArrayIteratorTest::TestOrderingForNArrayConstRef3(void) {
    NArrayConstRefType::const_iterator iterator = NArrayConstRef3.begin();
    NArrayConstRefType::size_type position = RandomGenerator.ExtractRandomSizeT(0, NArrayConstRef3.size());
    iterator += position;
    NArrayConstRefType::difference_type difference = RandomGenerator.ExtractRandomPtrdiffT(0, std::min(position, NArrayConstRef3.size() - position) );
    NArrayConstRef3Test::TestOrdering(iterator, difference);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctVarStrideNArrayIteratorTest);
