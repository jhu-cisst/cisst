/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Anton Deguet
  Created on:	2007-02-06
  
  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctDynamicNArrayRefTest.h"

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicNArray.h>
#include <cisstVector/vctDynamicConstNArrayRef.h>
#include <cisstVector/vctDynamicNArrayRef.h>
#include <cisstVector/vctRandomDynamicNArray.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include "vctGenericNArrayTest.h"


template <class _elementType>
void vctDynamicNArrayRefTest::TestRefOnArray(void)
{
    typedef _elementType value_type;

    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION> ArrayRefType;
    typedef vctDynamicConstNArrayRef<value_type, DIMENSION> ConstArrayRefType;

    typedef typename ArrayType::index_type index_type;
    typedef typename ArrayType::nsize_type SizesType;
    typedef typename ArrayType::nsize_type IndicesType;

    SizesType sizes;
    vctRandom(sizes, static_cast<index_type>(MIN_SIZE), static_cast<index_type>(MAX_SIZE));

    ArrayType nArray1(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));

    ArrayRefType nArray1Ref1(nArray1);
    ArrayRefType nArray1Ref2;
    nArray1Ref2.SetRef(nArray1);
    
    ConstArrayRefType nArray1ConstRef1(nArray1);
    ConstArrayRefType nArray1ConstRef2;
    nArray1ConstRef2.SetRef(nArray1);
    
    CPPUNIT_ASSERT(nArray1.sizes() == nArray1Ref1.sizes());
    CPPUNIT_ASSERT(nArray1.strides() == nArray1Ref1.strides());

    CPPUNIT_ASSERT(nArray1.sizes() == nArray1Ref2.sizes());
    CPPUNIT_ASSERT(nArray1.strides() == nArray1Ref2.strides());

    CPPUNIT_ASSERT(nArray1.sizes() == nArray1ConstRef1.sizes());
    CPPUNIT_ASSERT(nArray1.strides() == nArray1ConstRef1.strides());

    CPPUNIT_ASSERT(nArray1.sizes() == nArray1ConstRef2.sizes());
    CPPUNIT_ASSERT(nArray1.strides() == nArray1ConstRef2.strides());

    index_type i, j, k, l;
    IndicesType indices;
    for (i = 0; i < nArray1.size(0); i++) {
        for (j = 0; j < nArray1.size(1); j++) {
            for (k = 0; k < nArray1.size(2); k++) {
                for (l = 0; l < nArray1.size(3); l++) {
                    indices.Assign(i, j, k, l);
                    CPPUNIT_ASSERT(nArray1(indices)         == nArray1Ref1(indices));
                    CPPUNIT_ASSERT(nArray1.at(indices)      == nArray1Ref1.at(indices));
                    CPPUNIT_ASSERT(nArray1.Element(indices) == nArray1Ref1.Element(indices));
                    CPPUNIT_ASSERT(nArray1.Pointer(indices) == nArray1Ref1.Pointer(indices));
                    CPPUNIT_ASSERT(nArray1[i][j][k][l]      == nArray1Ref1[i][j][k][l]);

                    CPPUNIT_ASSERT(nArray1(indices)         == nArray1Ref2(indices));
                    CPPUNIT_ASSERT(nArray1.at(indices)      == nArray1Ref2.at(indices));
                    CPPUNIT_ASSERT(nArray1.Element(indices) == nArray1Ref2.Element(indices));
                    CPPUNIT_ASSERT(nArray1.Pointer(indices) == nArray1Ref2.Pointer(indices));
                    CPPUNIT_ASSERT(nArray1[i][j][k][l]      == nArray1Ref2[i][j][k][l]);

                    CPPUNIT_ASSERT(nArray1(indices)         == nArray1ConstRef1(indices));
                    CPPUNIT_ASSERT(nArray1.at(indices)      == nArray1ConstRef1.at(indices));
                    CPPUNIT_ASSERT(nArray1.Element(indices) == nArray1ConstRef1.Element(indices));
                    CPPUNIT_ASSERT(nArray1.Pointer(indices) == nArray1ConstRef1.Pointer(indices));
                    CPPUNIT_ASSERT(nArray1[i][j][k][l]      == nArray1ConstRef1[i][j][k][l]);

                    CPPUNIT_ASSERT(nArray1(indices)         == nArray1ConstRef2(indices));
                    CPPUNIT_ASSERT(nArray1.at(indices)      == nArray1ConstRef2.at(indices));
                    CPPUNIT_ASSERT(nArray1.Element(indices) == nArray1ConstRef2.Element(indices));
                    CPPUNIT_ASSERT(nArray1.Pointer(indices) == nArray1ConstRef2.Pointer(indices));
                }
            }
        }
    }
    
}

