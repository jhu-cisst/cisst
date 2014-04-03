/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-07-09
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctDynamicMatrixTest.h"
#include "vctGenericContainerTest.h"
#include "vctGenericMatrixTest.h"


#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDynamicConstMatrixRef.h>
#include <cisstVector/vctRandomDynamicVector.h>
#include <cisstVector/vctRandomDynamicMatrix.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>


template <class _elementType>
void vctDynamicMatrixTest::TestAssignment(void) {
    enum {ROWS = 7, COLS = 2};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COLS);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctDynamicMatrix<value_type> matrix2(ROWS, COLS);
    vctGenericMatrixTest::TestAssignment(matrix1, matrix2);
}

void vctDynamicMatrixTest::TestAssignmentDouble(void) {
    TestAssignment<double>();
}
void vctDynamicMatrixTest::TestAssignmentFloat(void) {
    TestAssignment<float>();
}
void vctDynamicMatrixTest::TestAssignmentInt(void) {
    TestAssignment<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestZeros(void) {
    enum {ROWS = 10, COLS = 6}; // even numbers!
    typedef _elementType value_type;
    /* test on compact matrices for both storage orders */
    vctDynamicMatrix<value_type> matrix1(ROWS, COLS, VCT_ROW_MAJOR);
    vctGenericMatrixTest::TestZeros(matrix1);
    vctDynamicMatrix<value_type> matrix2(ROWS, COLS, VCT_COL_MAJOR);
    vctGenericMatrixTest::TestZeros(matrix2);
    /* test on matrices compact by rows or columns. */
    vctDynamicMatrixRef<value_type> matrixRef;
    matrixRef.SetRef(matrix1, 0, 0, 4, 4);
    vctGenericMatrixTest::TestZeros(matrixRef);
    matrixRef.SetRef(matrix2, 0, 0, 4, 4);
    vctGenericMatrixTest::TestZeros(matrixRef);
    /* test on last case, i.e. neither row nor column compact */
    matrixRef.SetRef(matrix1.rows() / 2, matrix1.cols() / 2,
                     matrix1.row_stride() * 2, matrix1.col_stride() * 2,
                     matrix1.Pointer());
    vctGenericMatrixTest::TestZeros(matrixRef);
    matrixRef.SetRef(matrix2.rows() / 2, matrix2.cols() / 2,
                     matrix2.row_stride() * 2, matrix2.col_stride() * 2,
                     matrix2.Pointer());
    vctGenericMatrixTest::TestZeros(matrixRef);
}

void vctDynamicMatrixTest::TestZerosDouble(void) {
    TestZeros<double>();
}
void vctDynamicMatrixTest::TestZerosFloat(void) {
    TestZeros<float>();
}
void vctDynamicMatrixTest::TestZerosInt(void) {
    TestZeros<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestSizingMethods(bool storageOrder) {
    typedef _elementType value_type;
    const unsigned int rows = cmnRandomSequence::GetInstance().ExtractRandomInt(2, 13);
    const unsigned int cols = cmnRandomSequence::GetInstance().ExtractRandomInt(2, 13);
    const value_type minValue = static_cast<_elementType>(0);
    const value_type maxValue = static_cast<_elementType>(30);
    // default constructor, size 0, null pointer
    vctDynamicMatrix<value_type> matrix1;
    CPPUNIT_ASSERT(matrix1.size() == 0);
    CPPUNIT_ASSERT(matrix1.rows() == 0);
    CPPUNIT_ASSERT(matrix1.cols() == 0);
    CPPUNIT_ASSERT(matrix1.Pointer() == 0);
    // size should be set and pointer should != 0
    matrix1.SetSize(rows, cols, storageOrder);
    _elementType * pointer1 = matrix1.Pointer();
    CPPUNIT_ASSERT(matrix1.size() == rows * cols);
    CPPUNIT_ASSERT(matrix1.rows() == rows);
    CPPUNIT_ASSERT(matrix1.cols() == cols);
    CPPUNIT_ASSERT(pointer1 != 0);
    // same size, pointer shouldn't change
    matrix1.SetSize(rows, cols, storageOrder);
    CPPUNIT_ASSERT(matrix1.size() == rows * cols);
    CPPUNIT_ASSERT(matrix1.rows() == rows);
    CPPUNIT_ASSERT(matrix1.cols() == cols);
    CPPUNIT_ASSERT(matrix1.Pointer() == pointer1);
    // same size, pointer shouldn't change
    matrix1.resize(rows, cols);
    CPPUNIT_ASSERT(matrix1.size() == rows * cols);
    CPPUNIT_ASSERT(matrix1.rows() == rows);
    CPPUNIT_ASSERT(matrix1.cols() == cols);
    CPPUNIT_ASSERT(matrix1.Pointer() == pointer1);

    // different size
    matrix1.SetSize(rows * 2, cols * 2, storageOrder);
    CPPUNIT_ASSERT(matrix1.size() == rows * cols * 4);
    // different size with resize
    matrix1.resize(rows, cols);
    CPPUNIT_ASSERT(matrix1.size() == rows * cols);

    // try with other constructors
    vctDynamicMatrix<_elementType> matrix2(rows, cols, storageOrder);
    CPPUNIT_ASSERT(matrix2.size() == rows * cols);
    CPPUNIT_ASSERT(matrix2.rows() == rows);
    CPPUNIT_ASSERT(matrix2.cols() == cols);
    CPPUNIT_ASSERT(matrix2.Pointer() != 0);

    vctDynamicMatrix<_elementType> matrix3(matrix1);
    CPPUNIT_ASSERT(matrix3.size() == rows * cols);
    CPPUNIT_ASSERT(matrix3.rows() == rows);
    CPPUNIT_ASSERT(matrix3.cols() == cols);
    CPPUNIT_ASSERT(matrix3.Pointer() != 0);    
    CPPUNIT_ASSERT(matrix3.Pointer() != matrix1.Pointer());

    // test that the resize method preserves the data in the matrix object
    matrix1.SetSize(rows, cols, storageOrder);
    vctRandom(matrix1, minValue, maxValue);
    matrix3.SetSize(rows, cols, storageOrder);
    matrix3.Assign(matrix1);
    CPPUNIT_ASSERT(matrix3.Equal(matrix1));
    matrix1.resize(rows * 2, cols * 2);
    vctDynamicConstMatrixRef<_elementType> matrix1OldSizeOverlay;
    matrix1OldSizeOverlay.SetRef(matrix1, 0, 0, rows, cols);
    CPPUNIT_ASSERT(matrix3.Equal(matrix1OldSizeOverlay));
    matrix1.resize(rows, cols);
    CPPUNIT_ASSERT(matrix3.Equal(matrix1));
    vctDynamicConstMatrixRef<_elementType> submatrix3Overlay;
    submatrix3Overlay.SetRef(matrix3, 0, 0, rows-1, cols-1);
    matrix1.resize(rows-1, cols-1);
    CPPUNIT_ASSERT(submatrix3Overlay.Equal(matrix1));
    matrix1.resize(rows, cols);
    matrix1OldSizeOverlay.SetRef(matrix1, 0, 0, rows-1, cols-1);
    CPPUNIT_ASSERT(matrix1OldSizeOverlay.Equal(submatrix3Overlay));
    matrix1.Assign(matrix3);
    matrix1.resize(rows+2, cols-2);
    matrix1OldSizeOverlay.SetRef(matrix1, 0, 0, rows, cols-2);
    submatrix3Overlay.SetRef(matrix3, 0, 0, rows, cols-2);
    CPPUNIT_ASSERT(submatrix3Overlay.Equal(matrix1OldSizeOverlay));
    matrix1.resize(rows-2, cols-2);
    matrix1OldSizeOverlay.SetRef(matrix1, 0, 0, rows-2, cols-2);
    submatrix3Overlay.SetRef(matrix3, 0, 0, rows-2, cols-2);
    CPPUNIT_ASSERT(submatrix3Overlay.Equal(matrix1OldSizeOverlay));

    // release memory
    matrix1.SetSize(0, 0, storageOrder);
    CPPUNIT_ASSERT(matrix1.size() == 0);
    CPPUNIT_ASSERT(matrix1.rows() == 0);
    CPPUNIT_ASSERT(matrix1.cols() == 0);
    CPPUNIT_ASSERT(matrix1.Pointer() == 0);
    matrix2.resize(0, 0);
    CPPUNIT_ASSERT(matrix2.size() == 0);
    CPPUNIT_ASSERT(matrix2.rows() == 0);
    CPPUNIT_ASSERT(matrix2.cols() == 0);
    CPPUNIT_ASSERT(matrix2.Pointer() == 0);

    // call again to make sure 
    matrix1.SetSize(0, 0, storageOrder);
    CPPUNIT_ASSERT(matrix1.size() == 0);
    CPPUNIT_ASSERT(matrix1.rows() == 0);
    CPPUNIT_ASSERT(matrix1.cols() == 0);
    CPPUNIT_ASSERT(matrix1.Pointer() == 0);
    matrix2.resize(0, 0);
    CPPUNIT_ASSERT(matrix2.size() == 0);
    CPPUNIT_ASSERT(matrix2.rows() == 0);
    CPPUNIT_ASSERT(matrix2.cols() == 0);
    CPPUNIT_ASSERT(matrix2.Pointer() == 0);
}

void vctDynamicMatrixTest::TestSizingMethodsDouble(void) {
    TestSizingMethods<double>(VCT_COL_MAJOR);
    TestSizingMethods<double>(VCT_ROW_MAJOR);
}
void vctDynamicMatrixTest::TestSizingMethodsFloat(void) {
    TestSizingMethods<float>(VCT_COL_MAJOR);
    TestSizingMethods<float>(VCT_ROW_MAJOR);
}
void vctDynamicMatrixTest::TestSizingMethodsInt(void) {
    TestSizingMethods<int>(VCT_COL_MAJOR);
    TestSizingMethods<int>(VCT_ROW_MAJOR);
}



template <class _elementType>
void vctDynamicMatrixTest::TestAccessMethods(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> inputMatrix(ROWS, COLS);
    vctRandom(inputMatrix, value_type(-10), value_type(10));
    vctDynamicConstMatrixRef<value_type> constInputMatrix(ROWS, COLS,
                                                          inputMatrix.row_stride(), inputMatrix.col_stride(),
                                                          inputMatrix.Pointer());
    vctGenericMatrixTest::TestAccessMethods(inputMatrix, constInputMatrix);
}

void vctDynamicMatrixTest::TestAccessMethodsDouble(void) {
    TestAccessMethods<double>();
}
void vctDynamicMatrixTest::TestAccessMethodsFloat(void) {
    TestAccessMethods<float>();
}
void vctDynamicMatrixTest::TestAccessMethodsInt(void) {
    TestAccessMethods<int>();
}



template<class _elementType>
void vctDynamicMatrixTest::TestExchangeAndPermutation(void)
{
    enum {ROWS = 8, COLS = 10};
    typedef _elementType value_type;
    typedef vctDynamicMatrix<_elementType> MatrixType;
    typedef typename MatrixType::size_type size_type;
    typedef typename MatrixType::index_type index_type;
    MatrixType matrix(ROWS, COLS);
    vctRandom(matrix, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    const size_type row1 = randomSequence.ExtractRandomInt(0, ROWS);
    const size_type row2 = randomSequence.ExtractRandomInt(0, ROWS);
    const size_type col1 = randomSequence.ExtractRandomInt(0, COLS);
    const size_type col2 = randomSequence.ExtractRandomInt(0, COLS);
    index_type rowPermutation[ROWS];
    randomSequence.ExtractRandomPermutation(ROWS, rowPermutation);
    index_type colPermutation[COLS];
    randomSequence.ExtractRandomPermutation(COLS, colPermutation);
    vctGenericMatrixTest::TestExchangeAndPermutationOperations(matrix, row1, row2, 
                                                               col1, col2, rowPermutation, colPermutation);
}


void vctDynamicMatrixTest::TestExchangeAndPermutationDouble(void)
{
    TestExchangeAndPermutation<double>();
}


void vctDynamicMatrixTest::TestExchangeAndPermutationFloat(void)
{
    TestExchangeAndPermutation<float>();
}


void vctDynamicMatrixTest::TestExchangeAndPermutationInt(void)
{
    TestExchangeAndPermutation<int>();
}


template<class _elementType>
void vctDynamicMatrixTest::TestSelect(void)
{
    enum {INPUT_ROWS = 30, INPUT_COLS = 40};
    enum {OUTPUT_ROWS = 12, OUTPUT_COLS = 24};
    typedef _elementType value_type;
    vctDynamicMatrix<_elementType> inputMatrix(INPUT_ROWS, INPUT_COLS);
    vctDynamicVector<vct::index_type> rowIndexes(OUTPUT_ROWS);
    vctDynamicVector<vct::index_type> colIndexes(OUTPUT_COLS);
    vctDynamicMatrix<_elementType> selectedRows(OUTPUT_ROWS, INPUT_COLS);
    vctDynamicMatrix<_elementType> selectedCols(INPUT_ROWS, OUTPUT_COLS);

    vctRandom(inputMatrix, value_type(-10), value_type(10));
    vctRandom(rowIndexes, static_cast<vct::index_type>(0), static_cast<vct::index_type>(INPUT_ROWS));
    vctRandom(colIndexes, static_cast<vct::index_type>(0), static_cast<vct::index_type>(INPUT_COLS));
    vctGenericMatrixTest::TestSelect(inputMatrix, rowIndexes, colIndexes, selectedRows, selectedCols);
}

void vctDynamicMatrixTest::TestSelectDouble(void)
{
    vctDynamicMatrixTest::TestSelect<double>();
}

void vctDynamicMatrixTest::TestSelectFloat(void)
{
    vctDynamicMatrixTest::TestSelect<float>();
}

void vctDynamicMatrixTest::TestSelectInt(void)
{
    vctDynamicMatrixTest::TestSelect<int>();
}


template <class _elementType>
void vctDynamicMatrixTest::TestSoMiOperations(void) {
    enum {ROWS = 2, COLS = 4};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiOperations(matrix, result);
}

void vctDynamicMatrixTest::TestSoMiOperationsDouble(void) {
    TestSoMiOperations<double>();
}
void vctDynamicMatrixTest::TestSoMiOperationsFloat(void) {
    TestSoMiOperations<float>();
}
void vctDynamicMatrixTest::TestSoMiOperationsInt(void) {
    TestSoMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestSoMiMiOperations(void) {
    enum {ROWS = 5, COLS = 9};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COLS);
    vctDynamicMatrix<value_type> matrix2(ROWS, COLS);
    vctDynamicMatrix<value_type> matrix3(ROWS, COLS);
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiCiOperations(matrix1, matrix2, matrix3);
}

void vctDynamicMatrixTest::TestSoMiMiOperationsDouble(void) {
    TestSoMiMiOperations<double>();
}
void vctDynamicMatrixTest::TestSoMiMiOperationsFloat(void) {
    TestSoMiMiOperations<float>();
}
void vctDynamicMatrixTest::TestSoMiMiOperationsInt(void) {
    TestSoMiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMioMiOperations(void) {
    enum {ROWS = 8, COLS = 4};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COLS);
    vctDynamicMatrix<value_type> matrix2(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);

    vctGenericContainerTest::TestCioCiOperations(matrix1, matrix2, result);
}

void vctDynamicMatrixTest::TestMioMiOperationsDouble(void) {
    TestMioMiOperations<double>();
}
void vctDynamicMatrixTest::TestMioMiOperationsFloat(void) {
    TestMioMiOperations<float>();
}
void vctDynamicMatrixTest::TestMioMiOperationsInt(void) {
    TestMioMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMioSiMiOperations(void) {
    enum {ROWS = 5, COLS = 6};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COLS);
    value_type scalar;
    vctDynamicMatrix<value_type> matrix2(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    
    vctRandom(matrix1, value_type(-10), value_type(10));
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-10),
                                      value_type(10),
                                      scalar);
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);

    vctGenericContainerTest::TestCioSiCiOperations(matrix1, scalar, matrix2, result);
}

void vctDynamicMatrixTest::TestMioSiMiOperationsDouble(void) {
    TestMioSiMiOperations<double>();
}
void vctDynamicMatrixTest::TestMioSiMiOperationsFloat(void) {
    TestMioSiMiOperations<float>();
}
void vctDynamicMatrixTest::TestMioSiMiOperationsInt(void) {
    TestMioSiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMioMiMiOperations(void) {
    enum {ROWS = 5, COLS = 6};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COLS);
    vctDynamicMatrix<value_type> matrix2(ROWS, COLS);
    vctDynamicMatrix<value_type> matrix3(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctRandom(matrix3, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix3);

    vctGenericContainerTest::TestCioCiCiOperations(matrix1, matrix2, matrix3, result);
}

void vctDynamicMatrixTest::TestMioMiMiOperationsDouble(void) {
    TestMioMiMiOperations<double>();
}
void vctDynamicMatrixTest::TestMioMiMiOperationsFloat(void) {
    TestMioMiMiOperations<float>();
}
void vctDynamicMatrixTest::TestMioMiMiOperationsInt(void) {
    TestMioMiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMoMiMiOperations(void) {
    enum {ROWS = 4, COLS = 5};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COLS);
    vctDynamicMatrix<value_type> matrix2(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix2);

    vctGenericContainerTest::TestCoCiCiOperations(matrix1, matrix2, result);
}

void vctDynamicMatrixTest::TestMoMiMiOperationsDouble(void) {
    TestMoMiMiOperations<double>();
}
void vctDynamicMatrixTest::TestMoMiMiOperationsFloat(void) {
    TestMoMiMiOperations<float>();
}
void vctDynamicMatrixTest::TestMoMiMiOperationsInt(void) {
    TestMoMiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestProductOperations(void) {
    // result size
    enum {ROWS = 4, COLS = 5, COMSIZE = 7};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix1(ROWS, COMSIZE);
    vctDynamicMatrix<value_type> matrix2(COMSIZE, COLS);
    vctDynamicMatrix<value_type> matrix3(ROWS, COLS);
    vctDynamicVector<value_type> vector1(COMSIZE);
    vctDynamicVector<value_type> vector2(ROWS);
    
    vctRandom(matrix1, value_type(-10), value_type(10));
    vctRandom(matrix2, value_type(-10), value_type(10));
    vctRandom(vector1, value_type(-10), value_type(10));

    vctGenericMatrixTest::TestMatrixMatrixProductOperations(matrix1, matrix2, matrix3);
    vctGenericMatrixTest::TestMatrixVectorProductOperations(matrix1, vector1, vector2);
    
    vctRandom(vector2, value_type(-10), value_type(10));
    vctGenericMatrixTest::TestVectorMatrixProductOperations(matrix1, vector2, vector1);

    matrix1.SetSize(COMSIZE, COMSIZE);
    matrix2.SetSize(COMSIZE, COMSIZE);
    vctGenericMatrixTest::TestMatrixMatrixProductExceptions(matrix1, matrix2);
}

void vctDynamicMatrixTest::TestProductOperationsDouble(void) {
    TestProductOperations<double>();
}
void vctDynamicMatrixTest::TestProductOperationsFloat(void) {
    TestProductOperations<float>();
}
void vctDynamicMatrixTest::TestProductOperationsInt(void) {
    TestProductOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMoMiOperations(void) {
    enum {ROWS = 2, COLS = 10};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiOperations(matrix, result);
}

void vctDynamicMatrixTest::TestMoMiOperationsDouble(void) {
    TestMoMiOperations<double>();
}
void vctDynamicMatrixTest::TestMoMiOperationsFloat(void) {
    TestMoMiOperations<float>();
}
void vctDynamicMatrixTest::TestMoMiOperationsInt(void) {
    TestMoMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMoMiSiOperations(void) {
    enum {ROWS = 8, COLS = 8};
    typedef _elementType value_type;
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    if ((scalar < value_type(1))
        && (scalar > -value_type(1))) {
        scalar = value_type(3);
    }
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCoCiSiOperations(matrix, scalar, result);
}

void vctDynamicMatrixTest::TestMoMiSiOperationsDouble(void) {
    TestMoMiSiOperations<double>();
}
void vctDynamicMatrixTest::TestMoMiSiOperationsFloat(void) {
    TestMoMiSiOperations<float>();
}
void vctDynamicMatrixTest::TestMoMiSiOperationsInt(void) {
    TestMoMiSiOperations<int>();
}




template <class _elementType>
void vctDynamicMatrixTest::TestMoSiMiOperations(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    value_type scalar;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                       scalar);
    vctRandom(matrix, value_type(-10), value_type(10));
    RemoveQuasiZero(matrix);

    vctGenericContainerTest::TestCoSiCiOperations(scalar, matrix, result);
}

void vctDynamicMatrixTest::TestMoSiMiOperationsDouble(void) {
    TestMoSiMiOperations<double>();
}
void vctDynamicMatrixTest::TestMoSiMiOperationsFloat(void) {
    TestMoSiMiOperations<float>();
}
void vctDynamicMatrixTest::TestMoSiMiOperationsInt(void) {
    TestMoSiMiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMioSiOperations(void) {
    enum {ROWS = 6, COLS = 3};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
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

void vctDynamicMatrixTest::TestMioSiOperationsDouble(void) {
    TestMioSiOperations<double>();
}
void vctDynamicMatrixTest::TestMioSiOperationsFloat(void) {
    TestMioSiOperations<float>();
}
void vctDynamicMatrixTest::TestMioSiOperationsInt(void) {
    TestMioSiOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestMioOperations(void) {
    enum {ROWS = 2, COLS = 3};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestCioOperations(matrix, result);
}

void vctDynamicMatrixTest::TestMioOperationsDouble(void) {
    TestMioOperations<double>();
}
void vctDynamicMatrixTest::TestMioOperationsFloat(void) {
    TestMioOperations<float>();
}
void vctDynamicMatrixTest::TestMioOperationsInt(void) {
    TestMioOperations<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestSoMiSiOperations(void) {
    enum {ROWS = 4, COLS = 4};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    value_type scalar;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(value_type(-5),
                                      value_type(5),
                                      scalar);
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSoCiSiOperations(matrix, scalar, result);
}

void vctDynamicMatrixTest::TestSoMiSiOperationsDouble(void) {
    TestSoMiSiOperations<double>();
}
void vctDynamicMatrixTest::TestSoMiSiOperationsFloat(void) {
    TestSoMiSiOperations<float>();
}
void vctDynamicMatrixTest::TestSoMiSiOperationsInt(void) {
    TestSoMiSiOperations<int>();
}



template<class _elementType>
void vctDynamicMatrixTest::TestMinAndMax(void) {
    enum {ROWS = 5, COLS = 4};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctRandom(matrix, value_type(-50), value_type(50));
    vctGenericContainerTest::TestMinAndMax(matrix);
}

void vctDynamicMatrixTest::TestMinAndMaxDouble(void) {
    TestMinAndMax<double>();
}

void vctDynamicMatrixTest::TestMinAndMaxFloat(void) {
    TestMinAndMax<float>();
}

void vctDynamicMatrixTest::TestMinAndMaxInt(void) {
    TestMinAndMax<int>();
}


template <class _elementType>
void vctDynamicMatrixTest::TestIterators(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestIterators(matrix);
}

void vctDynamicMatrixTest::TestIteratorsDouble(void) {
    TestIterators<double>();
}
void vctDynamicMatrixTest::TestIteratorsFloat(void) {
    TestIterators<float>();
}
void vctDynamicMatrixTest::TestIteratorsInt(void) {
    TestIterators<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestSTLFunctions(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> matrix(ROWS, COLS);
    vctDynamicMatrix<value_type> result(ROWS, COLS);
    vctRandom(matrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestSTLFunctions(matrix, result);
}

void vctDynamicMatrixTest::TestSTLFunctionsDouble(void) {
    TestSTLFunctions<double>();
}
void vctDynamicMatrixTest::TestSTLFunctionsFloat(void) {
    TestSTLFunctions<float>();
}
void vctDynamicMatrixTest::TestSTLFunctionsInt(void) {
    TestSTLFunctions<int>();
}



template <class _elementType>
void vctDynamicMatrixTest::TestFastCopyOf(void) {
    enum {ROWS = 3, COLS = 5};
    typedef _elementType value_type;
    
    // dynamic matrix
    vctDynamicMatrix<value_type> destination(ROWS, COLS, VCT_ROW_MAJOR);
    vctDynamicMatrix<value_type> validSource(ROWS, COLS, VCT_ROW_MAJOR);
    vctRandom(validSource, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSource, destination);

    // test for different storage order
    vctDynamicMatrix<value_type> invalidSource(ROWS, COLS, VCT_COL_MAJOR);
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidSource));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidSource));

    // test for exception if different size
    vctDynamicMatrix<value_type> largerMatrix(ROWS + 1, COLS + 1, VCT_ROW_MAJOR);
    vctRandom(largerMatrix, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOfException(largerMatrix, destination);

    // test for compact by row    
    vctDynamicMatrixRef<value_type> compactRow(largerMatrix, 0, 0, ROWS, COLS);
    vctGenericContainerTest::TestFastCopyOf(compactRow, destination);

    // test for incompatible strides, since we will not make any memory access these are totally wrong
    vctDynamicMatrixRef<value_type> invalidRef(ROWS, COLS, 100, 100, largerMatrix.Pointer());
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidRef));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidRef));

    // different storage order for dynamic matrix
    vctDynamicMatrix<value_type> destinationCol(ROWS, COLS, VCT_COL_MAJOR);
    vctDynamicMatrix<value_type> validSourceCol(ROWS, COLS, VCT_COL_MAJOR);
    vctRandom(validSourceCol, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSourceCol, destinationCol);

    // test for compact by col
    vctDynamicMatrix<value_type> largerMatrixCol(ROWS + 1, COLS + 1, VCT_COL_MAJOR);
    vctRandom(largerMatrixCol, value_type(-10), value_type(10));
    vctDynamicMatrixRef<value_type> compactCol(largerMatrixCol, 0, 0, ROWS, COLS);
    vctGenericContainerTest::TestFastCopyOf(compactCol, destinationCol);


    // safe tests using a larger source and not performing checks
    CPPUNIT_ASSERT(largerMatrix.FastCopyOf(destination, vctFastCopy::SkipChecks));
    CPPUNIT_ASSERT(destination.FastCopyOf(invalidSource, vctFastCopy::SkipChecks));

    // fixed size matrix
    vctFixedSizeMatrix<value_type, ROWS, COLS, VCT_ROW_MAJOR> validSourceFixed;
    vctRandom(validSourceFixed, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSourceFixed, destination);

    // test for different storage order
    vctFixedSizeMatrix<value_type, ROWS, COLS, VCT_COL_MAJOR> invalidSourceFixed;
    CPPUNIT_ASSERT(!destination.FastCopyCompatible(invalidSourceFixed));
    CPPUNIT_ASSERT(!destination.FastCopyOf(invalidSourceFixed));

    // test for exception if different size
    typedef vctFixedSizeMatrix<value_type, ROWS + 1, COLS + 1, VCT_ROW_MAJOR> LargerMatrixType;
    LargerMatrixType largerMatrixFixed;
    vctRandom(largerMatrixFixed, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOfException(largerMatrixFixed, destination);

    // test for compact by row
    vctFixedSizeMatrixRef<value_type, ROWS, COLS, LargerMatrixType::ROWSTRIDE, LargerMatrixType::COLSTRIDE> compactRowFixed(largerMatrixFixed);
    vctGenericContainerTest::TestFastCopyOf(compactRowFixed, destination);


    // fixed size matrix
    vctFixedSizeMatrix<value_type, ROWS, COLS, VCT_COL_MAJOR> validSourceFixedCol;
    vctRandom(validSourceFixedCol, value_type(-10), value_type(10));
    vctGenericContainerTest::TestFastCopyOf(validSourceFixedCol, destinationCol);
    typedef vctFixedSizeMatrix<value_type, ROWS + 1, COLS + 1, VCT_COL_MAJOR> LargerMatrixColType;
    LargerMatrixColType largerMatrixFixedCol;
    vctRandom(largerMatrixFixedCol, value_type(-10), value_type(10));

    // test for compact by col
    vctFixedSizeMatrixRef<value_type, ROWS, COLS, LargerMatrixColType::ROWSTRIDE, LargerMatrixColType::COLSTRIDE> compactRowFixedCol(largerMatrixFixedCol);
    vctGenericContainerTest::TestFastCopyOf(compactRowFixedCol, destinationCol);


    // safe tests using a larger source and not performing checks
    CPPUNIT_ASSERT(destination.FastCopyOf(invalidSourceFixed, vctFastCopy::SkipChecks));
}

void vctDynamicMatrixTest::TestFastCopyOfDouble(void) {
    TestFastCopyOf<double>();
}
void vctDynamicMatrixTest::TestFastCopyOfFloat(void) {
    TestFastCopyOf<float>();
}
void vctDynamicMatrixTest::TestFastCopyOfInt(void) {
    TestFastCopyOf<int>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDynamicMatrixTest);

