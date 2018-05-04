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
  \brief Declaration of nmrSVDRSSolver
 */

#ifndef _nmrSVDRSSolver_h
#define _nmrSVDRSSolver_h


#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>


/*!
  \ingroup cisstNumerical

  Algorithm SVDRS: Singular Value Decomposition also treating Right
  Side Vector.

  The original version of this code was developed by Charles L. Lawson
  and Richard J. Hanson at Jet Propulsion Laboratory 1974 SEP 25, and
  published in the book "Solving Least Squares Problems",
  Prentice-Hall, 1974.

  This subroutine computes the singular value decomposition of the
  given \f$ M \times N \f$ matrix A, and optionally applies the
  transformations from the left to the \f$ Nb \f$ column vectors of
  the \f$ Mb \times Nb \f$ matrix B.  Either \f$ M > N \f$ or
  \f$ M < N \f$ is permitted.

  The singular value decomposition of A is of the form:

  \f$ A  =  U  S  V^{T} \f$

  where U is \f$ M \times M \f$ orthogonal, S is \f$ M \times N \f$
  diagonal with the diagonal terms nonnegative and ordered from large
  to small, and V is \f$ N \times N \f$ orthogonal.  Note that these
  matrices also satisfy:

  \f$ S = (U^{T})  A  V \f$

  The singular values, i.e. the diagonal terms of the matrix S, are
  returned in the matrix GetS().  If \f$ M < N \f$, positions \f$ M+1
  \f$ through N of S() will be set to zero.

  The product matrix  \f$ G = U^{T} B \f$ replaces the given matrix B
  in the matrix B(,).

  If the user wishes to obtain a minimum length least squares
  solution of the linear system:

  \f$ \| Ax - B \| \f$

  the solution X can be constructed, following use of this subroutine,
  by computing the sum for i = 1, ..., R of the outer products

  (Col i of V) * (1/S(i)) * (Row i of G)

  Here R denotes the pseudorank of A which the user may choose in the
  range 0 through \f$ \mbox{min}(M, N) \f$ based on the sizes of the
  singular values.

  The data members of this class are:

  - M: Number of rows of matrix A, B, G.
  - N: Number of columns of matrix A. No of rows and columns of matrix
        V.
  - Lda: First dimensioning parameter for A. \f$ Lda = \mbox{max}(M,N) \f$
  - Ldb: First dimensioning parameter for B. \f$ Ldb \geq M \f$
  - Nb: Number of columns in the matrices B and G.
  - A: On input contains the \f$ M \times N \f$ matrix A.
       On output contains the \f$ N \times N \f$ matrix V.
  - B: If \f$ Nb > 0 \f$ this array must contain a
       \f$ M \times Nb \f$ matrix on input and will contain the
       \f$ M \times Nb \f$ product matrix, \f$ G = U^{T}  B \f$ on output.
  - S: On return will contain the singular values of A, with the ordering
       \f$ S(1) > S(2) > ... > S(N1) > 0 \f$.
       If \f$ M < N \f$ the singular values indexed from \f$ M+1 \f$
       through N will be zero.
  - Work   Work space of total size at least \f$ 2N \f$.

  \note This code gives special treatment to rows and columns that are
  entirely zero.  This causes certain zero singular values to appear
  as exact zeros rather than as about MACHEPS times the largest
  singular value It similarly cleans up the associated columns of U
  and V.

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare
  a matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrSVDRSSolver {

protected:
    CISSTNETLIB_INTEGER M;                    /* No. of rows of A, B, G */
    CISSTNETLIB_INTEGER N;                    /* No. of cols of A, No of rows and cols of V */
    CISSTNETLIB_INTEGER Lda;                  /* >= max(m, n) */
    CISSTNETLIB_INTEGER Ldb;                  /* >= m */
    CISSTNETLIB_INTEGER Nb;                   /* No. of cols of B and G */
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> S;   /* Singular values, Dim(S)>n */
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> Work;/* Work space */

public:

    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrSVDRSSolver(void): M(0), N(0)
    { Allocate(M, N); }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M, N and Nb.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    nmrSVDRSSolver( CISSTNETLIB_INTEGER m,
                    CISSTNETLIB_INTEGER n,
                    CISSTNETLIB_INTEGER nb = 1) {
        Allocate(m, n, nb);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on the actual input of the Solve()
      method.  It relies on the method Allocate().  The next call to
      the Solve() method will check that the parameters match the
      dimension. */
    nmrSVDRSSolver( vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
                    vctDynamicMatrix<CISSTNETLIB_DOUBLE> &B )
    { Allocate(A, B); }


    /*! This method allocates the memory based on M, N and Nb.  The
      next call to the Solve() method will check that the parameters
      match the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    inline void Allocate( CISSTNETLIB_INTEGER m,
                          CISSTNETLIB_INTEGER n,
                          CISSTNETLIB_INTEGER nb = 1) {
        M = m;
        N = n;
        Lda = std::max(M, N);
        Ldb = M;
        Nb = nb;
        S.SetSize(N, 1, VCT_COL_MAJOR);
        Work.SetSize(2 * N, 1, VCT_COL_MAJOR);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate( vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
                          vctDynamicMatrix<CISSTNETLIB_DOUBLE> &B )
    { Allocate(A.rows(), A.cols(), B.cols()); }


    /*! This subroutine computes the singular value decomposition of the
      given \f$ M \times N \f$ matrix A, and optionally applies the
      transformations from the left to the \f$ Nb \f$ column vectors of
      the \f$ Mb \times Nb \f$ matrix B.  Either \f$ M > N \f$ or \f$ M <
      N \f$ is permitted.

      The singular value decomposition of A is of the form:

      \f$ A  =  U  S  V^{T} \f$

      \note This method verifies that the input parameters are using a
      column major storage order and that they are compact.  Both
      conditions are tested using vctDynamicMatrix::IsFortran().
      If the parameters don't meet all the requirements, an exception
      is thrown (std::runtime_error).
    */
    inline void Solve( vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
                       vctDynamicMatrix<CISSTNETLIB_DOUBLE> &B )
        CISST_THROW(std::runtime_error)
    {
        /* check that the size matches with Allocate() */
        if ((M  != static_cast<CISSTNETLIB_INTEGER>(A.rows())) ||
            (N  != static_cast<CISSTNETLIB_INTEGER>(A.cols())) ||
            (Nb != static_cast<CISSTNETLIB_INTEGER>(B.cols()))) {
            cmnThrow(std::runtime_error("nmrSVDRSSolver Solve: Sizes used for Allocate were different"));
        }

        /* check other dimensions */
        if (M != static_cast<CISSTNETLIB_INTEGER>(B.rows())) {
            cmnThrow(std::runtime_error("nmrSVDRSSolver Solve: Sizes of parameters are incompatible"));
        }

        /* check that the matrices are Fortran like */
        if (! (A.IsFortran() && B.IsFortran())) {
            cmnThrow(std::runtime_error("nmrSVDRSSolver Solve: All parameters must be Fortran compatible"));
        }

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
        cisstNetlib_svdrs_(A.Pointer(), &Lda, &M, &N, B.Pointer(),
                           &Ldb, &Nb, S.Pointer(), Work.Pointer());
#endif
#else // no major version
        svdrs_(A.Pointer(), &Lda, &M, &N, B.Pointer(),
               &Ldb, &Nb, S.Pointer(), Work.Pointer());
#endif // CISSTNETLIB_VERSION
    }


    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> & GetS(void) const
    { return S; }

};


#endif // _nmrSVDRSSolver_h
