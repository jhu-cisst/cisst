/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Ankur Kapoor
  Created on: 2005-10-18

  (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrLSqLin
*/


#ifndef _nmrLSqLin_h
#define _nmrLSqLin_h

#include <algorithm>
#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

// Always include last
#include <cisstNumerical/nmrExport.h>

/*!
  \ingroup cisstNumerical

  \note *On exit*, the contents of *A and b are altered.*

  There are three ways to call this method to solve the least-squares problem Ax = b, possibly
  subject to constraints.
     LS:   Solves Ax = b
     LSI:  Solves Ax = b, subject to Gx >= h (inequality constraint)
     LSEI: Solves Ax = b, subject to Ex = f and Gx >= h (inequality and equality constraints)
  Following examples illustrate the LS solution.

  METHOD 1: User provides input and output matrices/vectors
     1) The User allocates memory for these matrices and vectors.
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4, VCT_COL_MAJOR);
     vctDynamicVector<CISSTNETLIB_DOUBLE> b(5);
     vctDynamicVector<CISSTNETLIB_DOUBLE> x(4);
     2) The user calls the LS routine
     nmrLSqLin(A, b, x);
     The LSqLin method verifies that the size of the solution object matches
     the input, and allocates workspace memory, which is deallocated when
     the function ends.
     The LSqLin function alters the contents of matrix A and vector b.
     For fixed size the function call is templated by size, e.g.
     nmrLSqLin<4, 3>(A, b, x);

  METHOD 2: Using a preallocated solution object
     1) The user creates the input matrix/vector
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(rows, cols, VCT_COL_MAJOR);
     vctDynamicVector<CISSTNETLIB_DOUBLE> b(rows);
     2) The user allocates a solution object which could be of
     type nmrLSqLinSolutionFixedSize, nmrLSqLinSolutionDynamic and nmrLSqLinSolutionDynamicRef
     corresponding to fixed size, dynamic matrix or dynamic matrix reference.
     nmrLSqLinSolutionDynamic solution(A);
     3) Call the nmrLSqLin function
     nmrLSqLin(A, b, solution);
     The contents of the input matrix A and vector b are modified by this routine.
     The output vector is available through solution.GetX().

  METHOD 3: User provides input and output matrices/vectors and
     workspace required by LAPACK routine.
     1) User creates input and output matrices and vector
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4, VCT_COL_MAJOR);
     vctDynamicVector<CISSTNETLIB_DOUBLE> b(5);
     vctDynamicVector<CISSTNETLIB_DOUBLE> x(4);
     2) User also needs to allocate memory for the workspace. This method is particularly
     useful when the user is using more than one numerical method from the library
     and is willing to share the workspace between them. In such a case, the user
     can allocate memory greater than the maximum required by the different methods.
     To aid the user to determine the minimum workspace required (and not spend time digging
     into LAPACK documentation) the library provides helper function
     nmrLSqLinSolutionDynamic::GetWorkspaceSize(input)
     vctDynamicVector<CISSTNETLIB_DOUBLE> Work(nmrLSqLinSolutionDynamic::GetWorkspaceSize(A));
     3) Call the LS function
     nmrLSqLin(A, b, x, Work);
     or
     For fixed size the above two steps are replaced by
     nmrLSqLinSolutionFixedSize<5, 0, 0, 4>::TypeWork Work;
     nmrLSqLin<5, 4>(A, b, x, Work);

     \note The LSqLin functions make use of LAPACK routines.  To activate this
     code, set the CISST_HAS_CISSTNETLIB flag to ON during the configuration
     with CMake.
     \note The general rule for numerical functions which depend on LAPACK is that
     column-major matrices should be used everywhere, and that all
     matrices should be compact.
 */

/*
   ****************************************************************************
                                  DYNAMIC SIZE
   ****************************************************************************
 */

/*! This is the class for the composite solution container of LSqLin.
 */
class nmrLSqLinSolutionDynamic {
protected:
    /*!
      Memory allocated for Workspace matrices if needed
    */
    vctDynamicVector<CISSTNETLIB_DOUBLE> WorkspaceMemory;
    vctDynamicVector<CISSTNETLIB_INTEGER> IWorkspaceMemory;
    /*!
      Memory allocated for output X if needed.
    */
    vctDynamicVector<CISSTNETLIB_DOUBLE> OutputMemory;
    vctDynamicVector<CISSTNETLIB_DOUBLE> RNorm;
    /*!
      Memory allocated for input if needed
      The LSEI (constrained least squares) requires
      that the input matrices be in one continuous
      memory block ordered according to fortran order (Column
      Major format), such that first Mc rows and N columns
      represent A, Ma rows and N+1 th column represent b,
      next Me rows represent (E, f) and last Mg rows represent
      (G, h) where the original LSEI problem is
      arg min || A x - b ||, s.t. E x = f and G x >= h.
      The input for LSI is similar other than Me == 0.
    */
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> InputMemory;

    /*! References to work or return or input types, these point either
      to user allocated memory or our memory chunks if needed
    */
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> A;
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> E;
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> G;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> b;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> f;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> h;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> X;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> RNormL;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> RNormE;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> Work;
    vctDynamicVectorRef<CISSTNETLIB_INTEGER> IWork;

    /* Just store Ma, Me, Mg, and N, which are needed
       to check if A matrix passed to solve method matches
       the allocated size.
       For LS problem Me == 0 and Mg ==0
       For LSI problem Me == 0
       Otherwise LSEI problem
    */
    size_t m_Ma;
    size_t m_Me;
    size_t m_Mg;
    size_t m_N;

public:
    /*! Helper methods for user to set min working space required by LAPACK
      LS routine.
      \param ma, me, mg, n The size of matrix whose LS/LSI/LSEI needs to be computed
    */

    static inline CISSTNETLIB_INTEGER GetWorkspaceSize(size_t ma, size_t me, size_t mg, size_t n)
    {
        CISSTNETLIB_INTEGER lwork;
        if ((me == 0) && (mg ==0)) { // case LS (dgels)
            // Following produces same result as QueryWorkspaceSize_LS().
            CISSTNETLIB_INTEGER minmn = static_cast<CISSTNETLIB_INTEGER>(std::max(std::min(ma,n),static_cast<size_t>(1)));
            lwork = 2*minmn;
        } else if (me == 0) { // case LSI
            size_t k = std::max(ma+mg,n);
            lwork = static_cast<CISSTNETLIB_INTEGER>(k+n+(mg+2)*(n+7));
        } else { // case LSEI
            size_t k = std::max(ma+mg,n);
            lwork = static_cast<CISSTNETLIB_INTEGER>(2*(me+n)+k+(mg+2)*(n+7));
        }
        return lwork;
    }

    static inline CISSTNETLIB_INTEGER GetIWorkspaceSize(size_t CMN_UNUSED(ma), size_t me, size_t mg, size_t n)
    {
        if ((me == 0) && (mg ==0)) { // case LS
            return 0;
        } else if (me == 0) { // case LSI
            return static_cast<CISSTNETLIB_INTEGER>(mg+2*n+1);
        } else { // case LSEI
            return static_cast<CISSTNETLIB_INTEGER>(mg+2*n+2);
        }
    }

    /*! Method to query LAPACK routine for optimal workspace size. So far, only implemented for LS (dgels).
        Not needed because result is same as GetWorkspaceSize() above. */
    static CISSTNETLIB_INTEGER QueryWorkspaceSize_LS(size_t ma, size_t n)
    {
        // @see http://www.netlib.org/clapack/CLAPACK-3.1.1/SRC/dgels.c
        // ma = rows of matrix A, aka M
        // n  = cols of matrix A, aka N
        char trans = 'N';
        CISSTNETLIB_INTEGER Mrows = static_cast<CISSTNETLIB_INTEGER>(ma);
        CISSTNETLIB_INTEGER Ncols = static_cast<CISSTNETLIB_INTEGER>(n);
        CISSTNETLIB_INTEGER nrhs = 1; ///< @todo can we assume nhrs=1?
        CISSTNETLIB_INTEGER lda = static_cast<CISSTNETLIB_INTEGER>(std::max(ma,static_cast<size_t>(1)));
        CISSTNETLIB_INTEGER ldb = static_cast<CISSTNETLIB_INTEGER>(std::max(std::max(static_cast<size_t>(1),ma),n));
        CISSTNETLIB_INTEGER lwork = -1; // calculate what lwork should be
        CISSTNETLIB_INTEGER info;
        CISSTNETLIB_DOUBLE work;

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
        cisstNetlib_dgels_(&trans, &Mrows, &Ncols, &nrhs,
                                   0, &lda,
                                   0, &ldb,
                                   &work, &lwork, &info);
#endif
#else // no major version
        dgels_(&trans, &Mrows, &Ncols, &nrhs,
                      0, &lda,
                      0, &ldb,
                      &work, &lwork, &info);
#endif // CISSTNETLIB_VERSION
        CMN_ASSERT(info == 0);
        // optimal work array size lwork is stored in first entry of work array
        return static_cast<CISSTNETLIB_INTEGER>(work);
    }

    /*! Helper method to determine the min working space required by LAPACK LS (dgels) routine.
      \param inA The matrix whose LS needs to be computed
    */
    template <typename _matrixOwnerTypeA>
    static inline CISSTNETLIB_INTEGER GetWorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA)
    {
        return nmrLSqLinSolutionDynamic::GetWorkspaceSize(inA.rows(), 0, 0, inA.cols());
    }
    /*! Helper method to determine the min working space required by LAPACK LSI routine.
      \param inA, inG The input matrices for LSI.
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG>
    static inline CISSTNETLIB_INTEGER GetWorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                            vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG)
    {
        return nmrLSqLinSolutionDynamic::GetWorkspaceSize(inA.rows(), 0, inG.rows(), inA.cols());
    }
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG>
    static inline CISSTNETLIB_INTEGER GetIWorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG)
    {
        return nmrLSqLinSolutionDynamic::GetIWorkspaceSize(inA.rows(), 0, inG.rows(), inA.cols());
    }
    /*! Helper method to determine the min working space required by LAPACK LSEI routine.
      \param inA, inE, inG The input matrices for LSEI.
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE, typename _matrixOwnerTypeG>
    static inline CISSTNETLIB_INTEGER GetWorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                            vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &inE,
                                            vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG)
    {
        return nmrLSqLinSolutionDynamic::GetWorkspaceSize(inA.rows(), inE.rows(), inG.rows(), inA.cols());
    }
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE, typename _matrixOwnerTypeG>
    static inline CISSTNETLIB_INTEGER GetIWorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                             vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &inE,
                                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG)
    {
        return nmrLSqLinSolutionDynamic::GetIWorkspaceSize(inA.rows(), inE.rows(), inG.rows(), inA.cols());
    }

    /*! Helper methods for user to set min working space required by LAPACK LS/LSI/LSEI routine.
      \param ma, me, mg, n The size of matrix whose LS/LSI/LSEI needs to be computed
      \param inWork A vector that would be resized to meet the requirements of
      LAPACK LS/LSI/LSEI routine.
    */
    static inline void AllocateWorkspace(CISSTNETLIB_INTEGER ma, CISSTNETLIB_INTEGER me, CISSTNETLIB_INTEGER mg, CISSTNETLIB_INTEGER n, vctDynamicVector<CISSTNETLIB_DOUBLE> &inWork)
    {
        inWork.SetSize(nmrLSqLinSolutionDynamic::GetWorkspaceSize(ma,me,mg,n));
    }
    static inline void AllocateIWorkspace(CISSTNETLIB_INTEGER ma, CISSTNETLIB_INTEGER me, CISSTNETLIB_INTEGER mg, CISSTNETLIB_INTEGER n, vctDynamicVector<CISSTNETLIB_INTEGER> &inIWork)
    {
        inIWork.SetSize(nmrLSqLinSolutionDynamic::GetIWorkspaceSize(ma,me,mg,n));
    }
    /*! Helper methods for user to set min working space required by LAPACK LS routine.
      \param inA The matrix whose LS needs to be computed
      \param inWork A vector that would be resized to meet the requirements of
      LAPACK LS routine.
    */
    template <typename _matrixOwnerTypeA>
    static inline void AllocateWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA, vctDynamicVector<CISSTNETLIB_DOUBLE> &inWork)
    {
        inWork.SetSize(nmrLSqLinSolutionDynamic::GetWorkspaceSize(inA));
    }
    /*! Helper methods for user to set min working space required by LAPACK LSI routine.
      \param inA, inG The input matrices for LSI.
      \param inWork A vector that would be resized to meet the requirements of
      LAPACK LSI routine.
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG>
    static inline void AllocateWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                         vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                                         vctDynamicVector<CISSTNETLIB_DOUBLE> &inWork)
    {
        inWork.SetSize(nmrLSqLinSolutionDynamic::GetWorkspaceSize(inA, inG));
    }
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG>
    static inline void AllocateIWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                                          vctDynamicVector<CISSTNETLIB_INTEGER> &inIWork)
    {
        inIWork.SetSize(nmrLSqLinSolutionDynamic::GetIWorkspaceSize(inA, inG));
    }
    /*! Helper methods for user to set min working space required by LAPACK LSEI routine.
      \param inA, inE, inG  The input matrices for LSEI.
      \param inWork A vector that would be resized to meet the requirements of
      LAPACK LSEI routine.
    */
    template <typename _matrixOwnerTypeA,  typename _matrixOwnerTypeE,  typename _matrixOwnerTypeG>
    static inline void AllocateWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                         vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &inE,
                                         vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                                         vctDynamicVector<CISSTNETLIB_DOUBLE> &inWork)
    {
        inWork.SetSize(nmrLSqLinSolutionDynamic::GetWorkspaceSize(inA, inE, inG));
    }
    template <typename _matrixOwnerTypeA,  typename _matrixOwnerTypeE,  typename _matrixOwnerTypeG>
    static inline void AllocateIWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                                          vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &inE,
                                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                                          vctDynamicVector<CISSTNETLIB_INTEGER> &inIWork)
    {
        inIWork.SetSize(nmrLSqLinSolutionDynamic::GetIWorkspaceSize(inA, inE, inG));
    }

    /*! This class is not intended to be a top-level API.
      It has been provided to avoid making the templated
      LSqLin function as a friend of this class, which turns
      out to be not so easy in .NET. Instead the Friend class
      provides a cumbersome way to get non-const references
      to the private data.
      In order to get non-const references the user has
      to first create an object of nmrLSqLinSolutionDynamic::Friend
      and then use the get* method on that object. Our philosophy
      here is that this should be a deterent for a general user
      and should ring alarm bells in a reasonable programmer.
    */
    class Friend {
    private:
        nmrLSqLinSolutionDynamic &solution;
    public:
        Friend(nmrLSqLinSolutionDynamic &insolution):solution(insolution) {
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &GetA(void) {
            return solution.A;
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &GetE(void) {
            return solution.E;
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &GetG(void) {
            return solution.G;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &Getb(void) {
            return solution.b;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &Getf(void) {
            return solution.f;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &Geth(void) {
            return solution.h;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &GetX(void) {
            return solution.X;
        }
        inline vctDynamicVector<CISSTNETLIB_DOUBLE> &GetRNorm(void) {
            return solution.RNorm;
        }
        inline vctDynamicMatrix<CISSTNETLIB_DOUBLE> &GetInput(void) {
            return solution.InputMemory;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &GetWork(void) {
            return solution.Work;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_INTEGER> &GetIWork(void) {
            return solution.IWork;
        }
        inline CISSTNETLIB_INTEGER GetMa(void) {
            return static_cast<CISSTNETLIB_INTEGER>(solution.m_Ma);
        }
        inline CISSTNETLIB_INTEGER GetMe(void) {
            return static_cast<CISSTNETLIB_INTEGER>(solution.m_Me);
        }
        inline CISSTNETLIB_INTEGER GetMg(void) {
            return static_cast<CISSTNETLIB_INTEGER>(solution.m_Mg);
        }
        inline CISSTNETLIB_INTEGER GetN(void) {
            return static_cast<CISSTNETLIB_INTEGER>(solution.m_N);
        }
    };
    friend class Friend;

    /*! The default constuctor.
      For dynamic size, there are assigned default values,
      which MUST be changed by calling appropriate methods.
      (See nmrLSqLinSolutionDynamic::Allocate and
      nmrLSqLinSolutionDynamic::SetRef)
    */
    nmrLSqLinSolutionDynamic():
        m_Ma(0),
        m_Me(0),
        m_Mg(0),
        m_N(0) {};

    /*! constructor to use with LS */
    nmrLSqLinSolutionDynamic(size_t ma, size_t n)
    {
        this->Allocate(ma, 0, 0, n);
    }
    /*! constructor to use with LSI */
    nmrLSqLinSolutionDynamic(size_t ma, size_t mg, size_t n)
    {
        this->Allocate(ma, 0, mg, n);
    }
    /*! constructor to use with LSEI */
    nmrLSqLinSolutionDynamic(size_t ma, size_t me, size_t mg, size_t n)
    {
        this->Allocate(ma, me, mg, n);
    }

    /************************************************************************/
    /* The following are various constructors for the LS problem            */
    /************************************************************************/

    /*! Constructor where user provides the input matrix to specify size.
      Memory allocation is done for output vector as well as for the
      Workspace used by LAPACK. This case covers the scenario where the user
      wants to make all system calls for memory allocation before entering
      time critical code sections.
      \param A input matrix
    */
    template <typename _matrixOwnerTypeA>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        this->Allocate(A.rows(), 0, 0, A.cols());
    }
    /*! Constructor where user provides the input matrix to specify size.
      Memory allocation is done for the workspace, inWork.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing him/her to share the workspace for LAPACK.
      \param A input matrix
      \output inWork workspace for LS
    */
    template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeWork>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork)
    {
        this->Allocate(A.rows(), 0, 0, A.cols(), inWork);
    }
    /*! Constructor where user provides the size of the input matrix, along with the
      output vector X and workspace.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the size of the input matrix.
      \param ma, n The size of input matrix
      \param inX The output vector for LSqLin
      \param inWork The workspace for LAPACK.
    */
    template <typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork>
    nmrLSqLinSolutionDynamic(size_t ma, size_t n,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork)
    {
        this->SetRef(ma, 0, 0, n, inX, inWork);
    }
    /*! Constructor where user provides the size of the input matrix,
      along with vector X.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LS algorithm. Checks are made on the
      validity of the input and its consistency with the size of input matrix.
      Memory allocation for the workspace is done by the method.
      This case covers the scenario where user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using the LS matrix elsewhere in the *same* thread.
      \param ma, n The size of input matrix
      \param inX The output vector for LSqLin
    */
    template <typename _vectorOwnerTypeX>
    nmrLSqLinSolutionDynamic(size_t ma, size_t n,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX)
    {
        this->SetRef(ma, 0, 0, n, inX);
    }
    /*! Constructor where user provides the input matrix to specify size,
      along with vector X and workspace.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LS algorithm. Checks are made on the
      validity of the input and its consistency with the size of input matrix.
      \param inA The input matrix
      \param inX The output vector for LSqLin
      \param inWork The workspace for LAPACK.
    */
    template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork)
    {
        this->SetRef(inA.rows(), 0, 0, inA.cols(), inX, inWork);
    }

    /************************************************************************/
    /* The following are various constructors for the LSI problem           */
    /************************************************************************/

    /*! Constructor where user provides the input matrices to specify size.
      Memory allocation is done for the output vector as well as for the
      Workspace used by LAPACK. This case covers the scenario where the user
      wants to make all system calls for memory allocation before entering
      time critical code sections.
      \param A, G input matrices
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G)
    {
        this->Allocate(A.rows(), 0, G.rows(), A.cols());
    }
    /*! Constructor where user provides the input matrices to specify size and the workspaces.
      Memory allocation is done for the output vectors.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing him/her to share the workspace for LAPACK.
      \param A, G input matrices
      \output inWork, inIWork workspace for LSI
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG,
              typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                             vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_INTEGER> &inIWork)
    {
        this->Allocate(A.rows(), 0, G.rows(), A.cols(), inWork, inIWork);
    }
    /*! Constructor where user provides the size of the input matrices
      along with output X and workspaces.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the size of the input matrices.
      \param ma, mg, n The size of input matrices
      \param inX The output vector for LSqLin
      \output inWork, inIWork workspace for LSI
    */
    template <typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    nmrLSqLinSolutionDynamic(size_t ma, size_t mg, size_t n,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                             vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_INTEGER> &inIWork)
    {
        this->SetRef(ma, 0, mg, n, inX, inWork, inIWork);
    }
    /*! Constructor where user provides the size and storage order of the input matrices,
      along with vector X.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSI algorithm. Checks are made on the
      validity of the input and its consistency with the size of input matrices.
      Memory allocation for the workspace is done by the method.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using the LSI matrix elsewhere in the *same* thread.
      \param ma, mg, n The size of input matrices
      \param inX The output vector for LSqLin
    */
    template <typename _vectorOwnerTypeX>
    nmrLSqLinSolutionDynamic(size_t ma, size_t mg, size_t n,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX)
    {
        this->SetRef(ma, 0, mg, n, inX);
    }
    /*! Constructor where user provides the input matrices to specify size,
      along with vector X.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSI algorithm. Checks are made on the
      validity of the input and its consistency with the size of the input matrix.
      \param inA, inG The input matrices
      \param inX The output vector for LSqLin
      \output inWork, inIWork workspace for LSI
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG,
              typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork,
              typename _vectorOwnerTypeIWork>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                             vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_INTEGER> &inIWork)
    {
        this->SetRef(inA.rows(), 0, inG.rows(), inA.cols(), inX, inWork, inIWork);
    }

    /************************************************************************/
    /* The following are various constructors for LSEI problem              */
    /************************************************************************/

    /*! Constructor where user provides the input matrices to specify size.
      Memory allocation is done for the output vector as well as the
      Workspace used by LAPACK. This case covers the scenario where the user
      wants to make all system calls for memory allocation before entering
      time critical code sections.
      \param A, E, G input matrices
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE, typename _matrixOwnerTypeG>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &E,
                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G)
    {
        this->Allocate(A.rows(), E.rows(), G.rows(), A.cols());
    }
    /*! Constructor where user provides the input matrices to specify size
      and the workspaces.
      Memory allocation is done for the output vector.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing him/her to share the workspace for LAPACK.
      \param A, E, G input matrices
      \output inWork, inIWork workspace for LSI
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE,
              typename _matrixOwnerTypeG, typename _vectorOwnerTypeWork,
              typename _vectorOwnerTypeIWork>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &E,
                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                             vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_INTEGER> &inIWork)
    {
        this->Allocate(A.rows(), E.rows(), G.rows(), A.cols(), inWork, inIWork);
    }
    /*! Constructor where user provides the size of the input matrices, the output vector X,
      and the workspaces.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the sizes of the input matrices.
      \param ma, me, mg, n The sizes of input matrices
      \param inX The output vector for LSqLin
      \output inWork, inIWork workspace for LSI
    */
    template <typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    nmrLSqLinSolutionDynamic(size_t ma, size_t me, size_t mg, size_t n,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                             vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_INTEGER> &inIWork)
    {
        this->SetRef(ma, me, mg, n, inX, inWork, inIWork);
    }
    /*! Constructor where user provides the size of the input matrices,
      along with vector X.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSEI algorithm. Checks are made on the
      validity of the input and its consistency with the size of input matrices.
      Memory allocation for the workspace is done by the method.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using the LSEI matrix elsewhere in the *same* thread.
      \param ma, me, mg, n The size of input matrices
      \param inX The output vector for LSqLin
    */
    template <typename _vectorOwnerTypeX>
    nmrLSqLinSolutionDynamic(size_t ma, size_t me, size_t mg, size_t n,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX)
    {
        this->SetRef(ma, me, mg, n, inX);
    }
    /*! Constructor where user provides the input matrices to specify size,
      along with vector X and the workspaces
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSEI algorithm. Checks are made on the
      validity of the input and its consistency with the sizes of input matrices.
      \param inA, inE, inG The input matrices
      \param inX The output vector for LSqLin
      \output inWork, inIWork workspace for LSI
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE,
              typename _matrixOwnerTypeG, typename _vectorOwnerTypeX,
              typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    nmrLSqLinSolutionDynamic(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                             vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &inE,
                             vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                             vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                             vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                             vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_INTEGER> &inIWork)
    {
        this->SetRef(inA.rows(), inE.rows(), inG.rows(), inA.cols(), inX, inWork, inIWork);
    }

    /************************************************************************/
    /* The following are Allocate methods for the LS problem                */
    /************************************************************************/

    /*! This method allocates memory of the output vector
      as well as the workspace.
      This method should be called before the nmrLSqLinSolutionDynamic
      object is passed to the nmrLSqLin function, as the memory
      required for the output matrices and workspace are allocated
      here or to reallocate memory previously allocated by the constructor.
      Typically, this method is called from a code segment
      where it is safe to allocate memory and use
      the solution and workspace later.
      \param A The matrix for which LS needs to be computed, size MxN
    */
    template <typename _matrixOwnerTypeA>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        this->Allocate(A.rows(), 0, 0, A.cols());
    }
    /*! This method allocates memory of the output vector
      and uses the memory provided by the user for the workspace.
      Check is made to ensure that memory provided by user is sufficient
      for LS routine of LAPACK.
      This method should be called before the nmrLSqLinSolutionDynamic
      object is passed on to the nmrLSqLin function, as the memory
      required for the output vector and workspace are allocated
      here or to reallocate memory previously allocated by the constructor.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing him/her to share the workspace for LAPACK.
      Typically this method is called from a code segment
      where it is safe to allocate memory and use
      the solution and workspace later.
      \param A The matrix for which LS needs to be computed, size MxN
      \param inWork The vector used for workspace by LS.
    */
    template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeWork>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                         vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork)
    {
        this->Allocate(A.rows(), 0, 0, A.cols(), inWork);
    }

    /************************************************************************/
    /* The following are SetRef methods for the LS problem                  */
    /************************************************************************/

    /*! This method must be called before the solution object
      is passed to the nmrLSqLin function.
      The user provides the input matrix to specify size,
      along with vector X and workspace.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the size of the input matrix.
      \param inA The input matrix
      \param inX The output vector for LSqLin
      \param inWork The workspace for LS.
    */
    template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork)
    {
        this->SetRef(inA.rows(), 0, 0, inA.cols(), inX, inWork);
    }
    /*! This method must be called before the solution object
      is passed to the nmrLSqLin function.
      The user provides the input matrix to specify size,
      along with vector X.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the size of the input matrix.
      Memory allocation for the workspace is done by the method.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using the LS matrix elsewhere in the *same* thread.
      \param inA The input matrix
      \param inX The output matrix for LSqLin
    */
    template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeX>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX)
    {
        this->SetRef(inA.rows(), 0, 0, inA.cols(), inX, this->WorkspaceMemory);
    }

    /************************************************************************/
    /* The following are Allocate methods for the LSI problem               */
    /************************************************************************/

    /*! This method allocates memory of the output vector
      as well as the workspace.
      This method should be called before the nmrLSqLinSolutionDynamic
      object is passed to the nmrLSqLin function, as the memory
      required for the output vector and workspace are allocated
      here or to reallocate memory previously allocated by the constructor.
      Typically, this method is called from a code segment
      where it is safe to allocate memory and use
      the solution and workspace later.
      \param A The matrix for which LSI needs to be computed, size Ma x N
      \param G The constraints matrix for LSI, size Mg x N
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                         vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G)
    {
        this->Allocate(A.rows(), 0, G.rows(), A.cols());
    }
    /*! This method allocates memory of the output vector
      and uses the memory provided by the user for the workspace.
      Check is made to ensure that memory provided by user is sufficient
      for LSI routine of LAPACK.
      This method should be called before the nmrLSqLinSolutionDynamic
      object is passed to the nmrLSqLin function, as the memory
      required for the output vector and workspace are allocated
      here or to reallocate memory previously allocated by the constructor.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing him/her to share the workspace for LAPACK.
      Typically this method is called from a code segment
      where it is safe to allocate memory and use
      the solution and workspace later.
      \param A The matrix for which LSI needs to be computed, size Ma x N
      \param G The constraints matrix  for LSI, size Mg x N
      \param inWork, inIWork The workspace for LSI.
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG,
              typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                         vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                         vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                         vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inIWork)
    {
        this->Allocate(A.rows(), 0, G.rows(), A.cols(), inWork, inIWork);
    }

    /************************************************************************/
    /* The following are SetRef methods for the LSI problem                 */
    /************************************************************************/

    /*! This method must be called before the solution object
      is passed to the nmrLSqLin function.
      The user provides the input matrices to specify size,
      along with vector X and workspace.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the size of input matrices.
      \param inA, inG The input matrices
      \param inX The output vector for LSqLin
      \param inWork, inIWork The workspace for LSI.
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG,
              typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork,
              typename _vectorOwnerTypeIWork>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inIWork)
    {
        this->SetRef(inA.rows(), 0, inG.rows(), inA.cols(), inX, inWork, inIWork);
    }
    /*! This method must be called before the solution object
      is passed to the nmrLSqLin function.
      The user provides the input matrices to specify size,
      along with vector X.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the size of the input matrices.
      Memory allocation for the workspace is done by the method.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using the LS matrix elsewhere in the *same* thread.
      \param inA, inG The input matrices
      \param inX The output matrix for LSqLin
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeG, typename _vectorOwnerTypeX>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX)
    {
        this->SetRef(inA.rows(), 0, inG.rows(), inA.cols(), inX);
    }

    /************************************************************************/
    /* The following are Allocate methods for the LSEI problem              */
    /************************************************************************/

    /*! This method allocates memory of output vector
      as well as the workspace.
      This method should be called before the nmrLSqLinSolutionDynamic
      object is passed to the nmrLSqLin function, as the memory
      required for the output vector and workspace are allocated
      here or to reallocate memory previously allocated by the constructor.
      Typically, this method is called from a code segment
      where it is safe to allocate memory and use
      the solution and workspace later.
      \param A The matrix for which LSEI needs to be computed, size MxN
      \param E The equality constraints matrix  for LSEI, size Me x N
      \param G The inequality constraints matrix  for LSEI, size Mg x N
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE, typename _matrixOwnerTypeG>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                         vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &E,
                         vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G)
    {
        this->Allocate(A.rows(), E.rows(), G.rows(), A.cols());
    }
    /*! This method allocates memory of output vector
      and uses the memory provided by user for workspace.
      Check is made to ensure that memory provided by user is sufficient
      for LSEI routine of LAPACK.
      This method should be called before the nmrLSqLinSolutionDynamic
      object is passed to the nmrLSqLin function, as the memory
      required for the output vector and workspace are allocated
      here or to reallocate memory previously allocated by the constructor.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing him/her to share the workspace for LAPACK.
      Typically this method is called from a code segment
      where it is safe to allocate memory and use
      the solution and workspace later.
      \param A The matrix for which LSEI needs to be computed, size MxN
      \param E The equality constraints matrix for LSEI, size Me x N
      \param G The inequality constraints matrix for LSEI, size Mg x N
      \param inWork, inIWork The vector used for workspace by LSEI.
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE,
              typename _matrixOwnerTypeG, typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                         vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &E,
                         vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                         vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                         vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_DOUBLE> &inIWork)
    {
        this->Allocate(A.rows(), E.rows(), G.rows(), A.cols(), inWork, inIWork);
    }

    /************************************************************************/
    /* The following are SetRef methods for the LSEI problem                */
    /************************************************************************/

    /*! This method must be called before the solution object
      is passed to the nmrLSqLin function.
      The user provides the input matrices to specify size,
      along with vector X and workspace.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the size of the input matrices.
      \param inA, inE, inG The input matrices
      \param inX The output vector for LSqLin
      \param inWork, inIWork The workspace for LSI.
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE, typename _matrixOwnerTypeG,
              typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork,
              typename _vectorOwnerTypeIWork>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &inE,
                vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_DOUBLE> &inIWork)
    {
        this->SetRef(inA.rows(), inE.rows(), inG.rows(), inA.cols(), inX, inWork, inIWork);
    }
    /*! This method must be called before the solution object
      is passed to the nmrLSqLin function.
      The user provides the input matrices to specify size,
      along with vector X.
      The solution object now acts as a composite container to hold, pass and
      manipulate a convenient storage for the LSqLin algorithm. Checks are made on the
      validity of the input and its consistency with the sizes of the input matrices.
      Memory allocation for the workspace is done by the method.
      This case covers the scenario where the user wants to make all system
      calls for memory allocation before entering time critical code sections
      and might be using the LSEI matrix elsewhere in the *same* thread.
      \param inA, inE, inG The input matrices
      \param inX The output matrix for LSqLin
    */
    template <typename _matrixOwnerTypeA, typename _matrixOwnerTypeE,
              typename _matrixOwnerTypeG, typename _vectorOwnerTypeX>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &inA,
                vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &inE,
                vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &inG,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX)
    {
        this->SetRef(inA.rows(), inE.rows(), inG.rows(), inA.cols(), inX, this->WorkspaceMemory);
    }

    /************************************************************************/
    /* The following is the Base Allocate Method called by others           */
    /************************************************************************/

    /*! This method allocates memory of the output vector
      and optionally for the workspace required by LS/LSI/LSEI.
      This method is not meant to be a top-level user API, but is
      used by other overloaded Allocate methods.
      \param ma Number of rows of input matrix A (objective)
      \param me Number of rows of input matrix E (equality constraints)
      \param mg Number of rows of input matrix G (inequality constraints)
      \param n Number of cols of input matrix A
    */
    inline void Allocate(size_t ma, size_t me, size_t mg, size_t n)
    {
        this->Malloc(ma, me, mg, n, true, true, true);
        this->SetRef(ma, me, mg, n, this->WorkspaceMemory, this->InputMemory, this->OutputMemory);
        (this->IWork).SetRef((this->IWorkspaceMemory));
    }
    /*! This method allocates memory of the output vector
      and optionally for the workspace required by LS/LSI/LSEI.
      This method is not meant to be a top-level user API, but is
      used by other overloaded Allocate methods.
      \param ma Number of rows of input matrix A (objective)
      \param me Number of rows of input matrix E (equality constraints)
      \param mg Number of rows of input matrix G (inequality constraints)
      \param n Number of cols of input matrix A
      \param inWork Workspace provided by user
    */
    template <typename _vectorOwnerTypeWork>
    inline void Allocate(size_t ma, size_t me, size_t mg, size_t n,
                         vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork)
    {
        CISSTNETLIB_INTEGER lwork = nmrLSqLinSolutionDynamic::GetWorkspaceSize(ma, me, mg, n);
        if (lwork > static_cast<CISSTNETLIB_INTEGER>(inWork.size())) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for Work"));
        }
        this->Malloc(ma, me, mg, n, false, true, true);
        this->SetRef(ma, me, mg, n, inWork, this->InputMemory, this->OutputMemory);
    }
    /*! This method allocates memory of the output vector
      and optionally for the workspace required by LS/LSI/LSEI.
      This method is not meant to be a top-level user API, but is
      used by other overloaded Allocate methods.
      \param ma Number of rows of input matrix A (objective)
      \param me Number of rows of input matrix E (equality constraints)
      \param mg Number of rows of input matrix G (inequality constraints)
      \param n Number of cols of input matrix A
      \param inWork, inIWork Workspace provided by user
    */
    template <typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    inline void Allocate(size_t ma, size_t me, size_t mg, size_t n,
                         vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                         vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inIWork)
    {
        CISSTNETLIB_INTEGER lwork = nmrLSqLinSolutionDynamic::GetWorkspaceSize(ma, me, mg, n);
        if (lwork > static_cast<CISSTNETLIB_INTEGER>(inWork.size())) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for Work"));
        }
        CISSTNETLIB_INTEGER liwork = nmrLSqLinSolutionDynamic::GetIWorkspaceSize(ma, me, mg, n);
        if (liwork > static_cast<CISSTNETLIB_INTEGER>(inIWork.size())) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for IWork"));
        }
        this->Malloc(ma, me, mg, n, false, true, true);
        this->SetRef(ma, me, mg, n, inWork, this->InputMemory, this->OutputMemory);
        (this->IWork).SetRef(inIWork);
    }

    /************************************************************************/
    /* The following is the Base SetRef Method called by others             */
    /************************************************************************/

    /*! This method sets memory references of the output vector
      and for the workspace required by LS/LSI/LSEI.
      This method is not meant to be a top-level user API, but is
      used by other overloaded SetRef methods.
      \param ma, me, mg Number of rows of input matrix A, E and G respectively
      \param n Number of cols of input matrix A
      \param inX The output matrix for LSqLin
      \param inWork The workspace for LS.
    */
    template <typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork>
    void SetRef(size_t ma, size_t me, size_t mg, size_t n,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX, vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork)
    {
        CISSTNETLIB_INTEGER lwork = nmrLSqLinSolutionDynamic::GetWorkspaceSize(ma, me, mg, n);
        if (lwork > static_cast<CISSTNETLIB_INTEGER>(inWork.size())) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for Work"));
        }
        if (n > static_cast<CISSTNETLIB_INTEGER>(inX.size())) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for X"));
        }
        this->Malloc(ma, me, mg, n, false, true, false);
        this->SetRef(ma, me, mg, n, inWork, this->InputMemory, inX);
    }
    /*! This method sets memory references of the output vector
      and for the workspace required by LS/LSI/LSEI.
      This method is not meant to be a top-level user API, but is
      used by other overloaded SetRef methods.
      \param ma, me, mg Number of rows of input matrix A, E and G respectively
      \param n Number of cols of input matrix A
      \param inX The output matrix for LSqLin
      \param inWork/inIWork The workspace for LSI/LSEI.
    */
    template <typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork, typename _vectorOwnerTypeIWork>
    void SetRef(size_t ma, size_t me, size_t mg, size_t n,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX,
                vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &inWork,
                vctDynamicVectorBase<_vectorOwnerTypeIWork, CISSTNETLIB_DOUBLE> &inIWork)
    {
        CISSTNETLIB_INTEGER lwork = nmrLSqLinSolutionDynamic::GetWorkspaceSize(ma, me, mg, n);
        if (lwork > static_cast<CISSTNETLIB_INTEGER>(inWork.size())) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for Work"));
        }
        CISSTNETLIB_INTEGER liwork = nmrLSqLinSolutionDynamic::GetIWorkspaceSize(ma, me, mg, n);
        if (liwork > static_cast<CISSTNETLIB_INTEGER>(inIWork.size())) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for IWork"));
        }
        if (n > inX.size()) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for X"));
        }
        this->Malloc(ma, me, mg, n, false, true, false);
        this->SetRef(ma, me, mg, n, inWork, this->InputMemory, inX);
        (this->IWork).SetRef(inIWork);
    }

    /*! This method sets memory references of the output vector
      and for the workspace required by LS/LSI/LSEI.
      This method is not meant to be a top-level user API, but is
      used by other overloaded SetRef methods.
      \param ma, me, mg Number of rows of input matrix A, E and G respectively
      \param n Number of cols of input matrix A
      \param inX The output matrix for LSqLin
    */
    template <typename _vectorOwnerTypeX>
    void SetRef(size_t ma, size_t me, size_t mg, size_t n,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &inX)
    {
        if (n > inX.size()) {
            cmnThrow(std::runtime_error("nmrLSqLin: Incorrect size for X"));
        }
        this->Malloc(ma, me, mg, n, true, true, false);
        this->SetRef(ma, me, mg, n, this->WorkspaceMemory, this->InputMemory, inX);
        (this->IWork).SetRef((this->IWorkspaceMemory));
    }