void vctDynamicNArrayRefTest::TestRefOnArrayDouble(void) {
    TestRefOnArray<double>();
}
void vctDynamicNArrayRefTest::TestRefOnArrayFloat(void) {
    TestRefOnArray<float>();
}
void vctDynamicNArrayRefTest::TestRefOnArrayInt(void) {
    TestRefOnArray<int>();
}




template <class _elementType>
void vctDynamicNArrayRefTest::TestSubarrayRef(void)
{
    typedef _elementType value_type;

    enum {DIMENSION = 3};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION> ArrayRefType;
    typedef vctDynamicConstNArrayRef<value_type, DIMENSION> ConstArrayRefType;

    typedef typename ArrayType::size_type size_type;
    typedef typename ArrayType::index_type index_type;
    typedef typename ArrayType::nsize_type SizesType;
    typedef typename ArrayType::nsize_type IndicesType;

    enum {ALL_MIN = 10, ALL_MAX = 12};
    enum {START_MIN = 2, START_MAX = 4};
    enum {LENGTH_MIN = 2, LENGTH_MAX = 4};
    SizesType sizes, starts, lengths;
    vctRandom(sizes, static_cast<size_type>(ALL_MIN), static_cast<size_type>(ALL_MAX));
    vctRandom(starts, static_cast<size_type>(START_MIN), static_cast<size_type>(START_MAX));
    vctRandom(lengths, static_cast<size_type>(LENGTH_MIN), static_cast<size_type>(LENGTH_MAX));

    ArrayType nArray1(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));

    ArrayRefType subNArray1;
    subNArray1.SubarrayOf(nArray1, starts, lengths);

    ConstArrayRefType subConstNArray1;
    subConstNArray1.SubarrayOf(nArray1, starts, lengths);
    
    index_type i, j, k;
    IndicesType indicesSub, indices;
    
    CPPUNIT_ASSERT(subNArray1.sizes() == lengths);
  
    for (i = 0; i < subNArray1.size(0); i++) {
        for (j = 0; j < subNArray1.size(1); j++) {
            for (k = 0; k < subNArray1.size(2); k++) {
                indicesSub.Assign(i, j, k);
                indices.SumOf(starts, indicesSub);
            
                CPPUNIT_ASSERT(nArray1(indices)         == subNArray1(indicesSub));
                CPPUNIT_ASSERT(nArray1.at(indices)      == subNArray1.at(indicesSub));
                CPPUNIT_ASSERT(nArray1.Element(indices) == subNArray1.Element(indicesSub));
                CPPUNIT_ASSERT(nArray1.Pointer(indices) == subNArray1.Pointer(indicesSub));
                CPPUNIT_ASSERT(nArray1[i + starts.Element(0)][j + starts.Element(1)][k + starts.Element(2)] == subNArray1[i][j][k]);

                CPPUNIT_ASSERT(nArray1(indices)         == subConstNArray1(indicesSub));
                CPPUNIT_ASSERT(nArray1.at(indices)      == subConstNArray1.at(indicesSub));
                CPPUNIT_ASSERT(nArray1.Element(indices) == subConstNArray1.Element(indicesSub));
                CPPUNIT_ASSERT(nArray1.Pointer(indices) == subConstNArray1.Pointer(indicesSub));
                CPPUNIT_ASSERT(nArray1[i + starts.Element(0)][j + starts.Element(1)][k + starts.Element(2)] == subConstNArray1[i][j][k]);
            }
        }
    }
}

