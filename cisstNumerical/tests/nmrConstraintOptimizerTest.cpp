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
    CPPUNIT_ASSERT_EQUAL(CRows, co.GetObjectiveMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetObjectiveMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(CRows, co.GetObjectiveVector().size());

    CPPUNIT_ASSERT_EQUAL(ARows+Slacks, co.GetIneqConstraintMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetIneqConstraintMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(ARows+Slacks, co.GetIneqConstraintVector().size());

    CPPUNIT_ASSERT_EQUAL(ERows, co.GetEqConstraintMatrix().rows());
    CPPUNIT_ASSERT_EQUAL(NumVars+Slacks, co.GetEqConstraintMatrix().cols());
    CPPUNIT_ASSERT_EQUAL(ERows, co.GetEqConstraintVector().size());

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
    vctDoubleVec SlackLimits(1);
    SlackLimits[0] = 50;
    co.ResetIndices();
    co.ReserveSpace(CRows, ARows, ERows, Slacks);
    co.Allocate();
    co.ResetIndices();
    vctDynamicMatrixRef<double> CRef, CSlackRef, ARef, ASlackRef, ERef, ESlackRef;
    vctDynamicVectorRef<double> dRef, bRef, fRef;
    co.SetRefs(CRows, ARows, ERows, Slacks, SlackLimits, CRef, CSlackRef, dRef, ARef, ASlackRef, bRef, ERef, ESlackRef, fRef);

    // References should be the proper sizes
    CPPUNIT_ASSERT_EQUAL(CRows, CRef.rows());
    CPPUNIT_ASSERT_EQUAL(CRows, CSlackRef.rows());
    CPPUNIT_ASSERT_EQUAL(NumVars, CRef.cols());
    CPPUNIT_ASSERT_EQUAL(Slacks, CSlackRef.cols());
    CPPUNIT_ASSERT_EQUAL(CRows, dRef.size());

    CPPUNIT_ASSERT_EQUAL(ARows, ARef.rows());
    CPPUNIT_ASSERT_EQUAL(ARows, ASlackRef.rows());
    CPPUNIT_ASSERT_EQUAL(NumVars, ARef.cols());
    CPPUNIT_ASSERT_EQUAL(Slacks, ASlackRef.cols());
    CPPUNIT_ASSERT_EQUAL(ARows, bRef.size());

    CPPUNIT_ASSERT_EQUAL(ERows, ERef.rows());
    CPPUNIT_ASSERT_EQUAL(ERows, ESlackRef.rows());
    CPPUNIT_ASSERT_EQUAL(NumVars, ERef.cols());
    CPPUNIT_ASSERT_EQUAL(Slacks, ESlackRef.cols());
    CPPUNIT_ASSERT_EQUAL(ERows, fRef.size());
}

/*! Test Solve */
void nmrConstraintOptimizerTest::TestSolve(void)
{
    size_t NumVars = 5;
    nmrConstraintOptimizer co(NumVars);
    size_t CRows = 2, ARows = 5, ERows = 7, Slacks = 1;
    vctDoubleVec SlackLimits(1);
    SlackLimits[0] = 50;
    co.ResetIndices();
    co.ReserveSpace(CRows, ARows, ERows, Slacks);
    co.Allocate();
    co.ResetIndices();
    vctDynamicMatrixRef<double> CRef, CSlackRef, ARef, ASlackRef, ERef, ESlackRef;
    vctDynamicVectorRef<double> dRef, bRef, fRef;
    co.SetRefs(CRows, ARows, ERows, Slacks, SlackLimits, CRef, CSlackRef, dRef, ARef, ASlackRef, bRef, ERef, ESlackRef, fRef);
    CRef.SetAll(1.0);
    CSlackRef.SetAll(1.0);
    dRef.SetAll(1.0);
    ARef.SetAll(1.0);
    ASlackRef.SetAll(1.0);
    bRef.SetAll(1.0);
    ERef.SetAll(1.0);
    ESlackRef.SetAll(1.0);
    fRef.SetAll(1.0);

    // Make sure result is valid
    vctDoubleVec dq(NumVars);
    CPPUNIT_ASSERT_EQUAL(co.Solve(dq), nmrConstraintOptimizer::NMR_OK);
}
