#ifndef _nmrControlOptimizer_h
#define _nmrControlOptimizer_h


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

//! enum used for solver results.
//! 0  Both equality and inequality constraints are compatible and have been satisfied.
//! 1  Equality constraints are contradictory. A generalized inverse solution of EX=F was used to minimize the residual vector length F-EX. In this sense, the solution is still meaningful.
//! 2  Inequality constraints are contradictory.
//! 3  Both equality and inequality constraints are contradictory.
//! 4  Input has a NaN or INF
enum STATUS {OK = 0, EQ_CONTRADICTION = 1, INEQ_CONTRADICTION = 2, BOTH_CONTRADICTION = 3, MALFORMED = 4};

/*!
  Eye Robot Jacobian typedef used for Eye Robot-specific calculations.
 */
typedef vctFixedSizeMatrix<double,6,5> EyeRobotJ_Type;

/*!
  Fixed-size vector typedef of size 10.
 */
typedef vctFixedSizeVector<double, 10> vctDouble10;

/*!
  Fixed-size vector typedef of size 12.
 */
typedef vctFixedSizeVector<double, 12> vctDouble12;

//! This is a container for constrained control optimizer. 
//! It provides high level functions to add common functionality.
//! Solves the LSI problem  arg min || C x - d ||, s.t. E x = f and A x >= b.
/*! \brief nmrControlOptimizer: A class that makes using the constraint control algorithm more efficient
 */
class nmrControlOptimizer : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_VERBOSE);

