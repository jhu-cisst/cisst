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
  \brief Declaration of nmrPInverseEconomy
*/


#ifndef _nmrPInverseEconomy_h
#define _nmrPInverseEconomy_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>
#include <cisstNumerical/nmrSVDEconomy.h>

// Always include last
#include <cisstNumerical/nmrExport.h>

/*!
  \ingroup cisstNumerical

  Algorithm P-Inverse: Moore-Penrose pseudo-inverse Calculates the
  Moore-Penrose pseudo-inverse of the M by N matrix A, and stores the
  result in PInverse.  The singular values of A are returned in S.
  The left singular vectors are returned in U, and the right singular
  vectors are returned in V.

  \f$ A^{+} = V * \Sigma^{+} * U^{T} \f$

  where \f$ \Sigma^{+} \f$ is a \f$ N \times M \f$ matrix which is
  zero except for its min(m,n) diagonal elements, U is a \f$ M \times
  M \f$ orthogonal matrix, and V is a \f$ N \times N \f$ orthogonal
  matrix.  The diagonal elements of \f$ \Sigma^{+} \f$ are the
  reciprocal ofc non-zero singular values of A; they are real and
  non-negative, andc are returned in descending order.  The first \f$
  \mbox{min}(m,n) \f$ columns of U and V are the left and right
  singular vectors of A.

  \note This routine returns \f$ V^{T} \f$, not \f$ V \f$.
  \note *On exit*, the content of *A is altered.*

  There are three ways to call this method to compute the pseudo-inverse of
  the matrix A.
  METHOD 1: User provides matrices A and A^{+}
     1) The User allocates memory for these matrices and
     vector.
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> PInverse(4, 5);
     ...
     2) The user calls the SVD routine
     nmrPInverseEconomy(A, PInverse);
     The PInverse method verifies that the size of the data objects matches
     the input, and allocates workspace memory, which is deallocated when
     the function ends.
     The PInverse function alters the contents of matrix A.
     For fixed size the function call is templated by size and row/column major, e.g.
     nmrPInverseEconomy<4, 3, VCT_COL_MAJOR>(A, PInverse);

  METHOD 2: Using a preallocated data object
     1) The user creates the input matrix
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> input(rows, cols , VCT_ROW_MAJOR);
     2) The user allocats a data object which could be of
     type nmrPInverseEconomyDynamicData and nmrPInverseEconomyDynamicDataRef
     corresponding to fixed size, dynamic matrix or dynamic matrix reference.
     nmrPInverseEconomyDynamicData data(input);
     3) Call the nmrPInverseEconomy function
     nmrPInverseEconomy(input, data);
     The contents of input matrix is modified by this routine.
     The matrices U, Vt and vector S are available through the following methods
     std::cout << data.U() << data.S() << data.Vt() << std::endl;
     The pseudo-inverse is available through data.PInverse()

  METHOD 3: User provides matrix PInverse
     with workspace required by pseudo-inverse routine of LAPACK.
     1) User creates matrices and vector
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
     vctDynamicMatrix<CISSTNETLIB_DOUBLE> PInverse(4, 5);
     2) User also needs to allocate memory for workspace. This method is particularly
     useful when the user is using more than one numerical methods from the library
     and is willing to share the workspace between them. In such as case, the user
     can allocate the a memory greater than the maximum required by different methods.
     To aid the user determine the minimum workspace required (and not spend time digging
     LAPACK documentation) the library provides helper function
     nmrPInverseEconomyDynamicData::WorkspaceSize(input)
     vctDynamicVector<CISSTNETLIB_DOUBLE> Work(nmrPInverseEconomyDynamicData::WorkspaceSize(A));
     3) Call the SVD function
     nmrPInverseEconomy(A, PInverse, Work);

     \note The PInverse functions make use of LAPACK routines.  To activate this
     code, set the CISST_HAS_CISSTNETLIB flag to ON during the configuration
     with CMake.
     \note The general rule for numerical functions which depend on LAPACK is that
     column-major matrices should be used everywhere, and that all
     matrices should be compact.
     \note For the specific case of PInverse, a valid result is also obtained if
     all the matrices are stored in row-major order.  This is an exeption to
     the general rule.  However, mixed-order is not tolerated.
 */


