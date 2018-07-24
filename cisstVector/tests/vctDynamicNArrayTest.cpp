/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Li, Anton Deguet
  Created on: 2006-07-10

  (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctDynamicNArrayTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericNArrayTest.h"

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicNArray.h>
#include <cisstVector/vctDynamicConstNArrayRef.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomDynamicNArray.h>



template <class _elementType>
void vctDynamicNArrayTest::TestAssignment(void)
{
    typedef _elementType value_type;

    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;

    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));

    ArrayType nArray1(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));
    ArrayType nArray2(sizes);
    vctGenericNArrayTest::TestAssignment(nArray1, nArray2);
    return;
}

void vctDynamicNArrayTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctDynamicNArrayTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctDynamicNArrayTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}



#if DANIEL
template <class _elementType>
void vctDynamicNArrayTest::TestAccessMethods(void)
{
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    vctDynamicNArray<value_type> inputNArray(ROWS, COLS);
    vctRandom(inputNArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctDynamicConstNArrayRef<value_type> constInputNArray(ROWS, COLS,
                                                          inputNArray.row_stride(), inputNArray.col_stride(),
                                                          inputNArray.Pointer());
    vctGenericNArrayTest::TestAccessMethods(inputNArray, constInputNArray);
}

void vctDynamicNArrayTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctDynamicNArrayTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctDynamicNArrayTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}
#endif  // DANIEL



template <class _elementType>
void vctDynamicNArrayTest::TestSoNiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;

    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));

    ArrayType nArray(sizes);
    ArrayType result(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctGenericContainerTest::TestSoCiOperations(nArray, result);
}

void vctDynamicNArrayTest::TestSoNiOperationsDouble(void) {
    TestSoNiOperations<double>();
}
void vctDynamicNArrayTest::TestSoNiOperationsFloat(void) {
    TestSoNiOperations<float>();
}
void vctDynamicNArrayTest::TestSoNiOperationsInt(void) {
    TestSoNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestSoNiNiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));

    ArrayType nArray1(sizes);
    ArrayType nArray2(sizes);
    ArrayType nArray3(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(nArray2, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(nArray3, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctGenericContainerTest::TestSoCiCiOperations(nArray1, nArray2, nArray3);
}

void vctDynamicNArrayTest::TestSoNiNiOperationsDouble(void) {
    TestSoNiNiOperations<double>();
}
void vctDynamicNArrayTest::TestSoNiNiOperationsFloat(void) {
    TestSoNiNiOperations<float>();
}
void vctDynamicNArrayTest::TestSoNiNiOperationsInt(void) {
    TestSoNiNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNioNiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray1(sizes);
    ArrayType nArray2(sizes);
    ArrayType result(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(nArray2, static_cast<value_type>(-10), static_cast<value_type>(10));
    RemoveQuasiZero(nArray2);

    vctGenericContainerTest::TestCioCiOperations(nArray1, nArray2, result);
}

void vctDynamicNArrayTest::TestNioNiOperationsDouble(void) {
    TestNioNiOperations<double>();
}
void vctDynamicNArrayTest::TestNioNiOperationsFloat(void) {
    TestNioNiOperations<float>();
}
void vctDynamicNArrayTest::TestNioNiOperationsInt(void) {
    TestNioNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNioSiNiOperations(void) {
   typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray1(sizes);
    ArrayType nArray2(sizes);
    ArrayType result(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(nArray2, static_cast<value_type>(-10), static_cast<value_type>(10));
    RemoveQuasiZero(nArray2);
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-10),
                                      value_type(10),
                                      scalar);

    vctGenericContainerTest::TestCioSiCiOperations(nArray1, scalar, nArray2, result);
}

void vctDynamicNArrayTest::TestNioSiNiOperationsDouble(void) {
    TestNioSiNiOperations<double>();
}
void vctDynamicNArrayTest::TestNioSiNiOperationsFloat(void) {
    TestNioSiNiOperations<float>();
}
void vctDynamicNArrayTest::TestNioSiNiOperationsInt(void) {
    TestNioSiNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNioNiNiOperations(void) {
   typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray1(sizes);
    ArrayType nArray2(sizes);
    ArrayType nArray3(sizes);
    ArrayType result(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(nArray2, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(nArray3, static_cast<value_type>(-10), static_cast<value_type>(10));
    RemoveQuasiZero(nArray3);

    vctGenericContainerTest::TestCioCiCiOperations(nArray1, nArray2, nArray3, result);
}

void vctDynamicNArrayTest::TestNioNiNiOperationsDouble(void) {
    TestNioNiNiOperations<double>();
}
void vctDynamicNArrayTest::TestNioNiNiOperationsFloat(void) {
    TestNioNiNiOperations<float>();
}
void vctDynamicNArrayTest::TestNioNiNiOperationsInt(void) {
    TestNioNiNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNoNiNiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray1(sizes);
    ArrayType nArray2(sizes);
    ArrayType result(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(nArray2, static_cast<value_type>(-10), static_cast<value_type>(10));
    RemoveQuasiZero(nArray2);

    vctGenericContainerTest::TestCoCiCiOperations(nArray1, nArray2, result);
}

void vctDynamicNArrayTest::TestNoNiNiOperationsDouble(void) {
    TestNoNiNiOperations<double>();
}
void vctDynamicNArrayTest::TestNoNiNiOperationsFloat(void) {
    TestNoNiNiOperations<float>();
}
void vctDynamicNArrayTest::TestNoNiNiOperationsInt(void) {
    TestNoNiNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNoNiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    ArrayType result(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctRandom(result, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctGenericContainerTest::TestCoCiOperations(nArray, result);
}

void vctDynamicNArrayTest::TestNoNiOperationsDouble(void) {
    TestNoNiOperations<double>();
}
void vctDynamicNArrayTest::TestNoNiOperationsFloat(void) {
    TestNoNiOperations<float>();
}
void vctDynamicNArrayTest::TestNoNiOperationsInt(void) {
    TestNoNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNoNiSiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    ArrayType result(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));

    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5), value_type(5), scalar);
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }

    vctGenericContainerTest::TestCoCiSiOperations(nArray, scalar, result);
}

void vctDynamicNArrayTest::TestNoNiSiOperationsDouble(void) {
    TestNoNiSiOperations<double>();
}
void vctDynamicNArrayTest::TestNoNiSiOperationsFloat(void) {
    TestNoNiSiOperations<float>();
}
void vctDynamicNArrayTest::TestNoNiSiOperationsInt(void) {
    TestNoNiSiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNoSiNiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    ArrayType result(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    RemoveQuasiZero(nArray);

    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5), value_type(5), scalar);

    vctGenericContainerTest::TestCoSiCiOperations(scalar, nArray, result);
}

void vctDynamicNArrayTest::TestNoSiNiOperationsDouble(void) {
    TestNoSiNiOperations<double>();
}
void vctDynamicNArrayTest::TestNoSiNiOperationsFloat(void) {
    TestNoSiNiOperations<float>();
}
void vctDynamicNArrayTest::TestNoSiNiOperationsInt(void) {
    TestNoSiNiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNioSiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 3};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    ArrayType result(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));

    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5), value_type(5), scalar);
    if ( (scalar < value_type(1)) && (scalar > -value_type(1)) )
    {
        scalar = value_type(3);
    }

    vctGenericContainerTest::TestCioSiOperations(nArray, scalar, result);
}

