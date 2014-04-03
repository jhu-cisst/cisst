/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Daniel Li
  Created on:	2006-07-31
  
  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctVarStrideNArrayIteratorTest_h
#define _vctVarStrideNArrayIteratorTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctDynamicNArray.h>
#include <cisstVector/vctDynamicConstNArrayRef.h>

#include "vctGenericIteratorTest.h"

/*! A concrete implementation of the generic iterator tests for the case of
  nArrays.
  \sa vctGenericIteratorTest
*/
class vctVarStrideNArrayIteratorTest: public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(vctVarStrideNArrayIteratorTest);
    
    CPPUNIT_TEST(TestIncrementDecrementForNArray);
    CPPUNIT_TEST(TestDecrementIncrementForNArray);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForNArray);
    CPPUNIT_TEST(TestContainerSizeForIncrementNArray);
    CPPUNIT_TEST(TestContainerSizeForDecrementNArray);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementNArray);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementNArray);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForNArray);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForNArray);
    CPPUNIT_TEST(TestOrderingForNArray);

    CPPUNIT_TEST(TestIncrementDecrementForNArrayConstRef1);
    CPPUNIT_TEST(TestDecrementIncrementForNArrayConstRef1);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForNArrayConstRef1);
    CPPUNIT_TEST(TestContainerSizeForIncrementNArrayConstRef1);
    CPPUNIT_TEST(TestContainerSizeForDecrementNArrayConstRef1);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementNArrayConstRef1);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementNArrayConstRef1);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForNArrayConstRef1);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForNArrayConstRef1);
    CPPUNIT_TEST(TestOrderingForNArrayConstRef1);

    CPPUNIT_TEST(TestIncrementDecrementForNArrayConstRef2);
    CPPUNIT_TEST(TestDecrementIncrementForNArrayConstRef2);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForNArrayConstRef2);
    CPPUNIT_TEST(TestContainerSizeForIncrementNArrayConstRef2);
    CPPUNIT_TEST(TestContainerSizeForDecrementNArrayConstRef2);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementNArrayConstRef2);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementNArrayConstRef2);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForNArrayConstRef2);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForNArrayConstRef2);
    CPPUNIT_TEST(TestOrderingForNArrayConstRef2);

    CPPUNIT_TEST(TestIncrementDecrementForNArrayConstRef3);
    CPPUNIT_TEST(TestDecrementIncrementForNArrayConstRef3);
    CPPUNIT_TEST(TestPreAndPostIncrementAndDecrementForNArrayConstRef3);
    CPPUNIT_TEST(TestContainerSizeForIncrementNArrayConstRef3);
    CPPUNIT_TEST(TestContainerSizeForDecrementNArrayConstRef3);
    CPPUNIT_TEST(TestContainerSizeForReverseIncrementNArrayConstRef3);
    CPPUNIT_TEST(TestContainerSizeForReverseDecrementNArrayConstRef3);
    CPPUNIT_TEST(TestAdditionAndSubtractionSymmetryForNArrayConstRef3);
    CPPUNIT_TEST(TestAdditionAndIncrementEquivalenceForNArrayConstRef3);
    CPPUNIT_TEST(TestOrderingForNArrayConstRef3);

    CPPUNIT_TEST_SUITE_END();