/*
   ****************************************************************************
                                  DYNAMIC SIZE
   ****************************************************************************
 */

/*! This is the class for the composite data container of PInverse.
 */
class nmrPInverseEconomyDynamicData {

public:
    /*! Type used for sizes within nmrSVDEconomyDynamicData.  This type
      is compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      CISSTNETLIB_INTEGER. */
    typedef unsigned int size_type;

protected:
    /*!
      Memory allocated for Workspace matrices if needed
      This includes memory needed for SVD as well.
      Order of storage is m x m elements of U followed by
      n x n elements of Vt followed by min (m, n) elements of
      S, followed by memory for LAPACK workspace.
     */
    vctDynamicVector<CISSTNETLIB_DOUBLE> WorkspaceMemory;
    /*!
      Memory allocated for PInverse if needed.
     */
    vctDynamicVector<CISSTNETLIB_DOUBLE> OutputMemory;
    /*! References to workspace or return types, these point either
      to user allocated memory or our memory chunks if needed
     */
    //@{
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> PInverseReference;
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> UReference;
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> VtReference;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> SReference;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> WorkspaceReference;
    //@}

    /* Just store M, N, and StorageOrder which are needed
       to check if A matrix passed to solve method matches
       the allocated size. */
    size_type MMember;
    size_type NMember;
    bool StorageOrderMember;


    /*! Private method to set the data members MMember, NMember and
      StorageOrder.  This method must be called before
      AllocateOutputWorkspace, ThrowUnlessOutputSizeIsCorrect or
      ThrowUnlessWorkspaceSizeIsCorrect. */
    inline void SetDimension(size_type m, size_type n, bool storageOrder)
    {
        StorageOrderMember = storageOrder;
        MMember = m;
        NMember = n;
    }

    /*! Private method to allocate memory for the output and the
      workspace if needed.  This method assumes that the dimension m
      and n as well as the storage order are already set.  It is
      important to use this method in all the methods provided in the
      user API, even if all the memory is provided by the user since
      this method will ensure that the data
      (nmrPInverseEconomyDynamicData) does not keep any memory allocated.
      This is for the case where a single data is used first to
      allocate everything and, later on, used with user allocated
      memory (for either the workspace or the output).

      \note The method SetDimension must have been called before.
    */
    inline void AllocateOutputWorkspace(bool allocateOutput, bool allocateWorkspace)
    {
        // allocate output
        if (allocateOutput) {
            OutputMemory.SetSize(MMember * NMember);
            PInverseReference.SetRef(NMember, MMember,
                                     (StorageOrderMember) ? MMember : 1,
                                     (StorageOrderMember) ? 1 : NMember,
                                     OutputMemory.Pointer());
        } else {
            OutputMemory.SetSize(0);
        }
        // allocate workspace
        if (allocateWorkspace) {
            this->WorkspaceMemory.SetSize(WorkspaceSize(MMember, NMember));
            this->SetRefSVD(this->WorkspaceMemory);
        } else {
            this->WorkspaceMemory.SetSize(0);
        }
    }