protected:
    /*! This method allocates memory of the output vector
      and optionally for the workspace required by LS/LSI/LSEI.
      This method is not meant to be a top-level user API, but is
      used by other overloaded Allocate methods.
      \param ma Number of rows of input matrix A (objective)
      \param me Number of rows of input matrix E (equality constraints)
      \param mg Number of rows of input matrix G (inequality constraints)
      \param n Number of cols of input matrix A
      \param allocateWorkspace If true, allocate memory of workspace as well.
      \param allocateInput If true, allocate memory of input as well.
      \param allocateOutput If true, allocate memory of output as well.
    */

    void Malloc(size_t ma, size_t me, size_t mg, size_t n, bool allocateWorkspace, bool allocateInput, bool allocateOutput)
    {
        m_Ma = ma; m_N = n;
        m_Me = me; m_Mg = mg;
        if (allocateWorkspace) {
            (this->WorkspaceMemory).SetSize(nmrLSqLinSolutionDynamic::GetWorkspaceSize(ma, me, mg, n));
            (this->IWorkspaceMemory).SetSize(nmrLSqLinSolutionDynamic::GetIWorkspaceSize(ma, me, mg, n));
        }
        // allocate InputMemory if mg >0 (LSI or LSEI)
        if (mg > 0 && allocateInput) {
            (this->InputMemory).SetSize(ma + me + mg, n+1, VCT_COL_MAJOR);
        }
        if (allocateOutput) {
            (this->OutputMemory).SetSize(n);
        }
        (this->RNorm).SetSize(ma+me);
        (this->RNormL).SetRef(ma, (this->RNorm).Pointer(), 1);
        (this->RNormE).SetRef(me, (this->RNorm).Pointer(ma), 1);
    }
    template <typename _vectorOwnerTypeWork, typename _matrixOwnerTypeI, typename _vectorOwnerTypeX>
    void SetRef(size_t ma, size_t me, size_t mg, size_t n,
                vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &work,
                vctDynamicMatrixBase<_matrixOwnerTypeI, CISSTNETLIB_DOUBLE> &input,
                vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &x)
    {
        //workspace
        (this->Work).SetRef(work);
        // setref InputMemory if mg >0 (LSI or LSEI)
        if (mg > 0) {
            if (me > 0) {
                //size_type startRow, size_type startCol, size_type rows, size_type cols
                //size_type startPosition, size_type length
                (this->E).SetRef(input, 0, 0, me, n);
                (this->f).SetRef(me, input.Column(n).Pointer(0), 1);
                (this->A).SetRef(input, me, 0, ma, n);
                (this->b).SetRef(ma, input.Column(n).Pointer(me), 1);
                (this->G).SetRef(input, ma + me, 0, mg, n);
                (this->h).SetRef(mg, input.Column(n).Pointer(ma + me), 1);
            } else {
                (this->A).SetRef(input, 0, 0, ma, n);
                (this->b).SetRef(ma, input.Column(n).Pointer(0), 1);
                (this->G).SetRef(input, ma, 0, mg, n);
                (this->h).SetRef(mg, input.Column(n).Pointer(ma), 1);
            }
        }
        //output
        (this->X).SetRef(n, x.Pointer(0), 1);
    }

