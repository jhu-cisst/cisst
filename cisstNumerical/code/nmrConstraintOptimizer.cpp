/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 Author(s):  Paul Wilkening
 Created on: 2014

 (C) Copyright 2014-2015 Johns Hopkins University (JHU), All Rights Reserved.

 --- begin cisst license - do not edit ---

 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.

 --- end cisst license ---
 */

#include <cisstNumerical/nmrConstraintOptimizer.h>

//! This is a container for constrained control optimizer.
//! It provides high level functions to add common functionality.
//! Solves the LSI problem  arg min || C x - d ||, s.t. E x = f and A x >= b.
//! Example:
//! - 0. Create object
//! - 1. Build objective (sets number of joints)
//! -   1.1 AddObjectiveCartMotion
//! -   1.2 AddObjectiveGeneral
//! - 2. Build constraints
//! - 3. SolvE->
/*! \brief nmrControlOptimizer: A class that makes using the constraint control algorithm more efficient
 */

/*! Initialize control optimizer
  \param n Number of variables
*/
nmrConstraintOptimizer::nmrConstraintOptimizer(const size_t n)
{
    Slacks = 0;
    NumVars = n;
    ResetIndices();
}

//! Calls the solver and stores the result in dq.
/*! Solve
  \param dq Vector of current joint values
  \return STATUS enum that dictates whether the solver worked or there was a problem
*/
nmrConstraintOptimizer::STATUS nmrConstraintOptimizer::Solve(vctDoubleVec &dq)
{
    CISSTNETLIB_INTEGER res;

    // make sure input is the correct size
    dq.SetSize(NumVars+Slacks);

    // if we don't see an objective
    if (C.rows() == 0 || d.size() == 0) {
        dq.SetAll(0);
        return NMR_EMPTY;
    }

    // if the sizes don't match for C,d
    if (C.rows() != d.size() || dq.size() != NumVars+Slacks) {
        return NMR_MALFORMED;
    }

    // if we don't have any constraints, solve
    if (A.size() == 0 && E.size() == 0) {
        if (C.cols() != dq.size() + Slacks) {
            return NMR_MALFORMED;
        }
        lsiSolution.Allocate(C);
        res = nmrLSqLin(C, d, lsiSolution);
        for (size_t i = 0; i < dq.size(); i++) {
            dq[i] = lsiSolution.GetX().Element(i);
        }
    }
    // if we have an inequality constraint, solve
    else if (A.size() > 0 && b.size() > 0 && E.size() == 0 && f.size() == 0) {
        if (A.cols() != dq.size() || A.rows() != b.size()) {
            return NMR_INEQ_CONTRADICTION;
        }
        lsiSolution.Allocate(C, A);
        res = nmrLSqLin(C, d, A, b, lsiSolution);
        for (size_t i = 0; i < dq.size(); i++) {
            dq[i] = lsiSolution.GetX().Element(i);
        }
    }
    else if (E.size() > 0 && f.size() > 0) {
        if (A.cols() != dq.size() || A.rows() != b.size()) {
            if (E.cols() != dq.size() || E.rows() != f.size()) {
                return NMR_BOTH_CONTRADICTION;
            }
            else {
                return NMR_INEQ_CONTRADICTION;
            }
        } else if (E.cols() != dq.size() || E.rows() != f.size()) {
            return NMR_EQ_CONTRADICTION;
        }
        lsiSolution.Allocate(C, E, A);
        res = nmrLSqLin(C, d, E, f, A, b, lsiSolution);
        dq.resize(NumVars);
        for (size_t i = 0; i < dq.size(); i++) {
            dq[i] = lsiSolution.GetX().Element(i);
        }
    }
    else {
        res = NMR_MALFORMED;
    }

    // res is our current status, check and return its value as a STATUS
    return (STATUS)res;
}

//! Returns the number of variables.
/*! GetNumVars
  \return size_t Number of variables
*/
size_t nmrConstraintOptimizer::GetNumVars(void) const
{
    return NumVars;
}

//! Helper function for converting status enum to a string message
/*! GetStatusString
  \param status A STATUS enum variable
  \return A string representation of that STATUS
*/
const std::string nmrConstraintOptimizer::GetStatusString(STATUS status) const
{
    switch(status) {
    case NMR_OK:
        return "OK";
        break;
    case NMR_EQ_CONTRADICTION:
        return "EQ_CONTRADICTION";
        break;
    case NMR_INEQ_CONTRADICTION:
        return "INEQ_CONTRADICTION";
        break;
    case NMR_BOTH_CONTRADICTION:
        return "BOTH_CONTRADICTION";
        break;
    default:
        return "MALFORMED";
        break;
    }
}