public:

	/*! Constructor 
	\param n Number of variables
	*/
    nmrControlOptimizer(int n);
    /*! Constructor 	
	*/
    nmrControlOptimizer(){};
	/*! Destructor 
	*/
    ~nmrControlOptimizer() {};

    //! Calls the solver and stores the result in dq. 
	/*! Solve
	\param dq Vector of current joint values
	\return STATUS enum that dictates whether the solver worked or there was a problem
	*/
    STATUS Solve(vctDoubleVec & dq);

	//! Returns the number of variables. 
	/*! GetNumVars	
	\return size_t Number of variables
	*/
	size_t GetNumVars();
    
    //! Clear all indices.
	/*! Reset
	*/
    void reset_indices();

	//! Allocate memory indicated by indices
	/*! allocate
	*/
    void allocate();

	//! Reserves space in the tableau 
	/*! ReserveSpace
	\param CRows Number of rows needed for the objective
	\param ARows Number of rows needed for the inequality constraint
	\param ERows Number of rows needed for the equality constraint
	\param num_slacks The number of slacks needed	
	*/
	void ReserveSpace(size_t CRows, size_t ARows, size_t ERows, size_t num_slacks);

    //! Returns a reference to space in the tableau for the objective. 
	/*! GetObjectiveSpace
	\param CRows Number of rows needed for the data
	\param SlackIndex The assigned slack index
	\param CData A reference to the data portion of the matrix
	\param CSlacks A reference to the slack portion of the matrix
	\param d A reference to the vector	
	*/
	void GetObjectiveSpace(size_t CRows, size_t SlackIndex, size_t num_slacks, vctDynamicMatrixRef<double> & CData, vctDynamicMatrixRef<double> & CSlacks, vctDynamicVectorRef<double> & d);

    //! Returns a reference to space in the tableau for the inequality constraint. 
	/*! GetIneqConstraintSpace
	\param ARows Number of rows needed for the data
	\param SlackIndex The assigned slack index
	\param AData A reference to the data portion of the matrix
	\param ASlacks A reference to the slack portion of the matrix
	\param b A reference to the vector	
	*/
	void GetIneqConstraintSpace(size_t ARows, size_t SlackIndex, size_t num_slacks, vctDynamicMatrixRef<double> & AData, vctDynamicMatrixRef<double> & ASlacks, vctDynamicVectorRef<double> & b);

    //! Returns a reference to space in the tableau for the equality constraint. 
	/*! GetEqConstraintSpace
	\param ERows Number of rows needed for the data
	\param SlackIndex The assigned slack index
	\param EData A reference to the data portion of the matrix
	\param ESlacks A reference to the slack portion of the matrix
	\param f A reference to the vector	
	*/
	void GetEqConstraintSpace(size_t ERows, size_t SlackIndex, size_t num_slacks, vctDynamicMatrixRef<double> & EData, vctDynamicMatrixRef<double> & ESlacks, vctDynamicVectorRef<double> & f);

	//! Increments the slack index. 
	/*! IncSlackIndex
	\param slacks Number of slacks to increment the index
	*/
	void IncSlackIndex(size_t slacks);

	//! Gets the slack index. 
	/*! GetSlackIndex	
	\return size_t The value of SlackIndex
	*/
	size_t GetSlackIndex();

	//! Gets the number of slacks. 
	/*! GetSlacks	
	\return size_t The number of slacks
	*/
	size_t GetSlacks();

	//! Gets the number of rows for the objective expression. 
	/*! GetObjectiveRows	
	\return size_t The number of objective rows
	*/
	size_t GetObjectiveRows();

	//! Gets the objective index. 
	/*! GetObjectiveIndex
	\return size_t The objective index
	*/
	size_t GetObjectiveIndex();

	//! Gets the number of rows for the inequality constraint. 
	/*! GetIneqConstraintRows
	\return size_t The number of inequality constraint rows
	*/
	size_t GetIneqConstraintRows();

	//! Gets the inequality constraint index. 
	/*! GetIneqConstraintIndex
	\return size_t The inequality constraint index
	*/
	size_t GetIneqConstraintIndex();

	//! Gets the number of rows for the equality constraint. 
	/*! GetEqConstraintRows
	\return size_t The number of rows for the equality constraint
	*/
	size_t GetEqConstraintRows();

	//! Gets the equality constraint index. 
	/*! GetEqConstraintIndex
	\return size_t The equality constraint index
	*/
	size_t GetEqConstraintIndex();

    //! Gets the objective matrix. 
	/*! GetObjectiveMatrix
	\return vctDoubleMat The objective matrix
	*/
	vctDoubleMat GetObjectiveMatrix();

	//! Gets the objective vector. 
	/*! GetObjectiveVector
	\return vctDoubleVec The objective vector
	*/
	vctDoubleVec GetObjectiveVector();

	//! Gets the inequality constraint matrix. 
	/*! GetIneqConstraintMatrix
	\return vctDoubleMat The inequality constraint matrix
	*/
	vctDoubleMat GetIneqConstraintMatrix();

	//! Gets the inequality constraint vector. 
	/*! GetIneqConstraintVector
	\return vctDoubleVec The inequality constraint vector
	*/
	vctDoubleVec GetIneqConstraintVector();

	//! Gets the equality constraint matrix. 
	/*! GetEqConstraintMatrix
	\return vctDoubleMat The equality constraint matrix
	*/
	vctDoubleMat GetEqConstraintMatrix();

	//! Gets the equality constraint vector. 
	/*! GetEqConstraintVector
	\return vctDoubleVec The equality constraint vector
	*/
	vctDoubleVec GetEqConstraintVector();
    
    //! Helper function for converting status enum to a string message.
	/*! GetStatusString
	\param status A STATUS enum variable
	\return A string representation of that STATUS
	*/
    std::string GetStatusString(STATUS status);

private: 

	//!Objective Matrix
    vctDoubleMat C;
    //!Objective Vector
	vctDoubleVec d;
	//!Inequality Matrix
	vctDoubleMat A;
	//!Inequality Vector
	vctDoubleVec b;
	//!Equality Matrix
	vctDoubleMat E;
	//!Equality Vector
	vctDoubleVec f;

	//!holds the solution
    nmrLSqLinSolutionDynamic lsiSolution;

    //!number of variables for incremental joint optimization (can be inferred from objective function).
    size_t NumVars; 

	//!Objective rows
	size_t CRows;
	//!Inequality rows
	size_t ARows;
	//!Equality rows
	size_t ERows;
	//!Number of slacks
	size_t Slacks;
	//!Objective Index
	size_t CIndex;
	//!Inequality Index
	size_t AIndex;
	//!Equality Index
	size_t EIndex;
	//!Slack Index
	size_t SlackIndex;

};

CMN_DECLARE_SERVICES_INSTANTIATION(nmrControlOptimizer);

#endif
