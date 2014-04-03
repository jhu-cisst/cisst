/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2003-08-20

  (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctFixedSizeVectorTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericVectorTest.h"


#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeConstVectorRef.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomDynamicVector.h>


void vctFixedSizeVectorTest::TestSubsequence(void)
{
    typedef float ElementType;
    enum {SIZE0 = 14};
    enum {STRIDE1=3, POSITION1=1, SIZE1= (SIZE0 - POSITION1) / STRIDE1 +
          ( ((SIZE0-POSITION1)%STRIDE1) != 0) };
    enum {STRIDE2=2, POSITION2=0, SIZE2= (SIZE1 - POSITION2) / STRIDE2 +
          ( ((SIZE1-POSITION2)%STRIDE2) != 0) };


    typedef vctFixedSizeVector<ElementType, SIZE0> VectorType;

    /* build an initial array */
    VectorType initialArray;
    unsigned int i;
    for (i = 0; i < initialArray.size(); ++i) {
        initialArray[i] = (ElementType) i;
    }

    /* use an iterator to browse the elements */
    i = 0;
    VectorType::iterator elementIterator = initialArray.begin();
    for (; elementIterator != initialArray.end(); ++elementIterator) {
        CPPUNIT_ASSERT(((*elementIterator) == initialArray[i]));
        CPPUNIT_ASSERT((*elementIterator) == (ElementType) i);
        i++;
    }


    /* get a subsequence and make sure the elements match */
    typedef vctFixedSizeVectorRef<ElementType, SIZE1, STRIDE1> Subsequence1Type;
    int initialPosition1 = POSITION1;
    Subsequence1Type subsequence1(initialArray.Pointer(initialPosition1));

    Subsequence1Type::const_iterator subsequence1Iterator = subsequence1.begin();
    i = 0;
    for (; subsequence1Iterator != subsequence1.end(); ++subsequence1Iterator) {
        CPPUNIT_ASSERT( (*subsequence1Iterator) == initialArray[POSITION1 + i * STRIDE1] );
        i++;
    }

    /* modify the last element of the subsequence and make use the initial array is modified */
    subsequence1[SIZE1 - 1]++;
    CPPUNIT_ASSERT( subsequence1[SIZE1 - 1] == initialArray[POSITION1 + (SIZE1 - 1) * STRIDE1]);

    /* get a subsequence of the subsequence */
    typedef vctFixedSizeConstVectorRef<ElementType, SIZE2, STRIDE2 * STRIDE1> Subsequence2Type;
    int initialPosition2 = POSITION2;
    Subsequence2Type subsequence2(subsequence1.Pointer(initialPosition2));
    i = 0;
    Subsequence2Type::const_iterator subsequence2Iterator = ((const Subsequence2Type &)subsequence2).begin();
    for (; subsequence2Iterator != subsequence2.end(); ++subsequence2Iterator) {
        CPPUNIT_ASSERT( *subsequence2Iterator == subsequence1[POSITION2 + i * STRIDE2] );
        CPPUNIT_ASSERT( *subsequence2Iterator == initialArray[POSITION1 + POSITION2 + i * STRIDE1 * STRIDE2] );
        i++;
    }

    /* test the reverse iterator on the initial array */
    VectorType::const_reverse_iterator reverseIterator = initialArray.rbegin();
    i = SIZE0 - 1;
    for (; reverseIterator != initialArray.rend(); ++reverseIterator) {
        /* special case because the last element has been modified */
        if (reverseIterator == initialArray.rbegin()) {
            CPPUNIT_ASSERT( (*reverseIterator) == (i + 1) );
        } else {
            CPPUNIT_ASSERT( (*reverseIterator) == i );
        }
        CPPUNIT_ASSERT( (*reverseIterator) == initialArray[i] );
        i--;
    }
}




template <class _elementType>
void vctFixedSizeVectorTest::TestConcatenation(void) {
    typedef vctFixedSizeVector<_elementType, 3> VectorType3;
    typedef vctFixedSizeVector<_elementType, 4> VectorType4;
    unsigned int i;

    VectorType3 testVector3;
    VectorType4 testVector4;

    _elementType refVector[4] = {_elementType(122),
                                 _elementType(-25),
                                 _elementType(33),
                                 _elementType(-10)};

    testVector3.Assign(refVector[0],
                       refVector[1],
                       refVector[2]);
    testVector4.ConcatenationOf(testVector3, -10);

    for (i = 0; i < testVector4.size(); i++) {
        CPPUNIT_ASSERT(refVector[i] == testVector4[i]);
    }
}


void vctFixedSizeVectorTest::TestConcatenationDouble(void) {
    TestConcatenation<double>();
}

void vctFixedSizeVectorTest::TestConcatenationFloat(void) {
    TestConcatenation<float>();
}

void vctFixedSizeVectorTest::TestConcatenationInt(void) {
    TestConcatenation<int>();
}

void vctFixedSizeVectorTest::TestConcatenationChar(void) {
    TestConcatenation<char>();
}


template<class _elementType>
void vctFixedSizeVectorTest::TestConcatenationOperator()
{
    typedef vctFixedSizeVector<_elementType, 2> VectorType2;
    typedef vctFixedSizeVector<_elementType, 3> VectorType3;
    typedef vctFixedSizeVector<_elementType, 5> VectorType5;
    typedef vctFixedSizeVector<_elementType, 7> VectorType7;
    typedef vctFixedSizeVector<_elementType, 8> VectorType8;

    VectorType3 testVector3_1(1, 2, 3);
    VectorType2 testVector2_1(11, 12);
    VectorType5 testVector5_1 = testVector3_1 & testVector2_1;
    const vctFixedSizeConstVectorRef<_elementType, 3, 1> subsequence1(testVector5_1.Pointer(0));
    const vctFixedSizeConstVectorRef<_elementType, 2, 1> subsequence2(testVector5_1.Pointer(3));
    CPPUNIT_ASSERT( subsequence1 == testVector3_1 );
    CPPUNIT_ASSERT( subsequence2 == testVector2_1 );

    VectorType2 testVector2_2(21, 22);
    VectorType7 testVector7_1 = testVector2_1 & testVector3_1 & testVector2_2;
    const vctFixedSizeConstVectorRef<_elementType, 2, 1> subsequence3(testVector7_1.Pointer(0));
    const vctFixedSizeConstVectorRef<_elementType, 3, 1> subsequence4(testVector7_1.Pointer(2));
    const vctFixedSizeConstVectorRef<_elementType, 2, 1> subsequence5(testVector7_1.Pointer(5));
    CPPUNIT_ASSERT(subsequence3 == testVector2_1);
    CPPUNIT_ASSERT(subsequence4 == testVector3_1);
    CPPUNIT_ASSERT(subsequence5 == testVector2_2);

    VectorType3 testVector3_2(31, 32, 33);
    VectorType8 testVector8_1 = testVector3_1 & testVector3_2 & testVector2_1;
    const vctFixedSizeConstVectorRef<_elementType, 3, 1> subsequence6(testVector8_1.Pointer(0));
    const vctFixedSizeConstVectorRef<_elementType, 3, 1> subsequence7(testVector8_1.Pointer(3));
    const vctFixedSizeConstVectorRef<_elementType, 2, 1> subsequence8(testVector8_1.Pointer(6));
    CPPUNIT_ASSERT(subsequence6 == testVector3_1);
    CPPUNIT_ASSERT(subsequence7 == testVector3_2);
    CPPUNIT_ASSERT(subsequence8 == testVector2_1);
}

void vctFixedSizeVectorTest::TestConcatenationOperatorDouble()
{
    TestConcatenationOperator<double>();
}

void vctFixedSizeVectorTest::TestConcatenationOperatorFloat()
{
    TestConcatenationOperator<float>();
}


void vctFixedSizeVectorTest::TestConcatenationOperatorInt()
{
    TestConcatenationOperator<int>();
}

void vctFixedSizeVectorTest::TestConcatenationOperatorChar()
{
    TestConcatenationOperator<char>();
}


template <class _elementType>
void vctFixedSizeVectorTest::TestAssignment(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> inputVector;
    vctRandom(inputVector, value_type(-10), value_type(10));
    vctFixedSizeVector<value_type, SIZE> outputVector;
    vctGenericVectorTest::TestAssignment(inputVector, outputVector);
}

void vctFixedSizeVectorTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctFixedSizeVectorTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctFixedSizeVectorTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestAccessMethods(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> inputVector;
    vctRandom(inputVector, value_type(-10), value_type(10));
    vctFixedSizeConstVectorRef<value_type, SIZE, 1> constInputVector(inputVector.Pointer());
    vctGenericVectorTest::TestAccessMethods(inputVector, constInputVector);
}

void vctFixedSizeVectorTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctFixedSizeVectorTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctFixedSizeVectorTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}