    /*! Set all internal references used for the SVD problem.  This
      method requires a valid workspace but it doesn't check the size.
      It is the caller responsability to do so.

      \note The method SetDimension must have been called before.
    */
    template <class _vectorOwnerTypeWorkspace>
    inline void SetRefSVD(vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
    {
        const size_type minmn = (MMember < NMember) ? MMember : NMember;
        size_type current = 0;
        UReference.SetRef(MMember, minmn,
                          (StorageOrderMember) ? minmn : 1,
                          (StorageOrderMember) ? 1 : MMember,
                          workspace.Pointer(current));
        current += (MMember * minmn);
        VtReference.SetRef(NMember, NMember,
                           (StorageOrderMember) ? NMember : 1,
                           (StorageOrderMember) ? 1 : NMember,
                           workspace.Pointer(current));
        current += (NMember * NMember);
        SReference.SetRef(minmn,
                          workspace.Pointer(current),
                          1);
        current += minmn;
        WorkspaceReference.SetRef(nmrSVDEconomyDynamicData::WorkspaceSize(MMember, NMember),
                                  workspace.Pointer(current),
                                  1);
    }



    /*!  Verifies that the user provided reference for the output
      match the size of the data as set by SetDimension.  This
      method also checks that the storage order is consistent.

      \note The method SetDimension must have been called before.
    */
    template <typename _matrixOwnerTypePInverse>
    inline void ThrowUnlessOutputSizeIsCorrect(vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> & pInverse) const
        CISST_THROW(std::runtime_error)
    {
        // check sizes and storage order
        if ((MMember != pInverse.cols()) || (NMember != pInverse.rows())) {
            cmnThrow(std::runtime_error("nmrPInverseEconomyDynamicData: Size of matrix pInverse is incorrect."));
        }
        if (pInverse.StorageOrder() != StorageOrderMember) {
            cmnThrow(std::runtime_error("nmrPInverseEconomyDynamicData: Storage order of pInverse is incorrect."));
        }
    }

    /*! Verifies that the user provided references for the workspace
      match (or is greated than) the size of the data as set by
      SetDimension.  This method also checks that the workspace is
      compact.

      \note The method SetDimension must have been called before.
    */
    template <typename _vectorOwnerTypeWorkspace>
    inline void
    ThrowUnlessWorkspaceSizeIsCorrect(vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace) const
        CISST_THROW(std::runtime_error)
    {
        const size_type lwork = nmrSVDEconomyDynamicData::WorkspaceSize(MMember, NMember);
        if (lwork > workspace.size()) {
            cmnThrow(std::runtime_error("nmrPInverseEconomyDynamicData: Workspace is too small."));
        }
        if (!workspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrPInverseEconomyDynamicData: Workspace must be compact."));
        }
    }


public:
    /*! Helper methods for user to set min working space required by LAPACK
      SVD routine plus memory needed for S, U & Vt matrices used to compute
      PInverse.
      \param m, n The size of matrix whose SVD needs to be computed
     */
    static inline size_type WorkspaceSize(size_type m, size_type n)
    {
        const size_type minmn = (m < n) ? m : n;
        const size_type maxmn = (m > n) ? m : n;
        const size_type lwork_1 = 3 * minmn + maxmn;
        const size_type lwork_2 = 5 * minmn;
        const size_type lwork = (lwork_1 > lwork_2) ? lwork_1 : lwork_2;
        //     u       vt      s       workspace
        return m * minmn + n * n + minmn + lwork;
    }

    /*! Helper method to determine the min working space required by LAPACK
      SVD routine plus memory needed for S, U & Vt matrices used to compute
      PInverse.
      \param A The matrix whose SVD needs to be computed
     */
    template <class _matrixOwnerTypeA>
    static inline size_type WorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        return nmrPInverseEconomyDynamicData::WorkspaceSize(A.rows(), A.cols());
    }

#ifndef SWIG
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated SVD function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrPInverseEconomyDynamicData & Data;
    public:
        Friend(nmrPInverseEconomyDynamicData &data): Data(data) {
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &S(void) {
            return Data.SReference;
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &PInverse(void) {
            return Data.PInverseReference;
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &U(void) {
            return Data.UReference;
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &Vt(void) {
            return Data.VtReference;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &Workspace(void) {
            return Data.WorkspaceReference;
        }
        inline size_type M(void) {
            return Data.MMember;
        }
        inline size_type N(void) {
            return Data.NMember;
        }
        inline bool StorageOrder(void) {
            return Data.StorageOrderMember;
        }
    };
    friend class Friend;
#endif // DOXYGEN
#endif // SWIG

    /*! The default constuctor.
      For dynamic size, there are assigned default values,
      which MUST be changed by calling appropriate methods.
      (See nmrPInverseEconomyDynamicData::Allocate and
      nmrPInverseEconomyDynamicData::SetRef)
     */
    nmrPInverseEconomyDynamicData():
        MMember(static_cast<size_type>(0)),
        NMember(static_cast<size_type>(0)),
        StorageOrderMember(VCT_COL_MAJOR)
    {
        AllocateOutputWorkspace(false, false);
    };


    /*! Constructor where user provides the input matrix to specify
      size, Memory allocation is done for output matrices and vectors
      as well as Workspace used by LAPACK. This case covers the
      scenario when user wants to make all system calls for memory
      allocation before entrying time critical code sections.

      \param A Input matrix
    */
    template <class _matrixOwnerTypeA>
    nmrPInverseEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        this->Allocate(A.rows(), A.cols(), A.StorageOrder());
    }

    /*! Constructor where user provides the input matrix to specify
      size, Memory allocation is done for output matrices and vectors.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code
      sections and might be using more than one numerical method in
      the *same* thread, allowing her to share the workspace for
      LAPACK.

      \param A Input matrix
      \param workspace Workspace for SVD
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    nmrPInverseEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                                  vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
    {
        this->SetRefWorkspace(A, workspace);
    }

    /*! Constructor where user provides the size and storage order of
      the input matrix, along with matrix AP and workspace.  The data
      object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks
      are made on the validity of the input and its consitency with
      the size of input matrix.

      \param A The input matrix
      \param pInverse The output matrices for PInverse
      \param workspace The workspace for LAPACK.
    */
    template <class _matrixOwnerTypeA,
              class _matrixOwnerTypePInverse,
              class _vectorOwnerTypeWorkspace>
    nmrPInverseEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & CMN_UNUSED(A),
                                  vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> & pInverse,
                                  vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
    {
        this->SetRef(pInverse, workspace);
    }
    /*! Constructor where user provides the size and storage order of
      the input matrix, along with matrix PInverse.  The data object
      now acts as a composite container to hold, pass and manipulate a
      convenitent storage for SVD algorithm. Checks are made on thec
      validity of the input and its consitency with the size of input
      matrix.  Memory allocation for workspace is done by the method.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code
      sections and might be using the PInverse matrix elsewhere in the
      *same* thread.

      \param A Input matrix
      \param pInverse The output matrix for PInverse
    */
    template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse>
    nmrPInverseEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & CMN_UNUSED(A),
                               vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> & pInverse)
    {
        this->SetRefOutput(pInverse);
    }


    /*! This method allocates memory of output matrices and vector as
      well as the workspace.  This method should be called before the
      nmrPInverseEconomyDynamicData object is passed on to
      nmrPInverseEconomy function, as the memory required for output
      matrices and workspace are allocated here or to reallocate
      memory previously allocated by constructor.  Typically this
      method is called from a code segment where it is safe to
      allocate memory and use the data and workspace space later.

      \param A The matrix for which SVD needs to be computed, size MxN
    */
    template <class _matrixOwnerTypeA>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        this->SetDimension(A.rows(), A.cols(), A.StorageOrder());
        this->AllocateOutputWorkspace(true, true);
    }


    /*! This method allocates memory of output matrices and vector and
      uses the memory provided by user for workspace.  Check is made
      to ensure that memory provided by user is sufficient for SVD
      routine of LAPACK.  This method should be called before the
      nmrPInverseEconomyDynamicData object is passed on to
      nmrPInverseEconomy function, as the memory required for output
      matrices and workspace are allocated here or to reallocate
      memory previously allocated by constructor.  This case covers
      the scenario when user wants to make all system calls for memory
      allocation before entrying time critical code sections and might
      be using more than one numerical method in the *same* thread,
      allowing her to share the workspace for LAPACK.  Typically this
      method is called from a code segment where it is safe to
      allocate memory and use the data and workspace space later.

      \param A The matrix for which SVD needs to be computed, size MxN
      \param workspace The vector used for workspace by LAPACK.
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    inline void SetRefWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> &workspace)
    {
        this->SetDimension(A.rows(), A.cols(), A.StorageOrder());
        this->AllocateOutputWorkspace(true, false);
        // call helper method to set references for SVD components
        this->ThrowUnlessWorkspaceSizeIsCorrect(workspace);
        this->SetRefSVD(workspace);
    }

    /*! This method must be called before the data object is passed to
      nmrPInverseEconomy function.  The user provides the PInverse
      matrix to specify size, along with the workspace.  The data
      object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks
      are made on the validity of the input and its consitency with
      the size of input matrix.

      \param pInverse The output matrix  for PInverse
      \param workspace The workspace for LAPACK.
    */
    template <class _matrixOwnerTypePInverse,
              class _vectorOwnerTypeWorkspace>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> &pInverse,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> &workspace)
    {
        this->SetDimension(pInverse.cols(), pInverse.rows(), pInverse.StorageOrder());
        this->AllocateOutputWorkspace(false, false);
        // set reference on output
        this->ThrowUnlessOutputSizeIsCorrect(pInverse);
        this->PInverseReference.SetRef(pInverse);
        // set reference on workspace
        this->ThrowUnlessWorkspaceSizeIsCorrect(workspace);
        this->SetRefSVD(workspace);
    }

