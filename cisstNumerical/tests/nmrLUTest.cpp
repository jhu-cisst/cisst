/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2006-01-10

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrLUTest.h"

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>


void nmrLUTest::TestDynamicDataColumnMajor(void) {
    vct::size_type rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    rows = randomSequence.ExtractRandomSizeT(10, 20);
    cols = randomSequence.ExtractRandomSizeT(10, 20);

    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    vctDynamicMatrix<double> output(rows, cols, VCT_COL_MAJOR);
    output.Assign(input);
    nmrLUDynamicData luData(input);
    nmrLU(output, luData);
    nmrLUTest::GenericTestDynamic(input, output, luData.PivotIndices());
}


void nmrLUTest::TestDynamicUserOutputColumnMajor(void) {
    vct::size_type rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    rows = randomSequence.ExtractRandomSizeT(10, 20);
    cols = randomSequence.ExtractRandomSizeT(10, 20);

    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    vctDynamicMatrix<double> output(rows, cols, VCT_COL_MAJOR);
    output.Assign(input);

    const vct::size_type minmn = (rows > cols) ? cols : rows;
    vctDynamicVector<CISSTNETLIB_INTEGER> pivotIndices(minmn);
    nmrLU(output, pivotIndices);
    nmrLUTest::GenericTestDynamic(input, output, pivotIndices);
}




template <vct::size_type _rows, vct::size_type _cols, vct::size_type _minmn>
void nmrLUTest::GenericTestFixedSize(const vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> & input,
                                     const vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> & output,
                                     const vctFixedSizeVector<CISSTNETLIB_INTEGER, _minmn> & pivotIndices) {
    typedef nmrLUFixedSizeData<_rows, _cols> DataType;
    typename DataType::MatrixTypeP P;
    typename DataType::MatrixTypeL L;
    typename DataType::MatrixTypeU U;
    DataType::UpdateMatrixP(pivotIndices, P);
    DataType::UpdateMatrixLU(output, L, U);

    double error = (input - (P * L * U)).LinfNorm();
    CPPUNIT_ASSERT(error < cmnTypeTraits<double>::Tolerance());
}


void nmrLUTest::TestFixedSizeDataColumnMajorMLeqN(void) {
    enum {ROWS = 6, COLS = 8};
    vctFixedSizeMatrix<double, ROWS, COLS, VCT_COL_MAJOR> input, output;
    vctRandom(input, 0.0, 10.0);
    output.Assign(input);
    typedef nmrLUFixedSizeData<ROWS, COLS> DataType;
    DataType luData;
    nmrLU(output, luData);
    nmrLUTest::GenericTestFixedSize(input, output, luData.PivotIndices());
}


void nmrLUTest::TestFixedSizeDataColumnMajorMGeqN(void) {
    enum {ROWS = 9, COLS = 7};
    vctFixedSizeMatrix<double, ROWS, COLS, VCT_COL_MAJOR> input, output;
    vctRandom(input, 0.0, 10.0);
    output.Assign(input);
    typedef nmrLUFixedSizeData<ROWS, COLS> DataType;
    DataType luData;
    nmrLU(output, luData);
    nmrLUTest::GenericTestFixedSize(input, output, luData.PivotIndices());
}


void nmrLUTest::TestFixedSizeUserOutputColumnMajorMLeqN(void) {
    enum {ROWS = 6, COLS = 8};
    enum {MIN_MN = (ROWS > COLS) ? COLS : ROWS};
    vctFixedSizeMatrix<double, ROWS, COLS, VCT_COL_MAJOR> input, output;
    vctRandom(input, 0.0, 10.0);
    output.Assign(input);
    vctFixedSizeVector<CISSTNETLIB_INTEGER, MIN_MN> pivotIndices;
    nmrLU(output, pivotIndices);
    nmrLUTest::GenericTestFixedSize(input, output, pivotIndices);
}


void nmrLUTest::TestFixedSizeUserOutputColumnMajorMGeqN(void) {
    enum {ROWS = 7, COLS = 5};
    enum {MIN_MN = (ROWS > COLS) ? COLS : ROWS};
    vctFixedSizeMatrix<double, ROWS, COLS, VCT_COL_MAJOR> input, output;
    vctRandom(input, 0.0, 10.0);
    output.Assign(input);
    vctFixedSizeVector<CISSTNETLIB_INTEGER, MIN_MN> pivotIndices;
    nmrLU(output, pivotIndices);
    nmrLUTest::GenericTestFixedSize(input, output, pivotIndices);
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrLUTest);