public:
    /*! In order to get access to X, after
      it has been computed by calling the nmrLSqLin function,
      use the following methods.
    */
    inline const vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &GetX(void) const {
        return X;
    }
    inline const vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &GetRNorm(void) const {
        return RNormL;
    }
    inline const vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &GetRNormE(void) const {
        return RNormE;
    }
};

/*! This function checks for valid input and
  calls the LAPACK function. The approach behind this definition of the
  function is that the user creates a solution object from a code
  wherein it is safe to do memory allocation. This solution object
  is then passed to this method along with the matrix and vector whose
  LSqLin is to be computed. The solution object has members X
  and Work etc., which can be accessed through calls to method
  get*() along with adequate workspace for LAPACK.
  This function modifies the contents of matrix A and possibly the contents of vector b.
  For details about the nature of the solution matrices see the text above.
  \param A A matrix of size MxN, of one of vctDynamicMatrix or vctDynamicMatrixRef
  \param b A vector of size N, of one of vctDynamicVector or vctDynamicVectorRef
  \param solution A solution object of one of the types corresponding to
  input matrix
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajor
  nmrLSqLinSolverTest::TestDynamicRowMajor
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerType, typename _vectorOwnerType>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerType, CISSTNETLIB_DOUBLE> &b,
                          nmrLSqLinSolutionDynamic &solution) CISST_THROW(std::runtime_error)
{
    typename nmrLSqLinSolutionDynamic::Friend solutionFriend(solution);
    CISSTNETLIB_INTEGER ret_value;
    /* check that the size and storage order matches with Allocate() */
    if (A.IsRowMajor() != VCT_COL_MAJOR) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Input must be in Column Major format"));
    }
    CISSTNETLIB_INTEGER Mrows = static_cast<CISSTNETLIB_INTEGER>(A.rows());
    CISSTNETLIB_INTEGER Ncols = static_cast<CISSTNETLIB_INTEGER>(A.cols());
    if ((Mrows != solutionFriend.GetMa()) || (Ncols != solutionFriend.GetN())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size used for Allocate was different"));
    }
    if (Mrows != static_cast<CISSTNETLIB_INTEGER>(b.size())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size of b must be same as number of rows of A"));
    }
    char trans = 'N';
    CISSTNETLIB_INTEGER nrhs = 1;
    CISSTNETLIB_INTEGER lda = std::max(Mrows,static_cast<CISSTNETLIB_INTEGER>(1));
    CISSTNETLIB_INTEGER ldb = std::max(std::max(static_cast<CISSTNETLIB_INTEGER>(1),Mrows),Ncols);
    CISSTNETLIB_INTEGER lwork = nmrLSqLinSolutionDynamic::GetWorkspaceSize(Mrows, 0, 0, Ncols);

    CISSTNETLIB_DOUBLE *b_ptr = b.Pointer();
    if (Mrows < Ncols) {
        // Since dgels returns the result, x, in the input array, b, we need
        // to make sure that b is large enough. In the underdetermined case (M < N)
        // we would therefore have to resize b from M to N; it is more efficient to instead
        // pass x, after assigning all M elements of b to x.
        solutionFriend.GetX().Assign(b, Mrows);
        b_ptr = solutionFriend.GetX().Pointer();
    }
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgels_(&trans, &Mrows, &Ncols, &nrhs,
                       A.Pointer(), &lda,
                       b_ptr, &ldb,
                       solutionFriend.GetWork().Pointer(), &lwork, &ret_value);
