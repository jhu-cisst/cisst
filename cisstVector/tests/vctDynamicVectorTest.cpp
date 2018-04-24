/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-07-09

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctDynamicVectorTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericVectorTest.h"


#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomDynamicVector.h>


template <class _elementType>
void vctDynamicVectorTest::TestAssignment(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;
    vctDynamicVector<value_type> inputVector(SIZE);
    vctRandom(inputVector, value_type(-10), value_type(10));
    vctDynamicVector<value_type> outputVector(SIZE);
    vctGenericVectorTest::TestAssignment(inputVector, outputVector);
}

void vctDynamicVectorTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctDynamicVectorTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctDynamicVectorTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestSizingMethods(void) {
    typedef _elementType value_type;
    const unsigned int size = cmnRandomSequence::GetInstance().ExtractRandomInt(1, 13);
    const value_type minValue = static_cast<_elementType>(0);
    const value_type maxValue = static_cast<_elementType>(30);
    // default constructor, size 0, null pointer
    vctDynamicVector<value_type> vector1;
    CPPUNIT_ASSERT(vector1.size() == 0);
    CPPUNIT_ASSERT(vector1.Pointer() == 0);
    // size should be set and pointer should != 0
    vector1.SetSize(size);
    _elementType * pointer1 = vector1.Pointer();
    CPPUNIT_ASSERT(vector1.size() == size);
    CPPUNIT_ASSERT(pointer1 != 0);
    // same size, pointer shouldn't change
    vector1.SetSize(size);
    CPPUNIT_ASSERT(vector1.size() == size);
    CPPUNIT_ASSERT(vector1.Pointer() == pointer1);
    // same size, pointer shouldn't change
    vector1.resize(size);
    CPPUNIT_ASSERT(vector1.size() == size);
    CPPUNIT_ASSERT(vector1.Pointer() == pointer1);

    // different size
    vector1.SetSize(size * 2);
    CPPUNIT_ASSERT(vector1.size() == size * 2);
    // different size with resize
    vector1.resize(size);
    CPPUNIT_ASSERT(vector1.size() == size);

    // try with other constructors
    vctDynamicVector<_elementType> vector2(size);
    CPPUNIT_ASSERT(vector2.size() == size);
    CPPUNIT_ASSERT(vector2.Pointer() != 0);

    vctDynamicVector<_elementType> vector3(vector1);
    CPPUNIT_ASSERT(vector3.size() == size);
    CPPUNIT_ASSERT(vector3.Pointer() != 0);
    CPPUNIT_ASSERT(vector3.Pointer() != vector1.Pointer());


    // test that the resize method preserves the data in the vector object
    vector1.SetSize(size);
    vctRandom(vector1, minValue, maxValue);
    vector3.SetSize(size);
    vector3.Assign(vector1);
    CPPUNIT_ASSERT(vector3.Equal(vector1));
    vector1.resize(size * 2);
    vctDynamicConstVectorRef<_elementType> vector1OldSizeOverlay;
    vector1OldSizeOverlay.SetRef(vector1, 0, size);
    CPPUNIT_ASSERT(vector3.Equal(vector1OldSizeOverlay));
    vector1.resize(size);
    CPPUNIT_ASSERT(vector3.Equal(vector1));
    vctDynamicConstVectorRef<_elementType> subvector3Overlay;
    subvector3Overlay.SetRef(vector3, 0, size-1);
    vector1.resize(size-1);
    CPPUNIT_ASSERT(subvector3Overlay.Equal(vector1));
    vector1.resize(size);
    vector1OldSizeOverlay.SetRef(vector1, 0, size-1);
    CPPUNIT_ASSERT(vector1OldSizeOverlay.Equal(subvector3Overlay));

    // release memory
    vector1.SetSize(0);
    CPPUNIT_ASSERT(vector1.size() == 0);
    CPPUNIT_ASSERT(vector1.Pointer() == 0);
    vector2.resize(0);
    CPPUNIT_ASSERT(vector2.size() == 0);
    CPPUNIT_ASSERT(vector2.Pointer() == 0);

    // call again to make sure
    vector1.SetSize(0);
    CPPUNIT_ASSERT(vector1.size() == 0);
    CPPUNIT_ASSERT(vector1.Pointer() == 0);
    vector2.resize(0);
    CPPUNIT_ASSERT(vector2.size() == 0);
    CPPUNIT_ASSERT(vector2.Pointer() == 0);
}

