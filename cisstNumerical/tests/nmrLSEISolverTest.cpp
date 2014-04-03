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


#include "nmrLSEISolverTest.h"


void nmrLSEISolverTest::TestSolveBookExample(void) {
    nmrLSEISolver LSEISolver(E, A, G);
    
    /* Solve */
    LSEISolver.Solve(E, f, A, b, G, h);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(RX(0, 0), LSEISolver.GetX()(0, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(RX(1, 0), LSEISolver.GetX()(1, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(RX(2, 0), LSEISolver.GetX()(2, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(RX(3, 0), LSEISolver.GetX()(3, 0), 1.0e-5);
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrLSEISolverTest);