//! Clear all indices.
/*! Reset
 */
void nmrConstraintOptimizer::ResetIndices(void)
{
    CIndex = 0;
    AIndex = 0;
    EIndex = 0;
    SlackIndex = 0;
}

//! Allocate memory indicated by indices
/*! allocate
 */
void nmrConstraintOptimizer::Allocate(void)
{
    if (C.rows() != CIndex || C.cols() != NumVars+Slacks) {
        C.SetSize(CIndex,NumVars+Slacks,VCT_COL_MAJOR);
        C.SetAll(0);
    }
    if (d.size() != CIndex) {
        d.SetSize(CIndex);
        d.SetAll(0);
    }
    if (A.rows() != AIndex || A.cols() != NumVars+Slacks) {
        A.SetSize(AIndex,NumVars+Slacks,VCT_COL_MAJOR);
        A.SetAll(0);
    }
    if (b.size() != AIndex) {
        b.SetSize(AIndex);
        b.SetAll(0);
    }
    if (E.rows() != EIndex || E.cols() != NumVars+Slacks) {
        E.SetSize(EIndex,NumVars+Slacks,VCT_COL_MAJOR);
        E.SetAll(0);
    }
    if (f.size() != EIndex) {
        f.SetSize(EIndex);
        f.SetAll(0);
    }
}

//! Allocate memory indicated by input
/*! allocate
 */
void nmrConstraintOptimizer::Allocate(const size_t CRows, const size_t CCols, const size_t ARows = 0, const size_t ACols = 0, const size_t ERows = 0, const size_t ECols = 0)
{
    if (C.rows() != CRows || C.cols() != CCols) {
        C.SetSize(CRows,CCols,VCT_COL_MAJOR);
        C.SetAll(0);
    }
    if (d.size() != CRows) {
        d.SetSize(CRows);
        d.SetAll(0);
    }
    if (A.rows() != ARows || A.cols() != ACols) {
        A.SetSize(ARows,ACols,VCT_COL_MAJOR);
        A.SetAll(0);
    }
    if (b.size() != ARows) {
        b.SetSize(ARows);
        b.SetAll(0);
    }
    if (E.rows() != ERows || E.cols() != ECols) {
        E.SetSize(ERows,ECols,VCT_COL_MAJOR);
        E.SetAll(0);
    }
    if (f.size() != ERows) {
        f.SetSize(ERows);
        f.SetAll(0);
    }
}

//! Reserves space in the tableau
/*! ReserveSpace
  \param CRows Number of rows needed for the objective
  \param ARows Number of rows needed for the inequality constraint
  \param ERows Number of rows needed for the equality constraint
  \param num_slacks The number of slacks needed
*/
void nmrConstraintOptimizer::ReserveSpace(const size_t CRows_in, const size_t ARows_in, const size_t ERows_in, const size_t num_slacks_in)
{
    CIndex += CRows_in;
    AIndex += ARows_in+num_slacks_in; // We're going to use the extra space to set slack limits
    EIndex += ERows_in;
    Slacks += num_slacks_in;
}

//! Returns references to spaces in the tableau.
/*! GetObjectiveSpace
  \param CRows Number of rows needed for the objective data
  \param ARows Number of rows needed for the inequality constraint data
  \param ERows Number of rows needed for the equality constraint data
  \param SlackIndex_in The assigned slack index
  \param num_slacks The number of slacks
  \param CData A reference to the data portion of the objective matrix
  \param CSlacks A reference to the slack portion of the objective matrix
  \param d A reference to the objective vector
  \param AData A reference to the data portion of the inequality constraint matrix
  \param ASlacks A reference to the slack portion of the inequality constraint matrix
  \param b A reference to the inequality constraint vector
  \param EData A reference to the data portion of the equality constraint matrix
  \param ESlacks A reference to the slack portion of the equality constraint matrix
  \param f A reference to the equality constraint vector
*/
void nmrConstraintOptimizer::SetRefs(const size_t CRows, const size_t ARows, const size_t ERows, const size_t num_slacks,
                                     const vctDoubleVec & limits, vctDynamicMatrixRef<double> & CData,
                                     vctDynamicMatrixRef<double> & CSlacks, vctDynamicVectorRef<double> & dData,
                                     vctDynamicMatrixRef<double> & AData, vctDynamicMatrixRef<double> & ASlacks,
                                     vctDynamicVectorRef<double> & bData, vctDynamicMatrixRef<double> & EData,
                                     vctDynamicMatrixRef<double> & ESlacks, vctDynamicVectorRef<double> & fData)
{
    //Objectives
    CData.SetRef(C, CIndex, 0, CRows, NumVars);
    if (num_slacks > 0) {
        CSlacks.SetRef(C, CIndex, NumVars+SlackIndex, CRows, num_slacks);
    }
    dData.SetRef(d, CIndex, CRows);
    CIndex += CRows;

    //Inequality Constraints
    AData.SetRef(A, AIndex, 0, ARows, NumVars);
    if (num_slacks > 0) {
        ASlacks.SetRef(A, AIndex, NumVars+SlackIndex, ARows, num_slacks);
    }
    bData.SetRef(b, AIndex, ARows);
    AIndex += ARows;

    //Add slack limits constraint
    vctDynamicMatrixRef<double> Identity;
    Identity.SetRef(A,AIndex,NumVars+SlackIndex,num_slacks,num_slacks);
    vctDynamicVectorRef<double> LimitRef;
    LimitRef.SetRef(b,AIndex,num_slacks);
    for (size_t i = 0; i < num_slacks; i++) {
        Identity[i][i] = 1;
        LimitRef[i] = -limits[i];
    }

    //Equality Constraints
    EData.SetRef(E, EIndex, 0, ERows, NumVars);
    if (num_slacks > 0) {
        ESlacks.SetRef(E, EIndex, NumVars+SlackIndex, ERows, num_slacks);
    }
    fData.SetRef(f, EIndex, ERows);
    EIndex += ERows;

    //Slacks
    SlackIndex += num_slacks;
}