#endif
#else // no major version
    dgels_(&trans, &Mrows, &Ncols, &nrhs,
           A.Pointer(), &lda,
           b_ptr, &ldb,
           solutionFriend.GetWork().Pointer(), &lwork, &ret_value);
#endif // CISSTNETLIB_VERSION

    if (Mrows >= Ncols) {
        // Dgels returns the result in the "b" vector. For the underdetermined case
        // (M < N), we already passed the solution vector, x, as "b". Thus, we only
        // need to handle the other case here (M >= N), where we need to copy the
        // first Ncols elements of b to the solution vector, x.
        solutionFriend.GetX().Assign(b, Ncols);
    }
    return ret_value;
}

/*! This function checks for valid input and
  calls the LAPACK function. The approach behind this definition of the
  function is that the user creates a solution object from a code
  wherein it is safe to do memory allocation. This solution object
  is then passed to this method along with the matrix and vector whose
  LSqLin is to be computed. The solution object has members X
  and Work etc., which can be accessed through calls to method
  get*() along with adequate workspace for LAPACK.
  This function modifies the contents of matrix A and possibly the contents of vector b.
  For details about the nature of the solution matrices see the text above.
  \param A A matrix of size MxN, of one of vctDynamicMatrix or vctDynamicMatrixRef
  \param b A vector of size N, of one of vctDynamicVector or vctDynamicVectorRef
  \param G is a reference to a dynamic matrix of size McxN
  \param h A vector of size N, of one of vctDynamicVector or vctDynamicVectorRef
  \param solution A solution object of one of the types corresponding to
  input matrix
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajor
  nmrLSqLinSolverTest::TestDynamicRowMajor
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb,
          typename _matrixOwnerTypeG, typename _vectorOwnerTypeh>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                          vctDynamicVectorBase<_vectorOwnerTypeh, CISSTNETLIB_DOUBLE> &h,
                          nmrLSqLinSolutionDynamic &solution) CISST_THROW(std::runtime_error)
{
    typename nmrLSqLinSolutionDynamic::Friend solutionFriend(solution);
    /* check that the size and storage order matches with Allocate() */
    if ((A.IsRowMajor() != VCT_COL_MAJOR) || (G.IsRowMajor() != VCT_COL_MAJOR)) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Input must be in Column Major format"));
    }
    CISSTNETLIB_INTEGER ma = static_cast<CISSTNETLIB_INTEGER>(A.rows());
    CISSTNETLIB_INTEGER mg = static_cast<CISSTNETLIB_INTEGER>(G.rows());
    CISSTNETLIB_INTEGER na = static_cast<CISSTNETLIB_INTEGER>(A.cols());
    CISSTNETLIB_INTEGER ng = static_cast<CISSTNETLIB_INTEGER>(G.cols());
    if ((ma != solutionFriend.GetMa()) || (mg != solutionFriend.GetMg())
        || (na != solutionFriend.GetN()) || (ng != solutionFriend.GetN())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size used for Allocate was different"));
    }
    if (ma != static_cast<CISSTNETLIB_INTEGER>(b.size())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size of b must be same as number of rows of A"));
    }
    if (mg != static_cast<CISSTNETLIB_INTEGER>(h.size())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size of h must be same as number of rows of G"));
    }
    // make a copy of A, b, G, h
    solutionFriend.GetA().Assign(A);
    solutionFriend.GetG().Assign(G);
    solutionFriend.Getb().Assign(b);
    solutionFriend.Geth().Assign(h);
    CISSTNETLIB_INTEGER mdw = ma + mg;
    CISSTNETLIB_INTEGER mode = 0;
    CISSTNETLIB_DOUBLE prgopt = 1.;
    solutionFriend.GetIWork()(0) = -1;
    solutionFriend.GetIWork()(1) = -1;

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_lsi_(solutionFriend.GetInput().Pointer(), &mdw, &ma, &mg, &na,
                     &prgopt, solutionFriend.GetX().Pointer(), solutionFriend.GetRNorm().Pointer(), &mode,
                     solutionFriend.GetWork().Pointer(), solutionFriend.GetIWork().Pointer());
