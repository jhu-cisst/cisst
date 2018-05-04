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
  \brief Declaration of nmrHFTISolver
 */

#ifndef _nmrHFTISolver_h
#define _nmrHFTISolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

// forward declaration of fortran routine for very old cisstNetlib
#ifndef CISSTNETLIB_VERSION
#  define NEEDS_HFTI_FORWARD_DECLARATION
#else
#  if (CISSTNETLIB_VERSION_MAJOR == 3)
#    define NEEDS_HFTI_FORWARD_DECLARATION
#  endif
#endif


#ifdef NEEDS_HFTI_FORWARD_DECLARATION
CISSTNETLIB_INTEGER hfti_(CISSTNETLIB_DOUBLE * a, CISSTNETLIB_INTEGER * mda, CISSTNETLIB_INTEGER * m, CISSTNETLIB_INTEGER * n,
                          CISSTNETLIB_DOUBLE * b, CISSTNETLIB_INTEGER * mdb, CISSTNETLIB_INTEGER * nb, CISSTNETLIB_DOUBLE * tau,
                          CISSTNETLIB_INTEGER * krank, CISSTNETLIB_DOUBLE * rnorm, CISSTNETLIB_DOUBLE * h__, CISSTNETLIB_DOUBLE * g,
                          CISSTNETLIB_INTEGER * ip);
#  undef NEEDS_HFTI_FORWARD_DECLARATION
#endif

/*!
  \ingroup cisstNumerical

  Algorithm LDP: Least Distance Programming

  The original version of this code was developed by Charles L. Lawson
  and Richard J. Hanson at Jet Propulsion Laboratory 1974 MAR 1, and
  published in the book "Solving Least Squares Problems",
  Prentice-Hall, 1974.

  This can be used to solve a linear least squares problem or a set
  of least square problem having same matrix but different right-hand
  side vectors.

  The data members of this class are:
  - M, N: Dimension of input matrix
  - NB: Dimension of right side input matrix
  - A: The input matrix
  - B: A \f$ M \times NB \f$ matrix, the \f$ NB \f$ columns of \f$ B \f$
   represent the right-side vectors \f$ b_j \f$ vectors for \f$ NB \f$
   linear least squares problem
  - tau: An absolute tolerance parameter
  - X: A \f$ M \times NB \f$ matrix having column vectors \f$ x_j \f$
  (NOTE: if \f$ B \f$ is a \f$ M \times M \f$ matrix then \f$ X \f$ is
  the pseudoinverse of \f$ A \f$ .
  - krank: The pseudorank of A as determined by algorithm.
  - RNORM: RNORM(j) would contain the euclidean norm of the residual vector
  for the \f$ j \f$ th column vector
  - H, G: Working arrays
  - IP: Integer working array

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare
  a matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrHFTISolver {

protected:
    CISSTNETLIB_INTEGER M;
    CISSTNETLIB_INTEGER N;
    CISSTNETLIB_INTEGER NB;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> A;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> B;
    CISSTNETLIB_DOUBLE tau;
    CISSTNETLIB_INTEGER krank;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> RNORM;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> G;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> H;
    vctDynamicMatrix<CISSTNETLIB_INTEGER> IP;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> X;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrHFTISolver(void):
        M(0),
        N(0),
        NB(0)
    {
        Allocate(M, N, NB);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M, N and NB.  It relies on the method
      Allocate().  The next call to the Solve() method will check that
      the parameters match the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    nmrHFTISolver(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n, CISSTNETLIB_INTEGER nb) {
        Allocate(m, n, nb);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on the actual input of the Solve()
      method.  It relies on the method Allocate().  The next call to
      the Solve() method will check that the parameters match the
      dimension. */
    nmrHFTISolver(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &B) {
        Allocate(A, B);
    }


    /*! This method allocates the memory based on Ma, Na and Nb.  The
      next call to the Solve() method will check that the parameters
      match the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    inline void Allocate(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n, CISSTNETLIB_INTEGER nb) {
        M = m;
        N = n;
        NB = nb;
        vctDynamicMatrix<CISSTNETLIB_DOUBLE> RNORM;
        vctDynamicMatrix<CISSTNETLIB_DOUBLE> G;
        vctDynamicMatrix<CISSTNETLIB_DOUBLE> H;
        vctDynamicMatrix<CISSTNETLIB_INTEGER> IP;
        A.SetSize(M, N, VCT_COL_MAJOR);
        B.SetSize(M, NB, VCT_COL_MAJOR);
        RNORM.SetSize(NB, 1, VCT_COL_MAJOR);
        G.SetSize(N, 1, VCT_COL_MAJOR);
        H.SetSize(N, 1, VCT_COL_MAJOR);
        IP.SetSize(N, 1, VCT_COL_MAJOR);
        X.SetSize(M, NB, VCT_COL_MAJOR);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &B) {
        Allocate(A.rows(), A.cols(), B.cols());
    }


    /*!  \note This method verifies that the input parameters are
      using a column major storage order and that they are compact.
      Both conditions are tested using vctDynamicMatrix::IsFortran().
      If the parameters don't meet all the requirements, an exception
      is thrown (std::runtime_error).
     */
    inline void Solve(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &B) CISST_THROW(std::runtime_error) {
        /* check that the size matches with Allocate() */
        if ((M != static_cast<CISSTNETLIB_INTEGER>(A.rows()))
            || (N != static_cast<CISSTNETLIB_INTEGER>(A.cols()))
            || (NB != static_cast<CISSTNETLIB_INTEGER>(B.cols()))) {
            cmnThrow(std::runtime_error("nmrHFTISolver Solve: Sizes used for Allocate were different"));
        }

        /* check that the matrices are Fortran like */
        if (! (A.IsFortran()
               && B.IsFortran())) {
            cmnThrow(std::runtime_error("nmrHFTISolver Solve: All parameters must be Fortran compatible"));
        }

        hfti_(A.Pointer(), &M, &M, &N, B.Pointer(), &M, &NB, &tau,
              &krank, RNORM.Pointer(),
              H.Pointer(), G.Pointer(), IP.Pointer());
        //error handling??
    }
};


#endif // _nmrHFTISolver_h
