/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor
  Created on: 2004-10-30

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrNNLSSolver
 */

#ifndef _nmrNNLSSolver_h
#define _nmrNNLSSolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm NNLS: Non Negative Least Squares

  The original version of this code was developed by Charles L. Lawson
  and Richard J. Hanson at Jet Propulsion Laboratory 1973 JUN 15, and
  published in the book "Solves Least Squares Problems",
  Prentice-Hall, 1974.

  Given a \f$ M \times N\f$ matrix A, and a \f$ M \times 1 \f$ vector B,
  compute a \f$ N \times 1 \f$ vector X, that solves the least squares
  problem:

  \f$ \mbox{min} \; 1 / 2 \| AX - B \| \; \mbox{subject to} \; X \geq 0\f$

  The data members of this class are:
  - M, N: Dimension of input matrix.
  - MDA: Is the first dimension parameter for the matrix A.
  - A: On entry contains the \f$ M \times N \f$ matrix A.
       On exit the matrix product of \f$ QA \f$, where Q is a \f$ M \times N \f$ orthogonal matrix generated implicitly by this subroutine.
  - B: On entry contains the \f$ M \times 1 \f$ matrix B>
       On exit the matrix product if \f$ QB \f$
  - X: On exit X contains the solution vector>
  - rnorm: Contains the Euclidean norm of the residual vector.
  - W: A \f$ N \times 1 \f$ working array. On exit W constrains the dual solution vector>
  - ZZ: A \f$ M \times 1 \f$ working array.
  - Index: A working array of integers of size atleast \f$ N \times 1 \f$

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare a
  matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrNNLSSolver {

protected:
    CISSTNETLIB_INTEGER M;
    CISSTNETLIB_INTEGER N;
    CISSTNETLIB_INTEGER Mda;
    CISSTNETLIB_INTEGER Mode;
    CISSTNETLIB_DOUBLE RNorm;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> X;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> W;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> Zz;
    vctDynamicMatrix<CISSTNETLIB_INTEGER> Index;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrNNLSSolver(void):
        M(0),
        N(0)
    {
        Allocate(M, N);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of rows of C
      \param n Number of columns of C
    */
    nmrNNLSSolver(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n) {
        Allocate(m, n);
    }


    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension. */
    nmrNNLSSolver(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &C, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &d) {
        Allocate(C, d);
    }


    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.

      \param m Number of rows of C
      \param n Number of columns of C
    */
    inline void Allocate(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n) {
        M = m;
        N = n;
        Mda = M;
        X.SetSize(N, 1, VCT_COL_MAJOR);
        W.SetSize(N,1, VCT_COL_MAJOR);
        Zz.SetSize(M,1, VCT_COL_MAJOR);
        Index.SetSize(N, 1, VCT_COL_MAJOR);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &C, vctDynamicMatrix<CISSTNETLIB_DOUBLE> & CMN_UNUSED(d)) {
        Allocate(C.rows(), C.cols());
    }


    /*! Given a \f$ M \times N\f$ matrix A, and a \f$ M \times 1 \f$ vector B,
      compute a \f$ N \times 1 \f$ vector X, that solves the least squares
      problem:

      \f$ \mbox{min} \; 1 / 2 \| AX - B \| \; \mbox{subject to} \; X \geq 0\f$

      \note This method verifies that the input parameters are using a
      column major storage order and that they are compact.  Both
      conditions are tested using vctDynamicMatrix::IsFortran().
      If the parameters don't meet all the requirements, an exception
      is thrown (std::runtime_error).
    */
    inline void Solve(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &C, vctDynamicMatrix<CISSTNETLIB_DOUBLE> d)
        CISST_THROW(std::runtime_error)
    {
        /* check that the size matches with Allocate() */
      if ((M != static_cast<CISSTNETLIB_INTEGER>(C.rows()))
          || (N != static_cast<CISSTNETLIB_INTEGER>(C.cols()))) {
            cmnThrow(std::runtime_error("nmrNNLSSolver Solve: Sizes used for Allocate were different"));
        }

        /* check other dimensions */
        if (C.rows() != d.rows()) {
            cmnThrow(std::runtime_error("nmrNNLSSolver Solve: Sizes of parameters are incompatible"));
        }

        /* check that the matrices are Fortran like */
        if (! (C.IsFortran()
               && d.IsFortran())) {
            cmnThrow(std::runtime_error("nmrNNLSSolver Solve: All parameters must be Fortran compatible"));
        }

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
        cisstNetlib_nnls_(C.Pointer(), &Mda, &M, &N, d.Pointer(), X.Pointer(), &RNorm,
              W.Pointer(), Zz.Pointer(), Index.Pointer(), &Mode);
#endif
#else // no major version
        nnls_(C.Pointer(), &Mda, &M, &N, d.Pointer(), X.Pointer(), &RNorm,
              W.Pointer(), Zz.Pointer(), Index.Pointer(), &Mode);
#endif // CISSTNETLIB_VERSION

        //error handling??
    }


    /*! Get X.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetX(void) const {
        return X;
    }


    /* Get W.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetDual(void) const {
        return W;
    }


    /* Get RNorm.  This method must be used after Solve(). */
    inline CISSTNETLIB_DOUBLE GetRNorm(void) const {
        return RNorm;
    }
};


#endif // _nmrNNLSSolver_h
