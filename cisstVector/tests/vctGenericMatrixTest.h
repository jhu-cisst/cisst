/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-07-09

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _vctGenericMatrixTest_h
#define _vctGenericMatrixTest_h

#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnPortability.h>

#include "vctGenericContainerTest.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>


/* Please check vctGenericContainerTest.h for some basic
   guidelines. */
class vctGenericMatrixTest
{
public:

    /*! Test assignements */
    template <class _containerType1, class _containerType2>
    static void TestAssignment(const _containerType1 & matrix1,
                               _containerType2 & matrix2) {
        typedef typename _containerType1::value_type value_type;
        typedef typename _containerType1::size_type size_type;
        typedef typename _containerType1::index_type index_type;
        index_type colIndex, rowIndex;
        const size_type cols = matrix1.cols();
        const size_type rows = matrix1.rows();

		// Test a simple assignment of one matrix to another.
        vctRandom(matrix2, value_type(-10), value_type(10));
        matrix2.Assign(matrix1);
        CPPUNIT_ASSERT(matrix2.cols() == cols);
        CPPUNIT_ASSERT(matrix2.rows() == rows);
        for (rowIndex = 0; rowIndex < rows; rowIndex++) {
            for (colIndex = 0; colIndex < cols; colIndex++) {
                CPPUNIT_ASSERT(matrix2[rowIndex][colIndex] == matrix1[rowIndex][colIndex]);
            }
        }

        // Assign by rows, first by using the Assign method, then by operator =
        vctRandom(matrix2, value_type(-10), value_type(10));
        for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
            matrix2.Row(rowIndex).Assign( matrix1.Row(rowIndex) );
        }
        CPPUNIT_ASSERT(matrix2.Equal(matrix1));

