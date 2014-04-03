/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