template<class _elementType>
void vctFixedSizeVectorTest::TestXYZWMethods(void) {
    vctFixedSizeVector<_elementType, 16> inputVector1;
    vctRandom(inputVector1, _elementType(-10), _elementType(10));
    vctGenericVectorTest::TestConstXYZW(inputVector1);
    vctGenericVectorTest::TestNonConstXYZW(inputVector1);

    vctFixedSizeVectorRef<_elementType, 4, 4> inputVector2(inputVector1.Pointer(0));
    vctGenericVectorTest::TestConstXYZW(inputVector2);
    vctGenericVectorTest::TestNonConstXYZW(inputVector2);

    vctFixedSizeConstVectorRef<_elementType, 4, 2> inputVector3(inputVector1.Pointer(1));
    vctGenericVectorTest::TestConstXYZW(inputVector3);
}

void vctFixedSizeVectorTest::TestXYZWMethodsDouble() {
    TestXYZWMethods<double>();
}

void vctFixedSizeVectorTest::TestXYZWMethodsFloat() {
    TestXYZWMethods<float>();
}

void vctFixedSizeVectorTest::TestXYZWMethodsInt() {
    TestXYZWMethods<int>();
}


template <class _elementType>
void vctFixedSizeVectorTest::TestSelect(void)
{
    enum {INPUT_SIZE = 12, OUTPUT_SIZE = 5};
    vctFixedSizeVector<_elementType, INPUT_SIZE> inputVector;
    vctFixedSizeVector<_elementType, OUTPUT_SIZE> outputVector;
    vctFixedSizeVector<vct::index_type, OUTPUT_SIZE> indexVector;
    vctRandom(inputVector, _elementType(-10), _elementType(10));
    vctRandom(indexVector, static_cast<vct::index_type>(0), static_cast<vct::index_type>(INPUT_SIZE));
    vctGenericVectorTest::TestSelect(inputVector, indexVector, outputVector);
}