void vctDynamicVectorTest::TestSizingMethodsDouble(void) {
    TestSizingMethods<double>();
}
void vctDynamicVectorTest::TestSizingMethodsFloat(void) {
    TestSizingMethods<float>();
}
void vctDynamicVectorTest::TestSizingMethodsInt(void) {
    TestSizingMethods<int>();
}

/*! Test conversion from fixed-size vector types */
template<class _elementType>
void vctDynamicVectorTest::TestConversionFromFixedSize(void)
{
    typedef _elementType value_type;
    enum { LONGSIZE = 7, SHORTSIZE = 4 };
    const value_type minValue = static_cast<_elementType>(0);
    const value_type maxValue = static_cast<_elementType>(30);
    vctFixedSizeVector<_elementType, LONGSIZE> fixedVector;
    vctRandom(fixedVector, minValue, maxValue);
    // make sure that the second and third elements in fixedVector are not equal.
    // see why below.
    if (fixedVector[1] == fixedVector[2])
        fixedVector[2] = fixedVector[2] + 1;

    // Test simple creation of dynamic from fixed
    vctDynamicVector<_elementType> dynamic1(fixedVector);
    CPPUNIT_ASSERT(dynamic1.size() == fixedVector.size());
    CPPUNIT_ASSERT(dynamic1.Equal( vctDynamicConstVectorRef<_elementType>(fixedVector) ));

    // Test creation of dynamic from non-const fixed overlay
    vctFixedSizeVectorRef<_elementType, SHORTSIZE, 1> fixedSlice1(fixedVector, 0);
    vctDynamicVector<_elementType>  dynamic2(fixedSlice1);
    CPPUNIT_ASSERT(dynamic2.size() == fixedSlice1.size());
    CPPUNIT_ASSERT(dynamic2.Equal( vctDynamicConstVectorRef<_elementType>(fixedSlice1) ));

    // Test creation of dynamic from const fixed overlay
    vctFixedSizeConstVectorRef<_elementType, SHORTSIZE, 2> fixedSlice2(fixedVector.Pointer());
    vctDynamicVector<_elementType>  dynamic3(fixedSlice2);
    CPPUNIT_ASSERT(dynamic3.size() == fixedSlice2.size());
    CPPUNIT_ASSERT(dynamic3.Equal( vctDynamicConstVectorRef<_elementType>(fixedSlice2) ));
    // Assert that dynamic3 and dynamic2 are NOT EQUAL because they contain different
    // subsequences of fixedVector
    CPPUNIT_ASSERT(!dynamic3.Equal(dynamic2));
}

void vctDynamicVectorTest::TestConversionFromFixedSizeDouble(void)
{
    TestConversionFromFixedSize<double>();
}

void vctDynamicVectorTest::TestConverstionFromFixedSizeFloat(void)
{
    TestConversionFromFixedSize<float>();
}

void vctDynamicVectorTest::TestConversionFromFixedSizeInt(void)
{
    TestConversionFromFixedSize<int>();
}


template <class _elementType>
void vctDynamicVectorTest::TestAccessMethods(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;
    vctDynamicVector<value_type> inputVector(SIZE);
    vctRandom(inputVector, value_type(-10), value_type(10));
    vctDynamicConstVectorRef<value_type> constInputVector(SIZE, inputVector.Pointer());
    vctGenericVectorTest::TestAccessMethods(inputVector, constInputVector);
}

void vctDynamicVectorTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctDynamicVectorTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctDynamicVectorTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}



