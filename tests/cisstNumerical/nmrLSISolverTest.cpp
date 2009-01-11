/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSISolverTest.cpp,v 1.7 2007/04/26 20:12:05 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 2004-10-31
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrLSISolverTest.h"


void nmrLSISolverTest::TestSolveBookExample(void) {

    /* data from www.netlib.org/lawson-hanson/all.f
       results from Matlab */
    vctDynamicMatrix<double> b(6, 1 , VCT_FORTRAN_ORDER);
    b.Assign(InputMatrix.Pointer(9, 5));

    vctDynamicMatrix<double> d(5, 1 , VCT_FORTRAN_ORDER);
    d.Assign(InputMatrix.Pointer(0, 5));

    vctDynamicMatrix<double> A(6, 5, VCT_FORTRAN_ORDER);
    A.Assign(vctDynamicConstMatrixRef<double>(InputMatrix, /* from */ 9, 0, /* size */ 6, 5));

    vctDynamicMatrix<double> C(5, 5, VCT_FORTRAN_ORDER);
    C.Assign(vctDynamicConstMatrixRef<double>(InputMatrix, /* from */ 0, 0, /* size */ 5, 5));

    /* Memory allocation */
    nmrLSISolver LSISolver(C, d, A, b);
    
    /* Solve */
    LSISolver.Solve(C, d, A, b);;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(327.99740458630680, LSISolver.GetX()(0, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-463.91504497345306, LSISolver.GetX()(1, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(362.61001495719779, LSISolver.GetX()(2, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-410.34272327035387, LSISolver.GetX()(3, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(382.24683243709086, LSISolver.GetX()(4, 0), 1.0e-5);
}


CPPUNIT_TEST_SUITE_REGISTRATION(nmrLSISolverTest);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLSISolverTest.cpp,v $
// Revision 1.7  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.6  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.5  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.3  2005/02/04 17:01:45  anton
// cisstNumerical tests: Use VCT_FORTRAN_ORDER as well as VCT_COL_MAJOR
//
// Revision 1.2  2004/11/03 22:28:34  anton
// cisstNumerical Tests: Added sanity check tests for LDP and NNLS.  Updated
// other tests to use VCT_COL_MAJOR.
//
// Revision 1.1  2004/11/02 17:03:17  anton
// cisstNumerical Tests: Add a sanity check test for nmrLSISolver.
//
// ****************************************************************************