void vctFixedSizeVectorTest::TestSelectDouble(void)
{
    vctFixedSizeVectorTest::TestSelect<double>();
}

void vctFixedSizeVectorTest::TestSelectFloat(void)
{
    vctFixedSizeVectorTest::TestSelect<float>();
}

void vctFixedSizeVectorTest::TestSelectInt(void)
{
    vctFixedSizeVectorTest::TestSelect<int>();
}


template<class _elementType>
void vctFixedSizeVectorTest::TestSwapElements(void)
{
    enum {SIZE = 6};
    typedef _elementType value_type;
    typedef vctFixedSizeVector<_elementType, SIZE> VectorType;
    VectorType v1, v2;
    vctRandom(v1, value_type(-10), value_type(10));
    vctRandom(v2, value_type(-10), value_type(10));
    vctGenericVectorTest::TestSwapElements(v1, v2);

    typedef vctFixedSizeVectorRef<_elementType, SIZE/2, 1> Sequence1Type;
    Sequence1Type h1(v1.Pointer(0));
    Sequence1Type h2(v1.Pointer(SIZE/2));
    vctGenericVectorTest::TestSwapElements(h1, h2);

    typedef vctFixedSizeVectorRef<_elementType, SIZE/2, 2> Sequence2Type;
    Sequence2Type s1(v2.Pointer(0));
    Sequence2Type s2(v2.Pointer(1));
    vctGenericVectorTest::TestSwapElements(s1, s2);
}