void vctDynamicNArrayTest::TestNioSiOperationsDouble(void) {
    TestNioSiOperations<double>();
}
void vctDynamicNArrayTest::TestNioSiOperationsFloat(void) {
    TestNioSiOperations<float>();
}
void vctDynamicNArrayTest::TestNioSiOperationsInt(void) {
    TestNioSiOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestNioOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    ArrayType result(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctGenericContainerTest::TestCioOperations(nArray, result);
}

void vctDynamicNArrayTest::TestNioOperationsDouble(void) {
    TestNioOperations<double>();
}
void vctDynamicNArrayTest::TestNioOperationsFloat(void) {
    TestNioOperations<float>();
}
void vctDynamicNArrayTest::TestNioOperationsInt(void) {
    TestNioOperations<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestSoNiSiOperations(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    ArrayType result(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));

    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5), value_type(5), scalar);

    vctGenericContainerTest::TestSoCiSiOperations(nArray, scalar, result);
}

void vctDynamicNArrayTest::TestSoNiSiOperationsDouble(void) {
    TestSoNiSiOperations<double>();
}
void vctDynamicNArrayTest::TestSoNiSiOperationsFloat(void) {
    TestSoNiSiOperations<float>();
}
void vctDynamicNArrayTest::TestSoNiSiOperationsInt(void) {
    TestSoNiSiOperations<int>();
}



template<class _elementType>
void vctDynamicNArrayTest::TestMinAndMax(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctGenericContainerTest::TestMinAndMax(nArray);
}

void vctDynamicNArrayTest::TestMinAndMaxDouble(void) {
    TestMinAndMax<double>();
}

void vctDynamicNArrayTest::TestMinAndMaxFloat(void) {
    TestMinAndMax<float>();
}

