/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-07-22
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrSVDSolverTest.h"

#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>
#include <cisstNumerical/nmrIsOrthonormal.h>


template <class _matrixType>
void nmrSVDSolverTest::GenericTest(_matrixType & input) {
    typedef typename _matrixType::value_type value_type;
    
    /* Keep a copy of original since SVD modifies input */
    vctDynamicMatrix<value_type> inputCopy(input.rows(), input.cols(), input.IsRowMajor());
    inputCopy.Assign(input);
    
    /* Memory allocation */
    nmrSVDSolver SVDSolver(input);

    /* Solve */
    SVDSolver.Solve(input);

    /* Create a diagonal matrix from the singular values */
    vctDynamicMatrix<value_type> matrixS(input.rows(), input.cols(), input.IsRowMajor());
    matrixS.SetAll(0.0);
    matrixS.Diagonal().Assign(SVDSolver.GetS().Column(0));

    /* Recompose */
    vctDynamicMatrix<value_type> product(input.rows(), input.cols(), input.IsRowMajor());
    product = SVDSolver.GetU() * (matrixS * SVDSolver.GetVt());

    /* Compare initial with result */
    value_type error = (inputCopy - product).LinfNorm();
    CPPUNIT_ASSERT(error < cmnTypeTraits<value_type>::Tolerance());

    /* Make sure that both U and V are orthonormal */
    CPPUNIT_ASSERT(nmrIsOrthonormal(SVDSolver.GetU()));
    CPPUNIT_ASSERT(nmrIsOrthonormal(SVDSolver.GetVt()));
}


void nmrSVDSolverTest::TestDynamicColumnMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    nmrSVDSolverTest::GenericTest(input);
}




void nmrSVDSolverTest::TestDynamicRowMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_ROW_MAJOR);
    vctRandom(input, 0.0, 10.0);
    nmrSVDSolverTest::GenericTest(input);
}




void nmrSVDSolverTest::TestFixedSizeColumnMajor(void) {
    vctFixedSizeMatrix<double, 5, 7, VCT_COL_MAJOR> input;
    vctRandom(input, 0.0, 10.0);
    nmrSVDSolverTest::GenericTest(input);
}


void nmrSVDSolverTest::TestFixedSizeRowMajor(void) {
    vctFixedSizeMatrix<double, 4, 6, VCT_ROW_MAJOR> input;
    vctRandom(input, 0.0, 10.0);
    nmrSVDSolverTest::GenericTest(input);
}




CPPUNIT_TEST_SUITE_REGISTRATION(nmrSVDSolverTest);

