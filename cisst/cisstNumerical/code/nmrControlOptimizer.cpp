/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id: $
 
 Author(s):  Paul Wilkening
 Created on:
 
 (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 */

#include <cisstCommon/cmnGenericObject.h> 
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstNumerical/nmrLSqLin.h>
#include <cisstNumerical/nmrControlOptimizer.h>

CMN_IMPLEMENT_SERVICES(nmrControlOptimizer);

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

/*! Constructor 
\param n Number of variables
*/
nmrControlOptimizer::nmrControlOptimizer(int vars)
{
	NumVars = vars;
}

//! Calls the solver and stores the result in dq. 
/*! Solve
\param dq Vector of current joint values
\return STATUS enum that dictates whether the solver worked or there was a problem
*/
STATUS nmrControlOptimizer::Solve(vctDoubleVec &dq)
{
	CISSTNETLIB_INTEGER res;
	// if the sizes don't match for C,d
	if(C.rows() != d.size() || dq.size() != NumVars) { 
		return MALFORMED;
	}

	// if we don't have any constraints, solve
	if(A.size() == 0 && E.size() == 0) {
		if(C.cols() != dq.size()) {
			return MALFORMED;
		}
		lsiSolution.Allocate(C);
		res = nmrLSqLin(C, d, lsiSolution); 
		for (size_t i = 0; i < dq.size(); i++) {
			dq[i] = lsiSolution.GetX().Element(i);
		}
	}
	// if we have an inequality constraint, solve
	else if(A.size() > 0 && b.size() > 0 && E.size() == 0 && f.size() == 0) {
		if(A.cols() != dq.size() + Slacks || A.rows() != b.size()) {
			return MALFORMED;
		}
		lsiSolution.Allocate(C, A);
		res = nmrLSqLin(C, d, A, b, lsiSolution);
		for (size_t i = 0; i < dq.size(); i++) {
			dq[i] = lsiSolution.GetX().Element(i);
		}
	}
	else if(E.size() > 0 && f.size() > 0)
	{
		if(A.cols() != dq.size() + Slacks || A.rows() != b.size() || E.rows() != f.size()) {
			return MALFORMED;
		}
		lsiSolution.Allocate(C, A, E);		
		res = nmrLSqLin(C, d, E, f, A, b, lsiSolution); 
		dq.resize(NumVars);
		for (size_t i = 0; i < dq.size(); i++) {
			dq[i] = lsiSolution.GetX().Element(i);
		}
	}

	// res is our current status, check and return its value as a STATUS
	if(res == 0) {
		return OK;	
	}
	else if(res == 1){
		return EQ_CONTRADICTION;
	}
	else if(res == 2){
		return INEQ_CONTRADICTION;
	}
	else if(res == 3){
		return BOTH_CONTRADICTION;
	}
	else {
		return MALFORMED;
	}
}

//! Returns the number of variables. 
/*! GetNumVars	
\return size_t Number of variables
*/
size_t nmrControlOptimizer::GetNumVars()
{
	return NumVars;
}

//! Helper function for converting status enum to a string message
/*! GetStatusString
\param status A STATUS enum variable
\return A string representation of that STATUS
*/
std::string nmrControlOptimizer::GetStatusString(STATUS status)
{
	if(status == 0)
	{
		return "OK";
	}
	else if (status == 1)
	{
		return "EQ_CONTRADICTION";
	}
	else if (status == 2)
	{
		return "INEQ_CONTRADICTION";
	}
	else
	{
		return "MALFORMED";
	}
}

//! Clear all indices.
/*! Reset
*/
void nmrControlOptimizer::reset_indices()
{
	CIndex = 0;
	AIndex = 0;
	EIndex = 0;
	Slacks = 0;
	SlackIndex = 0;
}

//! Allocate memory indicated by indices
/*! allocate
*/
void nmrControlOptimizer::allocate()
{
	if(C.rows() != CIndex || C.cols() != NumVars+Slacks)
	{
		C.SetSize(CIndex,NumVars+Slacks,VCT_COL_MAJOR);
		C.SetAll(0);
	}
	if(d.size() != CIndex)
	{
		d.SetSize(CIndex);
		d.SetAll(0);
	}
	if(A.rows() != AIndex || A.cols() != NumVars+Slacks)
	{
		A.SetSize(AIndex,NumVars+Slacks,VCT_COL_MAJOR);
		A.SetAll(0);
	}
	if(b.size() != AIndex)
	{
		b.SetSize(AIndex);
		b.SetAll(0);
	}
	if(E.rows() != EIndex || E.cols() != NumVars+Slacks)
	{
		E.SetSize(EIndex,NumVars+Slacks,VCT_COL_MAJOR);
		E.SetAll(0);
	}
	if(f.size() != EIndex)
	{
		f.SetSize(EIndex);
		f.SetAll(0);
	}
}

//! Allocate memory indicated by input
/*! allocate
*/
void nmrControlOptimizer::allocate(size_t CRows, size_t CCols, size_t ARows = 0, size_t ACols = 0, size_t ERows = 0, size_t ECols = 0)
{
	if(C.rows() != CRows || C.cols() != CCols)
	{
		C.SetSize(CRows,CCols,VCT_COL_MAJOR);
		C.SetAll(0);
	}
	if(d.size() != CRows)
	{
		d.SetSize(CRows);
		d.SetAll(0);
	}
	if(A.rows() != ARows || A.cols() != ACols)
	{
		A.SetSize(ARows,ACols,VCT_COL_MAJOR);
		A.SetAll(0);
	}
	if(b.size() != ARows)
	{
		b.SetSize(ARows);
		b.SetAll(0);
	}
	if(E.rows() != ERows || E.cols() != ECols)
	{
		E.SetSize(ERows,ECols,VCT_COL_MAJOR);
		E.SetAll(0);
	}
	if(f.size() != ERows)
	{
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
void nmrControlOptimizer::ReserveSpace(size_t CRows_in, size_t ARows_in, size_t ERows_in, size_t num_slacks_in)
{
	CIndex += CRows_in;
	AIndex += ARows_in;
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
void nmrControlOptimizer::GetRefs(size_t CRows, size_t ARows, size_t ERows, size_t num_slacks, vctDynamicMatrixRef<double> & CData, vctDynamicMatrixRef<double> & CSlacks, vctDynamicVectorRef<double> & dData, vctDynamicMatrixRef<double> & AData, vctDynamicMatrixRef<double> & ASlacks, vctDynamicVectorRef<double> & bData, vctDynamicMatrixRef<double> & EData, vctDynamicMatrixRef<double> & ESlacks, vctDynamicVectorRef<double> & fData)
{
	//Objectives
	CData.SetRef(C, CIndex, 0, CRows, NumVars);
	if(num_slacks > 0)
	{
		CSlacks.SetRef(C, CIndex, NumVars+SlackIndex, CRows, num_slacks);
	}
	dData.SetRef(d, CIndex, CRows);
	CIndex += CRows;

	//Inequality Constraints
	AData.SetRef(A, AIndex, 0, ARows, NumVars);
	if(num_slacks > 0)
	{
		ASlacks.SetRef(A, AIndex, NumVars+SlackIndex, ARows, num_slacks);
		vctDynamicMatrixRef<double> SetZero;	
	}
	bData.SetRef(b, AIndex, ARows);
	AIndex += ARows;

	//Equality Constraints
	EData.SetRef(E, EIndex, 0, ERows, NumVars);
	if(num_slacks > 0)
	{
		ESlacks.SetRef(E, EIndex, NumVars+SlackIndex, ERows, num_slacks);
		vctDynamicMatrixRef<double> SetZero;	
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
size_t nmrControlOptimizer::GetObjectiveRows()
{
	return C.rows();
}

//! Gets the number of rows for the inequality constraint. 
/*! GetIneqConstraintRows
\return size_t The number of inequality constraint rows
*/
size_t nmrControlOptimizer::GetIneqConstraintRows()
{
	return A.rows();
}

//! Gets the number of rows for the equality constraint. 
/*! GetEqConstraintRows
\return size_t The number of rows for the equality constraint
*/
size_t nmrControlOptimizer::GetEqConstraintRows()
{
	return E.rows();
}

//! Gets the objective index. 
/*! GetObjectiveIndex
\return size_t The objective index
*/
size_t nmrControlOptimizer::GetObjectiveIndex()
{
	return CIndex;
}

//! Gets the inequality constraint index. 
/*! GetIneqConstraintIndex
\return size_t The inequality constraint index
*/
size_t nmrControlOptimizer::GetIneqConstraintIndex()
{
	return AIndex;
}

//! Gets the equality constraint index. 
/*! GetEqConstraintIndex
\return size_t The equality constraint index
*/
size_t nmrControlOptimizer::GetEqConstraintIndex()
{
	return EIndex;
}

//! Gets the number of slacks. 
/*! GetSlacks	
\return size_t The number of slacks
*/
size_t nmrControlOptimizer::GetSlacks()
{
	return Slacks;
}

//! Gets the objective matrix. 
/*! GetObjectiveMatrix
\return vctDoubleMat The objective matrix
*/
vctDoubleMat nmrControlOptimizer::GetObjectiveMatrix()
{
	return C;
}

//! Gets the objective vector. 
/*! GetObjectiveVector
\return vctDoubleVec The objective vector
*/
vctDoubleVec nmrControlOptimizer::GetObjectiveVector()
{
	return d;
}

//! Gets the inequality constraint matrix. 
/*! GetIneqConstraintMatrix
\return vctDoubleMat The inequality constraint matrix
*/
vctDoubleMat nmrControlOptimizer::GetIneqConstraintMatrix()
{
	return A;
}

//! Gets the inequality constraint vector. 
/*! GetIneqConstraintVector
\return vctDoubleVec The inequality constraint vector
*/
vctDoubleVec nmrControlOptimizer::GetIneqConstraintVector()
{
	return b;
}

//! Gets the equality constraint matrix. 
/*! GetEqConstraintMatrix
\return vctDoubleMat The equality constraint matrix
*/
vctDoubleMat nmrControlOptimizer::GetEqConstraintMatrix()
{
	return E;
}

//! Gets the equality constraint vector. 
/*! GetEqConstraintVector
\return vctDoubleVec The equality constraint vector
*/
vctDoubleVec nmrControlOptimizer::GetEqConstraintVector()
{
	return f;
}