    /*! This method must be called before the data object is passed to
      nmrPInverseEconomy function.  The user provides the PInverse
      matrix to specify size.  The data object now acts as a composite
      container to hold, pass and manipulate a convenitent storage for
      PInverse algorithm. Checks are made on thec validity of the
      input and its consitency with the size of input matrix.  Memory
      allocation for workspace is done by the method.  This case
      covers the scenario when user wants to make all system calls for
      memory allocation before entrying time critical code sections
      and might be using the PInverse matrix elsewhere in the *same*
      thread.

      \param pInverse The output matrix for PInverse
    */
    template <class _matrixOwnerTypePInverse>
    void SetRefOutput(vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> &pInverse)
    {
        this->SetDimension(pInverse.cols(), pInverse.rows(), pInverse.StorageOrder());
        this->AllocateOutputWorkspace(false, true);
        this->ThrowUnlessOutputSizeIsCorrect(pInverse);
        this->PInverseReference.SetRef(pInverse);
    }



public:
    /*!
      \name Retrieving results.

      In order to get access to U, V^t and S, after the have been
      computed by calling nmrPInverseEconomy, use the following
      methods.
    */
    //@{
    inline const vctDynamicVectorRef<CISSTNETLIB_DOUBLE> &S(void) const {
        return SReference;
    }
    inline const vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &U(void) const {
        return UReference;
    }
    inline const vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &Vt(void) const {
        return VtReference;
    }
    inline const vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> &PInverse(void) const {
        return PInverseReference;
    }
    //@}
};