template<class _elementType>
void vctDynamicVectorTest::TestXYZWMethods(void) {
    vctDynamicVector<_elementType> inputVector1(16);
    vctRandom(inputVector1, _elementType(-10), _elementType(10));
    vctGenericVectorTest::TestConstXYZW(inputVector1);
    vctGenericVectorTest::TestNonConstXYZW(inputVector1);

    vctDynamicVectorRef<_elementType> inputVector2(4, inputVector1.Pointer(0), 4);
    vctGenericVectorTest::TestConstXYZW(inputVector2);
    vctGenericVectorTest::TestNonConstXYZW(inputVector2);

    vctDynamicConstVectorRef<_elementType> inputVector3(4, inputVector1.Pointer(1), 2);
    vctGenericVectorTest::TestConstXYZW(inputVector3);
}

void vctDynamicVectorTest::TestXYZWMethodsDouble() {
    TestXYZWMethods<double>();
}

void vctDynamicVectorTest::TestXYZWMethodsFloat() {
    TestXYZWMethods<float>();
}

void vctDynamicVectorTest::TestXYZWMethodsInt() {
    TestXYZWMethods<int>();
}


template <class _elementType>
void vctDynamicVectorTest::TestSelect(void)
{
    enum {INPUT_SIZE = 12, OUTPUT_SIZE = 5};
    vctDynamicVector<_elementType> inputVector(INPUT_SIZE);
    vctDynamicVector<_elementType> outputVector(OUTPUT_SIZE);
    vctDynamicVector<vct::index_type> indexVector(OUTPUT_SIZE);
    vctRandom(inputVector, _elementType(-10), _elementType(10));
    vctRandom(indexVector, static_cast<vct::size_type>(0), static_cast<vct::size_type>(INPUT_SIZE));
    vctGenericVectorTest::TestSelect(inputVector, indexVector, outputVector);
}

void vctDynamicVectorTest::TestSelectDouble(void)
{
    vctDynamicVectorTest::TestSelect<double>();
}

void vctDynamicVectorTest::TestSelectFloat(void)
{
    vctDynamicVectorTest::TestSelect<float>();
}

void vctDynamicVectorTest::TestSelectInt(void)
{
    vctDynamicVectorTest::TestSelect<int>();
}



template<class _elementType>
void vctDynamicVectorTest::TestSwapElements(void)
{
    enum {SIZE = 6};
    typedef _elementType value_type;
    typedef vctDynamicVector<_elementType> VectorType;
    VectorType v1(SIZE);
    VectorType v2(SIZE);
    vctRandom(v1, value_type(-10), value_type(10));
    vctRandom(v2, value_type(-10), value_type(10));
    vctGenericVectorTest::TestSwapElements(v1, v2);

    typedef vctDynamicVectorRef<_elementType> Sequence1Type;
    Sequence1Type h1(SIZE/2, v1.Pointer(0), 1);
    Sequence1Type h2(SIZE/2, v1.Pointer(SIZE/2), 1);
    vctGenericVectorTest::TestSwapElements(h1, h2);

    typedef vctDynamicVectorRef<_elementType> Sequence2Type;
    Sequence2Type s1(SIZE/2, v2.Pointer(0), 2);
    Sequence2Type s2(SIZE/2, v2.Pointer(1), 2);
    vctGenericVectorTest::TestSwapElements(s1, s2);
}

void vctDynamicVectorTest::TestSwapElementsDouble(void)
{
    TestSwapElements<double>();
}

void vctDynamicVectorTest::TestSwapElementsFloat(void)
{
    TestSwapElements<float>();
}

