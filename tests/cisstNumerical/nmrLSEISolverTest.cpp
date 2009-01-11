/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSEISolverTest.cpp,v 1.5 2007/04/26 20:12:05 anton Exp $
  
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


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLSEISolverTest.cpp,v $
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
//
// ****************************************************************************
