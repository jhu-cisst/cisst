/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-11-03
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "nmrLDPSolverTest.h"


void nmrLDPSolverTest::TestSolveBookExample(void) {

    /* data from www.netlib.org/lawson-hanson/all.f
       results from Matlab */
    vctDynamicMatrix<double> h(15, 1 , VCT_COL_MAJOR);
    h.Assign(InputMatrix.Pointer(0, 5));

    vctDynamicMatrix<double> G(15, 5, VCT_COL_MAJOR);
    G.Assign(vctDynamicConstMatrixRef<double>(InputMatrix, /* from */ 0, 0, /* size */ 15, 5));

    /* Memory allocation */
    nmrLDPSolver LDPSolver(G, h);
    
    /* Solve */
    LDPSolver.Solve(G, h);;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.95414376985139, LDPSolver.GetX()(0, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-20.92326261616518, LDPSolver.GetX()(1, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.35473965650424, LDPSolver.GetX()(2, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(23.39007954906827, LDPSolver.GetX()(3, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-5.06578811916773, LDPSolver.GetX()(4, 0), 1.0e-5);
}


CPPUNIT_TEST_SUITE_REGISTRATION(nmrLDPSolverTest);

