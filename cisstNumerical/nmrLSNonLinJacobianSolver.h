/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ankur Kapoor
  Created on: 2004-10-26

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrLSNonLinJacobianSolver
*/


#ifndef _nmrLSNonLinJacobianSolver_h
#define _nmrLSNonLinJacobianSolver_h


#include <cisstVector/vctDynamicMatrix.h>

#include <cnetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm LSNonLin: Non Linear Least Squares by Levenberg Marquardt method
  The purpose of this is to minimize the sum of the squares of
       M nonlinear functions in N variables by a modification of the
       levenberg-marquardt algorithm. The user must provide a
       subroutine which calculates the functions. The user must provide a
       subroutine which calculates the functions and the jacobian

  Rather than compute the value f(x) (the sum of squares), LSNonLin requires 
  the user-defined function to compute the vector-valued function. 
       \f$ \textbf{F}(x) = [f_1(x), f_2(x), f_3(x)]^{t} \f$
  Then, in vector terms, you can restate this optimization problem as 
       \f$ \mbox{min} \frac{1}{2} \| \textbf{F}(x) \|_2^2 = \frac{1}{2} sum_i f_i(x)^2 \f$
  where \f$x\f$ is a vector and \f$\textbf{F}(x)\f$ is a function that 
  returns a vector value.

  The data members of this class are:

  - M: The number of nonlinear functions  M >= 0.
  - N: The number of variables.  N >= 0.
  - Tolerance: A nonnegative input variable. Termination occurs
           when the algorithm estimates either that the relative
           error in the sum of squares is at most Tolerance or that
           the relative error between X and the solution is at
           most Tolerance. (Default set to 1.0e-6).
  - IWork: An integer working vector of length N.
  - Work: A working vector of length M*N+5*N+M.
  - LWork: Length of working array.
  - Info: = info is an integer output variable. if the user has
            terminated execution, info is set to the (negative)
            value of Flag. see description of FunctionPointer.
	    otherwise, info is set as follows.
   
            info = 0  improper input parameters.
   
            info = 1  algorithm estimates that the relative error
                      in the sum of squares is at most tol.
   
            info = 2  algorithm estimates that the relative error
                      between x and the solution is at most tol.
   
            info = 3  conditions for info = 1 and info = 2 both hold.
   
            info = 4  fvec is orthogonal to the columns of the
                      jacobian to machine precision.
   
            info = 5  number of calls to fcn with Flag = 1 has 
	              reached or exceeded 200*(n+1).
   
            info = 6  tol is too small. no further reduction in
                      the sum of squares is possible.
   
            info = 7  tol is too small. no further improvement in
                      the approximate solution x is possible

  The input/output from this class is:
  - X: On entry, the inital estimate of solution vector.
       On exit, final estimate of solution vector.
  - RNorm: If used, the value of the squared 2-norm of the residual at 
       final value of x
  - callBack: Is object of type nmrCallBackLSNonLinSolver used to supply the user method.
        The user method which belongs to a user defined class 'Cfoo' has the
        following definition
          int Cfoo::Mbar (vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &X, vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &F, 
	      vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &J, CISSTNETLIB_INTEGER &Flag)
	if Flag = 1 calculate the functions at X and
	return this vector in F. do not alter J.
	if Flag = 2 calculate the jacobian at X and
	return this matrix in J. do not alter F.
	the value of Flag should not be changed by Mbar unless
        the user wants to terminate execution of Solver.
        in this case set Flag to a negative integer.


        The solver calls this method when needed to obtain values for F for
        a given variable values X.

        The following constructor can be used to contruct the nmrCallBackLSNonLinSolver object
        required to be passed to the Solve() method. This object needs to be created
        only once per given set of nonlinear functions to be minized.
            nmrCallBackLSNonLinSolver<nmrUNIQUE_IDENTIFIER_LINE, Cfoo> callBackObject(this, &Cfoo::Mbar);

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrLSNonLinJacobianSolver {
	// we have this class so that we reserve memory only one
	// would help if svd of a same size matrix (or a matrix)
	// that doesnt change much is desired.

protected:
	CISSTNETLIB_INTEGER M;
	CISSTNETLIB_INTEGER N;
	CISSTNETLIB_INTEGER Ldfjac;
	CISSTNETLIB_DOUBLE Tolerance;
	CISSTNETLIB_INTEGER Info;
	CISSTNETLIB_INTEGER Lwork;
	vctDynamicVector<CISSTNETLIB_INTEGER> IWork;
	vctDynamicVector<CISSTNETLIB_DOUBLE> Work;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLSNonLinJacobianSolver(void)
    {
        Allocate(0, 0);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of nonlinear functions.
      \param n Number of variables
      This order will be used for the output as well.
    */
	nmrLSNonLinJacobianSolver(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n)
    {
        Allocate(m, n);
    }

    
    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension and storage order. */
    //@{
	nmrLSNonLinJacobianSolver(vctDynamicVector<CISSTNETLIB_DOUBLE> &X, vctDynamicVector<CISSTNETLIB_DOUBLE> &F) {
        Allocate(X, F);
    }
    //@}


    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param m Number of nonlinear functions
      \param n Number of variables
    */
	inline void Allocate(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n) {
        M = m;
        N = n;
	Lwork = M * N + 5 * N + M;
	Ldfjac = M;
	Tolerance = 1e-6;
        Work.SetSize(Lwork);
	IWork.SetSize(N);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    //@{
    inline void Allocate(vctDynamicVector<CISSTNETLIB_DOUBLE> &X, vctDynamicVector<CISSTNETLIB_DOUBLE> &F) {
        Allocate(X.size(), F.size());
    }
    //@}


    /*! This computes the solves nonlinear least squares problem invloving
        M functions in N variables.
	On input, X contains the starting point,
	On output, X contains the final estimate, F contrains the final value
    */

    //@{
    template <int __instanceLineF, 
              class __elementTypeF, 
              int __instanceLineJ, 
              class __elementTypeJ>
    inline void Solve(nmrCallBackFunctionF<__instanceLineF, __elementTypeF> &callBackF,
                      nmrCallBackFunctionJ<__instanceLineJ, __elementTypeJ> &callBackJ,
                      vctDynamicVector<CISSTNETLIB_DOUBLE> &X,
                      vctDynamicVector<CISSTNETLIB_DOUBLE> &F, 
                      vctDynamicVector<CISSTNETLIB_DOUBLE> &J, 
                      CISSTNETLIB_DOUBLE tolerance) throw (std::runtime_error) {
	//if ((N != (CISSTNETLIB_INTEGER) X.size()) || (M != (CISSTNETLIB_INTEGER) F.size()) || (Ldfjac != (CISSTNETLIB_INTEGER) J.rows()) || (N != (CISSTNETLIB_INTEGER) J.cols())) {
        if ((N != static_cast<CISSTNETLIB_INTEGER>(X.size())) || (M != static_cast<CISSTNETLIB_INTEGER>(F.size())) || (Ldfjac*N != static_cast<CISSTNETLIB_INTEGER>(J.size()))) {
	    cmnThrow(std::runtime_error("nmrLSNonLinJacobianSolver Solve: Size used for Allocate was different"));
	}
	Tolerance = tolerance;
        /* call the MINPACK C function */
	lmder2_(callBackF.FunctionFlmdif, callBackJ.FunctionFlmder2, &M, &N,
			X.Pointer(), F.Pointer(), J.Pointer(),
			&Ldfjac, &Tolerance, &Info,
			IWork.Pointer(), Work.Pointer(), &Lwork);
    }
    //@}
    
};

#endif // _nmrLSNonLinJacobianSolver_h