void vctDynamicNArrayTest::TestMinAndMaxInt(void) {
    TestMinAndMax<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestIterators(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    vctGenericContainerTest::TestIterators(nArray);
}

void vctDynamicNArrayTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctDynamicNArrayTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctDynamicNArrayTest::TestIteratorsInt(void) {
    TestIterators<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestSTLFunctions(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typename ArrayType::nsize_type sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    ArrayType nArray(sizes);
    vctRandom(nArray, static_cast<value_type>(-10), static_cast<value_type>(10));
    ArrayType result(sizes);

    vctGenericContainerTest::TestSTLFunctions(nArray, result);
}

void vctDynamicNArrayTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctDynamicNArrayTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctDynamicNArrayTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestIsCompact(void)
{
    typedef _elementType value_type;
    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION> ArrayRefType;
    typedef vctDynamicConstNArrayRef<value_type, DIMENSION> ConstArrayRefType;
    typedef typename ArrayType::nsize_type SizesType;;
    typedef typename ArrayType::ndimension_type DimensionsType;
    typedef typename ArrayType::nindex_type IndicesType;
    typedef typename ArrayType::index_type IndexType;

    SizesType sizes;
    vctRandom(sizes, static_cast<size_type>(1), static_cast<size_type>(MAX_SIZE));

    // compact arrays, same order for dimensions
    ArrayType array(sizes);
    CPPUNIT_ASSERT(array.IsCompact());
    ArrayRefType arrayRef(array);
    CPPUNIT_ASSERT(arrayRef.IsCompact());
    ConstArrayRefType constArrayRef(arrayRef);
    CPPUNIT_ASSERT(constArrayRef.IsCompact());

    // build a permutation vector
    DimensionsType permutation;
    IndicesType swaps;
    IndexType index;
    for (index = 0; index < permutation.size(); index++) {
        permutation.Element(index) = index;
    }
    vctRandom(swaps, static_cast<IndexType>(0), permutation.size());
    IndexType temp;
    for (index = 0; index < permutation.size(); index++) {
        temp = permutation[0];
        permutation[0] = permutation[swaps[index]];
        permutation[swaps[index]] = temp;
    }
    // test on compact permutations,i.e. no windowing
    arrayRef.PermutationOf(array, permutation);
    CPPUNIT_ASSERT(arrayRef.IsCompact());
    constArrayRef.PermutationOf(arrayRef, permutation);
    CPPUNIT_ASSERT(constArrayRef.IsCompact());

    // test on non compact permutations, create windows.  For each
    // dimension, take all but first elements.
    vctRandom(sizes, static_cast<size_type>(2), static_cast<size_type>(MAX_SIZE));
    array.SetSize(sizes);
    arrayRef.SubarrayOf(array, SizesType(1), array.sizes() - SizesType(1));
    CPPUNIT_ASSERT(!arrayRef.IsCompact());
    constArrayRef.SubarrayOf(array, SizesType(1), array.sizes() - SizesType(1));
    CPPUNIT_ASSERT(!constArrayRef.IsCompact());
}

void vctDynamicNArrayTest::TestIsCompactDouble(void) {
    TestIsCompact<double>();
}
void vctDynamicNArrayTest::TestIsCompactFloat(void) {
    TestIsCompact<float>();
}
void vctDynamicNArrayTest::TestIsCompactInt(void) {
    TestIsCompact<int>();
}



template <class _elementType>
void vctDynamicNArrayTest::TestFastCopyOf(void) {
    enum {DIMENSION = 4};
    enum {DIM0 = 3, DIM1 = 5, DIM2 = 7, DIM3 = 5}; // Important, DIM1 == DIM3
    typedef _elementType value_type;
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION> ArrayRefType;
    typedef typename ArrayType::nsize_type nsize_type;

    const nsize_type baseSize(DIM0, DIM1, DIM2, DIM3);

    // dynamic nArray
    ArrayType destination(baseSize);
    ArrayType validSource(baseSize);
    vctRandom(validSource, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSource, destination);

    // test for different storage order, but same sizes.  Permute dim
    // 1 and 3 which have the same size.
    ArrayRefType invalidSource;
    invalidSource.PermutationOf(validSource, nsize_type(0, 3, 2, 1));
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidSource));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidSource));

    // test for exception if different size
    ArrayType largerNArray(nsize_type(baseSize.MaxElement()));
    vctGenericContainerTest::TestFastCopyOfException(largerNArray, destination);

    // safe tests using a larger source and not performing checks
    CPPUNIT_ASSERT(largerNArray.FastCopyOf(destination, false));
    CPPUNIT_ASSERT(destination.FastCopyOf(invalidSource, false));
}

void vctDynamicNArrayTest::TestFastCopyOfDouble(void) {
    TestFastCopyOf<double>();
}
void vctDynamicNArrayTest::TestFastCopyOfFloat(void) {
    TestFastCopyOf<float>();
}
void vctDynamicNArrayTest::TestFastCopyOfInt(void) {
    TestFastCopyOf<int>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDynamicNArrayTest);