/*! This function checks for valid input and calls the LAPACK
  function. The approach behind this defintion of the function is that
  the user creates a data object from a code wherein it is safe to do
  memory allocation. This data object is then passed on to this method
  along with the matrix whose PInverse is to be computed. The data
  object has members S, U, Vt and PInverse, which can be accessed
  through calls to method *() along with adequate workspace for
  LAPACK.  This function modifies the contents of matrix A.  For
  details about nature of the data matrices see text above.
  \param A A matrix of size MxN
  \param data A data object of one of the types corresponding to input matrix

  \test nmrPInverseEconomyTest::TestDynamicColumnMajor
        nmrPInverseEconomyTest::TestDynamicRowMajor
        nmrPInverseEconomyTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseEconomyTest::TestDynamicRowMajorUserAlloc
*/
template <class _matrixOwnerType>
inline CISSTNETLIB_INTEGER nmrPInverseEconomy(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> &A,
                                    nmrPInverseEconomyDynamicData &data) CISST_THROW(std::runtime_error)
{
    typedef unsigned int size_type;

    typename nmrPInverseEconomyDynamicData::Friend dataFriend(data);
    CISSTNETLIB_INTEGER ret_value;
    /* check that the size and storage order matches with Allocate() */
    if (A.StorageOrder() != dataFriend.StorageOrder()) {
        cmnThrow(std::runtime_error("nmrPInverseEconomy Solve: Storage order used for Allocate was different"));
    }
    if ((A.rows() != dataFriend.M()) || (A.cols() != dataFriend.N())) {
        cmnThrow(std::runtime_error("nmrPInverseEconomy Solve: Size used for Allocate was different"));
    }
    const size_type rows = A.rows();
    const size_type cols = A.cols();
    const size_type minmn = (rows < cols) ? rows : cols;

    ret_value = nmrSVDEconomy(A, dataFriend.U(), dataFriend.S(),
                       dataFriend.Vt(), dataFriend.Workspace());
    const CISSTNETLIB_DOUBLE eps = cmnTypeTraits<CISSTNETLIB_DOUBLE>::Tolerance() * dataFriend.S().at(0);

    dataFriend.PInverse().SetAll(0);
    CISSTNETLIB_DOUBLE singularValue;
    size_type irank, i, j;
    for (irank = 0; irank < minmn; irank++) {
        if ((singularValue = dataFriend.S().at(irank)) > eps) {
            for (j = 0; j < rows; j++) {
                for (i = 0; i < cols; i++) {
                    dataFriend.PInverse().at(i, j) = dataFriend.PInverse().at(i, j)
                        + dataFriend.Vt().at(irank, i) * dataFriend.U().at(j, irank) / singularValue;
                }
            }
        }
    }
    return ret_value;
}

