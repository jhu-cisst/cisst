/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):	Anton Deguet
  Created on: 2005-07-27

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
  \brief Declaration of nmrLUSolver
*/


#ifndef _nmrLUSolver_h
#define _nmrLUSolver_h


#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrix.h>

#include <cisstNumerical/nmrNetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm LU: LU factorization of a general M by N matrix

  The class is a wrapper for the LAPACK function dgetrf.  It computes
  an LU factorization of a general M-by-N matrix A using partial
  pivoting with row interchanges.

  The factorization has the form \f$ A = P * L * U \f$ where P is a
  permutation matrix, L is lower triangular with unit diagonal
  elements (lower trapezoidal if m > n), and U is upper triangular
  (upper trapezoidal if m < n).

  The data members of this class are:

  - M: The number of rows of the input matrix A.  M >= 0.
  - N: The number of columns of the input matrix A.  N >= 0.
  - Lda: The leading dimension of the array A.  \f$ Lda \geq \mbox{max}(1,M) \f$.
  - MinMN: Lenght of diagonal \f$ MinMN = \mbox{min}(M,N)\f$
  - Ipiv: The pivot indices, from 1 to min(M, N).

  The LAPACK function modifies the input matrix in place.  The
  matrices L and U are stored in the same place which is pretty
  inconvenient to use.  To reconstruct both L and U, the user can set
  the AllocateLU flag to true.  This will allocate the correct amount
  of memory for L and U and the method UpdateLU will run automatically
  after each Solve.

  Similarly, the flag AllocateP will allocate some extra memory and
  built the permutation matrix P based on the pivots used (IPiv).

  \note The input matrices of this class must be column major.  To
  select the storage order, use #VCT_COL_MAJOR whenever you declare a
  matrix.

  \note The input matrix must be compact (see
  vctDynamicMatrix::IsCompact() or vctFixedSizeMatrix::IsCompact()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.

  \deprecated This class has been replaced by ::nmrLU,
  nmrLUDynamicData and nmrLUFixedSizeData.
*/
class nmrLUSolver {
    // we have this class so that we reserve memory only one
    // would help if svd of a same size matrix (or a matrix)
    // that doesnt change much is desired.

protected:
    CISSTNETLIB_INTEGER M;
    CISSTNETLIB_INTEGER N;
    CISSTNETLIB_INTEGER Lda;
    CISSTNETLIB_INTEGER MinMN, MaxMN;
    // vctDynamicMatrix<CISSTNETLIB_INTEGER> Ipiv;
    vctDynamicVector<CISSTNETLIB_INTEGER> Ipiv;
    CISSTNETLIB_INTEGER Info;
    bool AllocatePFlag, AllocateLUFlag;
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> P, L, U;

    /*! Allocates memory for the permutation matrix P. */
    void AllocateP(void) {
        if (M > N) {
            P.SetSize(MaxMN, MaxMN, VCT_COL_MAJOR);
        } else {
            P.SetSize(MinMN, MinMN, VCT_COL_MAJOR);
        }
    }

    /*! Allocates memory for usable copies of L and U. */ 
    void AllocateLU(void) {
        L.SetSize(M, MinMN, VCT_COL_MAJOR);
        U.SetSize(MinMN, N, VCT_COL_MAJOR);
        // These wont change, assign now
        L.SetAll(0.0);
        L.Diagonal().SetAll(1.0);
        U.SetAll(0.0);
    }

    /*! Update the content of the optional matrices L and U based on
      the matrix returned by the LAPACK function dgetrf. */
    template <class _matrixOwnerType>
    void UpdateLU(const vctDynamicConstMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE>& A) {
        const unsigned int rows = A.rows();
        const unsigned int cols = A.cols();
        unsigned int rowIndex, colIndex;
        for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
            for (colIndex = 0; colIndex < cols; ++colIndex) {
                if (rowIndex > colIndex) {
                    L.Element(rowIndex, colIndex) = A[rowIndex][colIndex];
                } else {
                    U.Element(rowIndex, colIndex) = A[rowIndex][colIndex];
                }
            }
        }
    }
    
    /*! Update the content of the optional permutation matrix P based
      on the pivot vector IPiv. */
    void UpdateP(void) {
        P.SetAll(0.0);
        P.Diagonal().SetAll(1.0);
        CISSTNETLIB_INTEGER rowIndex, colIndex;
        for (rowIndex = 0; rowIndex < MinMN; ++rowIndex) {
            colIndex = Ipiv[rowIndex] - 1;
            P.ExchangeColumns(rowIndex, colIndex);
        }
    }
    
    
public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLUSolver(void):
        M(0),
        N(0),
        AllocatePFlag(false),
        AllocateLUFlag(false)
    {
        Allocate(M, N, AllocateLUFlag, AllocatePFlag);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the method
      Allocate().  The next call to the Solve() method will check that
      the input matches the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param allocateLU Allocates memory to maintain a usable copy of L and U
      \param allocateP Allocates memory to maintain a usable copy of P
    */
    nmrLUSolver(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n,
                bool allocateLU = false,
                bool allocateP = false) {
        Allocate(m, n);
    }

    
    /*!
      \name Constructor with memory allocation.

      This constructor allocates the memory based on the actual input
      of the Solve() method.  It relies on the method Allocate().  The
      next call to the Solve() method will check that the input
      matches the dimension.

      \param A The matrix to be factorized (or any matrix of the same size)
      \param allocateLU Allocates memory to maintain a usable copy of L and U
      \param allocateP Allocates memory to maintain a usable copy of P
    */
    //@{
    nmrLUSolver(const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
                bool allocateLU = false, bool allocateP = false) {
        Allocate(A, allocateLU, allocateP);
    }

    template <vct::size_type _rows, vct::size_type _cols>
    nmrLUSolver(const vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, VCT_COL_MAJOR> & A,
                bool allocateLU = false, bool allocateP = false) {
        Allocate(A, allocateLU, allocateP);
    }
    //@}


    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the input matches the
      dimension.
      
      \param m Number of rows of A
      \param n Number of columns of A
      \param allocateLU Allocates memory to maintain a usable copy of L and U
      \param allocateP Allocates memory to maintain a usable copy of P
      
    */
    inline void Allocate(CISSTNETLIB_INTEGER m, CISSTNETLIB_INTEGER n,
                         bool allocateLU = false, bool allocateP = false) {
        const CISSTNETLIB_INTEGER one = 1;
        M = m;
        N = n;
        Lda = std::max(one, M);
        MinMN = std::min(M, N);
        MaxMN = std::max(M, N);
        Ipiv.SetSize(MinMN);
        
        AllocateLUFlag = allocateLU;
        AllocatePFlag = allocateP;
        if (AllocateLUFlag) AllocateLU();
        if (AllocatePFlag) AllocateP();
    }


    /*!
      \name Allocate memory to solve this problem.

      This method provides a convenient way to extract the required
      sizes from the input containers.  The next call to the Solve()
      method will check that the input matches the dimension.

      \param A The matrix to be factorized (or any matrix of the same size)
      \param allocateLU Allocates memory to maintain a usable copy of L and U
      \param allocateP Allocates memory to maintain a usable copy of P
    */
    //@{
    inline void Allocate(const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &A,
                         bool allocateLU = false, bool allocateP = false) {
        Allocate(A.rows(), A.cols(), allocateLU, allocateP);
    }

    template <vct::size_type _rows, vct::size_type _cols>
    inline void Allocate(const vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, VCT_COL_MAJOR> & A,
                         bool allocateLU = false, bool allocateP = false) {
        Allocate(_rows, _cols, allocateLU, allocateP);
    }
    //@}


    /*!  Performs the factorization.  The result is stored in the
      modified input and the data member IPiv (vector of pivots).

      To get a more convenient representation of the results, use turn
      allocateLU to true with Allocate() or the constructor.  This
      method will then update the data members L and U (matrices).
      Then, one can use GetL and GetU to retrieve their content.
      Similarly, if one needs the complete factorization as \f$ P * L
      * U \f$, the matrix P can be obtained when allocateP is set with
      the method GetP.
      
      \note This method requires a compact matrix with column major
      storage order.  The size must match the dimension passed to the
      Allocate method or the constructor.  An std::runtime_error
      exception will be thrown if these conditions are not met.
    */
    //@{
    template <class _matrixOwnerType>
    inline void Solve(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> &A) throw (std::runtime_error) {
        /* check that the size and storage order matches with Allocate() */
        if (! A.IsColMajor()) {
            cmnThrow(std::runtime_error("nmrLUSolver Solve: The input must be column major"));
        }

        /* check sizes */
        if ((M != static_cast<CISSTNETLIB_INTEGER>(A.rows())) || (N != static_cast<CISSTNETLIB_INTEGER>(A.cols()))) {
            cmnThrow(std::runtime_error("nmrLUSolver Solve: Size used for Allocate was different"));
        }

        /* check that the matrix is compact */
        if (! A.IsCompact()) {
            cmnThrow(std::runtime_error("nmrLUSolver Solve: Requires a compact matrix"));
        }

        /* call the LAPACK C function */
        dgetrf_(&M, &N,
                A.Pointer(), &Lda,
                Ipiv.Pointer(), &Info);

        /* Fill P, LU if required */
        if (AllocateLUFlag) UpdateLU(A);
        if (AllocatePFlag) UpdateP();
    }
    
    template <vct::size_type _rows, vct::size_type _cols>
    inline void Solve(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, VCT_COL_MAJOR> & A) {
        vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> Aref(A);
        Solve(Aref);
    }
    //@}


    /*! Get the vector of pivots. Since the pivots are defined in
      Fortran, they go from 1 to \f$min(M, N)\f$ included. To use
      these as C/C++ values, subtract 1. */
    inline const vctDynamicVector<CISSTNETLIB_INTEGER> &GetIpiv(void) const {
        return Ipiv;
    }

    /*! The status returned by the dgetrf function. */
    inline CISSTNETLIB_INTEGER GetInfo(void) const {
        return Info;
    }

    /*! Get the permutation matrix calculated based on IPiv. */
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetP(void) const throw(std::runtime_error) {
        if (!AllocatePFlag) {
            cmnThrow(std::runtime_error("nmrLUSolver GetP: P was not calculated since allocateP is not set"));
        }
        return P;
    }

    /*! Get the L matrix calculated from the output of the dgetrf function .*/
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetL(void) const throw(std::runtime_error) {
        if (!AllocateLUFlag) {
            cmnThrow(std::runtime_error("nmrLUSolver GetL: L was not calculated since allocateLU is not set"));
        }
        return L;
    }

    /*! Get the U matrix calculated from the output of the dgetrf function .*/
    inline const vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetU(void) const throw(std::runtime_error) {
        if (!AllocateLUFlag) {
            cmnThrow(std::runtime_error("nmrLUSolver GetU: U was not calculated since allocateLU is not set"));
        }
        return U;
    }
};


#ifdef CISST_COMPILER_IS_MSVC
class CISST_DEPRECATED nmrLUSolver;
#endif // CISST_COMPILER_IS_MSVC


#endif // _nmrLUSolver_h