void vctDynamicVectorTest::TestSwapElementsInt(void)
{
    TestSwapElements<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestSoViOperations(void) {
    enum {SIZE = 6};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector(SIZE);
    vctDynamicVector<value_type> result(SIZE);
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(vector, result);
}

void vctDynamicVectorTest::TestSoViOperationsDouble(void) {
    TestSoViOperations<double>();
}
void vctDynamicVectorTest::TestSoViOperationsFloat(void) {
    TestSoViOperations<float>();
}
void vctDynamicVectorTest::TestSoViOperationsInt(void) {
    TestSoViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestSoViViOperations(void) {
    enum {SIZE = 5};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctDynamicVector<value_type> vector2(SIZE);
    vctDynamicVector<value_type> vector3(SIZE);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(vector1, vector2, vector3);
}

void vctDynamicVectorTest::TestSoViViOperationsDouble(void) {
    TestSoViViOperations<double>();
}
void vctDynamicVectorTest::TestSoViViOperationsFloat(void) {
    TestSoViViOperations<float>();
}
void vctDynamicVectorTest::TestSoViViOperationsInt(void) {
    TestSoViViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVioViOperations(void) {
    enum {SIZE = 8};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctDynamicVector<value_type> vector2(SIZE);
    vctDynamicVector<value_type> result(SIZE);

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCioCiOperations(vector1, vector2, result);
}

void vctDynamicVectorTest::TestVioViOperationsDouble(void) {
    TestVioViOperations<double>();
}
void vctDynamicVectorTest::TestVioViOperationsFloat(void) {
    TestVioViOperations<float>();
}
void vctDynamicVectorTest::TestVioViOperationsInt(void) {
    TestVioViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVioSiViOperations(void) {
    enum {SIZE = 8};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    value_type scalar;
    vctDynamicVector<value_type> vector2(SIZE);
    vctDynamicVector<value_type> result(SIZE);

    vctRandom(vector1, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-10),
                                      value_type(10),
                                      scalar);
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCioSiCiOperations(vector1, scalar, vector2, result);
}

void vctDynamicVectorTest::TestVioSiViOperationsDouble(void) {
    TestVioSiViOperations<double>();
}
void vctDynamicVectorTest::TestVioSiViOperationsFloat(void) {
    TestVioSiViOperations<float>();
}
void vctDynamicVectorTest::TestVioSiViOperationsInt(void) {
    TestVioSiViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVioViViOperations(void) {
    enum {SIZE = 8};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctDynamicVector<value_type> vector2(SIZE);
    vctDynamicVector<value_type> vector3(SIZE);
    vctDynamicVector<value_type> result(SIZE);

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    vctRandom(vector3, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioCiCiOperations(vector1, vector2, vector3, result);
}

void vctDynamicVectorTest::TestVioViViOperationsDouble(void) {
    TestVioViViOperations<double>();
}
void vctDynamicVectorTest::TestVioViViOperationsFloat(void) {
    TestVioViViOperations<float>();
}
void vctDynamicVectorTest::TestVioViViOperationsInt(void) {
    TestVioViViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVoViViOperations(void) {
    enum {SIZE = 4};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctDynamicVector<value_type> vector2(SIZE);
    vctDynamicVector<value_type> result(SIZE);

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));
    RemoveQuasiZero(vector2);

    vctGenericContainerTest::TestCoCiCiOperations(vector1, vector2, result);
}

void vctDynamicVectorTest::TestVoViViOperationsDouble(void) {
    TestVoViViOperations<double>();
}
void vctDynamicVectorTest::TestVoViViOperationsFloat(void) {
    TestVoViViOperations<float>();
}
void vctDynamicVectorTest::TestVoViViOperationsInt(void) {
    TestVoViViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestCrossProduct(void) {
    enum {SIZE = 3};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctDynamicVector<value_type> vector2(SIZE);
    vctDynamicVector<value_type> result(SIZE);

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestCrossProduct(vector1, vector2, result);
}

void vctDynamicVectorTest::TestCrossProductDouble(void) {
    TestCrossProduct<double>();
}
void vctDynamicVectorTest::TestCrossProductFloat(void) {
    TestCrossProduct<float>();
}
void vctDynamicVectorTest::TestCrossProductInt(void) {
    TestCrossProduct<int>();
}


template <class _elementType>
void vctDynamicVectorTest::TestDotProduct(void) {
    enum {SIZE = 12};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctDynamicVector<value_type> vector2(SIZE);

    vctRandom(vector1, value_type(-10), value_type(10));
    vctRandom(vector2, value_type(-10), value_type(10));

    vctGenericVectorTest::TestDotProduct(vector1, vector2);
}

void vctDynamicVectorTest::TestDotProductDouble(void) {
    TestDotProduct<double>();
}
void vctDynamicVectorTest::TestDotProductFloat(void) {
    TestDotProduct<float>();
}
void vctDynamicVectorTest::TestDotProductInt(void) {
    TestDotProduct<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVoViOperations(void) {
    enum {SIZE = 4};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector(SIZE);
    vctDynamicVector<value_type> result(SIZE);
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(vector, result);
}

void vctDynamicVectorTest::TestVoViOperationsDouble(void) {
    TestVoViOperations<double>();
}
void vctDynamicVectorTest::TestVoViOperationsFloat(void) {
    TestVoViOperations<float>();
}
void vctDynamicVectorTest::TestVoViOperationsInt(void) {
    TestVoViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVoViSiOperations(void) {
    enum {SIZE = 8};
    typedef _elementType value_type;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctDynamicVector<value_type> vector(SIZE);
    vctDynamicVector<value_type> result(SIZE);
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiSiOperations(vector, scalar, result);
}

void vctDynamicVectorTest::TestVoViSiOperationsDouble(void) {
    TestVoViSiOperations<double>();
}
void vctDynamicVectorTest::TestVoViSiOperationsFloat(void) {
    TestVoViSiOperations<float>();
}
void vctDynamicVectorTest::TestVoViSiOperationsInt(void) {
    TestVoViSiOperations<int>();
}




template <class _elementType>
void vctDynamicVectorTest::TestVoSiViOperations(void) {
    enum {SIZE = 10};
    typedef _elementType value_type;
    value_type scalar;
    vctDynamicVector<value_type> vector(SIZE);
    vctDynamicVector<value_type> result(SIZE);
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                       scalar);
    vctRandom(vector, value_type(-10), value_type(10));
    RemoveQuasiZero(vector);

    vctGenericContainerTest::TestCoSiCiOperations(scalar, vector, result);
}

void vctDynamicVectorTest::TestVoSiViOperationsDouble(void) {
    TestVoSiViOperations<double>();
}
void vctDynamicVectorTest::TestVoSiViOperationsFloat(void) {
    TestVoSiViOperations<float>();
}
void vctDynamicVectorTest::TestVoSiViOperationsInt(void) {
    TestVoSiViOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVioSiOperations(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector(SIZE);
    vctDynamicVector<value_type> result(SIZE);
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

void vctDynamicVectorTest::TestVioSiOperationsDouble(void) {
    TestVioSiOperations<double>();
}
void vctDynamicVectorTest::TestVioSiOperationsFloat(void) {
    TestVioSiOperations<float>();
}
void vctDynamicVectorTest::TestVioSiOperationsInt(void) {
    TestVioSiOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestVioOperations(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector(SIZE);
    vctDynamicVector<value_type> result(SIZE);
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(vector, result);
}

void vctDynamicVectorTest::TestVioOperationsDouble(void) {
    TestVioOperations<double>();
}
void vctDynamicVectorTest::TestVioOperationsFloat(void) {
    TestVioOperations<float>();
}
void vctDynamicVectorTest::TestVioOperationsInt(void) {
    TestVioOperations<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestSoViSiOperations(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector(SIZE);
    vctDynamicVector<value_type> result(SIZE);
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctRandom(vector, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiSiOperations(vector, scalar, result);
}

void vctDynamicVectorTest::TestSoViSiOperationsDouble(void) {
    TestSoViSiOperations<double>();
}
void vctDynamicVectorTest::TestSoViSiOperationsFloat(void) {
    TestSoViSiOperations<float>();
}
void vctDynamicVectorTest::TestSoViSiOperationsInt(void) {
    TestSoViSiOperations<int>();
}



template<class _elementType>
void vctDynamicVectorTest::TestMinAndMax(void) {
    enum {SIZE = 12};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector(SIZE);
    vctRandom(vector, value_type(-50), value_type(50));
    vctGenericContainerTest::TestMinAndMax(vector);
}

void vctDynamicVectorTest::TestMinAndMaxDouble(void) {
    TestMinAndMax<double>();
}

void vctDynamicVectorTest::TestMinAndMaxFloat(void) {
    TestMinAndMax<float>();
}

void vctDynamicVectorTest::TestMinAndMaxInt(void) {
    TestMinAndMax<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestIterators(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(vector1);
}

void vctDynamicVectorTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctDynamicVectorTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctDynamicVectorTest::TestIteratorsInt(void) {
    TestIterators<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestSTLFunctions(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctDynamicVector<value_type> vector2(SIZE);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(vector1, vector2);
}

void vctDynamicVectorTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctDynamicVectorTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctDynamicVectorTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestFastCopyOf(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;

    // dynamic vector
    vctDynamicVector<value_type> destination(SIZE);
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


    // test for exception if different size
    typedef vctFixedSizeVector<value_type, SIZE * 2> LargerVectorType;
    LargerVectorType largerVectorFixed;
    vctGenericContainerTest::TestFastCopyOfException(largerVectorFixed, destination);

    // test for not compact
    vctFixedSizeVectorRef<value_type, SIZE, 2> invalidRefFixed(largerVectorFixed.Pointer());
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidRefFixed));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidRefFixed));

    // test for empty containers
    validSource.SetSize(0);
    destination.SetSize(0);
    CPPUNIT_ASSERT(destination.FastCopyCompatible(validSource));
    CPPUNIT_ASSERT(destination.FastCopyOf(validSource));
}

void vctDynamicVectorTest::TestFastCopyOfDouble(void) {
    TestFastCopyOf<double>();
}
void vctDynamicVectorTest::TestFastCopyOfFloat(void) {
    TestFastCopyOf<float>();
}
void vctDynamicVectorTest::TestFastCopyOfInt(void) {
    TestFastCopyOf<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestZeros(void) {
    enum {SIZE = 7};
    typedef _elementType value_type;

    // dynamic vector
    vctDynamicVector<value_type> destination(2 * SIZE);
    CPPUNIT_ASSERT(destination.Zeros());
    CPPUNIT_ASSERT(destination.Equal(static_cast<value_type>(0)));

    // test for not compact (every other element)
    vctDynamicVectorRef<value_type> nonCompact(SIZE, destination.Pointer(), 2);
    CPPUNIT_ASSERT(!nonCompact.Zeros());
    CPPUNIT_ASSERT(nonCompact.Equal(static_cast<value_type>(0)));

    // test for empty containers
    destination.SetSize(0);
    CPPUNIT_ASSERT(destination.Zeros());
}

void vctDynamicVectorTest::TestZerosDouble(void) {
    TestZeros<double>();
}
void vctDynamicVectorTest::TestZerosFloat(void) {
    TestZeros<float>();
}
void vctDynamicVectorTest::TestZerosInt(void) {
    TestZeros<int>();
}



template <class _elementType>
void vctDynamicVectorTest::TestNormalization(void) {
    enum {SIZE = 9};
    typedef _elementType value_type;
    vctDynamicVector<value_type> vector1(SIZE);
    vctRandom(vector1, value_type(-10), value_type(10));
    vctGenericVectorTest::TestNormalization(vector1);
}

void vctDynamicVectorTest::TestNormalizationDouble(void) {
    TestNormalization<double>();
}
void vctDynamicVectorTest::TestNormalizationFloat(void) {
    TestNormalization<float>();
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctDynamicVectorTest);
