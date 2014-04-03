/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ankur Kapoor
  Created on: 2005-07-28
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrSVDRSSolverTest.h"


void nmrSVDRSSolverTest::TestSolveBookExample(void) {

    /*! data and results for matlab */
    /* Memory allocation */
    nmrSVDRSSolver SVDRSSolver(InputMatrix, InputMatrixRS);
    
    /* Solve */
    SVDRSSolver.Solve(InputMatrix, InputMatrixRS);
    /* InputMatrix is now replaced by V and InputMatrixRS is replaced by U^t * B */
    vctDynamicMatrix<double> pS(15, 15, VCT_COL_MAJOR);
    pS.SetAll(0.);
    for (int rr = 0; rr < 15; rr++) {
	    double singularValue;
	    if ((singularValue = SVDRSSolver.GetS().at(rr, 0)) != 0) pS(rr, rr) = 1/singularValue;
    }
    for (int rr = 0; rr < 15; rr++) {
	    CPPUNIT_ASSERT_DOUBLES_EQUAL(SVDRSSolver.GetS().at(rr, 0), OutputS(rr, 0), 1.0e-5);
    }
    vctDynamicMatrix<double> X(15, 3, VCT_COL_MAJOR);
    X = InputMatrix * pS * InputMatrixRS;
    for (int rr = 0; rr < 15; rr++) {
	    for (int cc = 0; cc < 3; cc++) {
		    CPPUNIT_ASSERT_DOUBLES_EQUAL(X(rr, cc), OutputX(rr, cc), 1.0e-5);
	    }
    }
}


CPPUNIT_TEST_SUITE_REGISTRATION(nmrSVDRSSolverTest);

