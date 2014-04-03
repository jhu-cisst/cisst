/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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


#include "nmrNonLinSolverTest.h"

#define TIMES 4

int nmrNonLinSolverTest::FunctionF(vctDynamicVectorRef<double> &x,
		vctDynamicVectorRef<double> &fvec, long int &Flag)
{   
	double d__1 = x[0]; // create a copy on stack. access is faster than x[0]!
	fvec[0] = 10. * (x[1] - d__1 * d__1); 
	fvec[1] = 1. - x[0];
	return 0;
}               

int nmrNonLinSolverTest::FunctionJ(vctDynamicVectorRef<double> &x,
		vctDynamicVectorRef<double> &fjac, long int &Flag)
{
	fjac[0] = -20. * x[0];
	fjac[1] = -1.;
	fjac[2] = 10.;
	fjac[3] = 0.;
	return 0;
}


void nmrNonLinSolverTest::TestLSNonLinSolver(void) {
	/*! data and results for matlab */
	/* Memory allocation */
	unsigned int j;
	nmrLSNonLinSolver sol(X, F);
	/* Solve */
	for (int i = 0; i < TIMES; i++) {
		X[0] = -1.2 * (double)(i+1); X[1] =  1.0 * (double)(i+1);
		F.SetAll(1.0);
		sol.Solve (callBack, X, F, 1e-6);
		for (j = 0; j < X.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(X(j), ResultX(j), 1.0e-5);
		for (j = 0; j < F.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(F(j), ResultF(j), 1.0e-5);
	}
}

void nmrNonLinSolverTest::TestLSNonLinJacobianSolver(void) {
	/*! data and results for matlab */
	/* Memory allocation */
	unsigned int j;
	nmrLSNonLinJacobianSolver solJ(X, F);
	/* Solve */
	for (int i = 0; i < TIMES; i++) {
		X[0] = -1.2 * (double)(i+1); X[1] =  1.0 * (double)(i+1);
		F.SetAll(1.0); J.SetAll(1.0);
		solJ.Solve (callBack, callBackJ, X, F, J, 1e-6);
		for (j = 0; j < X.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(X(j), ResultX(j), 1.0e-5);
		for (j = 0; j < F.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(F(j), ResultF(j), 1.0e-5);
	}
}

void nmrNonLinSolverTest::TestFnSolver(void) {
	/*! data and results for matlab */
	/* Memory allocation */
	unsigned int j;
	nmrFnSolver solFn(X);
	/* Solve */
	for (int i = 0; i < TIMES; i++) {
		X[0] = -1.2 * (double)(i+1); X[1] =  1.0 * (double)(i+1);
		F.SetAll(1.0);
		solFn.Solve (callBack, X, F, 1e-16);
		for (j = 0; j < X.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(X(j), ResultX(j), 1.0e-5);
		for (j = 0; j < F.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(F(j), ResultF(j), 1.0e-5);
	}
}

void nmrNonLinSolverTest::TestFnJacobianSolver(void) {
	/*! data and results for matlab */
	/* Memory allocation */
	unsigned int j;
	nmrFnJacobianSolver solFnJ(X);
	/* Solve */
	for (int i = 0; i < TIMES; i++) {
		X[0] = -1.2 * (double)(i+1); X[1] =  1.0 * (double)(i+1);
		F.SetAll(1.0); J.SetAll(1.0);
		solFnJ.Solve (callBack, callBackJ, X, F, J, 1e-6);
		for (j = 0; j < X.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(X(j), ResultX(j), 1.0e-5);
		for (j = 0; j < F.size(); j++) CPPUNIT_ASSERT_DOUBLES_EQUAL(F(j), ResultF(j), 1.0e-5);
	}
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrNonLinSolverTest);

