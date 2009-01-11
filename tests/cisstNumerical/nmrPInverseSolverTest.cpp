/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPInverseSolverTest.cpp,v 1.5 2007/04/26 20:12:05 anton Exp $
  
  Author(s):  Ankur Kappor
  Created on: 2005-07-28
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrPInverseSolverTest.h"

void nmrPInverseSolverTest::TestSolveBookExample(void) {

    /*! data and results for matlab */
    /* Memory allocation */
    nmrPInverseSolver pInverse(Input);
    
    /* Solve */
    pInverse.Solve(Input);
    for (int rr = 0; rr < 15; rr++) {
	    for (int cc = 0; cc < 15; cc++) {
		    CPPUNIT_ASSERT_DOUBLES_EQUAL(pInverse.GetPInverse().at(rr, cc), Output(rr, cc), 1.0e-5);
	    }
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrPInverseSolverTest);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrPInverseSolverTest.cpp,v $
// Revision 1.5  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
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
// Revision 1.1  2005/07/29 00:02:18  kapoor
// Numerical Test: Some simple sanity checks for Least Squares (LS),
// pseudo-inverse and SVD with RHS matrix. Tests compare a matrix with
// results from MATLAB(r).
//
// ****************************************************************************
