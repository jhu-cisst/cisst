/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor
  Created on: 2004-10-26

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrLSSolver
*/


#ifndef _nmrLSSolver_h
#define _nmrLSSolver_h


#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm LS: Least Squares by QR or LQ decomposition
   This solves overdetermined or underdetermined real linear systems
   involving an M-by-N matrix A, or its transpose, using a QR or LQ
   factorization of A.  It is assumed that A has full rank.

   The following options are provided:

   1. If m >= n:  find the least squares solution of
      an overdetermined system, i.e., solve the least squares problem
                  \f$ \mbox {minimize} \| B - A*X \| \f$

   2. If m < n:  find the minimum norm solution of
      an underdetermined system \f$ A * X = B \f$

  The data members of this class are:

  - M: The number of rows of the input matrix A.  \f$ M >= 0 \f$.
  - N: The number of columns of the input matrix A.  \f$ N >= 0 \f$.
  - NRHS: The number of right hand sides, i.e., the number of
             columns of the matrices B and X. \f$ NRHS >=0 \f$.
  - Lda: The leading dimension of the array A.  \f$ Lda \geq \mbox{max}(1,M) \f$.
  - Ldb: The leading dimension of the array B.  \f$ Ldb \geq \mbox{max}(1,M,N). \f$.
  - Lwork: The dimension of the matrix Work.
            \f$ Lwork \geq \mbox{max}( 1, MN + max( MN, NRHS ) )\f$.
            For optimal performance,
            \f$ Lwork \geq \mbox{max}( 1, MN + max( MN, NRHS )*NB )\f$.
            where \f$ MN = \mbox{min}(M,N) \f$ and \f$ NB \f$ is the optimum block size.
  - Info: = 0: successful exit
          < 0: argument had an illegal value
  - Work: Working matrix of dimenstion \f$ Lwork \times 1 \f$.

  The input/output from this class is:
  - A: On entry, the \f$ M \times N \f$ matrix A.
       On exit,
            if M >= N, A is overwritten by details of its QR factorization
            if M <  N, A is overwritten by details of its LQ factorization
  - B: On entry, the matrix B of right hand side vectors, stored
             columnwise; B is M-by-NRHS
       On exit, B is overwritten by the solution vectors, stored
             columnwise:
             if m >= n, rows 1 to n of B contain the least
             squares solution vectors; the residual sum of squares for the
             solution in each column is given by the sum of squares of
             elements N+1 to M in that column;
             if m < n, rows 1 to N of B contain the
             minimum norm solution vectors;

  \note The input matrix must be compact (see
  vctDynamicMatrix::IsCompact() or vctFixedSizeMatrix::IsCompact()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrLSSolver {
    // we have this class so that we reserve memory only one would
    // help if svd of a same size matrix (or a matrix) that doesnt
    // change much is desired.

protected:
    CISSTNETLIB_INTEGER M;
    CISSTNETLIB_INTEGER N;
    CISSTNETLIB_INTEGER NRHS;
    CISSTNETLIB_INTEGER Lda;
    CISSTNETLIB_INTEGER Ldb;
    CISSTNETLIB_INTEGER Lwork;
    char Trans;
    vctDynamicMatrix<double> Work;
    CISSTNETLIB_INTEGER Info;
    bool StorageOrder;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLSSolver(void):
        M(0),
        N(0),
        NRHS(0),
        StorageOrder(VCT_COL_MAJOR)
    {
        Allocate(M, N, NRHS, StorageOrder);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param nrhs Number of columns of B
      \param storageOrder Storage order used for the input matrix.
      This order will be used for the output as well.
    */
    nmrLSSolver(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n, CISSTNETLIB_INTEGER nrhs, bool storageOrder) {
        Allocate(m, n, nrhs, storageOrder);
    }


    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension and storage order. */
    //@{
    nmrLSSolver(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A, B);
    }
    //@}


    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param nrhs Number of columns of B
      \param storageOrder Storage order used for all the matrices
    */
    inline void Allocate(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n, CISSTNETLIB_INTEGER nrhs, bool storageOrder) {
        const CISSTNETLIB_INTEGER one = 1;
        StorageOrder = storageOrder;
        M = m;
        N = n;
        NRHS = nrhs;
        Lda = std::max(one, M);
        Ldb = std::max(one, std::max(M, N));
        CISSTNETLIB_INTEGER MN = std::min(M, N);
        Lwork = std::max (one, MN + std::max (MN, NRHS));
        Trans = 'N';
        Work.SetSize(Lwork, 1, StorageOrder);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    //@{
    inline void Allocate(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A.rows(), A.cols(), B.cols(), A.IsRowMajor());
    }
    //@}


    /*! This computes the solves overdetermined or underdetermined real linear systems
        involving an M-by-N matrix A, using the right hand side M-by-NRHS matrix B.
    */

    //@{
    template <class _matrixOwnerType>
    inline void Solve(vctDynamicMatrixBase<_matrixOwnerType, double> &A, vctDynamicMatrixBase<_matrixOwnerType, double> &B) CISST_THROW(std::runtime_error) {
        /* check that the size and storage order matches with Allocate() */
        if (A.IsRowMajor() != StorageOrder) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Storage order used for Allocate was different"));
        }
        if (B.IsRowMajor() != StorageOrder) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Storage order used for Allocate was different"));
        }

        /* check sizes based on storage order, there is a more compact
           expression for this test but I find this easier to read and
           debug (Anton) */
        if (A.IsColMajor()) {
            if ((M != static_cast<CISSTNETLIB_INTEGER>(A.rows())) || (N != static_cast<CISSTNETLIB_INTEGER>(A.cols()))) {
                cmnThrow(std::runtime_error("nmrLSSolver Solve: Size used for Allocate was different"));
            }
        }
        if (B.IsColMajor()) {
            if ((M != static_cast<CISSTNETLIB_INTEGER>(B.rows())) || (NRHS != static_cast<CISSTNETLIB_INTEGER>(B.cols()))) {
                cmnThrow(std::runtime_error("nmrLSSolver Solve: Size used for Allocate was different"));
            }
        }

        /* check that the matrices are Fortran like */
        if (! A.IsCompact()) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Requires a compact matrix"));
        }
        if (! B.IsCompact()) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Requires a compact matrix"));
        }

        /* call the LAPACK C function */
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
        cisstNetlib_dgels_(&Trans, &M, &N, &NRHS,
                           A.Pointer(), &Lda,
                           B.Pointer(), &Ldb,
                           Work.Pointer(), &Lwork, &Info);
#endif
#else // no major version
        dgels_(&Trans, &M, &N, &NRHS,
               A.Pointer(), &Lda,
               B.Pointer(), &Ldb,
               Work.Pointer(), &Lwork, &Info);
#endif // CISSTNETLIB_VERSION
    }
    //@}

};


#endif // _nmrLSSolver_h
