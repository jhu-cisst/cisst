/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Ankur Kapoor
  Created on:	2004-10-30

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrLDPSolver
 */

#ifndef _nmrLDPSolver_h
#define _nmrLDPSolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>
#include <cisstNumerical/nmrExport.h>


/*!
  \ingroup cisstNumerical

  Algorithm LDP: Least Distance Programming

  The original version of this code was developed by Charles L. Lawson
  and Richard J. Hanson at Jet Propulsion Laboratory 1974 MAR 1, and
  published in the book "Solving Least Squares Problems",
  Prentice-Hall, 1974.

  Given a \f$ M \times N \f$ matrix G and a \f$ M \times 1 \f$ vector
  h, compute a \f$ N \times 1 \f$ vector X, that solves the linear
  distance problem:

  \f$ \mbox{min} \; \| X \| \; \mbox{subject to} \; GX \geq h \f$

  The data members of this class are:
  - M, N: Dimension of input matrix
  - G: The input matrix
  - H: On entry contains the \f$ M \times 1 \f$ matrix h
  - E: A \f$ N+1 \times M \f$ matrix passed as A matrix to NNLS
  - CopyE: A  copy of E made before passing to NNLS
  - F: A \f$ N+1 \times 1 \f$ matrix passed as B to NNLS
  - U: A \f$ N+1 \times 1 \f$ matrix in which solution to NNLS is obtained
  - R: A \f$ N+1 \times 1 \f$ working matrix
  - W: A \f$ M \times 1 \f$ working matrix
  - ZZ: A \f$ N+1 \times 1 \f$ working array.
  - Index: A working array of integers of size atleast \f$ M \times 1 \f$
  - X: On exit X contains the solution vector

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare
  a matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrLDPSolver {

protected:
    CISSTNETLIB_INTEGER M;
    CISSTNETLIB_INTEGER N;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> E;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> CopyE;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> F;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> R;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> U;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> W;
    vctDynamicMatrix<CISSTNETLIB_INTEGER> Index;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> Zz;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> X;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLDPSolver(void):
        M(0),
        N(0)
    {
        Allocate(M, N);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the method
      Allocate().  The next call to the Solve() method will check that
      the parameters match the dimension.

      \param m Number of rows of G
      \param n Number of columns of G
    */
    nmrLDPSolver(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n) {
        Allocate(m, n);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on the actual input of the Solve()
      method.  It relies on the method Allocate().  The next call to
      the Solve() method will check that the parameters match the
      dimension. */
    nmrLDPSolver(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &G, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &h) {
        Allocate(G, h);
    }


    /*! This method allocates the memory based on Ma, Na and Mg.  The
      next call to the Solve() method will check that the parameters
      match the dimension.

      \param m Number of rows of G
      \param n Number of columns of G
    */
    inline void Allocate(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n) {
        M = m;
        N = n;
        E.SetSize(N + 1, M, VCT_COL_MAJOR);
        CopyE.SetSize(N + 1, M, VCT_COL_MAJOR);
        F.SetSize(N + 1, 1, VCT_COL_MAJOR);
        R.SetSize(N + 1, 1, VCT_COL_MAJOR);
        U.SetSize(M, 1, VCT_COL_MAJOR);
        W.SetSize(M, 1, VCT_COL_MAJOR);
        Index.SetSize(M, 1, VCT_COL_MAJOR);
        Zz.SetSize(N + 1, 1, VCT_COL_MAJOR);
        X.SetSize(N, 1, VCT_COL_MAJOR);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &G, vctDynamicMatrix<CISSTNETLIB_DOUBLE> & CMN_UNUSED(h)) {
        Allocate(G.rows(), G.cols());
    }


    /*! Given a \f$ M \times N \f$ matrix G and a \f$ M \times 1 \f$
      vector h, compute a \f$ N \times 1 \f$ vector X, that solves the
      linear distance problem:

      \f$ \mbox{min} \; \| X \| \; \mbox{subject to} \; GX \geq h \f$

      \note This method verifies that the input parameters are using a
      column major storage order and that they are compact.  Both
      conditions are tested using vctDynamicMatrix::IsFortran().  If
      the parameters don't meet all the requirements, an exception is
      thrown (std::runtime_error).
    */
    CISST_EXPORT void Solve(vctDynamicMatrix<CISSTNETLIB_DOUBLE> &G, vctDynamicMatrix<CISSTNETLIB_DOUBLE> &h) CISST_THROW(std::runtime_error);


    /*! Get X.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetX(void) const {
        return X;
    }
};


#endif // _nmrLDPSolver_h
