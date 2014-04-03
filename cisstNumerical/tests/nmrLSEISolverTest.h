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


#ifndef _nmrLSEISolverTest_h
#define _nmrLSEISolverTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrLSEISolver.h>

class nmrLSEISolverTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrLSEISolverTest);
    CPPUNIT_TEST(TestSolveBookExample);
    CPPUNIT_TEST_SUITE_END();

protected:
    vctDynamicMatrix<double> E;
    vctDynamicMatrix<double> A;
    vctDynamicMatrix<double> G;
    vctDynamicMatrix<double> f;
    vctDynamicMatrix<double> b;
    vctDynamicMatrix<double> h;
    vctDynamicMatrix<double> X;
    vctDynamicMatrix<double> RX;

public:

    void setUp() {
	    E.SetSize(2, 4, VCT_FORTRAN_ORDER);
	    A.SetSize(3, 4, VCT_FORTRAN_ORDER);
	    G.SetSize(3, 4, VCT_FORTRAN_ORDER);
	    f.SetSize(2, 1, VCT_FORTRAN_ORDER);
	    b.SetSize(3, 1, VCT_FORTRAN_ORDER);
	    h.SetSize(3, 1, VCT_FORTRAN_ORDER);
	    X.SetSize(4, 1, VCT_FORTRAN_ORDER);
	    RX.SetSize(4, 1, VCT_FORTRAN_ORDER);
	    E.Assign(0.32729236,   -0.18670858,   -0.58831654,   -0.13639588,
		    0.17463914,    0.72579055,    2.18318582,    0.11393131);
	    f.Assign( 1.06676821,    0.05928146);
	    A.Assign(0.20270000,    0.27210000,    0.74670000,    0.46590000,
		0.19870000,    0.19880000,    0.44500000,    0.41860000,
		0.60370000,    0.01520000,    0.93180000,    0.84620000);
	     b.Assign( 0.52510000,    0.20260000,    0.67210000);
	    G.Assign(-0.20270000,   -0.27210000,   -0.74670000,   -0.46590000,
		-0.19870000,   -0.19880000,   -0.44500000,   -0.41860000,
		-0.60370000,   -0.01520000,   -0.93180000,   -0.84620000);
	    h.Assign(-0.52510000,   -0.20260000,   -0.67210000);
            RX.Assign(2.67547302288577, 0.68735479611120, -0.37830739673366, -0.71026237832735);
    }

    void tearDown()
    {}

    void TestSolveBookExample(void);
};


#endif // _nmrLSEISolverTest_h