#endif
#else // no major version
    lsi_(solutionFriend.GetInput().Pointer(), &mdw, &ma, &mg, &na,
         &prgopt, solutionFriend.GetX().Pointer(), solutionFriend.GetRNorm().Pointer(), &mode,
         solutionFriend.GetWork().Pointer(), solutionFriend.GetIWork().Pointer());
#endif // CISSTNETLIB_VERSION
    return mode;
}

/*! This function checks for valid input and
  calls the LAPACK function. The approach behind this definition of the
  function is that the user creates a solution object from a code
  wherein it is safe to do memory allocation. This solution object
  is then passed to this method along with the matrix and vector whose
  LSqLin is to be computed. The solution object has members X
  and Work etc., which can be accessed through calls to method
  get*() along with adequate workspace for LAPACK.
  This function modifies the contents of matrix A and possibly the contents of vector b.
  For details about the nature of the solution matrices see the text above.
  \param A is a reference to a dynamic matrix of size MaxN
  \param b A vector of size Ma, of one of vctDynamicVector or vctDynamicVectorRef
  \param E is a reference to a dynamic matrix of size MexN
  \param f A vector of size Me, of one of vctDynamicVector or vctDynamicVectorRef
  \param G is a reference to a dynamic matrix of size MgxN
  \param h A vector of size Mg, of one of vctDynamicVector or vctDynamicVectorRef
  \param X The output vector for LSqLin
  \param Work The workspace for LAPACK.
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajor
  nmrLSqLinSolverTest::TestDynamicRowMajor
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb,
          typename _matrixOwnerTypeE, typename _vectorOwnerTypef,
          typename _matrixOwnerTypeG, typename _vectorOwnerTypeh>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &E,
                          vctDynamicVectorBase<_vectorOwnerTypef, CISSTNETLIB_DOUBLE> &f,
                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                          vctDynamicVectorBase<_vectorOwnerTypeh, CISSTNETLIB_DOUBLE> &h,
                          nmrLSqLinSolutionDynamic &solution) CISST_THROW(std::runtime_error)
{
    typename nmrLSqLinSolutionDynamic::Friend solutionFriend(solution);
    /* check that the size and storage order matches with Allocate() */
    if ((A.IsRowMajor() != VCT_COL_MAJOR) || (G.IsRowMajor() != VCT_COL_MAJOR) || (E.IsRowMajor() != VCT_COL_MAJOR)) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Input must be in Column Major format"));
    }
    CISSTNETLIB_INTEGER ma = static_cast<CISSTNETLIB_INTEGER>(A.rows());
    CISSTNETLIB_INTEGER me = static_cast<CISSTNETLIB_INTEGER>(E.rows());
    CISSTNETLIB_INTEGER mg = static_cast<CISSTNETLIB_INTEGER>(G.rows());
    CISSTNETLIB_INTEGER na = static_cast<CISSTNETLIB_INTEGER>(A.cols());
    CISSTNETLIB_INTEGER ne = static_cast<CISSTNETLIB_INTEGER>(E.cols());
    CISSTNETLIB_INTEGER ng = static_cast<CISSTNETLIB_INTEGER>(G.cols());
    if ((ma != solutionFriend.GetMa()) || (mg != solutionFriend.GetMg()) || (me != solutionFriend.GetMe())
        || (na != solutionFriend.GetN()) || (ng != solutionFriend.GetN() || (ne != solutionFriend.GetN()))
        ) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size used for Allocate was different"));
    }
    if (ma != static_cast<CISSTNETLIB_INTEGER>(b.size())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size of b must be same as number of rows of A"));
    }
    if (mg != static_cast<CISSTNETLIB_INTEGER>(h.size())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size of h must be same as number of rows of G"));
    }
    if (me != static_cast<CISSTNETLIB_INTEGER>(f.size())) {
        cmnThrow(std::runtime_error("nmrLSqLinSolver Solve: Size of f must be same as number of rows of E"));
    }
    // make a copy of A, b, E, f, G, h
    solutionFriend.GetA().Assign(A);
    solutionFriend.GetE().Assign(E);
    solutionFriend.GetG().Assign(G);
    solutionFriend.Getb().Assign(b);
    solutionFriend.Getf().Assign(f);
    solutionFriend.Geth().Assign(h);
    CISSTNETLIB_INTEGER mdw = ma + mg + me;
    CISSTNETLIB_INTEGER mode = 0;
    CISSTNETLIB_DOUBLE prgopt = 1.;
    solutionFriend.GetIWork()(0) = -1;
    solutionFriend.GetIWork()(1) = -1;
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_lsei_(solutionFriend.GetInput().Pointer(), &mdw, &me, &ma, &mg, &na,
                      &prgopt, solutionFriend.GetX().Pointer(), solutionFriend.GetRNorm().Pointer(ma),
                      solutionFriend.GetRNorm().Pointer(), &mode,
                      solutionFriend.GetWork().Pointer(), solutionFriend.GetIWork().Pointer());
