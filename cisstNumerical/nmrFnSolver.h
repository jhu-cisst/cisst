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
  \brief Declaration of nmrFnSolver
*/


#ifndef _nmrFnSolver_h
#define _nmrFnSolver_h

#include <cisstVector/vctDynamicMatrix.h>

#include <cnetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm Fn: Solve nonlinear set of equations using Powell's method
  The purpose of this class is to find a zero of a system of
  N nonlinear functions in N variables by a modification
  of the powell hybrid method. the user must provide a
  subroutine which calculates the functions and the jacobian.

  The data members of this class are:

  - N: The number of variables and also the equations.  N >= 0.
  - Tolerance: A nonnegative input variable. Termination occurs
           when the algorithm estimates either that the relative
           error in the sum of squares is at most Tolerance or that
           the relative error between X and the solution is at
           most Tolerance. (Default set to 1.0e-6).
  - Work: A working vector of length N*(N+13)/2
  - LWork: Length of working array.
  - Info: = info is an integer output variable. if the user has
            terminated execution, info is set to the (negative)
            value of Flag. see description of FunctionPointer.
	    otherwise, info is set as follows.

          info = 0   improper input parameters.
 
          info = 1   algorithm estimates that the relative error
                     between x and the solution is at most tol.
 
          info = 2   number of calls to user function has
                     reached 200*(n+1).
 
          info = 3   tol is too small. no further improvement in
                     the approximate solution x is possible.
 
          info = 4   iteration is not making good progress.

  The input/output from this class is:
  - X: On entry, the inital estimate of solution vector.
       On exit, final estimate of solution vector.
  - callBack: Is object of type nmrCallBackFnSolver used to supply the user method.
  	The user method which belongs to a user defined class 'Cfoo' has the 
	following definition
	    int Cfoo::Mbar (vctDynamicVectorRef<double> &X,
	        vctDynamicVectorRef<double> &F, long int &Flag);
	The solver calls this method when needed to obtain values for F for
	a given variable values X.
	the value of Flag should not be changed by Mbar unless
	the user wants to terminate execution of Solver.
	in this case set Flag to a negative integer.

	The following constructor can be used to contruct the nmrCallBackFnSolver object
	required to be passed to the Solve() method. This object needs to be created
	only once per given set of nonlinear functions to be minized.
		nmrCallBackFnSolver<nmrUNIQUE_IDENTIFIER_LINE, Cfoo> callBackObject(this, &Cfoo::Mbar);
	

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrFnSolver {
	// we have this class so that we reserve memory only one
	// would help if svd of a same size matrix (or a matrix)
	// that doesnt change much is desired.

protected:
	CISSTNETLIB_INTEGER N;
	CISSTNETLIB_DOUBLE Tolerance;
	CISSTNETLIB_INTEGER Info;
	CISSTNETLIB_INTEGER Lwork;
	vctDynamicVector<CISSTNETLIB_DOUBLE> Work;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrFnSolver(void)
    {
        Allocate(0);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param n Number of variables
      This order will be used for the output as well.
    */
	nmrFnSolver(CISSTNETLIB_INTEGER n)
    {
        Allocate(n);
    }

    
    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension and storage order. */
    //@{
	nmrFnSolver(vctDynamicVector<CISSTNETLIB_DOUBLE> &X) {
        Allocate(X);
    }
    //@}


    /*! This method allocates the memory based on N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param n Number of variables
    */
	inline void Allocate(CISSTNETLIB_INTEGER n) {
        N = n;
        Lwork = N*(3*3*3*N+13)/2;
        Tolerance = 1e-6;
        Work.SetSize(Lwork);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    //@{
    inline void Allocate(vctDynamicVector<CISSTNETLIB_DOUBLE> &X) {
        Allocate(X.size());
    }
    //@}


    /*! This computes the solves nonlinear equations problem invloving
        N functions in N variables.
	On input, X contains the starting point,
	On output, X contains the final estimate
	F contains the final values
    */

    //@{
    template <int __instanceLine, class __elementType>
    inline void Solve(nmrCallBackFunctionF<__instanceLine, __elementType> &callBack, vctDynamicVector<CISSTNETLIB_DOUBLE> &X,
                      vctDynamicVector<CISSTNETLIB_DOUBLE> &F, CISSTNETLIB_DOUBLE tolerance) throw (std::runtime_error) {
        if ((N != static_cast<CISSTNETLIB_INTEGER>(X.size())) || (N != static_cast<CISSTNETLIB_INTEGER>(F.size()))) {
            cmnThrow(std::runtime_error("nmrFnSolver Solve: Size used for Allocate was different"));
        }
        Tolerance = tolerance;
        /* call the MINPACK C function */
        hybrd1_((U_fp)callBack.FunctionFhybrd, &N,
                X.Pointer(), F.Pointer(), 
                &Tolerance, &Info,
                Work.Pointer(), &Lwork);
    }
    //@}
    
};

#endif // _nmrFnSolver_h

