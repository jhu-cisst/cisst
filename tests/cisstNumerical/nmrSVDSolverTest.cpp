/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVDSolverTest.cpp,v 1.10 2007/04/26 20:12:05 anton Exp $
  
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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrSVDSolverTest.cpp,v $
// Revision 1.10  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.9  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.8  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.7  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/09/24 00:06:32  anton
// nmrSVDSolverTest.cpp: Requires #include vctRandom.h since nmrSVDSolver does
// not brutally include cisstVector.h.
//
// Revision 1.5  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.4  2005/08/25 16:55:35  anton
// cisstNumerical tests: Removed #include of cisstXyz.h to avoid useless
// dependencies and long compilations.
//
// Revision 1.3  2005/07/27 22:39:28  anton
// nmrSVDSolverTest: Now uses nmrIsOrthonormal to check the SVD output.
//
// Revision 1.2  2005/07/25 19:48:30  anton
// nmrSVDSolverTest: Added tests for different storage orders and fixed size
// matrices.
//
// Revision 1.1  2005/07/22 22:30:10  anton
// cisstNumerical Tests: Added basic test for nmrSVDSolver.
//
//
// ****************************************************************************
