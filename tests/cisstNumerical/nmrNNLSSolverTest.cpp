/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrNNLSSolverTest.cpp,v 1.6 2007/04/26 20:12:05 anton Exp $
  
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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrNNLSSolverTest.cpp,v $
// Revision 1.6  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.5  2006/11/20 20:33:52  anton
// Licensing: Applied new license to tests.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/09/06 15:41:37  anton
// cisstNumerical tests: Added license.
//
// Revision 1.2  2004/11/11 20:42:31  anton
// cisstNumerical Tests: Use VCT_COL_MAJOR instead of false (minor pb)
//
// Revision 1.1  2004/11/03 22:28:34  anton
// cisstNumerical Tests: Added sanity check tests for LDP and NNLS.  Updated
// other tests to use VCT_COL_MAJOR.
//
// ****************************************************************************
