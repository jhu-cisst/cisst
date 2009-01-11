/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVDSolver.h,v 1.20 2007/04/26 19:33:57 anton Exp $
  
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
  \brief Declaration of nmrSVDSolver
*/


#ifndef _nmrSVDSolver_h
#define _nmrSVDSolver_h


#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm SVD: Singular Value Decomposition

  This computes the singular value decomposition (SVD) of a real \f$ M
  \times N \f$ matrix A, optionally computing the left and/or right
  singular vectors. The SVD is written:

  \f$ A = U * \Sigma * V^{T} \f$

  where \f$ \Sigma \f$ is a \f$ M \times N \f$ matrix which is zero
  except for its min(m,n) diagonal elements, U is a \f$ M \times M \f$
  orthogonal matrix, and V is a \f$ N \times N \f$ orthogonal matrix.
  The diagonal elements of \f$ \Sigma \f$ are the singular values of
  A; they are real and non-negative, and are returned in descending
  order.  The first \f$ \mbox{min}(m,n) \f$ columns of U and V are the
  left and right singular vectors of A.

  Note that the routine returns \f$ V^{T} \f$, not \f$ V \f$.

  The data members of this class are:

  - M: The number of rows of the input matrix A.  M >= 0.
  - N: The number of columns of the input matrix A.  N >= 0.
  - Lda: The leading dimension of the array A.  \f$ Lda \geq \mbox{max}(1,M) \f$.
  - Ldu: The leading dimension of the array U.  \f$ Ldu \geq M \f$.
  - Ldvt: The leading dimension of the array VT. \f$ Ldvt \geq N \f$.
  - Lwork: The dimension of the matrix Work. \f$ Lwork \geq \mbox{max}(3 * \mbox{min}(M,N) + \mbox{max}(M,N),
         5 * \mbox{min}(M,N)) \f$.
  - Work: Working matrix of dimenstion \f$ Lwork \times 1 \f$.
  - S: The singular values of A, sorted so that \f$ S(i) \geq S(i+1) \f$.
  - U: Contains the \f$ M \times M \f$ orthogonal matrix U.
  - Vt: Contains the \f$ N \times N \f$ orthogonal matrix \f$ V^{T} \f$.
  - A: On entry, the \f$ M \times N \f$ matrix A.
       On exit, the content of A is altered.

  \note The input matrices of this class can use either column major
  or row major storage order.  To select the storage order, use either
  #VCT_COL_MAJOR or #VCT_ROW_MAJOR (default) whenever you declare a
  matrix.

  \note The input matrix must be compact (see
  vctDynamicMatrix::IsCompact() or vctFixedSizeMatrix::IsCompact()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.

  \deprecated This class has been replaced by ::nmrSVD,
  nmrSVDDynamicData and nmrSVDFixedSizeData.
*/
class nmrSVDSolver {
    // we have this class so that we reserve memory only one
    // would help if svd of a same size matrix (or a matrix)
    // that doesnt change much is desired.

protected:
    long int M;
    long int N;
    long int Lda;
    long int Ldu;
    long int Ldvt;
    long int Lwork;
    char Jobu;
    char Jobvt;
    vctDynamicMatrix<double> S;
    vctDynamicMatrix<double> U;
    vctDynamicMatrix<double> Vt;
    vctDynamicMatrix<double> Work;
    long int Info;
    bool StorageOrder;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrSVDSolver(void):
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
    nmrSVDSolver(long int m, long int n, bool storageOrder) {
        Allocate(m, n, storageOrder);
    }

    
    /*!
      \name Constructor with memory allocation.

      This constructor allocates the memory based on the actual input
      of the Solve() method.  It relies on the method Allocate().  The
      next call to the Solve() method will check that the parameters
      match the dimension and storage order.

      \param A Input matrix
    */
    //@{
    template <class _matrixOwnerType>
    nmrSVDSolver(const vctDynamicMatrixBase<_matrixOwnerType, double> &A) {
        Allocate(A);
    }
    template <unsigned int _rows, unsigned int _cols, bool _storageOrder>
    nmrSVDSolver(const vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> & A) {
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
    inline void Allocate(long int m, long int n, bool storageOrder) {
        const long int one = 1;
        StorageOrder = storageOrder;
        if (storageOrder == VCT_COL_MAJOR) {
            M = m;
            N = n;
        } else {
            M = n;
            N = m;
        }
        Lda = std::max(one, M);
        Ldu = M;
        Ldvt = N;
        Lwork = std::max(3 * std::min(M, N) + std::max(M, N),
                         5 * std::min(M, N));
        Jobu = 'A';
        Jobvt = 'A';
        S.SetSize(std::min(M, N), 1, StorageOrder);
        U.SetSize(Ldu, Ldu, StorageOrder);
        Vt.SetSize(Ldvt, Ldvt, StorageOrder);
        Work.SetSize(Lwork, 1, StorageOrder);
    }


    /*!
      \name Allocate memory to solve this problem.

      This methods provide a convenient way to extract the required
      sizes from the input containers.  The next call to the Solve()
      method will check that the parameters match the dimension. */
    //@{
    template <class _matrixOwnerType>
    inline void Allocate(const vctDynamicMatrixBase<_matrixOwnerType, double> &A) {
        Allocate(A.rows(), A.cols(), A.IsRowMajor());
    }
    template <unsigned int _rows, unsigned int _cols, bool _storageOrder>
    inline void Allocate(const vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> & A) {
        Allocate(_rows, _cols, _storageOrder);
    }
    //@}


    /*! This computes the singular value decomposition (SVD) of a real
      \f$ M \times N \f$ matrix A, optionally computing the left
      and/or right singular vectors. The SVD is written:

      \f$ A = U * \Sigma * V^{T} \f$
      
      \note This method requires a compact matrix with the same size
      and storage order used to Allocate.  An std::runtime_error
      exception will be thrown if these conditions are not met.
    */
    //@{
    template <class _matrixOwnerType>
    inline void Solve(vctDynamicMatrixBase<_matrixOwnerType, double> &A) throw (std::runtime_error) {
        /* check that the size and storage order matches with Allocate() */
        if (A.IsRowMajor() != StorageOrder) {
            cmnThrow(std::runtime_error("nmrSVDSolver Solve: Storage order used for Allocate was different"));
        }

        /* check sizes based on storage order, there is a more compact
           expression for this test but I find this easier to read and
           debug (Anton) */
        if (A.IsColMajor()) {
            if ((M != (int) A.rows()) || (N != (int) A.cols())) {
                cmnThrow(std::runtime_error("nmrSVDSolver Solve: Size used for Allocate was different"));
            }
        } else if (A.IsRowMajor()) {
            if ((M != (int) A.cols()) || (N != (int) A.rows())) {
                cmnThrow(std::runtime_error("nmrSVDSolver Solve: Size used for Allocate was different"));
            }
        }
        
        /* check that the matrices are Fortran like */
        if (! A.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDSolver Solve: Requires a compact matrix"));
        }

        /* call the LAPACK C function */
#if CISST_HAS_CNETLIB
        dgesvd_(&Jobu, &Jobvt, &M, &N,
                A.Pointer(), &Lda, S.Pointer(),
                U.Pointer(), &Ldu,
                Vt.Pointer(), &Ldvt,
                Work.Pointer(), &Lwork, &Info);
#elif CISST_HAS_CISSTNETLIB
        ftnlen jobu_len = (ftnlen)1, jobvt_len = (ftnlen)1;
        la_dzlapack_MP_sgesvd_nat(&Jobu, &Jobvt, &M, &N,
                                  A.Pointer(), &Lda, S.Pointer(),
                                  U.Pointer(), &Ldu,
                                  Vt.Pointer(), &Ldvt,
                                  Work.Pointer(), &Lwork, &Info,
                                  jobu_len, jobvt_len);
#endif

	}
    

    template <unsigned int _rows, unsigned int _cols, bool _storageOrder>
    inline void Solve(vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> & A) {
        vctDynamicMatrixRef<double> Aref(A);
        Solve(Aref);
    }
    //@}

    
    inline const vctDynamicMatrix<double> &GetS(void) const {
        return S;
    }

    inline const vctDynamicMatrix<double> &GetU(void) const {
        return (StorageOrder == VCT_COL_MAJOR) ? U : Vt;
    }

    inline const vctDynamicMatrix<double> &GetVt(void) const {
        return (StorageOrder == VCT_COL_MAJOR) ? Vt : U;
    }
};


#ifdef CISST_COMPILER_IS_MSVC
class CISST_DEPRECATED nmrSVDSolver;
#endif // CISST_COMPILER_IS_MSVC


#endif // _nmrSVDSolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrSVDSolver.h,v $
//  Revision 1.20  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.19  2006/11/20 20:33:20  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.18  2006/09/15 16:35:14  anton
//  cisstNumerical: Changed name of LAPACK3e routines to reflect changes in
//  g95 naming conventions.  WARNING: Requires latest version of cisstNetlib.
//
//  Revision 1.17  2006/01/31 21:12:47  anton
//  cisstNumerical: Added flags for deprecated symbols to avoid warnings when
//  the deprecated classes are not really used.
//
//  Revision 1.16  2006/01/27 00:59:49  anton
//  cisstNumerical: Renamed "solution" to "data".  See #205.
//
//  Revision 1.15  2006/01/20 15:11:11  anton
//  cisstNumerical: Marked nmrSVDSolver, nmrLUSolver and nmrPInverseSolver as
//  deprecated.
//
//  Revision 1.14  2005/12/23 21:33:07  anton
//  nmrSVDSolver.h:  Marked nmrSVDSolver as deprecated.
//
//  Revision 1.13  2005/11/29 03:06:56  anton
//  cisstNumerical:  Systematic use of nmrNetlib.h and, modified all "min" and "max"
//  to use STL ones.
//
//  Revision 1.12  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.11  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.10  2005/09/24 00:04:23  anton
//  cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
//  cisstVector.h (use more specialized vctXyz.h).
//
//  Revision 1.9  2005/08/15 06:06:16  anton
//  nmrSVDSolver.h: Constructor and Allocate are templated for dynamic matrices
//  to allow different matrix owners.  This is now consistent with the signature
//  of Solve().
//
//  Revision 1.8  2005/07/29 19:04:27  anton
//  cisstNumerical: Updated doc for Doxygen and replaced tabs by spaces.
//
//  Revision 1.7  2005/07/25 19:46:49  anton
//  nmrSVDSolver: Added support for any storage order and fixed size matrices.
//  Also replaced *assert* with exceptions *std::runtime_error*.
//
//  Revision 1.6  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.5  2005/01/10 19:19:55  anton
//  cisstNumerical: Updates to allow compilation as a Dll.
//
//  Revision 1.4  2004/11/08 18:06:54  anton
//  cisstNumerical: Major Doxygen update based on Ankur's notes and Ofri's
//  comments in the code.
//
//  Revision 1.3  2004/11/03 22:21:25  anton
//  cisstNumerical: Update the cnetlib front-end to separate allocation and
//  solve. Uses VCT_COL_MAJOR and tests if the matrices are Fortran like.
//
//  Revision 1.2  2004/11/01 17:44:48  anton
//  cisstNumerical: Added some Solvers and the files to use our own cnetlib
//  code added to the cisst CVS repository (module "cnetlib").
//
//  Revision 1.1  2004/10/27 14:42:23  anton
//  cisstNumerical: Added nmrSVDSolver based on netlib/CLAPACK function.
//
//
// ****************************************************************************




