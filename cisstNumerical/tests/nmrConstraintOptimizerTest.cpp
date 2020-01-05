/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 Author(s):  Paul Wilkening
 Created on: 2014

 (C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

 --- begin cisst license - do not edit ---

 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.

 --- end cisst license ---
 */

#include "nmrConstraintOptimizerTest.h"

/*! Test constructor. */
void nmrConstraintOptimizerTest::TestConstructor()
{
    nmrConstraintOptimizer co(5);

    // number of variables should be 5
    CPPUNIT_ASSERT(co.GetNumVars() == 5);
}

/*! Test allocate commands */
void nmrConstraintOptimizerTest::TestAllocate(void)
{
    size_t NumVars = 5;
    nmrConstraintOptimizer co(NumVars);
    size_t CRows = 2, ARows = 5, ERows = 7, Slacks = 1;
    co.ResetIndices();
    co.ReserveSpace(CRows, ARows, ERows, Slacks);
    co.Allocate();

    // Matrices and vectors should be resized to match indices
    CPPUNIT_ASSERT_EQUAL(CRows + Slacks, co.GetObjectiveMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetObjectiveMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(CRows + Slacks, co.GetObjectiveVector().size());

    CPPUNIT_ASSERT_EQUAL(ARows+Slacks,   co.GetIneqConstraintMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetIneqConstraintMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(ARows+Slacks,   co.GetIneqConstraintVector().size());

    CPPUNIT_ASSERT_EQUAL(ERows,          co.GetEqConstraintMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetEqConstraintMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(ERows,          co.GetEqConstraintVector().size());

    size_t zero = 0;
    co.ResetIndices();
    co.Allocate();

    // Indices should be reset
    CPPUNIT_ASSERT_EQUAL(zero, co.GetObjectiveRows());
    CPPUNIT_ASSERT_EQUAL(zero, co.GetIneqConstraintRows());
    CPPUNIT_ASSERT_EQUAL(zero, co.GetEqConstraintRows());
    CPPUNIT_ASSERT_EQUAL(zero, co.GetSlackIndex());

    co.Allocate(CRows, NumVars+Slacks, ARows, NumVars+Slacks, ERows, NumVars+Slacks);

    // Testing other allocate command
    CPPUNIT_ASSERT_EQUAL(CRows, co.GetObjectiveMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetObjectiveMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(CRows, co.GetObjectiveVector().size());

    CPPUNIT_ASSERT_EQUAL(ARows, co.GetIneqConstraintMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetIneqConstraintMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(ARows, co.GetIneqConstraintVector().size());

    CPPUNIT_ASSERT_EQUAL(ERows, co.GetEqConstraintMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetEqConstraintMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(ERows, co.GetEqConstraintVector().size());
}

/*! Test GetRefs command */
void nmrConstraintOptimizerTest::TestSetRefs(void)
{
    size_t NumVars = 5;
    nmrConstraintOptimizer co(NumVars);
    size_t CRows = 2, ARows = 5, ERows = 7, Slacks = 1;
    co.ResetIndices();
    co.ReserveSpace(CRows, ARows, ERows, Slacks);
    co.Allocate();
    co.ResetIndices();
    vctDynamicMatrixRef<double> CRef, CSlackRef, ARef, ASlackRef, ERef;
    vctDynamicVectorRef<double> dRef, bRef, bSlack, fRef;
    co.SetRefs(CRows, ARows, ERows, Slacks,
               CRef, CSlackRef,
               dRef,
               ARef, ASlackRef,
               bRef, bSlack,
               ERef, fRef);

    // References should be the proper sizes
    CPPUNIT_ASSERT_EQUAL(CRows, CRef.rows());
    CPPUNIT_ASSERT_EQUAL(NumVars, CRef.cols());
    CPPUNIT_ASSERT_EQUAL(Slacks, CSlackRef.rows());
    CPPUNIT_ASSERT_EQUAL(Slacks, CSlackRef.cols());
    CPPUNIT_ASSERT_EQUAL(CRows, dRef.size());

    CPPUNIT_ASSERT_EQUAL(ARows, ARef.rows());
    CPPUNIT_ASSERT_EQUAL(NumVars, ARef.cols());
    CPPUNIT_ASSERT_EQUAL(Slacks, ASlackRef.rows());
    CPPUNIT_ASSERT_EQUAL(Slacks, ASlackRef.cols());
    CPPUNIT_ASSERT_EQUAL(ARows, bRef.size());
    CPPUNIT_ASSERT_EQUAL(Slacks, bSlack.size());

    CPPUNIT_ASSERT_EQUAL(ERows, ERef.rows());
    CPPUNIT_ASSERT_EQUAL(NumVars, ERef.cols());
    CPPUNIT_ASSERT_EQUAL(ERows, fRef.size());
}

/*! Test Solve */
void nmrConstraintOptimizerTest::TestSolve(void)
{
    // construct a optimization problem
    // obj: || <x1, x2> - <1,2> ||^2 +  || 2 * s ||^2
    // ineq: -x1 - x2 + s >= -0.5, x2 >= 0, -s >= -1
    // eq: x2 = 2

    size_t NumVars = 2;
    nmrConstraintOptimizer co(NumVars);
    size_t CRows = 2, ARows = 2, ERows = 1, Slacks = 1;

    co.ResetIndices();
    co.ReserveSpace(CRows, ARows, ERows, Slacks);
    co.Allocate();
    co.ResetIndices();

    vctDynamicMatrixRef<double> CRef, CSlackRef, ARef, ASlackRef, ERef;
    vctDynamicVectorRef<double> dRef, bRef, bSlackRef, fRef;
    co.SetRefs(CRows, ARows, ERows, Slacks,
               CRef, CSlackRef,
               dRef,
               ARef, ASlackRef,
               bRef, bSlackRef,
               ERef, fRef);

    // obj
    CRef.Diagonal().SetAll(1.0);
    CSlackRef.Diagonal().SetAll(2.0);
    dRef(0) = 1.0; dRef(1) = 2.0;

    // ineq
    ARef(0,0) = -1.0; ARef(0,1) = -1.0; // solves for >=
    ARef(1,1) = 1.0;
    ASlackRef.SetAll(-1.0);
    bRef(0) = -0.5;
    bSlackRef.SetAll(-1.0);

    // eq
    ERef(0,1) = 1;
    fRef.SetAll(2.0);

    std::cout << "C \n" << co.GetObjectiveMatrix() << std::endl;
    std::cout << "d \n" << co.GetObjectiveVector() << std::endl;
    std::cout << "A \n" << co.GetIneqConstraintMatrix() << std::endl;
    std::cout << "b \n" << co.GetIneqConstraintVector() << std::endl;
    std::cout << "E \n" << co.GetEqConstraintMatrix() << std::endl;
    std::cout << "f \n" << co.GetEqConstraintVector() << std::endl;

    // Make sure result is valid
    vctDoubleVec dq;
    CPPUNIT_ASSERT_EQUAL(nmrConstraintOptimizer::NMR_OK,co.Solve(dq));
    std::cout << "Solution " << dq << std::endl;
    CPPUNIT_ASSERT_EQUAL(true, std::abs(-1.0 - dq(0)) < 1E-5); // x1 = -1.0
    CPPUNIT_ASSERT_EQUAL(true, std::abs( 2.0 - dq(1)) < 1E-5); // x1 = 2.0
    CPPUNIT_ASSERT_EQUAL(true, std::abs( 0.5 - dq(2)) < 1E-5); // s = 0.5
}
