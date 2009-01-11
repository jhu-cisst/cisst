/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLUSolverTest.cpp,v 1.6 2007/04/26 20:12:05 anton Exp $
  
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


#include "nmrLUSolverTest.h"

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>
#include <cisstVector/vctRandomDynamicMatrix.h>

template <class _matrixType>
void nmrLUSolverTest::GenericTest(_matrixType & input) {
    typedef typename _matrixType::value_type value_type;
    const unsigned int rows = input.rows();
    const unsigned int cols = input.cols();
    const bool storageOrder = input.IsRowMajor();

    /* Keep a copy of original since LU modifies input */
    vctDynamicMatrix<value_type> inputCopy(rows, cols, storageOrder);
    inputCopy.Assign(input);
    
    /* Memory allocation, also allocate LU and P */
    nmrLUSolver LUSolver(input, true, true);

    /* Solve */
    LUSolver.Solve(input);
#if 0
    std::cout << "\nL * U:\n" << LUSolver.GetL() * LUSolver.GetU() << std::endl;
    std::cout << "\nP * L * U:\n" << LUSolver.GetP() * (LUSolver.GetL() * LUSolver.GetU()) << std::endl;
    std::cout << "\ninput:\n" << inputCopy << std::endl;
#endif
    value_type error = (inputCopy - (LUSolver.GetP() * (LUSolver.GetL() * LUSolver.GetU()))).LinfNorm();
    CPPUNIT_ASSERT(error < cmnTypeTraits<value_type>::Tolerance());
}


void nmrLUSolverTest::TestDynamicColumnMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_COL_MAJOR);
    vctRandom(input, 0.0, 10.0);
    nmrLUSolverTest::GenericTest(input);
}



// Can't get this to work right now.
#if 0
void nmrLUSolverTest::TestDynamicRowMajor(void) {
    int rows, cols;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(10, 20, rows);
    randomSequence.ExtractRandomValue(10, 20, cols);
    vctDynamicMatrix<double> input(rows, cols , VCT_ROW_MAJOR);
    vctRandom(input, 0.0, 10.0);
    nmrLUSolverTest::GenericTest(input);
}
#endif



void nmrLUSolverTest::TestFixedSizeColumnMajor(void) {
    vctFixedSizeMatrix<double, 5, 7, VCT_COL_MAJOR> input;
    vctRandom(input, 0.0, 10.0);
    nmrLUSolverTest::GenericTest(input);
}


#if 0
void nmrLUSolverTest::TestFixedSizeRowMajor(void) {
    vctFixedSizeMatrix<double, 4, 6, VCT_ROW_MAJOR> input;
    vctRandom(input, 0.0, 10.0);
    nmrLUSolverTest::GenericTest(input);
}
#endif



CPPUNIT_TEST_SUITE_REGISTRATION(nmrLUSolverTest);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLUSolverTest.cpp,v $
// Revision 1.6  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.5  2007/02/12 03:25:03  anton
// cisstVector and cisstNumerical tests: Used more refined #include for vctRandom
// (see ticket #264).
//
// Revision 1.4  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.3  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.1  2005/08/04 18:56:44  anton
// cisstNumericalTests: Added nmrLUSolverTest.
//
// ****************************************************************************