void vctDynamicNArrayRefTest::TestSubarrayRefDouble(void) {
    TestSubarrayRef<double>();
}
void vctDynamicNArrayRefTest::TestSubarrayRefFloat(void) {
    TestSubarrayRef<float>();
}
void vctDynamicNArrayRefTest::TestSubarrayRefInt(void) {
    TestSubarrayRef<int>();
}



template <class _elementType>
void vctDynamicNArrayRefTest::TestPermutationRef(void)
{
    typedef _elementType value_type;

    enum {DIMENSION = 4};
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION> ArrayRefType;
    typedef vctDynamicConstNArrayRef<value_type, DIMENSION> ConstArrayRefType;

    typedef typename ArrayType::size_type size_type;
    typedef typename ArrayType::index_type index_type;
    typedef typename ArrayType::nsize_type SizesType;
    typedef typename ArrayType::ndimension_type DimensionsType;
    typedef typename ArrayType::nindex_type IndicesType;
    typedef typename ArrayType::index_type IndexType;

    SizesType sizes;
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    
    ArrayType nArray1(sizes);
    vctRandom(nArray1, static_cast<value_type>(-10), static_cast<value_type>(10));
    
    DimensionsType permutation;
    IndicesType swaps;
    
    // build a permutation vector
    index_type index;
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
    
    // apply the permutation for both const and non const narray
    ArrayRefType permutedNArray1;
    permutedNArray1.PermutationOf(nArray1, permutation);

    ConstArrayRefType permutedConstNArray1;
    permutedConstNArray1.PermutationOf(nArray1, permutation);

    // check that sizes are permuted
    for (index = 0; index < permutation.size(); index++) {
        CPPUNIT_ASSERT(permutedNArray1.size(index) == nArray1.size(permutation[index]));
        CPPUNIT_ASSERT(permutedConstNArray1.size(index) == nArray1.size(permutation[index]));
    }

    index_type i, j, k, l;
    IndicesType indicesPermuted, indices;
    
    for (i = 0; i < nArray1.size(0); i++) {
        for (j = 0; j < nArray1.size(1); j++) {
            for (k = 0; k < nArray1.size(2); k++) {
                for (l = 0; l < nArray1.size(3); l++) {
                    indices.Assign(i, j, k, l);
                    indicesPermuted.Assign(indices[permutation[0]],
                        indices[permutation[1]],
                        indices[permutation[2]],
                        indices[permutation[3]]);
                    CPPUNIT_ASSERT(nArray1(indices)         == permutedNArray1(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1.at(indices)      == permutedNArray1.at(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1.Element(indices) == permutedNArray1.Element(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1.Pointer(indices) == permutedNArray1.Pointer(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1[i][j][k][l] == permutedNArray1
                                           [indices[permutation[0]]]
                                           [indices[permutation[1]]]
                                           [indices[permutation[2]]]
                                           [indices[permutation[3]]]);
                    CPPUNIT_ASSERT(nArray1(indices)         == permutedConstNArray1(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1.at(indices)      == permutedConstNArray1.at(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1.Element(indices) == permutedConstNArray1.Element(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1.Pointer(indices) == permutedConstNArray1.Pointer(indicesPermuted));
                    CPPUNIT_ASSERT(nArray1[i][j][k][l] == permutedConstNArray1
                                           [indices[permutation[0]]]
                                           [indices[permutation[1]]]
                                           [indices[permutation[2]]]
                                           [indices[permutation[3]]]);
                }
            }
        }
    }
}

void vctDynamicNArrayRefTest::TestPermutationRefDouble(void) {
    TestPermutationRef<double>();
}
void vctDynamicNArrayRefTest::TestPermutationRefFloat(void) {
    TestPermutationRef<float>();
}
void vctDynamicNArrayRefTest::TestPermutationRefInt(void) {
    TestPermutationRef<int>();
}





// Function used to create a complex ref on a solid array.  It will
// first reslice, then permute and finally resize (windowing).  The
// sizes for windowing are provided so that many refs can be created
// with the same sizes.  The vector indices must contain all indices
// between 0 and _dimension - 1, it is used to compute a permutation.
template <vct::size_type _dimension>
void vctDynamicNArrayRefTestSetWorstArrayRef(vctDynamicNArray<double, _dimension> & baseArray,
                                             const vctFixedSizeVector<vct::size_type, _dimension - 1> & sizes,
                                             vctFixedSizeVector<vct::index_type, _dimension - 1> indices,
                                             vctDynamicNArrayRef<double, _dimension - 1> & array) {
    typedef vct::index_type index_type;

    // reslice
    vctDynamicNArrayRef<double, _dimension - 1> baseSliced;
    index_type sliceDirection;
    index_type sliceIndex;
    
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(static_cast<index_type>(0), _dimension, sliceDirection);
    randomSequence.ExtractRandomValue(static_cast<index_type>(0), baseArray.size(sliceDirection), sliceIndex);
    baseSliced.SliceOf(baseArray, sliceDirection, sliceIndex);

    // permute
    vctDynamicNArrayRef<double, _dimension - 1> basePermuted;
    vctFixedSizeVector<index_type, _dimension - 1> swaps;
    vctRandom(swaps, static_cast<index_type>(0), indices.size());
    index_type index, temp;
    for (index = 0; index < indices.size(); index++) {
        temp = indices[0];
        indices[0] = indices[swaps[index]];
        indices[swaps[index]] = temp;
    }
    basePermuted.PermutationOf(baseSliced, indices);

    // take a smaller window
    vctFixedSizeVector<index_type, _dimension - 1> starts(static_cast<index_type>(0));
    array.SubarrayOf(basePermuted, starts, sizes);

    // fill with random values
    vctRandom(array, static_cast<double>(-10), static_cast<double>(10));
}



void vctDynamicNArrayRefTest::TestEngines(void)
{
    /* define types for engine tests on narrays of doubles */
    enum {DIMENSION = 5};
    typedef double value_type;
    typedef vctDynamicNArray<value_type, DIMENSION> ArrayType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION - 1> ArrayRefType;

    typedef ArrayType::size_type size_type;
    typedef ArrayType::index_type index_type;
    typedef ArrayType::nsize_type larger_nsize_type;
    typedef ArrayType::nindex_type larger_nindex_type;

    typedef ArrayRefType::value_type value_type;
    typedef ArrayRefType::nsize_type nsize_type;
    typedef ArrayRefType::nindex_type nindex_type;
    typedef ArrayRefType::const_iterator const_iterator;
    typedef ArrayRefType::const_reverse_iterator const_reverse_iterator;

    ArrayType baseArray1, baseArray2, baseArray3;
    ArrayRefType array1, array2, array3;

    // build a permutation vector
    nindex_type allIndices;
    index_type index;
    nsize_type sizes;

    // allocate memory for all 3 containers
    baseArray1.SetSize(larger_nsize_type(MAX_SIZE));
    baseArray2.SetSize(larger_nsize_type(MAX_SIZE));
    baseArray3.SetSize(larger_nsize_type(MAX_SIZE));

    // create a list of indices for all permutations
    for (index = 0; index < allIndices.size(); index++) {
        allIndices.Element(index) = index;
    }

    // first test that the set worst array ref doesn't break everything
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    for (index = 0; index < array1.dimension(); index++) {
        CPPUNIT_ASSERT(array1.size(index) <= MAX_SIZE);
    }

    // test address of elements accessed via iterators
    const value_type * first = baseArray1.Pointer();
    const value_type * last = baseArray1.Pointer(larger_nindex_type(MAX_SIZE));
    const const_iterator begin = array1.begin();
    const const_iterator end = array1.end();
    const_iterator iter;
    for (iter = begin; iter != end; ++iter) {
        CPPUNIT_ASSERT(&(*iter) <= last);
        CPPUNIT_ASSERT(&(*iter) >= first);
        CPPUNIT_ASSERT(*iter >= static_cast<double>(-10));
        CPPUNIT_ASSERT(*iter <= static_cast<double>(10));
    }
    const const_reverse_iterator rbegin = array1.rbegin();
    const const_reverse_iterator rend = array1.rend();
    const_reverse_iterator riter;
    for (riter = rbegin; riter != rend; ++riter) {
        CPPUNIT_ASSERT(&(*riter) <= last);
        CPPUNIT_ASSERT(&(*riter) >= first);
    }
    
    // test address of elements accessed directly
    index_type i, j, k, l;
    value_type * currentPointer;
    nindex_type currentIndices;
    for (i = 0; i < array1.size(0); i++) {
        for (j = 0; j < array1.size(1); j++) {
            for (k = 0; k < array1.size(2); k++) {
                for (l = 0; l < array1.size(3); l++) {
                    currentPointer = &(array1[i][j][k][l]);
                    CPPUNIT_ASSERT(currentPointer <= last);
                    CPPUNIT_ASSERT(currentPointer >= first);
                    currentIndices.Assign(i, j, k, l);
                    CPPUNIT_ASSERT(array1.Pointer(currentIndices) <= last);
                    CPPUNIT_ASSERT(array1.Pointer(currentIndices) >= first);
                }
            }
        }
    }

    // scalar and random sequence used for all tests
    double scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();

    // tests per engine, new size and new content everytime
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctGenericNArrayTest::TestAssignment(array1, array2);

    // test SoNi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctGenericContainerTest::TestSoCiOperations(array1, array2);
    
    // test SoNiNi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray3, sizes, allIndices, array3);
    vctGenericContainerTest::TestSoCiCiOperations(array1, array2, array3);

    // test NioNi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray3, sizes, allIndices, array3);
    RemoveQuasiZero(array2);
    vctGenericContainerTest::TestCioCiOperations(array1, array2, array3);

    // test NioSiNi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray3, sizes, allIndices, array3);
    RemoveQuasiZero(array2);
    randomSequence.ExtractRandomValue(-5.0, 5.0, scalar);
    if ((scalar < 1.0)
        && (scalar > -1.0)) {
        scalar = 3.0;
    }
    vctGenericContainerTest::TestCioSiCiOperations(array1, scalar, array2, array3);

    // test NoNiNi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray3, sizes, allIndices, array3);
    RemoveQuasiZero(array2);
    vctGenericContainerTest::TestCoCiCiOperations(array1, array2, array3);

    // test NoNi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctGenericContainerTest::TestCoCiOperations(array1, array2);

    // test NoNiSi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    randomSequence.ExtractRandomValue(-5.0, 5.0, scalar);
    if ((scalar < 1.0)
        && (scalar > -1.0)) {
        scalar = 3.0;
    }
    vctGenericContainerTest::TestCoCiSiOperations(array1, scalar, array2);

    // test NoSiNi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    RemoveQuasiZero(array1);
    randomSequence.ExtractRandomValue(-5.0, 5.0, scalar);
    vctGenericContainerTest::TestCoSiCiOperations(scalar, array1, array2);

    // test NioSi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    randomSequence.ExtractRandomValue(-5.0, 5.0, scalar);
    if ((scalar < 1.0)
        && (scalar > -1.0)) {
        scalar = 3.0;
    }
    vctGenericContainerTest::TestCioSiOperations(array1, scalar, array2);

    // test Nio
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctGenericContainerTest::TestCioOperations(array1, array2);

    // test SoNiSi
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    randomSequence.ExtractRandomValue(-5.0, 5.0, scalar);
    vctGenericContainerTest::TestSoCiSiOperations(array1, scalar, array2);

    // test MinAndMax
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctGenericContainerTest::TestMinAndMax(array1);

    // test iterators
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctGenericContainerTest::TestIterators(array1);

    // test STL functions
    vctRandom(sizes, static_cast<size_type>(MIN_SIZE), static_cast<size_type>(MAX_SIZE));
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray1, sizes, allIndices, array1);
    vctDynamicNArrayRefTestSetWorstArrayRef<DIMENSION>(baseArray2, sizes, allIndices, array2);
    vctGenericContainerTest::TestSTLFunctions(array1, array2);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDynamicNArrayRefTest);

