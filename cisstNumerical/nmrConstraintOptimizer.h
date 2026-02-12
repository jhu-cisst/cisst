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

#ifndef _nmrConstraintOptimizer_h
#define _nmrConstraintOptimizer_h

#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstNumerical/nmrLSqLin.h>

// Always include last!
#include <cisstNumerical/nmrExport.h>

//! This is a container for constrained control optimizer.
//! It provides high level functions to add common functionality.
//! Solves the LSI problem  arg min || C x - d ||, s.t. E x = f and A x >= b.
/*! \brief nmrConstraintOptimizer: A class that makes using the constraint control algorithm more efficient
 */
class CISST_EXPORT nmrConstraintOptimizer
{

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

    // TODO: change name to NumSlacks
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

public:

    //! enum used for solver results.
    //! 0  Both equality and inequality constraints are compatible and have been satisfied.
    //! 1  Equality constraints are contradictory. A generalized inverse solution of EX=F was used to minimize the residual vector length F-EX. In this sense, the solution is still meaningful.
    //! 2  Inequality constraints are contradictory.
    //! 3  Both equality and inequality constraints are contradictory.
    //! 4  Input has a NaN or INF
    enum STATUS {NMR_OK, NMR_EQ_CONTRADICTION, NMR_INEQ_CONTRADICTION, NMR_BOTH_CONTRADICTION, NMR_MALFORMED, NMR_EMPTY};

    /*! Constructor
     */
    nmrConstraintOptimizer() {}

    /*! Destructor
     */
    ~nmrConstraintOptimizer() {}

    /*! Initialize control optimizer
      \param n Number of variables
    */
    nmrConstraintOptimizer(const size_t n);

    //! Calls the solver and stores the result in dq.
    /*! Solve
      \param dq Vector of current joint values
      \return STATUS enum that dictates whether the solver worked or there was a problem
    */
    STATUS Solve(vctDoubleVec & dq);

    //! Clear all indices.
    /*! Reset
     */
    void ResetIndices(void);

    //! Allocate memory indicated by indices
    /*! allocate
     */
    void Allocate(void);

    //! Allocate memory indicated by input
    /*! allocate
     */
    void Allocate(const size_t CRows, const size_t CCols, const size_t ARows, const size_t ACols, const size_t ERows, const size_t ECols);

    //! Reserves space in the tableau
    /*! ReserveSpace
      \param CRows Number of rows needed for the objective
      \param ARows Number of rows needed for the inequality constraint
      \param ERows Number of rows needed for the equality constraint
      \param num_slacks The number of slacks needed
    */
    void ReserveSpace(const size_t CRows, const size_t ARows, const size_t ERows, const size_t num_slacks);

    //! Returns references to spaces in the tableau.
    /*! GetObjectiveSpace
      \param CRows Number of rows needed for the objective data
      \param ARows Number of rows needed for the inequality constraint data
      \param ERows Number of rows needed for the equality constraint data
      \param SlackRows The number of slacks
      \param CData A reference to the data portion of the objective matrix
      \param CSlacks A reference to the slack portion of the objective matrix
      \param dData A reference to the objective vector
      \param AData A reference to the data portion of the inequality constraint matrix
      \param bData A reference to the inequality constraint vector
      \param bSlacks A reference to the inequality constraint vector for slack portion (slack limit)
      \param EData A reference to the data portion of the equality constraint matrix
      \param fData A reference to the equality constraint vector
    */
    void SetRefs(const size_t CRows, const size_t ARows, const size_t ERows, const size_t NumSlacks,
                  vctDynamicMatrixRef<double> & CData, vctDynamicMatrixRef<double> & CSlacks, 
                  vctDynamicVectorRef<double> & dData,
                  vctDynamicMatrixRef<double> & AData, vctDynamicMatrixRef<double> & ASlacks,  
                  vctDynamicVectorRef<double> & bData, vctDynamicVectorRef<double> & bSlacks, 
                  vctDynamicMatrixRef<double> & EData, vctDynamicVectorRef<double> & fData);

    //! Returns the number of variables.
    /*! GetNumVars
      \return size_t Number of variables
    */
    size_t GetNumVars(void) const;

    //! Gets the current slack index.
    /*! GetSlackIndex
      \return size_t The slack index
    */
    size_t GetSlackIndex(void) const;

    //! Gets the number of slacks.
    /*! GetSlacks
      \return size_t The number of slacks
    */
    size_t GetSlacks(void) const;

    //! Gets the number of rows for the objective expression.
    /*! GetObjectiveRows
      \return size_t The number of objective rows
    */
    size_t GetObjectiveRows(void) const;

    //! Gets the objective index.
    /*! GetObjectiveIndex
      \return size_t The objective index
    */
    size_t GetObjectiveIndex(void) const;

    //! Gets the number of rows for the inequality constraint.
    /*! GetIneqConstraintRows
      \return size_t The number of inequality constraint rows
    */
    size_t GetIneqConstraintRows(void) const;

    //! Gets the inequality constraint index.
    /*! GetIneqConstraintIndex
      \return size_t The inequality constraint index
    */
    size_t GetIneqConstraintIndex(void) const;

    //! Gets the number of rows for the equality constraint.
    /*! GetEqConstraintRows
      \return size_t The number of rows for the equality constraint
    */
    size_t GetEqConstraintRows(void) const;

    //! Gets the equality constraint index.
    /*! GetEqConstraintIndex
      \return size_t The equality constraint index
    */
    size_t GetEqConstraintIndex(void) const;

    //! Gets the objective matrix.
    /*! GetObjectiveMatrix
      \return vctDoubleMat The objective matrix
    */
    const vctDoubleMat & GetObjectiveMatrix(void) const;

    //! Gets the objective vector.
    /*! GetObjectiveVector
      \return vctDoubleVec The objective vector
    */
    const vctDoubleVec & GetObjectiveVector(void) const;

    //! Gets the inequality constraint matrix.
    /*! GetIneqConstraintMatrix
      \return vctDoubleMat The inequality constraint matrix
    */
    const vctDoubleMat & GetIneqConstraintMatrix(void) const;

    //! Gets the inequality constraint vector.
    /*! GetIneqConstraintVector
      \return vctDoubleVec The inequality constraint vector
    */
    const vctDoubleVec & GetIneqConstraintVector(void) const;

    //! Gets the equality constraint matrix.
    /*! GetEqConstraintMatrix
      \return vctDoubleMat The equality constraint matrix
    */
    const vctDoubleMat & GetEqConstraintMatrix(void) const;

    //! Gets the equality constraint vector.
    /*! GetEqConstraintVector
      \return vctDoubleVec The equality constraint vector
    */
    const vctDoubleVec & GetEqConstraintVector(void) const;

    //! Helper function for converting status enum to a string message.
    /*! GetStatusString
      \param status A STATUS enum variable
      \return A string representation of that STATUS
    */
    const std::string GetStatusString(STATUS status) const;

};

#endif // _nmrConstraintOptimizer_h
