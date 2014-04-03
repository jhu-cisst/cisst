/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2003-12-16

  (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctFixedSizeMatrixTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericMatrixTest.h"


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFixedSizeConstMatrixRef.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>


template <class _elementType>
void vctFixedSizeMatrixTest::TestAssignment(void) {
    enum {ROWS = 7, COLS = 2};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix1;
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix2;
    vctGenericMatrixTest::TestAssignment(matrix1, matrix2);
}

void vctFixedSizeMatrixTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctFixedSizeMatrixTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctFixedSizeMatrixTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestZeros(void) {
    enum {ROWS = 10, COLS = 6}; // even numbers
    typedef _elementType value_type;
    /* test on compact matrices for both storage orders */
    typedef vctFixedSizeMatrix<value_type, ROWS, COLS, VCT_ROW_MAJOR> MatrixType1;
    typedef vctFixedSizeMatrix<value_type, ROWS, COLS, VCT_COL_MAJOR> MatrixType2;
    MatrixType1 matrix1;
    vctGenericMatrixTest::TestZeros(matrix1);
    MatrixType2 matrix2;
    vctGenericMatrixTest::TestZeros(matrix2);
    /* test on matrices compact by rows or columns. */
    typedef vctFixedSizeMatrixRef<value_type, 4, 4, MatrixType1::ROWSTRIDE, MatrixType1::COLSTRIDE> SubMatrixType1;
    typedef vctFixedSizeMatrixRef<value_type, 4, 4, MatrixType2::ROWSTRIDE, MatrixType2::COLSTRIDE> SubMatrixType2;
    SubMatrixType1 subMatrix1(matrix1);
    vctGenericMatrixTest::TestZeros(subMatrix1);
    SubMatrixType2 subMatrix2(matrix2);
    vctGenericMatrixTest::TestZeros(subMatrix2);
    /* test on last case, i.e. neither row nor column compact */
    typedef vctFixedSizeMatrixRef<value_type, ROWS / 2, COLS / 2, MatrixType1::ROWSTRIDE * 2, MatrixType1::COLSTRIDE * 2> SubsampledMatrixType1;
    typedef vctFixedSizeMatrixRef<value_type, ROWS / 2, COLS / 2, MatrixType2::ROWSTRIDE * 2, MatrixType2::COLSTRIDE * 2> SubsampledMatrixType2;
    SubsampledMatrixType1 subsampledMatrix1(matrix1.Pointer());
    vctGenericMatrixTest::TestZeros(subsampledMatrix1);
    SubsampledMatrixType2 subsampledMatrix2(matrix2.Pointer());
    vctGenericMatrixTest::TestZeros(subsampledMatrix2);
}

void vctFixedSizeMatrixTest::TestZerosDouble(void) {
    TestZeros<double>();
}
void vctFixedSizeMatrixTest::TestZerosFloat(void) {
    TestZeros<float>();
}
void vctFixedSizeMatrixTest::TestZerosInt(void) {
    TestZeros<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestAccessMethods(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> inputMatrix;
    vctRandom(inputMatrix, value_type(-10), value_type(10));
    typename vctFixedSizeMatrix<value_type, ROWS, COLS>::ConstRefType constInputMatrix(inputMatrix.Pointer());
    vctGenericMatrixTest::TestAccessMethods(inputMatrix, constInputMatrix);
}

void vctFixedSizeMatrixTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctFixedSizeMatrixTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctFixedSizeMatrixTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}

template<class _elementType>
void vctFixedSizeMatrixTest::TestExchangeAndPermutation(void)
{
    enum {ROWS = 4, COLS = 5};
    typedef _elementType value_type;
    typedef vctFixedSizeMatrix<_elementType, ROWS, COLS> MatrixType;
    MatrixType matrix;
    vctRandom(matrix, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    const unsigned int row1 = randomSequence.ExtractRandomInt(0, ROWS);
    const unsigned int row2 = randomSequence.ExtractRandomInt(0, ROWS);
    const unsigned int col1 = randomSequence.ExtractRandomInt(0, COLS);
    const unsigned int col2 = randomSequence.ExtractRandomInt(0, COLS);
    vct::index_type rowPermutation[ROWS];
    randomSequence.ExtractRandomPermutation(ROWS, rowPermutation);
    vct::index_type colPermutation[COLS];
    randomSequence.ExtractRandomPermutation(COLS, colPermutation);
    vctGenericMatrixTest::TestExchangeAndPermutationOperations(matrix, row1, row2,
                                                               col1, col2, rowPermutation, colPermutation);
}

void vctFixedSizeMatrixTest::TestExchangeAndPermutationDouble(void)
{
    TestExchangeAndPermutation<double>();
}


void vctFixedSizeMatrixTest::TestExchangeAndPermutationFloat(void)
{
    TestExchangeAndPermutation<float>();
}


void vctFixedSizeMatrixTest::TestExchangeAndPermutationInt(void)
{
    TestExchangeAndPermutation<int>();
}


template<class _elementType>
void vctFixedSizeMatrixTest::TestSelect(void)
{
    enum {INPUT_ROWS = 7, INPUT_COLS = 8};
    enum {OUTPUT_ROWS = 4, OUTPUT_COLS = 4};
    typedef _elementType value_type;
    vctFixedSizeMatrix<_elementType, INPUT_ROWS, INPUT_COLS> inputMatrix;
    vctFixedSizeVector<vct::index_type, OUTPUT_ROWS> rowIndexes;
    vctFixedSizeVector<vct::index_type, OUTPUT_COLS> colIndexes;
    vctFixedSizeMatrix<_elementType, OUTPUT_ROWS, INPUT_COLS> selectedRows;
    vctFixedSizeMatrix<_elementType, INPUT_ROWS, OUTPUT_COLS> selectedCols;

    vctRandom(inputMatrix, value_type(-10), value_type(10));
    vctRandom(rowIndexes, static_cast<vct::index_type>(0), static_cast<vct::index_type>(INPUT_ROWS));
    vctRandom(colIndexes, static_cast<vct::index_type>(0), static_cast<vct::index_type>(INPUT_COLS));
    vctGenericMatrixTest::TestSelect(inputMatrix, rowIndexes, colIndexes, selectedRows, selectedCols);
}

void vctFixedSizeMatrixTest::TestSelectDouble(void)
{
    vctFixedSizeMatrixTest::TestSelect<double>();
}

void vctFixedSizeMatrixTest::TestSelectFloat(void)
{
    vctFixedSizeMatrixTest::TestSelect<float>();
}

void vctFixedSizeMatrixTest::TestSelectInt(void)
{
    vctFixedSizeMatrixTest::TestSelect<int>();
}


template <class _elementType>
void vctFixedSizeMatrixTest::TestSoMiOperations(void) {
    enum {ROWS = 2, COLS = 4};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(matrix, result);
}

void vctFixedSizeMatrixTest::TestSoMiOperationsDouble(void) {
    TestSoMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestSoMiOperationsFloat(void) {
    TestSoMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestSoMiOperationsInt(void) {
    TestSoMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestSoMiMiOperations(void) {
    enum {ROWS = 5, COLS = 9};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix1;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix2;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix3;
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(matrix1, matrix2, matrix3);
}

void vctFixedSizeMatrixTest::TestSoMiMiOperationsDouble(void) {
    TestSoMiMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestSoMiMiOperationsFloat(void) {
    TestSoMiMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestSoMiMiOperationsInt(void) {
    TestSoMiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMioMiOperations(void) {
    enum {ROWS = 8, COLS = 4};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix1;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix2;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;

    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);

    vctGenericContainerTest::TestCioCiOperations(matrix1, matrix2, result);
}

void vctFixedSizeMatrixTest::TestMioMiOperationsDouble(void) {
    TestMioMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMioMiOperationsFloat(void) {
    TestMioMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMioMiOperationsInt(void) {
    TestMioMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMioSiMiOperations(void) {
    enum {ROWS = 5, COLS = 6};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix1;
    value_type scalar;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix2;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;

    vctRandom(matrix1, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-10),
                                      value_type(10),
                                      scalar);
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);

    vctGenericContainerTest::TestCioSiCiOperations(matrix1, scalar, matrix2, result);
}

void vctFixedSizeMatrixTest::TestMioSiMiOperationsDouble(void) {
    TestMioSiMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMioSiMiOperationsFloat(void) {
    TestMioSiMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMioSiMiOperationsInt(void) {
    TestMioSiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMioMiMiOperations(void) {
    enum {ROWS = 5, COLS = 6};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix1;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix2;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix3;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;

    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctRandom(matrix3, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix3);

    vctGenericContainerTest::TestCioCiCiOperations(matrix1, matrix2, matrix3, result);
}

void vctFixedSizeMatrixTest::TestMioMiMiOperationsDouble(void) {
    TestMioMiMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMioMiMiOperationsFloat(void) {
    TestMioMiMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMioMiMiOperationsInt(void) {
    TestMioMiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMoMiMiOperations(void) {
    enum {ROWS = 4, COLS = 5};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix1;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix2;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;

    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);

    vctGenericContainerTest::TestCoCiCiOperations(matrix1, matrix2, result);
}

void vctFixedSizeMatrixTest::TestMoMiMiOperationsDouble(void) {
    TestMoMiMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMoMiMiOperationsFloat(void) {
    TestMoMiMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMoMiMiOperationsInt(void) {
    TestMoMiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestProductOperations(void) {
    // result size
    enum {ROWS = 4, COLS = 5, COMSIZE = 7};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COMSIZE> matrix1;
    vctFixedSizeMatrix<value_type, COMSIZE, COLS> matrix2;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix3;
    vctFixedSizeVector<value_type, COMSIZE> vector1;
    vctFixedSizeVector<value_type, ROWS> vector2;

    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctRandom(vector1, value_type(-10), value_type(10));

    vctGenericMatrixTest::TestMatrixMatrixProductOperations(matrix1, matrix2, matrix3);
    vctGenericMatrixTest::TestMatrixVectorProductOperations(matrix1, vector1, vector2);

    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericMatrixTest::TestVectorMatrixProductOperations(matrix1, vector2, vector1);

    vctFixedSizeMatrix<value_type, COMSIZE, COMSIZE> matrix4;
    vctFixedSizeMatrix<value_type, COMSIZE, COMSIZE> matrix5;
    vctGenericMatrixTest::TestMatrixMatrixProductExceptions(matrix4, matrix5);
}

void vctFixedSizeMatrixTest::TestProductOperationsDouble(void) {
    TestProductOperations<double>();
}
void vctFixedSizeMatrixTest::TestProductOperationsFloat(void) {
    TestProductOperations<float>();
}
void vctFixedSizeMatrixTest::TestProductOperationsInt(void) {
    TestProductOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMoMiOperations(void) {
    enum {ROWS = 2, COLS = 10};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(matrix, result);
}

void vctFixedSizeMatrixTest::TestMoMiOperationsDouble(void) {
    TestMoMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMoMiOperationsFloat(void) {
    TestMoMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMoMiOperationsInt(void) {
    TestMoMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMoMiSiOperations(void) {
    enum {ROWS = 8, COLS = 8};
    typedef _elementType value_type;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiSiOperations(matrix, scalar, result);
}

void vctFixedSizeMatrixTest::TestMoMiSiOperationsDouble(void) {
    TestMoMiSiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMoMiSiOperationsFloat(void) {
    TestMoMiSiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMoMiSiOperationsInt(void) {
    TestMoMiSiOperations<int>();
}




template <class _elementType>
void vctFixedSizeMatrixTest::TestMoSiMiOperations(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    value_type scalar;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                       scalar);
    vctRandom(matrix, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix);

    vctGenericContainerTest::TestCoSiCiOperations(scalar, matrix, result);
}

void vctFixedSizeMatrixTest::TestMoSiMiOperationsDouble(void) {
    TestMoSiMiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMoSiMiOperationsFloat(void) {
    TestMoSiMiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMoSiMiOperationsInt(void) {
    TestMoSiMiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMioSiOperations(void) {
    enum {ROWS = 6, COLS = 3};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioSiOperations(matrix, scalar, result);
}

void vctFixedSizeMatrixTest::TestMioSiOperationsDouble(void) {
    TestMioSiOperations<double>();
}
void vctFixedSizeMatrixTest::TestMioSiOperationsFloat(void) {
    TestMioSiOperations<float>();
}
void vctFixedSizeMatrixTest::TestMioSiOperationsInt(void) {
    TestMioSiOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestMioOperations(void) {
    enum {ROWS = 2, COLS = 3};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(matrix, result);
}

void vctFixedSizeMatrixTest::TestMioOperationsDouble(void) {
    TestMioOperations<double>();
}
void vctFixedSizeMatrixTest::TestMioOperationsFloat(void) {
    TestMioOperations<float>();
}
void vctFixedSizeMatrixTest::TestMioOperationsInt(void) {
    TestMioOperations<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestSoMiSiOperations(void) {
    enum {ROWS = 4, COLS = 4};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiSiOperations(matrix, scalar, result);
}

void vctFixedSizeMatrixTest::TestSoMiSiOperationsDouble(void) {
    TestSoMiSiOperations<double>();
}
void vctFixedSizeMatrixTest::TestSoMiSiOperationsFloat(void) {
    TestSoMiSiOperations<float>();
}
void vctFixedSizeMatrixTest::TestSoMiSiOperationsInt(void) {
    TestSoMiSiOperations<int>();
}



template<class _elementType>
void vctFixedSizeMatrixTest::TestMinAndMax(void) {
    enum {ROWS = 5, COLS = 4};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctRandom(matrix, value_type(-50), value_type(50));
    vctGenericContainerTest::TestMinAndMax(matrix);
}

void vctFixedSizeMatrixTest::TestMinAndMaxDouble(void) {
    TestMinAndMax<double>();
}

void vctFixedSizeMatrixTest::TestMinAndMaxFloat(void) {
    TestMinAndMax<float>();
}

void vctFixedSizeMatrixTest::TestMinAndMaxInt(void) {
    TestMinAndMax<int>();
}


template <class _elementType>
void vctFixedSizeMatrixTest::TestIterators(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(matrix);
}

void vctFixedSizeMatrixTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctFixedSizeMatrixTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctFixedSizeMatrixTest::TestIteratorsInt(void) {
    TestIterators<int>();
}



template <class _elementType>
void vctFixedSizeMatrixTest::TestSTLFunctions(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    vctFixedSizeMatrix<value_type, ROWS, COLS> matrix;
    vctFixedSizeMatrix<value_type, ROWS, COLS> result;
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(matrix, result);
}

void vctFixedSizeMatrixTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctFixedSizeMatrixTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctFixedSizeMatrixTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}



template <class _elementType, bool _storageOrder>
void vctFixedSizeMatrixTest::TestFastCopyOf(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;

    // dynamic matrix
    vctFixedSizeMatrix<value_type, ROWS, COLS, _storageOrder> destination;
    vctDynamicMatrix<value_type> validSource(ROWS, COLS, _storageOrder);
    vctRandom(validSource, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSource, destination);

    // test for different storage order
    vctDynamicMatrix<value_type> invalidSource(ROWS, COLS, !_storageOrder);
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidSource));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidSource));

    // test for exception if different size
    vctDynamicMatrix<value_type> largerMatrix(ROWS + 1, COLS + 1, _storageOrder);
    vctRandom(largerMatrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOfException(largerMatrix, destination);

    // test for compact by row or col
    vctDynamicMatrixRef<value_type> compactSub(largerMatrix, 0, 0, ROWS, COLS);
    vctGenericContainerTest::TestFastCopyOf(compactSub, destination);

    // test for incompatible strides, since we will not make any memory access these are totally wrong
    vctDynamicMatrixRef<value_type> invalidRef(ROWS, COLS, 100, 100, largerMatrix.Pointer());
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidRef));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidRef));

    // safe tests using a larger source and not performing checks
    CPPUNIT_ASSERT(largerMatrix.FastCopyOf(destination, vctFastCopy::SkipChecks));
    CPPUNIT_ASSERT(destination.FastCopyOf(invalidSource, vctFastCopy::SkipChecks));

    // fixed size matrix
    vctFixedSizeMatrix<value_type, ROWS, COLS, _storageOrder> validSourceFixed;
    vctRandom(validSourceFixed, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSourceFixed, destination);
}

void vctFixedSizeMatrixTest::TestFastCopyOfDouble(void) {
    TestFastCopyOf<double, VCT_ROW_MAJOR>();
    TestFastCopyOf<double, VCT_COL_MAJOR>();
}
void vctFixedSizeMatrixTest::TestFastCopyOfFloat(void) {
    TestFastCopyOf<float, VCT_ROW_MAJOR>();
    TestFastCopyOf<float, VCT_COL_MAJOR>();
}
void vctFixedSizeMatrixTest::TestFastCopyOfInt(void) {
    TestFastCopyOf<int, VCT_ROW_MAJOR>();
    TestFastCopyOf<int, VCT_COL_MAJOR>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctFixedSizeMatrixTest);

