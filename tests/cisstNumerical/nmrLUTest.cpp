/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLUTest.cpp,v 1.9 2007/05/18 21:56:36 anton Exp $
  
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
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    vctDynamicMatrix<double> output(rows, cols, VCT_COL_MAJOR);
    output.Assign(input);
    nmrLUDynamicData luData(input);
    nmrLU(output, luData);
    nmrLUTest::GenericTestDynamic(input, output, luData.PivotIndices());
}


void nmrLUTest::TestDynamicUserOutputColumnMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    vctDynamicMatrix<double> output(rows, cols, VCT_COL_MAJOR);
    output.Assign(input);

    const unsigned int minmn = (rows > cols) ? cols : rows;
    vctDynamicVector<F_INTEGER> pivotIndices(minmn);
    nmrLU(output, pivotIndices);
    nmrLUTest::GenericTestDynamic(input, output, pivotIndices);
}




template <unsigned int _rows, unsigned int _cols, unsigned int _minmn> 
void nmrLUTest::GenericTestFixedSize(const vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> & input,
                                     const vctFixedSizeMatrix<double, _rows, _cols, VCT_COL_MAJOR> & output,
                                     const vctFixedSizeVector<F_INTEGER, _minmn> & pivotIndices) {
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
    vctFixedSizeVector<F_INTEGER, MIN_MN> pivotIndices;
    nmrLU(output, pivotIndices);
    nmrLUTest::GenericTestFixedSize(input, output, pivotIndices);
}


void nmrLUTest::TestFixedSizeUserOutputColumnMajorMGeqN(void) {
    enum {ROWS = 7, COLS = 5};
    enum {MIN_MN = (ROWS > COLS) ? COLS : ROWS};
    vctFixedSizeMatrix<double, ROWS, COLS, VCT_COL_MAJOR> input, output;
    vctRandom(input, 0.0, 10.0);
    output.Assign(input);
    vctFixedSizeVector<F_INTEGER, MIN_MN> pivotIndices;
    nmrLU(output, pivotIndices);
    nmrLUTest::GenericTestFixedSize(input, output, pivotIndices);
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrLUTest);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLUTest.cpp,v $
// Revision 1.9  2007/05/18 21:56:36  anton
// cisstNumerical tests: Modified signature of test methods to avoid implicit
// conversion between containers and containersRef as these constructors are
// now "explicit".   See [2349] by Ofri.
//
// Revision 1.8  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.7  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.6  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.5  2006/01/27 01:00:42  anton
// cisstNumerical tests: Renamed "solution" to "data" (see #205).
//
// Revision 1.4  2006/01/26 17:00:02  anton
// cisstNumerical: Introduced new XyzSize() helper methods.  See #206.
//
// Revision 1.3  2006/01/11 19:36:24  anton
// nmrLUTest: Use operators now that bug in fixed size matrix operator is
// fixed.
//
// Revision 1.2  2006/01/11 04:50:32  anton
// nmrLU: Working version, including tests for fixed/dynamic, solution/user
// allocated pivot indices vector.  Still have to update the Doxygen comments.
//
// Revision 1.1  2006/01/10 23:25:33  anton
// cisstNumerical: Incomplete but somewhat working version of nmrLU.
//
//
// ****************************************************************************
