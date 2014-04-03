/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Ankur Kapoor
  Created on: 2005-07-29

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrPInverseSolver
*/


#ifndef _nmrPInverseSolver_h
#define _nmrPInverseSolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrSVDSolver.h>


/*!
  \ingroup cisstNumerical

  Algorithm P-Inverse:  Moore-Penrose pseudo-inverse
  Calculates the Moore-Penrose pseudo-inverse of the M by N
      matrix A, and stores the result in PInverseA.  The singular
      values of A are returned in S.  The left singular vectors
      are returned in U, and the right singular vectors are
      returned in V.

  \f$ A^{+} = V * \Sigma^{+} * U^{T} \f$

  where \f$ \Sigma^{+} \f$ is a \f$ N \times M \f$ matrix which is zero
  except for its min(m,n) diagonal elements, U is a \f$ M \times M \f$
  orthogonal matrix, and V is a \f$ N \times N \f$ orthogonal matrix.
  The diagonal elements of \f$ \Sigma^{+} \f$ are the reciprocal of 
  non-zero singular values of A; they are real and non-negative, and 
  are returned in descending order.  The first \f$ \mbox{min}(m,n) \f$ 
  columns of U and V are the left and right singular vectors of A.

  The data members of this class are:

  - M: The number of rows of the input matrix A.  M >= 0.
  - N: The number of columns of the input matrix A.  N >= 0.
  - MaxMN: leading dimension of all 2-dim. arrays.  MaxMN must 
       be greater than or equal to max(N,M).
  - MinMN: min(N,M);
  - A: On entry, the \f$ M \times N \f$ matrix A.
       On exit, the content of A is altered.

  \note The input matrix must be compact (see
  vctDynamicMatrix::IsCompact() or vctFixedSizeMatrix::IsCompact()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.

  \deprecated This class has been replaced by ::nmrPInverse,
  nmrPInverseDynamicData and nmrPInverseFixedSizeData.
*/
class nmrPInverseSolver {
    // we have this class so that we reserve memory only one
    // would help if svd of a same size matrix (or a matrix)
    // that doesnt change much is desired.

protected:
    CISSTNETLIB_INTEGER M;
    CISSTNETLIB_INTEGER N;
    CISSTNETLIB_INTEGER MaxMN;
    CISSTNETLIB_INTEGER MinMN;
    CISSTNETLIB_DOUBLE EPS;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> S;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> U;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> V;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> PInverseA;
    bool StorageOrder;

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning (push)
#pragma warning (disable: 4996)
#endif // CISST_COMPILER_IS_MSVC

    nmrSVDSolver svd;

#ifdef CISST_COMPILER_IS_MSVC
#pragma warning (pop)
#endif // CISST_COMPILER_IS_MSVC


public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrPInverseSolver(void):
        M(0),
        N(0),
        StorageOrder(VCT_COL_MAJOR)
    {
        Allocate(M, N, StorageOrder);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param storageOrder Storage order used for the input matrix.
      This order will be used for the output as well.
    */
    nmrPInverseSolver(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n, bool storageOrder = VCT_COL_MAJOR) {
        Allocate(m, n, storageOrder);
    }

    
    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension and storage order. */
    //@{
    nmrPInverseSolver(const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A) {
        Allocate(A);
    }
    //@}


    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param m Number of rows of A
      \param n Number of columns of A
      \param storageOrder Storage order used for all the matrices
    */
    inline void Allocate(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n, bool storageOrder) {
        StorageOrder = storageOrder;
        M = m;
        N = n;
        MaxMN = std::max(M, N);
        MinMN = std::min(M, N);
        svd.Allocate(M, N, storageOrder);
        S.SetSize(MinMN, 1, storageOrder);
        U.SetSize(M, M, storageOrder);
        V.SetSize(N, N, storageOrder);
        PInverseA.SetSize(N, M, storageOrder);
        /* compute machine precision */
        EPS = 1.0;
        do {
            EPS = EPS / 2.0;
        } while (EPS + 1.0 != 1.0);
        EPS = EPS * 2.0;
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    //@{
    inline void Allocate(const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A) {
        Allocate(A.rows(), A.cols(), A.IsRowMajor());
    }
    //@}


    /*! This computes the Moore-Penrose pseudo-inverse of a real 
      \f$ M \times N \f$ matrix A, optionally computing the left
      and/or right singular vectors. The SVD is written:

      \f$ A^{+} = V * \Sigma^{+} * U^{T} \f$
      
      \note This method requires a compact matrix with the same size
      and storage order used to Allocate.  An std::runtime_error
      exception will be thrown if these conditions are not met.
    */
    //@{
    template <class _matrixOwnerType>
    inline void Solve(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> &A) 
        throw (std::runtime_error) {
        /* all the checking is done by SVD */
        svd.Solve(A);
        S.Assign(svd.GetS());
        U.Assign(svd.GetU());
        V.Assign(svd.GetVt().Transpose());
        CISSTNETLIB_DOUBLE eps = EPS * svd.GetS().at(0, 0);
        PInverseA.SetAll(0);
        CISSTNETLIB_DOUBLE singularValue;
        for (int irank = 0; irank < MinMN; irank++) {
            if ((singularValue = S(irank, 0)) > eps) {
                for (int j = 0; j < M; j++) {
                    for (int i = 0; i < N; i++) {
                        PInverseA(i, j) = PInverseA(i, j)
                            + V(i, irank)*U(j, irank)/singularValue;
                    }
                }
            }
        }
    }
    //@}
    
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetS(void) const {
        return S;
    }

    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetU(void) const {
        return U;
    }

    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetV(void) const {
        return V;
    }

    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetPInverse(void) const {
        return PInverseA;
    }
};


#ifdef CISST_COMPILER_IS_MSVC
class CISST_DEPRECATED nmrPInverseSolver;
#endif // CISST_COMPILER_IS_MSVC


#endif // _nmrPInverseSolver_h

