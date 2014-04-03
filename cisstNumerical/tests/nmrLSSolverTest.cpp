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


#include "nmrLSSolverTest.h"


void nmrLSSolverTest::TestSolveBookExample(void) {

    /*! data and results for matlab */
    /* Memory allocation */
    nmrLSSolver LSSolver(InputMatrix, InputMatrixRS);
    
    /* Solve */
    LSSolver.Solve(InputMatrix, InputMatrixRS);
    /* InputMatrix is now replaced by QR factorization and InputMatrixRS is replaced by X */
    for (int rr = 0; rr < 15; rr++) {
	    for (int cc = 0; cc < 3; cc++) {
		    CPPUNIT_ASSERT_DOUBLES_EQUAL(InputMatrixRS(rr, cc), OutputX(rr, cc), 1.0e-5);
	    }
    }
}


CPPUNIT_TEST_SUITE_REGISTRATION(nmrLSSolverTest);

