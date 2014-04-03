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


#include "nmrNNLSSolverTest.h"


void nmrNNLSSolverTest::TestSolveBookExample(void) {

    /* data from www.netlib.org/lawson-hanson/all.f
       results from Matlab */
    vctDynamicMatrix<double> h(15, 1 , VCT_COL_MAJOR);
    h.Assign(InputMatrix.Pointer(0, 5));

    vctDynamicMatrix<double> G(15, 5, VCT_COL_MAJOR);
    G.Assign(vctDynamicConstMatrixRef<double>(InputMatrix, /* from */ 0, 0, /* size */ 15, 5));

    /* Memory allocation */
    nmrNNLSSolver NNLSSolver(G, h);
    
    /* Solve */
    NNLSSolver.Solve(G, h);;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.00000000000000, NNLSSolver.GetX()(0, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.00000000000000, NNLSSolver.GetX()(1, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.43925624849994, NNLSSolver.GetX()(2, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.00000000000000, NNLSSolver.GetX()(3, 0), 1.0e-5);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.00000000000000, NNLSSolver.GetX()(4, 0), 1.0e-5);
}


CPPUNIT_TEST_SUITE_REGISTRATION(nmrNNLSSolverTest);