/*! Basic version of PInverse where user provides the input
  matrix as well as storage for output and workspace needed
  by LAPACK.
  No memory allocation is done in this function, user allocates everything
  including workspace. The PInverse method verifies that the size
  of the data objects matchesc
  the input. See static methods in nmrPInverseEconomyDynamicData for helper
  functions that help determine the min workspace required.
  This function modifies the contents of matrix A.
  For sizes of other matrices see text above.
  \param A A dynamic matrix of size MxN
  \param pInverse The output matrix for PInverse
  \param workspace The workspace for LAPACK.

  \test nmrPInverseEconomyTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseEconomyTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse, class _vectorOwnerTypeWorkspace>
inline CISSTNETLIB_INTEGER nmrPInverseEconomy(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                                    vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> &pInverse,
                                    vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> &workspace)
{
    nmrPInverseEconomyDynamicData svdData(A, pInverse, workspace);
    CISSTNETLIB_INTEGER ret_value = nmrPInverseEconomy(A, svdData);
    return ret_value;
}

/*! Basic version of PInverse where user provides the input matrix as
  well as storage for output.  The PInverse method verifies that the
  size of the data objects matches the input and allocates workspace
  memory, which is deallocated when the function ends.  This function
  modifies the contents of matrix A.  For sizes of other matrices see
  text above.

  \param A A dynamic matrix of size MxN
  \param pInverse The output matrices and vector for PInverse

  \test nmrPInverseEconomyTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseEconomyTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse>
inline CISSTNETLIB_INTEGER nmrPInverseEconomy(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                                    vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> &pInverse)
{
    nmrPInverseEconomyDynamicData svdData(A, pInverse);
    CISSTNETLIB_INTEGER ret_value = nmrPInverseEconomy(A, svdData);
    return ret_value;
}


#endif