#endif
#else // no major version
    lsei_(solutionFriend.GetInput().Pointer(), &mdw, &me, &ma, &mg, &na,
          &prgopt, solutionFriend.GetX().Pointer(), solutionFriend.GetRNorm().Pointer(ma),
          solutionFriend.GetRNorm().Pointer(), &mode,
          solutionFriend.GetWork().Pointer(), solutionFriend.GetIWork().Pointer());
#endif // CISSTNETLIB_VERSION
    return mode;
}

/*! Basic version of LSqLin where user provides the input
  matrix as well as storage for output and workspace needed
  by LAPACK.
  No mem allocation is done in this function, user allocates everything
  inlcuding workspace. The LSqLin method verifies that the size
  of the solution objects matches
  the input. See static methods in nmrLSqLinSolutionDynamic for helper
  functions that help determine the min workspace required.
  This function modifies the contents of matrix A and possibly vector b.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MxN
  \param b A vector of size N, of one of vctDynamicVector or vctDynamicVectorRef
  \param X The output vector for LSqLin
  \param Work The workspace for LAPACK.
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb, typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &X,
                          vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &Work)
{
    nmrLSqLinSolutionDynamic lsqLinSolution(A.rows(), A.cols(), X, Work);
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, lsqLinSolution);
    return ret_value;
}

/*! Basic version of LSqLin where user provides the input
  matrix as well as storage for output.
  The LSqLin method verifies that the size
  of the solution objects matches
  the input and allocates workspace memory, which is deallocated when
  the function ends.
  This function modifies the contents of matrix A and possibly vector b.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MxN
  \param b A vector of size N, of one of vctDynamicVector or vctDynamicVectorRef
  \param X The output vector for LSqLin
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb, typename _vectorOwnerTypeX>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &X)
{
    nmrLSqLinSolutionDynamic lsqLinSolution(A.rows(), A.cols(), X);
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, lsqLinSolution);
    return ret_value;
}

/*! Basic version of LSqLin where user provides the input
  matrix as well as storage for output.
  The LSqLin method verifies that the size
  of the solution objects matches
  the input and allocates workspace memory, which is deallocated when
  the function ends.
  This function modifies the contents of matrix A and possibly vector b.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MaxN
  \param b A vector of size Ma, of one of vctDynamicVector or vctDynamicVectorRef
  \param G is a reference to a dynamic matrix of size MgxN
  \param h A vector of size Mg, of one of vctDynamicVector or vctDynamicVectorRef
  \param X The output vector for LSqLin
  \param Work The workspace for LAPACK.
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb,
          typename _matrixOwnerTypeG, typename _vectorOwnerTypeh,  typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                          vctDynamicVectorBase<_vectorOwnerTypeh, CISSTNETLIB_DOUBLE> &h,
                          vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &X,
                          vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &Work)
{
    nmrLSqLinSolutionDynamic lsqLinSolution(A.rows(), G.rows(), A.cols(), X, Work);
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, G, h, lsqLinSolution);
    return ret_value;
}

/*! Basic version of LSqLin where user provides the input
  matrix as well as storage for output.
  The LSqLin method verifies that the size
  of the solution objects matches
  the input and allocates workspace memory, which is deallocated when
  the function ends.
  This function modifies the contents of matrix A and possibly vector b.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MaxN
  \param b A vector of size Ma, of one of vctDynamicVector or vctDynamicVectorRef
  \param G is a reference to a dynamic matrix of size MgxN
  \param h A vector of size Mg, of one of vctDynamicVector or vctDynamicVectorRef
  \param X The output vector for LSqLin
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb,
          typename _matrixOwnerTypeG, typename _vectorOwnerTypeh,  typename _vectorOwnerTypeX>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                          vctDynamicVectorBase<_vectorOwnerTypeh, CISSTNETLIB_DOUBLE> &h,
                          vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &X)
{
    nmrLSqLinSolutionDynamic lsqLinSolution(A.rows(), G.rows(), A.cols(), X);
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, G, h, lsqLinSolution);
    return ret_value;
}

/*! Basic version of LSqLin where user provides the input
  matrix as well as storage for output.
  The LSqLin method verifies that the size
  of the solution objects matches
  the input and allocates workspace memory, which is deallocated when
  the function ends.
  This function modifies the contents of matrix A and possibly vector b.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MaxN
  \param b A vector of size Ma, of one of vctDynamicVector or vctDynamicVectorRef
  \param E is a reference to a dynamic matrix of size MexN
  \param f A vector of size Me, of one of vctDynamicVector or vctDynamicVectorRef
  \param G is a reference to a dynamic matrix of size MgxN
  \param h A vector of size Mg, of one of vctDynamicVector or vctDynamicVectorRef
  \param X The output vector for LSqLin
  \param Work The workspace for LAPACK.
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb,
          typename _matrixOwnerTypeE, typename _vectorOwnerTypef,
          typename _matrixOwnerTypeG, typename _vectorOwnerTypeh,
          typename _vectorOwnerTypeX, typename _vectorOwnerTypeWork>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &E,
                          vctDynamicVectorBase<_vectorOwnerTypef, CISSTNETLIB_DOUBLE> &f,
                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                          vctDynamicVectorBase<_vectorOwnerTypeh, CISSTNETLIB_DOUBLE> &h,
                          vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &X,
                          vctDynamicVectorBase<_vectorOwnerTypeWork, CISSTNETLIB_DOUBLE> &Work)
{
    nmrLSqLinSolutionDynamic lsqLinSolution(A.rows(), E.rows(), G.rows(), A.cols(), X, Work);
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, E, f, G, h, lsqLinSolution);
    return ret_value;
}

/*! Basic version of LSqLin where user provides the input
  matrix as well as storage for output.
  The LSqLin method verifies that the size
  of the solution objects matches
  the input and allocates workspace memory, which is deallocated when
  the function ends.
  This function modifies the contents of matrix A and possibly vector b.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MaxN
  \param b A vector of size Ma, of one of vctDynamicVector or vctDynamicVectorRef
  \param E is a reference to a dynamic matrix of size MexN
  \param f A vector of size Me, of one of vctDynamicVector or vctDynamicVectorRef
  \param G is a reference to a dynamic matrix of size MgxN
  \param h A vector of size Mg, of one of vctDynamicVector or vctDynamicVectorRef
  \param X The output vector for LSqLin
  TESTS:
  nmrLSqLinSolverTest::TestDynamicColumnMajorUserAlloc
  nmrLSqLinSolverTest::TestDynamicRowMajorUserAlloc
*/
template <typename _matrixOwnerTypeA, typename _vectorOwnerTypeb,
          typename _matrixOwnerTypeE, typename _vectorOwnerTypef,
          typename _matrixOwnerTypeG, typename _vectorOwnerTypeh,
          typename _vectorOwnerTypeX>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                          vctDynamicVectorBase<_vectorOwnerTypeb, CISSTNETLIB_DOUBLE> &b,
                          vctDynamicMatrixBase<_matrixOwnerTypeE, CISSTNETLIB_DOUBLE> &E,
                          vctDynamicVectorBase<_vectorOwnerTypef, CISSTNETLIB_DOUBLE> &f,
                          vctDynamicMatrixBase<_matrixOwnerTypeG, CISSTNETLIB_DOUBLE> &G,
                          vctDynamicVectorBase<_vectorOwnerTypeh, CISSTNETLIB_DOUBLE> &h,
                          vctDynamicVectorBase<_vectorOwnerTypeX, CISSTNETLIB_DOUBLE> &X)
{
    nmrLSqLinSolutionDynamic lsqLinSolution(A.rows(), E.rows(), G.rows(), A.cols(), X);
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, E, f, G, h, lsqLinSolution);
    return ret_value;
}

