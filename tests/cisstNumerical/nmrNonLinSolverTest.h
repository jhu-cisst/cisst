/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrNonLinSolverTest.h,v 1.6 2007/04/26 20:12:05 anton Exp $
  
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


#ifndef _nmrNonLinSolverTest_h
#define _nmrNonLinSolverTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrLSNonLinSolver.h>
#include <cisstNumerical/nmrLSNonLinJacobianSolver.h>
#include <cisstNumerical/nmrFnSolver.h>
#include <cisstNumerical/nmrFnJacobianSolver.h>

class nmrNonLinSolverTest  : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrNonLinSolverTest);
    CPPUNIT_TEST(TestLSNonLinSolver);
    CPPUNIT_TEST(TestLSNonLinJacobianSolver);
    CPPUNIT_TEST(TestFnSolver);
    CPPUNIT_TEST(TestFnJacobianSolver);
    CPPUNIT_TEST_SUITE_END();
protected:
    vctDynamicVector<double> X;
    vctDynamicVector<double> F;
    vctDynamicVector<double> J;
    vctDynamicVector<double> ResultX;
    vctDynamicVector<double> ResultF;
    nmrCallBackFunctionF<nmrUNIQUE_IDENTIFIER_LINE, nmrNonLinSolverTest> callBack;
    nmrCallBackFunctionJ<nmrUNIQUE_IDENTIFIER_LINE, nmrNonLinSolverTest> callBackJ;


public:

    void setUp() {
	    X.SetSize(2);
	    X[0] = -1.2; X[1]= 1.0;
	    F.SetSize(2);
	    J.SetSize(4); 
	    ResultX.SetSize(2);
	    ResultX.SetAll(1.0);
	    ResultF.SetSize(2);
	    ResultF.SetAll(0.0);
	    callBack.Set(this, &nmrNonLinSolverTest::FunctionF);
	    callBackJ.Set(this, &nmrNonLinSolverTest::FunctionJ);
    }

    void tearDown()
    {}

    int FunctionF(vctDynamicVectorRef<double> &x, vctDynamicVectorRef<double> &fvec, long int &Flag);

    int FunctionJ(vctDynamicVectorRef<double> &x, vctDynamicVectorRef<double> &fjac, long int &Flag);

    void TestLSNonLinSolver(void);
    void TestLSNonLinJacobianSolver(void);
    void TestFnSolver(void);
    void TestFnJacobianSolver(void);
};


#endif // _nmrNonLinSolverTest_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrNonLinSolverTest.h,v $
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
// Revision 1.2  2005/08/25 16:55:35  anton
// cisstNumerical tests: Removed #include of cisstXyz.h to avoid useless
// dependencies and long compilations.
//
// Revision 1.1  2005/08/17 20:05:01  kapoor
// cisstNumerical: Added LSEI (a more recent version of Hansons' Code)
//
// ****************************************************************************