//! Gets the number of rows for the objective expression.
/*! GetObjectiveRows
  \return size_t The number of objective rows
*/
size_t nmrConstraintOptimizer::GetObjectiveRows(void) const
{
    return C.rows();
}

//! Gets the number of rows for the inequality constraint.
/*! GetIneqConstraintRows
  \return size_t The number of inequality constraint rows
*/
size_t nmrConstraintOptimizer::GetIneqConstraintRows(void) const
{
    return A.rows();
}

//! Gets the number of rows for the equality constraint.
/*! GetEqConstraintRows
  \return size_t The number of rows for the equality constraint
*/
size_t nmrConstraintOptimizer::GetEqConstraintRows(void) const
{
    return E.rows();
}

//! Gets the objective index.
/*! GetObjectiveIndex
  \return size_t The objective index
*/
size_t nmrConstraintOptimizer::GetObjectiveIndex(void) const
{
    return CIndex;
}

//! Gets the inequality constraint index.
/*! GetIneqConstraintIndex
  \return size_t The inequality constraint index
*/
size_t nmrConstraintOptimizer::GetIneqConstraintIndex(void) const
{
    return AIndex;
}

//! Gets the equality constraint index.
/*! GetEqConstraintIndex
  \return size_t The equality constraint index
*/
size_t nmrConstraintOptimizer::GetEqConstraintIndex(void) const
{
    return EIndex;
}

size_t nmrConstraintOptimizer::GetSlackIndex(void) const
{
    return SlackIndex;
}

//! Gets the number of slacks.
/*! GetSlacks
  \return size_t The number of slacks
*/
size_t nmrConstraintOptimizer::GetSlacks(void) const
{
    return Slacks;
}

//! Gets the objective matrix.
/*! GetObjectiveMatrix
  \return vctDoubleMat The objective matrix
*/
const vctDoubleMat & nmrConstraintOptimizer::GetObjectiveMatrix(void) const
{
    return C;
}

//! Gets the objective vector.
/*! GetObjectiveVector
  \return vctDoubleVec The objective vector
*/
const vctDoubleVec & nmrConstraintOptimizer::GetObjectiveVector(void) const
{
    return d;
}

//! Gets the inequality constraint matrix.
/*! GetIneqConstraintMatrix
  \return vctDoubleMat The inequality constraint matrix
*/
const vctDoubleMat & nmrConstraintOptimizer::GetIneqConstraintMatrix(void) const
{
    return A;
}

//! Gets the inequality constraint vector.
/*! GetIneqConstraintVector
  \return vctDoubleVec The inequality constraint vector
*/
const vctDoubleVec & nmrConstraintOptimizer::GetIneqConstraintVector(void) const
{
    return b;
}

//! Gets the equality constraint matrix.
/*! GetEqConstraintMatrix
  \return vctDoubleMat The equality constraint matrix
*/
const vctDoubleMat & nmrConstraintOptimizer::GetEqConstraintMatrix(void) const
{
    return E;
}

//! Gets the equality constraint vector.
/*! GetEqConstraintVector
  \return vctDoubleVec The equality constraint vector
*/
const vctDoubleVec & nmrConstraintOptimizer::GetEqConstraintVector(void) const
{
    return f;
}