/*
****************************************************************************
FIXED SIZE
****************************************************************************
*/

/*! This provides the solution class for fixed size matrices
  and provides an easy to use template. That is
  nmrSVDSolutionFixedSize<4, 3, VCT_COL_MAJOR> vs.
  nmrSVDSolutionBase<vctFixedSizeMatrix<4, 3, VCT_COL_MAJOR>
  No extra work of allocation etc is required for fixed size.
*/
template <vct::size_type _ma, vct::size_type _me, vct::size_type _mg, vct::size_type _n>
class nmrLSqLinSolutionFixedSize
{
public:
    enum {MIN_MN = (_ma<_n) ? _ma : _n};
    enum {K = (_ma + _mg > _n)?(_ma + _mg) : _n};
    enum {LWORK_LS = 2*((vct::size_type)MIN_MN)};
    enum {LWORK_LSI = _n + (_mg + 2)*(_n + 7) + (vct::size_type)K};
    enum {LWORK_LSEI = 2*(_me + _n) + (_mg + 2)*(_n + 7) + (vct::size_type)K};
    enum {LWORK_TMP = ((vct::size_type)LWORK_LS > (vct::size_type)LWORK_LSI) ? (vct::size_type)LWORK_LS : (vct::size_type)LWORK_LSI};
    enum {LWORK = ((vct::size_type)LWORK_LSEI > (vct::size_type)LWORK_TMP) ? (vct::size_type)LWORK_LSEI : (vct::size_type)LWORK_TMP};
    enum {LIWORK = _mg + 2*_n + 2};
    typedef vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _ma, _n, 1, _ma> TypeA;
    //make sizes 1 if 0, it does not affect anything else
    typedef vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, (_me==0)?1:_me, _n, 1, (_me==0)?1:_me> TypeE;
    typedef vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, (_mg==0)?1:_mg, _n, 1, (_mg==0)?1:_mg> TypeG;
    typedef vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, _ma, 1> Typeb;
    typedef vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, (_me==0)?1:_me, 1> Typef;
    typedef vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, (_mg==0)?1:_mg, 1> Typeh;
    typedef vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, (_me==0)?1:_me, 1> TypeRNormE;
    typedef vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, _ma, 1> TypeRNormL;

    typedef vctFixedSizeVector<CISSTNETLIB_DOUBLE, LWORK> TypeWork;
    typedef vctFixedSizeVector<CISSTNETLIB_INTEGER, LIWORK> TypeIWork;
    typedef vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> TypeX;
    typedef vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma + _me> TypeRNorm;
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma + _me + _mg, _n + 1, VCT_COL_MAJOR> TypeInput;
protected:
    /*!
      Memory allocated for Workspace matrices if needed
    */
    TypeWork WorkspaceMemory;
    TypeIWork IWorkspaceMemory;
    /*!
      Memory allocated for norms if needed.
    */
    TypeRNorm RNorm;
    /*!
      Memory allocated for input if needed
      The LSEI (constrained least squares) requires
      that the input matrices be in one continuous
      memory block ordered according to fortran order (Column
      Major format), such that first Mc rows and N columns
      represent A, Ma rows and N+1 th column represent b,
      next Me rows represent (E, f) and last Mg rows represent
      (G, h) where the original LSEI problem is
      arg min || A x - b ||, s.t. E x = f and G x >= h.
      The input for LSI is similar other than Me == 0.
    */
    TypeInput InputMemory;
    /*! References to work or return or input types, these point either
      to user allocated memory or our memory chunks if needed
    */
    TypeX X;
    TypeRNormL RNormL;
    TypeRNormE RNormE;

public:
    /*! This class is not intended to be a top-level API.
      It has been provided to avoid making the templated
      LSqLin function as a friend of this class, which turns
      out to be not so easy in .NET. Instead, the Friend class
      provides a cumbersome way to get non-const references
      to the private data.
      In order to get non-const references the user has
      to first create a object of nmrLSqLinSolutionDynamic::Friend
      and then use the get* method on that object. Our philosophy
      here is that this should be a deterrent for a general user
      and should ring alarm bells in a reasonable programmer.
    */
    class Friend {
    private:
        nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n> &solution;
    public:
        Friend(nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n> &insolution):solution(insolution) {
        }
        inline TypeX &GetX(void) {
            return solution.X;
        }
        inline TypeRNorm &GetRNorm(void) {
            return solution.RNorm;
        }
        inline TypeInput &GetInput(void) {
            return solution.InputMemory;
        }
        inline TypeWork &GetWork(void) {
            return solution.WorkspaceMemory;
        }
        inline TypeIWork &GetIWork(void) {
            return solution.IWorkspaceMemory;
        }
    };
    friend class Friend;

public:
    /*we set the references too */
    nmrLSqLinSolutionFixedSize():
        RNormL(RNorm.Pointer()),
        RNormE(RNorm.Pointer(_ma)) {}

    /*! In order to get access to X, after
      it has been computed by calling the nmrLSqLin function,
      use the following methods.
    */
    inline const TypeX &GetX(void) const {
        return X;
    }
    inline const TypeRNormL &GetRNorm(void) const {
        return RNormL;
    }
    inline const TypeRNormE &GetRNormE(void) const {
        return RNormE;
    }
};

/******************************************************************************/
/* Overloaded functions for Least Squares (LS)                                 */
/******************************************************************************/

template <vct::size_type _ma, vct::size_type _n, vct::size_type _work>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> &x,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _work> &Work)
{
    CISSTNETLIB_INTEGER ret_value;
    char trans = 'N';
    CISSTNETLIB_INTEGER Mrows = static_cast<CISSTNETLIB_INTEGER>(_ma);
    CISSTNETLIB_INTEGER Ncols = static_cast<CISSTNETLIB_INTEGER>(_n);
    CISSTNETLIB_INTEGER nrhs = 1;
    CISSTNETLIB_INTEGER lda = std::max(Mrows,static_cast<CISSTNETLIB_INTEGER>(1));
    CISSTNETLIB_INTEGER ldb = std::max(std::max(static_cast<CISSTNETLIB_INTEGER>(1),Mrows),Ncols);
    CISSTNETLIB_INTEGER lwork = static_cast<CISSTNETLIB_INTEGER>(nmrLSqLinSolutionFixedSize<_ma, 0, 0, _n>::LWORK);
    CMN_ASSERT(lwork <= static_cast<CISSTNETLIB_INTEGER>(_work));
    CISSTNETLIB_DOUBLE *b_ptr = b.Pointer();
    if (Mrows < Ncols) {
        // Since dgels returns the result, x, in the input array, b, we need
        // to make sure that b is large enough. In the underdetermined case (M < N)
        // we would therefore have to resize b from M to N; it is more efficient to instead
        // pass x, after assigning all M elements of b to x.
        memcpy(x.Pointer(), b.Pointer(), Mrows*sizeof(CISSTNETLIB_DOUBLE));
        b_ptr = x.Pointer();
    }

#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgels_(&trans, &Mrows, &Ncols, &nrhs,
                       A.Pointer(), &lda,
                       b_ptr, &ldb,
                       Work.Pointer(), &lwork, &ret_value);