public:

    enum {DIMENSION = 3};  // DO NOT CHANGE 
    typedef int NArrayElementType;
    typedef vctDynamicNArray<NArrayElementType, DIMENSION> NArrayType;
    typedef vctDynamicConstNArrayRef<NArrayElementType, DIMENSION> NArrayConstRefType;

    typedef GenericIteratorTest<NArrayType> NArrayIteratorTest;             // memory-owning nArray
    typedef GenericIteratorTest<NArrayConstRefType> NArrayConstRef1Test;    // overlay of nArray
    typedef GenericIteratorTest<NArrayConstRefType> NArrayConstRef2Test;    // overlay of nArray w. startPos and sizes
    typedef GenericIteratorTest<NArrayConstRefType> NArrayConstRef3Test;    // overlay of nArray w. startPos, sizes and strides

    typedef NArrayType::iterator iterator;
    typedef NArrayType::const_iterator const_iterator;
    typedef NArrayType::reverse_iterator reverse_iterator;
    typedef NArrayType::const_reverse_iterator const_reverse_iterator;

    VCT_CONTAINER_TRAITS_TYPEDEFS(NArrayElementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

 protected:
    NArrayType MyNArray;
    cmnRandomSequence & RandomGenerator;
    NArrayConstRefType NArrayConstRef1;
    NArrayConstRefType NArrayConstRef2;
    NArrayConstRefType NArrayConstRef3;

 public:
    vctVarStrideNArrayIteratorTest():
    //    MyNArray(nsize_type(3, 4, 5, 6)),
        RandomGenerator(cmnRandomSequence::GetInstance())
     /*   NArrayConstRef1(MyNArray),
    //    NArrayConstRef2(MyNArray,
    //                    nsize_type(3, 1, 1, 3),
    //                    nsize_type(3, 3, 3, 2)),
        NArrayConstRef2(MyNArray.Subarray(nsize_type(3, 1, 1, 3),
                                          nsize_type(3, 3, 3, 2))),
    //    NArrayConstRef3(MyNArray,
    //                    nsize_type(3, 0, 1, 1),
    //                    nsize_type(3, 2, 2, 3),
    //                    nstride_type(3, 3, 3, 2))
        NArrayConstRef3(MyNArray.Subarray(nsize_type(3, 0, 1, 1),
                                          nsize_type(3, 2, 2, 3)))*/
    {}
    
    virtual void setUp() {
        MyNArray.SetSize(nsize_type(4, 5, 6));
        NArrayConstRef1.SetRef(MyNArray);
        NArrayConstRef2.SubarrayOf(MyNArray, nsize_type(1, 1, 3), nsize_type(3, 3, 2));
        NArrayConstRef3.SubarrayOf(MyNArray, nsize_type(0, 1, 1), nsize_type(2, 2, 3));

        unsigned int i;
        for (i = 0; i < MyNArray.size(); ++i) {
            MyNArray.at(i) = i;
        }
    }
    
    void TestIncrementDecrementForNArray(void);
    void TestDecrementIncrementForNArray(void);
    void TestPreAndPostIncrementAndDecrementForNArray(void);
    void TestContainerSizeForIncrementNArray(void);
    void TestContainerSizeForDecrementNArray(void);
    void TestContainerSizeForReverseIncrementNArray(void);
    void TestContainerSizeForReverseDecrementNArray(void);
    void TestAdditionAndSubtractionSymmetryForNArray(void);
    void TestAdditionAndIncrementEquivalenceForNArray(void);
    void TestOrderingForNArray(void);
    
    void TestIncrementDecrementForNArrayConstRef1(void);
    void TestDecrementIncrementForNArrayConstRef1(void);
    void TestPreAndPostIncrementAndDecrementForNArrayConstRef1(void);
    void TestContainerSizeForIncrementNArrayConstRef1(void);
    void TestContainerSizeForDecrementNArrayConstRef1(void);
    void TestContainerSizeForReverseIncrementNArrayConstRef1(void);
    void TestContainerSizeForReverseDecrementNArrayConstRef1(void);
    void TestAdditionAndSubtractionSymmetryForNArrayConstRef1(void);
    void TestAdditionAndIncrementEquivalenceForNArrayConstRef1(void);
    void TestOrderingForNArrayConstRef1(void);
    
    void TestIncrementDecrementForNArrayConstRef2(void);
    void TestDecrementIncrementForNArrayConstRef2(void);
    void TestPreAndPostIncrementAndDecrementForNArrayConstRef2(void);
    void TestContainerSizeForIncrementNArrayConstRef2(void);
    void TestContainerSizeForDecrementNArrayConstRef2(void);
    void TestContainerSizeForReverseIncrementNArrayConstRef2(void);
    void TestContainerSizeForReverseDecrementNArrayConstRef2(void);
    void TestAdditionAndSubtractionSymmetryForNArrayConstRef2(void);
    void TestAdditionAndIncrementEquivalenceForNArrayConstRef2(void);
    void TestOrderingForNArrayConstRef2(void);
    
    void TestIncrementDecrementForNArrayConstRef3(void);
    void TestDecrementIncrementForNArrayConstRef3(void);
    void TestPreAndPostIncrementAndDecrementForNArrayConstRef3(void);
    void TestContainerSizeForIncrementNArrayConstRef3(void);
    void TestContainerSizeForDecrementNArrayConstRef3(void);
    void TestContainerSizeForReverseIncrementNArrayConstRef3(void);
    void TestContainerSizeForReverseDecrementNArrayConstRef3(void);
    void TestAdditionAndSubtractionSymmetryForNArrayConstRef3(void);
    void TestAdditionAndIncrementEquivalenceForNArrayConstRef3(void);
    void TestOrderingForNArrayConstRef3(void);
/*
    template<class _nArrayType> void TestIncrementDecrement();
    template<class _nArrayType> void TestDecrementIncrement();
    template<class _nArrayType> void TestPreAndPostIncrementAndDecrement();
    template<class _nArrayType> void TestContainerSizeForIncrement();
    template<class _nArrayType> void TestContainerSizeForDecrement();
    template<class _nArrayType> void TestContainerSizeForReverseIncrement();
    template<class _nArrayType> void TestContainerSizeForReverseDecrement();
    template<class _nArrayType> void TestAdditionAndSubtractionSymmetry();
    template<class _nArrayType> void TestAdditionAndIncrementEquivalence();
    template<class _nArrayType> void TestOrdering();*/
};


#endif  // _vctVarStrideNArrayIteratorTest_h