void vctFixedSizeVectorTest::TestSwapElementsDouble(void)
{
    TestSwapElements<double>();
}

void vctFixedSizeVectorTest::TestSwapElementsFloat(void)
{
    TestSwapElements<float>();
}

void vctFixedSizeVectorTest::TestSwapElementsInt(void)
{
    TestSwapElements<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestSoViOperations(void) {
    enum {SIZE = 6};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector;
    vctFixedSizeVector<value_type, SIZE> result;
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(vector, result);
}

void vctFixedSizeVectorTest::TestSoViOperationsDouble(void) {
    TestSoViOperations<double>();
}
void vctFixedSizeVectorTest::TestSoViOperationsFloat(void) {
    TestSoViOperations<float>();
}
void vctFixedSizeVectorTest::TestSoViOperationsInt(void) {
    TestSoViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestSoViViOperations(void) {
    enum {SIZE = 5};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctFixedSizeVector<value_type, SIZE> vector2;
    vctFixedSizeVector<value_type, SIZE> vector3;
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(vector1, vector2, vector3);
}

void vctFixedSizeVectorTest::TestSoViViOperationsDouble(void) {
    TestSoViViOperations<double>();
}
void vctFixedSizeVectorTest::TestSoViViOperationsFloat(void) {
    TestSoViViOperations<float>();
}
void vctFixedSizeVectorTest::TestSoViViOperationsInt(void) {
    TestSoViViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVioViOperations(void) {
    enum {SIZE = 8};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctFixedSizeVector<value_type, SIZE> vector2;
    vctFixedSizeVector<value_type, SIZE> result;

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCioCiOperations(vector1, vector2, result);
}

void vctFixedSizeVectorTest::TestVioViOperationsDouble(void) {
    TestVioViOperations<double>();
}
void vctFixedSizeVectorTest::TestVioViOperationsFloat(void) {
    TestVioViOperations<float>();
}
void vctFixedSizeVectorTest::TestVioViOperationsInt(void) {
    TestVioViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVoViViOperations(void) {
    enum {SIZE = 4};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctFixedSizeVector<value_type, SIZE> vector2;
    vctFixedSizeVector<value_type, SIZE> result;

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCoCiCiOperations(vector1, vector2, result);
}

void vctFixedSizeVectorTest::TestVoViViOperationsDouble(void) {
    TestVoViViOperations<double>();
}
void vctFixedSizeVectorTest::TestVoViViOperationsFloat(void) {
    TestVoViViOperations<float>();
}
void vctFixedSizeVectorTest::TestVoViViOperationsInt(void) {
    TestVoViViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestCrossProduct(void) {
    enum {SIZE = 3};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctFixedSizeVector<value_type, SIZE> vector2;
    vctFixedSizeVector<value_type, SIZE> result;

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestCrossProduct(vector1, vector2, result);
}

void vctFixedSizeVectorTest::TestCrossProductDouble(void) {
    TestCrossProduct<double>();
}
void vctFixedSizeVectorTest::TestCrossProductFloat(void) {
    TestCrossProduct<float>();
}
void vctFixedSizeVectorTest::TestCrossProductInt(void) {
    TestCrossProduct<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestDotProduct(void) {
    enum {SIZE = 8};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctFixedSizeVector<value_type, SIZE> vector2;

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestDotProduct(vector1, vector2);
}

void vctFixedSizeVectorTest::TestDotProductDouble(void) {
    TestDotProduct<double>();
}
void vctFixedSizeVectorTest::TestDotProductFloat(void) {
    TestDotProduct<float>();
}
void vctFixedSizeVectorTest::TestDotProductInt(void) {
    TestDotProduct<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVoViOperations(void) {
    enum {SIZE = 4};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector;
    vctFixedSizeVector<value_type, SIZE> result;
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(vector, result);
}

void vctFixedSizeVectorTest::TestVoViOperationsDouble(void) {
    TestVoViOperations<double>();
}
void vctFixedSizeVectorTest::TestVoViOperationsFloat(void) {
    TestVoViOperations<float>();
}
void vctFixedSizeVectorTest::TestVoViOperationsInt(void) {
    TestVoViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVoViSiOperations(void) {
    enum {SIZE = 8};
    typedef _elementType value_type;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctFixedSizeVector<value_type, SIZE> vector;
    vctFixedSizeVector<value_type, SIZE> result;
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiSiOperations(vector, scalar, result);
}

void vctFixedSizeVectorTest::TestVoViSiOperationsDouble(void) {
    TestVoViSiOperations<double>();
}
void vctFixedSizeVectorTest::TestVoViSiOperationsFloat(void) {
    TestVoViSiOperations<float>();
}
void vctFixedSizeVectorTest::TestVoViSiOperationsInt(void) {
    TestVoViSiOperations<int>();
}




template <class _elementType>
void vctFixedSizeVectorTest::TestVoSiViOperations(void) {
    enum {SIZE = 10};
    typedef _elementType value_type;
    value_type scalar;
    vctFixedSizeVector<value_type, SIZE> vector;
    vctFixedSizeVector<value_type, SIZE> result;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                       scalar);
    vctRandom(vector, value_type(-10), value_type(10));
    RemoveQuasiZero(vector);

    vctGenericContainerTest::TestCoSiCiOperations(scalar, vector, result);
}

void vctFixedSizeVectorTest::TestVoSiViOperationsDouble(void) {
    TestVoSiViOperations<double>();
}
void vctFixedSizeVectorTest::TestVoSiViOperationsFloat(void) {
    TestVoSiViOperations<float>();
}
void vctFixedSizeVectorTest::TestVoSiViOperationsInt(void) {
    TestVoSiViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVioSiOperations(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector;
    vctFixedSizeVector<value_type, SIZE> result;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioSiOperations(vector, scalar, result);
}

void vctFixedSizeVectorTest::TestVioSiOperationsDouble(void) {
    TestVioSiOperations<double>();
}
void vctFixedSizeVectorTest::TestVioSiOperationsFloat(void) {
    TestVioSiOperations<float>();
}
void vctFixedSizeVectorTest::TestVioSiOperationsInt(void) {
    TestVioSiOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVioSiViOperations(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    value_type scalar;
    vctFixedSizeVector<value_type, SIZE> vector2;
    vctFixedSizeVector<value_type, SIZE> result;

    vctRandom(vector1, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-10),
                                      value_type(10),
                                      scalar);
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCioSiCiOperations(vector1, scalar, vector2, result);
}

void vctFixedSizeVectorTest::TestVioSiViOperationsDouble(void) {
    TestVioSiViOperations<double>();
}
void vctFixedSizeVectorTest::TestVioSiViOperationsFloat(void) {
    TestVioSiViOperations<float>();
}
void vctFixedSizeVectorTest::TestVioSiViOperationsInt(void) {
    TestVioSiViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVioViViOperations(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctFixedSizeVector<value_type, SIZE> vector2;
    vctFixedSizeVector<value_type, SIZE> vector3;
    vctFixedSizeVector<value_type, SIZE> result;

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    vctRandom(vector3, value_type(-10), value_type(10));
    RemoveQuasiZero(vector3);

    vctGenericContainerTest::TestCioCiCiOperations(vector1, vector2, vector3, result);
}

void vctFixedSizeVectorTest::TestVioViViOperationsDouble(void) {
    TestVioViViOperations<double>();
}
void vctFixedSizeVectorTest::TestVioViViOperationsFloat(void) {
    TestVioViViOperations<float>();
}
void vctFixedSizeVectorTest::TestVioViViOperationsInt(void) {
    TestVioViViOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestVioOperations(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector;
    vctFixedSizeVector<value_type, SIZE> result;
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(vector, result);
}

void vctFixedSizeVectorTest::TestVioOperationsDouble(void) {
    TestVioOperations<double>();
}
void vctFixedSizeVectorTest::TestVioOperationsFloat(void) {
    TestVioOperations<float>();
}
void vctFixedSizeVectorTest::TestVioOperationsInt(void) {
    TestVioOperations<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestSoViSiOperations(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector;
    vctFixedSizeVector<value_type, SIZE> result;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiSiOperations(vector, scalar, result);
}

void vctFixedSizeVectorTest::TestSoViSiOperationsDouble(void) {
    TestSoViSiOperations<double>();
}
void vctFixedSizeVectorTest::TestSoViSiOperationsFloat(void) {
    TestSoViSiOperations<float>();
}
void vctFixedSizeVectorTest::TestSoViSiOperationsInt(void) {
    TestSoViSiOperations<int>();
}


template<class _elementType>
void vctFixedSizeVectorTest::TestMinAndMax(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector;
    vctRandom(vector, value_type(-50), value_type(50));
    vctGenericContainerTest::TestMinAndMax(vector);
}

void vctFixedSizeVectorTest::TestMinAndMaxDouble(void) {
    TestMinAndMax<double>();
}

void vctFixedSizeVectorTest::TestMinAndMaxFloat(void) {
    TestMinAndMax<float>();
}

void vctFixedSizeVectorTest::TestMinAndMaxInt(void) {
    TestMinAndMax<int>();
}


template <class _elementType>
void vctFixedSizeVectorTest::TestIterators(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(vector1);
}

void vctFixedSizeVectorTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctFixedSizeVectorTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctFixedSizeVectorTest::TestIteratorsInt(void) {
    TestIterators<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestSTLFunctions(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctFixedSizeVector<value_type, SIZE> vector2;
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(vector1, vector2);
}

void vctFixedSizeVectorTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctFixedSizeVectorTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctFixedSizeVectorTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestFastCopyOf(void) {
    enum {SIZE = 6};
    typedef _elementType value_type;

    // dynamic vector
    vctFixedSizeVector<value_type, SIZE> destination;
    vctDynamicVector<value_type> validSource(SIZE);
    vctRandom(validSource, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSource, destination);

    // test for exception if different size
    vctDynamicVector<value_type> largerVector(SIZE * 2);
    vctGenericContainerTest::TestFastCopyOfException(largerVector, destination);

    // test for not compact
    vctDynamicVectorRef<value_type> invalidRef(SIZE, largerVector.Pointer(), 2);
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidRef));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidRef));

    // safe tests using a larger source and not performing checks
    CPPUNIT_ASSERT(largerVector.FastCopyOf(destination, false));

    // fixed size vector
    vctFixedSizeVector<value_type, SIZE> validSourceFixed;
    vctRandom(validSourceFixed, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSourceFixed, destination);
}

void vctFixedSizeVectorTest::TestFastCopyOfDouble(void) {
    TestFastCopyOf<double>();
}
void vctFixedSizeVectorTest::TestFastCopyOfFloat(void) {
    TestFastCopyOf<float>();
}
void vctFixedSizeVectorTest::TestFastCopyOfInt(void) {
    TestFastCopyOf<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestZeros(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;

    // dynamic vector
    vctFixedSizeVector<value_type, 2 * SIZE> destination;
    CPPUNIT_ASSERT(destination.Zeros());
    CPPUNIT_ASSERT(destination.Equal(static_cast<value_type>(0)));

    // test for not compact (every other element)
    vctFixedSizeVectorRef<value_type, SIZE, 2> nonCompact(destination.Pointer());
    CPPUNIT_ASSERT(!nonCompact.Zeros());
    CPPUNIT_ASSERT(nonCompact.Equal(static_cast<value_type>(0)));
}

void vctFixedSizeVectorTest::TestZerosDouble(void) {
    TestZeros<double>();
}
void vctFixedSizeVectorTest::TestZerosFloat(void) {
    TestZeros<float>();
}
void vctFixedSizeVectorTest::TestZerosInt(void) {
    TestZeros<int>();
}



template <class _elementType>
void vctFixedSizeVectorTest::TestNormalization(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctFixedSizeVector<value_type, SIZE> vector1;
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericVectorTest::TestNormalization(vector1);
}

void vctFixedSizeVectorTest::TestNormalizationDouble(void) {
    TestNormalization<double>();
}
void vctFixedSizeVectorTest::TestNormalizationFloat(void) {
    TestNormalization<float>();
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctFixedSizeVectorTest);