        vctRandom(matrix2, value_type(-10), value_type(10));
        for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
            matrix2.Row(rowIndex) = matrix1.Row(rowIndex);
		}
        CPPUNIT_ASSERT(matrix2.Equal(matrix1));

        // Assign by columnss, first by using the Assign method, then by operator =
        vctRandom(matrix2, value_type(-10), value_type(10));
        for (colIndex = 0; colIndex < cols; ++colIndex) {
            matrix2.Column(colIndex).Assign( matrix1.Column(colIndex) );
        }
        CPPUNIT_ASSERT(matrix2.Equal(matrix1));

        vctRandom(matrix2, value_type(-10), value_type(10));
        for (colIndex = 0; colIndex < cols; ++colIndex) {
            matrix2.Column(colIndex) = matrix1.Column(colIndex);
        }
        CPPUNIT_ASSERT(matrix2.Equal(matrix1));
    }



    /*! Test Zeros method */
    template <class _containerType1>
    static void TestZeros(_containerType1 & matrix1) {
        typedef typename _containerType1::value_type value_type;
        typedef typename _containerType1::size_type size_type;
        typedef typename _containerType1::index_type index_type;
        index_type colIndex, rowIndex;
        const size_type cols = matrix1.cols();
        const size_type rows = matrix1.rows();

        vctRandom(matrix1, value_type(-10), value_type(10));
        bool result = matrix1.Zeros();
        if (matrix1.IsCompact() || (matrix1.col_stride() == 1) || (matrix1.row_stride() == 1)) {
            CPPUNIT_ASSERT(result);
        } else {
            CPPUNIT_ASSERT(!result);
        }

        for (rowIndex = 0; rowIndex < rows; rowIndex++) {
            for (colIndex = 0; colIndex < cols; colIndex++) {
                CPPUNIT_ASSERT(matrix1[rowIndex][colIndex] == value_type(0));
            }
        }
    }



    /* Test access methods */
    template <class _containerType1, class _containerType2>
    static void TestAccessMethods(_containerType1 & matrix1,
                                  const _containerType2 & matrix2) {
        typedef typename _containerType1::value_type value_type;
        typedef typename _containerType1::size_type size_type;
        typedef typename _containerType1::index_type index_type;
        index_type index, rowIndex, colIndex;
        const size_type size = matrix2.size();
        const size_type rows = matrix2.rows();
        const size_type cols = matrix2.cols();

        value_type element;
        index = 0;

        for (rowIndex = 0; rowIndex < rows; rowIndex++) {
            for (colIndex = 0; colIndex < cols; colIndex++) {
                element = matrix2[rowIndex][colIndex];
                CPPUNIT_ASSERT(matrix1[rowIndex][colIndex] == element);
                CPPUNIT_ASSERT(matrix2.at(index) == element);
                CPPUNIT_ASSERT(matrix1.at(index) == element);
                CPPUNIT_ASSERT(matrix2.at(rowIndex, colIndex) == element);
                CPPUNIT_ASSERT(matrix1.at(rowIndex, colIndex) == element);
                CPPUNIT_ASSERT(matrix2(rowIndex, colIndex) == element);
       	        CPPUNIT_ASSERT(matrix1(rowIndex, colIndex) == element);
                CPPUNIT_ASSERT(matrix2.Element(rowIndex, colIndex) == element);
                CPPUNIT_ASSERT(matrix1.Element(rowIndex, colIndex) == element);
                CPPUNIT_ASSERT(*(matrix2.Pointer(rowIndex, colIndex)) == element);
                CPPUNIT_ASSERT(*(matrix1.Pointer(rowIndex, colIndex)) == element);
                CPPUNIT_ASSERT(matrix1.ValidRowIndex(rowIndex));
                CPPUNIT_ASSERT(matrix1.ValidColIndex(colIndex));
                CPPUNIT_ASSERT(matrix1.ValidIndex(rowIndex, colIndex));
                index++;
            }
        }
        CPPUNIT_ASSERT(size == index);
        CPPUNIT_ASSERT(!matrix1.ValidRowIndex(matrix1.rows()));
        CPPUNIT_ASSERT(!matrix1.ValidColIndex(matrix1.cols()));
        CPPUNIT_ASSERT(!matrix1.ValidIndex(matrix1.rows(), matrix1.cols()));

        // test exceptions on size
        bool gotException = false;
        try {
            matrix1.at(size);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix2.at(size);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        // test exceptions on rows
        gotException = false;
        try {
            matrix1.at(rows, 0);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix2.at(rows, 0);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix1(rows, 0);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix2(rows, 0);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        // test exceptions on columns
        gotException = false;
        try {
            matrix1.at(0, cols);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix2.at(0, cols);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix1(0, cols);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix2(0, cols);
        } catch (std::out_of_range &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

    }


    /*! Test SelectRowsFrom and SelectColumnsFrom operations. */
    template <class _inputMatrixType, class _rowIndexContainerType, class _colIndexContainerType, class _outputRowSelectionType, class _outputColSelectionType>
    static void TestSelect(const _inputMatrixType  & inputMatrix,
                           const _rowIndexContainerType  & rowIndexes,
                           const _colIndexContainerType  & colIndexes,
                           _outputRowSelectionType & outputRowSelection,
                           _outputColSelectionType & outputColSelection)
    {
        typedef typename _inputMatrixType::size_type size_type;
        typedef typename _inputMatrixType::index_type index_type;

        const size_type inputRows = inputMatrix.rows();
        const size_type inputCols = inputMatrix.cols();
        const size_type outputRows = outputRowSelection.rows();
        const size_type outputCols = outputColSelection.cols();

        CPPUNIT_ASSERT(rowIndexes.size() == outputRows);
        CPPUNIT_ASSERT(colIndexes.size() == outputCols);

        CPPUNIT_ASSERT(inputCols == outputRowSelection.cols());
        CPPUNIT_ASSERT(inputRows == outputColSelection.rows());

        CPPUNIT_ASSERT(rowIndexes.Lesser(inputRows));
        CPPUNIT_ASSERT(colIndexes.Lesser(inputCols));

        outputRowSelection.SelectRowsFrom(inputMatrix, rowIndexes);
        outputColSelection.SelectColsFrom(inputMatrix, colIndexes);

        index_type indx;
        for (indx = 0; indx < outputRows; ++indx) {
            CPPUNIT_ASSERT( outputRowSelection.Row(indx).Equal(inputMatrix.Row( rowIndexes[indx] )) );
        }
        for (indx = 0; indx < outputCols; ++indx) {
            CPPUNIT_ASSERT( outputColSelection.Column(indx).Equal(inputMatrix.Column( colIndexes[indx] )) );
        }
    }

    template <class _matrixType1>
    static void TestExchangeAndPermutationOperations(_matrixType1 & matrix,
                                                     const vct::size_type row1, const vct::size_type row2,
                                                     const vct::size_type col1, const vct::size_type col2,
                                                     const vct::index_type rowPermutation[], const vct::index_type colPermutation[] )
    {
        const vct::size_type rows = matrix.rows();
        const vct::size_type cols = matrix.cols();
        _matrixType1 matrixCopy(matrix);

        vct::index_type counter;
        matrix.ExchangeRows(row1, row2);
        CPPUNIT_ASSERT( matrixCopy.Row(row1) == matrix.Row(row2) );
        CPPUNIT_ASSERT( matrixCopy.Row(row2) == matrix.Row(row1) );
        for (counter = 0; counter < rows; ++counter) {
            if ((counter == row1) || (counter == row2))
                continue;
            CPPUNIT_ASSERT( matrixCopy.Row(counter) == matrix.Row(counter) );
        }
        matrix.ExchangeRows(row1, row2);

        matrix.ExchangeColumns(col1, col2);
        CPPUNIT_ASSERT( matrixCopy.Column(col1) == matrix.Column(col2) );
        CPPUNIT_ASSERT( matrixCopy.Column(col2) == matrix.Column(col1) );
        for (counter = 0; counter < cols; ++counter) {
            if ((counter == col1) || (counter == col2))
                continue;
            CPPUNIT_ASSERT( matrixCopy.Column(counter) == matrix.Column(counter) );
        }

        matrix.ExchangeRows(row1, row2);
        matrix.ExchangeColumns(col1, col2);
        matrix.ExchangeRows(row1, row2);
        CPPUNIT_ASSERT(matrixCopy == matrix);

        // This initialization is required for generic structure
        _matrixType1 matrixPermutation(matrix);

        matrixPermutation.RowPermutationOf(matrix, rowPermutation);
        for (counter = 0; counter < rows; ++counter) {
            CPPUNIT_ASSERT( matrixPermutation.Row(counter) == matrix.Row( rowPermutation[counter] ) );
        }
        matrixCopy.RowInversePermutationOf(matrixPermutation, rowPermutation);
        CPPUNIT_ASSERT(matrixCopy == matrix);

        matrixPermutation.ColumnPermutationOf(matrix, colPermutation);
        for (counter = 0; counter < cols; ++counter) {
            CPPUNIT_ASSERT( matrixPermutation.Column(counter) == matrix.Column( colPermutation[counter] ) );
        }
        matrixCopy.ColumnInversePermutationOf(matrixPermutation, colPermutation);
        CPPUNIT_ASSERT(matrixCopy == matrix);

    }



    /*! Test Product based operations. */
    template <class _matrixType1, class _matrixType2, class _matrixType3>
    static void TestMatrixMatrixProductOperations(const _matrixType1 & matrix1,
                                                  const _matrixType2 & matrix2,
                                                  _matrixType3 & matrix3,
                                                  typename _matrixType1::value_type tolerance
                                                  = cmnTypeTraits<typename _matrixType1::value_type>::Tolerance()) {
        typedef typename _matrixType1::value_type value_type;
        typedef typename _matrixType1::size_type size_type;
        typedef typename _matrixType1::index_type index_type;
        index_type rowIndex, colIndex, index;
        value_type sum;
        const size_type rows = matrix1.rows();
        const size_type cols = matrix2.cols();
        const size_type common = matrix1.cols();

        matrix3.ProductOf(matrix1, matrix2);
        for (rowIndex = 0; rowIndex < rows; rowIndex++) {
            for (colIndex = 0; colIndex < cols; colIndex++) {
                sum = value_type(0);
                for (index = 0; index < common; index++) {
                    sum += matrix1[rowIndex][index] * matrix2[index][colIndex];
                }
                CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(matrix3[rowIndex][colIndex]), static_cast<double>(sum), static_cast<double>(tolerance * common));
            }
        }

        matrix3 = matrix1 * matrix2;
        for (rowIndex = 0; rowIndex < rows; rowIndex++) {
            for (colIndex = 0; colIndex < cols; colIndex++) {
                sum = value_type(0);
                for (index = 0; index < common; index++) {
                    sum += matrix1[rowIndex][index] * matrix2[index][colIndex];
                }
                CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(matrix3[rowIndex][colIndex]), static_cast<double>(sum), static_cast<double>(tolerance * common));
            }
        }

        // Outer product
        matrix3.OuterProductOf(matrix1.Column(0), matrix2.Row(0));
        for (rowIndex = 0; rowIndex < matrix3.rows(); ++rowIndex) {
            for (colIndex = 0; colIndex < matrix3.cols(); ++colIndex) {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(matrix3[rowIndex][colIndex]), static_cast<double>(matrix1[rowIndex][0] * matrix2[0][colIndex]), static_cast<double>(tolerance));
            }
        }
    }


    /*! Test Product based operations. */
    template <class _matrixType1, class _matrixType2>
    static void TestMatrixMatrixProductExceptions(const _matrixType1 & matrix1,
                                                  _matrixType2 & matrix2) {
        // Check that exceptions are thrown if the output and inputs
        // share base pointers
        bool gotException = false;
        try {
            matrix2.ProductOf(matrix1, matrix2);
        } catch (std::runtime_error &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);

        gotException = false;
        try {
            matrix2.ProductOf(matrix2, matrix1);
        } catch (std::runtime_error &) {
            gotException = true;
        }
        CPPUNIT_ASSERT(gotException);
    }


    template <class _matrixType1, class __vectorType1, class __vectorType2>
    static void TestMatrixVectorProductOperations(const _matrixType1 & matrix1,
                                                  const __vectorType1 & vector1,
                                                  __vectorType2 & vector2,
                                                  typename _matrixType1::value_type tolerance
                                                  = cmnTypeTraits<typename _matrixType1::value_type>::Tolerance()) {
        typedef typename _matrixType1::value_type value_type;
        typedef typename _matrixType1::size_type size_type;
        typedef typename _matrixType1::index_type index_type;
        index_type rowIndex, index;
        value_type sum;
        const size_type rows = matrix1.rows();
        const size_type common = matrix1.cols();

        vector2.ProductOf(matrix1, vector1);
        for (rowIndex = 0; rowIndex < rows; rowIndex++) {
            sum = value_type(0);
            for (index = 0; index < common; index++) {
                sum += matrix1[rowIndex][index] * vector1[index];
            }
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(vector2[rowIndex]), static_cast<double>(sum), static_cast<double>(tolerance * common));
        }

        vector2 = matrix1 * vector1;
        for (rowIndex = 0; rowIndex < rows; rowIndex++) {
            sum = value_type(0);
            for (index = 0; index < common; index++) {
                sum += matrix1[rowIndex][index] * vector1[index];
            }
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(vector2[rowIndex]), static_cast<double>(sum), static_cast<double>(tolerance * common));
        }
    }


    template <class _matrixType1, class __vectorType1, class __vectorType2>
    static void TestVectorMatrixProductOperations(const _matrixType1 & matrix1,
                                                  const __vectorType1 & vector1,
                                                  __vectorType2 & vector2,
                                                  typename _matrixType1::value_type tolerance
                                                  = cmnTypeTraits<typename _matrixType1::value_type>::Tolerance()) {
        typedef typename _matrixType1::value_type value_type;
        typedef typename _matrixType1::size_type size_type;
        typedef typename _matrixType1::index_type index_type;
        index_type colIndex, index;
        value_type sum;
        const size_type common = matrix1.rows();
        const size_type cols = matrix1.cols();

        vector2.ProductOf(vector1, matrix1);
        for (colIndex = 0; colIndex < cols; colIndex++) {
            sum = value_type(0);
            for (index = 0; index < common; index++) {
                sum += vector1[index] * matrix1[index][colIndex];
            }
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(vector2[colIndex]), static_cast<double>(sum), static_cast<double>(tolerance * common));
        }

        vector2 = vector1 * matrix1;
        for (colIndex = 0; colIndex < cols; colIndex++) {
            sum = value_type(0);
            for (index = 0; index < common; index++) {
                sum += vector1[index] * matrix1[index][colIndex];
            }
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<double>(vector2[colIndex]), static_cast<double>(sum), static_cast<double>(tolerance * common));
        }
    }

};


#endif // _vctGenericMatrixTest_h