#endif
#else // no major version
    dgels_(&trans, &Mrows, &Ncols, &nrhs,
           A.Pointer(), &lda,
           b_ptr, &ldb,
           Work.Pointer(), &lwork, &ret_value);
#endif // CISSTNETLIB_VERSION

    if (Mrows >= Ncols) {
        // Dgels returns the result in the "b" vector. For the underdetermined case
        // (M < N), we already passed the solution vector, x, as "b". Thus, we only
        // need to handle the other case here (M >= N), where we need to copy the
        // first Ncols elements of b to the solution vector, x.
        x.Assign(b.Pointer());
    }
    return ret_value;
}

template <vct::size_type _ma, vct::size_type _n>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> &x)
{
    vctFixedSizeVector<CISSTNETLIB_DOUBLE, nmrLSqLinSolutionFixedSize<_ma, 0, 0, _n>::LWORK> work;
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, x, work);
    return ret_value;
}

template <vct::size_type _ma, vct::size_type _n>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          nmrLSqLinSolutionFixedSize<_ma, 0, 0, _n> &solution)
{
    typename nmrLSqLinSolutionFixedSize<_ma, 0, 0, _n>::Friend solutionFriend(solution);
    CISSTNETLIB_INTEGER ret_value = nmrLSqLin(A, b, solutionFriend.GetX(), solutionFriend.GetWork());
    return ret_value;
}

/******************************************************************************/
/* Overloaded functions for Least Squares Inequality (LSI)                    */
/******************************************************************************/

template <vct::size_type _ma, vct::size_type _mg, vct::size_type _n, vct::size_type _work, vct::size_type _iwork>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _mg, _n, VCT_COL_MAJOR> &G, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _mg> &h,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> &x,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma + _mg, _n + 1, VCT_COL_MAJOR> &W,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &RNorm,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _work> &Work, vctFixedSizeVector<CISSTNETLIB_INTEGER, _iwork> &IWork)
{
    CISSTNETLIB_INTEGER ma = static_cast<CISSTNETLIB_INTEGER>(_ma);
    CISSTNETLIB_INTEGER mg = static_cast<CISSTNETLIB_INTEGER>(_mg);
    CISSTNETLIB_INTEGER na = static_cast<CISSTNETLIB_INTEGER>(_n);
    vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _ma, _n, 1, _ma + _mg> ARef(W, 0, 0);
    vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _mg, _n, 1, _ma + _mg> GRef(W, _ma, 0);
    vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, _ma, 1> bRef(W.Column(_n).Pointer(0));
    vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, _mg, 1> hRef(W.Column(_n).Pointer(_ma));
    // make a copy of A, b, G, h
    ARef.Assign(A);
    GRef.Assign(G);
    bRef.Assign(b);
    hRef.Assign(h);
    CISSTNETLIB_INTEGER mdw = ma + mg;
    CISSTNETLIB_INTEGER mode = 0;
    CISSTNETLIB_DOUBLE prgopt = 1.;
    IWork(0) = -1;
    IWork(1) = -1;
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_lsi_(W.Pointer(), &mdw, &ma, &mg, &na,
                     &prgopt, x.Pointer(), RNorm.Pointer(), &mode,
                     Work.Pointer(), IWork.Pointer());
#endif
#else // no major version
    lsi_(W.Pointer(), &mdw, &ma, &mg, &na,
         &prgopt, x.Pointer(), RNorm.Pointer(), &mode,
         Work.Pointer(), IWork.Pointer());
#endif // CISSTNETLIB_VERSION
    return mode;
}

template <vct::size_type _ma, vct::size_type _mg, vct::size_type _n>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _mg, _n, VCT_COL_MAJOR> &G,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _mg> &h,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> &x)
{
    vctFixedSizeVector<CISSTNETLIB_DOUBLE, nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n>::LWORK> work;
    vctFixedSizeVector<CISSTNETLIB_INTEGER, nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n>::LIWORK> iwork;
    vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma + _mg, _n + 1, VCT_COL_MAJOR> w;
    vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> rNorm;
    CISSTNETLIB_INTEGER ret_value;
    // for some reason windows gets confused with the templates to match, so explicitly
    // specifying the template parameters for the functions helps .net
    ret_value = nmrLSqLin<_ma, _mg, _n, nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n>::LWORK,
        nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n>::LIWORK>
        (A, b, G, h, x, w, rNorm, work, iwork);
    return ret_value;
}

template <vct::size_type _ma, vct::size_type _mg, vct::size_type _n>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _mg, _n, VCT_COL_MAJOR> &G,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _mg> &h,
                          nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n> &solution)
{
    typename nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n>::Friend solutionFriend(solution);
    CISSTNETLIB_INTEGER ret_value;
    // for some reason windows gets confused with the templates to match, so explicitly
    // specifying the template parameters for the functions helps .net
    ret_value = nmrLSqLin<_ma, _mg, _n, nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n>::LWORK,
        nmrLSqLinSolutionFixedSize<_ma, 0, _mg, _n>::LIWORK>
        (A, b, G, h,
         solutionFriend.GetX(), solutionFriend.GetInput(),
         solutionFriend.GetRNorm(), solutionFriend.GetWork(), solutionFriend.GetIWork());
    return ret_value;
}

/******************************************************************************/
/* Overloaded functions for Least Squares Equality & Inequality (LSEI)        */
/******************************************************************************/

template <vct::size_type _ma, vct::size_type _me, vct::size_type _mg,
          vct::size_type _n, vct::size_type _work, vct::size_type _iwork>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _me, _n, VCT_COL_MAJOR> &E, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _me> &f,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _mg, _n, VCT_COL_MAJOR> &G, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _mg> &h,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> &x,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma + _me + _mg, _n + 1, VCT_COL_MAJOR> &W,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma + _me> &RNorm,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _work> &Work, vctFixedSizeVector<CISSTNETLIB_INTEGER, _iwork> &IWork)
{
    CISSTNETLIB_INTEGER me = static_cast<CISSTNETLIB_INTEGER>(_me);
    CISSTNETLIB_INTEGER ma = static_cast<CISSTNETLIB_INTEGER>(_ma);
    CISSTNETLIB_INTEGER mg = static_cast<CISSTNETLIB_INTEGER>(_mg);
    CISSTNETLIB_INTEGER na = static_cast<CISSTNETLIB_INTEGER>(_n);
    vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _me, _n, 1, _ma + _me + _mg> ERef(W, 0, 0);
    vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _ma, _n, 1, _ma + _me + _mg> ARef(W, _me, 0);
    vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _mg, _n, 1, _ma + _me + _mg> GRef(W, _me + _ma, 0);
    vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, _me, 1> fRef(W.Column(_n).Pointer(0));
    vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, _ma, 1> bRef(W.Column(_n).Pointer(_me));
    vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, _mg, 1> hRef(W.Column(_n).Pointer(_me + _ma));
    // make a copy of A, b, G, h
    ARef.Assign(A);
    ERef.Assign(E);
    GRef.Assign(G);
    bRef.Assign(b);
    fRef.Assign(f);
    hRef.Assign(h);
    CISSTNETLIB_INTEGER mdw = ma + me + mg;
    CISSTNETLIB_INTEGER mode = 0;
    CISSTNETLIB_DOUBLE prgopt = 1.;
    IWork(0) = -1;
    IWork(1) = -1;
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_lsei_(W.Pointer(), &mdw, &me, &ma, &mg, &na,
                      &prgopt, x.Pointer(), RNorm.Pointer(_ma), RNorm.Pointer(), &mode,
                      Work.Pointer(), IWork.Pointer());
#endif
#else // no major version
    lsei_(W.Pointer(), &mdw, &me, &ma, &mg, &na,
          &prgopt, x.Pointer(), RNorm.Pointer(_ma), RNorm.Pointer(), &mode,
          Work.Pointer(), IWork.Pointer());
#endif // CISSTNETLIB_VERSION
    return mode;
}

template <vct::size_type _ma, vct::size_type _me, vct::size_type _mg, vct::size_type _n>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _me, _n, VCT_COL_MAJOR> &E, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _me> &f,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _mg, _n, VCT_COL_MAJOR> &G, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _mg> &h,
                          vctFixedSizeVector<CISSTNETLIB_DOUBLE, _n> &x)
{
    vctFixedSizeVector<CISSTNETLIB_DOUBLE, nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n>::LWORK> work;
    vctFixedSizeVector<CISSTNETLIB_INTEGER, nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n>::LIWORK> iwork;
    vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma + _me + _mg, _n + 1, VCT_COL_MAJOR> w;
    vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma + _me> rNorm;
    CISSTNETLIB_INTEGER ret_value;
    // for some reason windows gets confused with the templates to match, so explicitly
    // specifying the template parameters for the functions helps .net
    ret_value = nmrLSqLin<_ma, _me, _mg, _n, nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n>::LWORK,
        nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n>::LIWORK>
        (A, b, E, f, G, h, x, w, rNorm, work, iwork);
    return ret_value;
}

template <vct::size_type _ma, vct::size_type _me, vct::size_type _mg, vct::size_type _n>
inline CISSTNETLIB_INTEGER nmrLSqLin(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _ma, _n, VCT_COL_MAJOR> &A, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _ma> &b,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _me, _n, VCT_COL_MAJOR> &E, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _me> &f,
                          vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _mg, _n, VCT_COL_MAJOR> &G, vctFixedSizeVector<CISSTNETLIB_DOUBLE, _mg> &h,
                          nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n> &solution)
{
    typename nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n>::Friend solutionFriend(solution);
    CISSTNETLIB_INTEGER ret_value;
    // for some reason windows gets confused with the templates to match, so explicitly
    // specifying the template parameters for the functions helps .net
    ret_value = nmrLSqLin<_ma, _me, _mg, _n, nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n>::LWORK,
        nmrLSqLinSolutionFixedSize<_ma, _me, _mg, _n>::LIWORK>
        (A, b, E, f, G, h,
         solutionFriend.GetX(), solutionFriend.GetInput(),
         solutionFriend.GetRNorm(), solutionFriend.GetWork(), solutionFriend.GetIWork());
    return ret_value;
}

#endif
