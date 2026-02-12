/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor
  Created on: 2005-10-18

  (C) Copyright 2005-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrPInverse
*/


#ifndef _nmrPInverse_h
#define _nmrPInverse_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>
#include <cisstNumerical/nmrSVD.h>

// Always include last
#include <cisstNumerical/nmrExport.h>

/*
   ****************************************************************************
                                  DYNAMIC SIZE
   ****************************************************************************
 */

/*! This is the class for the composite data container of PInverse.
 */
class nmrPInverseDynamicData {

public:
    /*! Type used for sizes within nmrSVDDynamicData.  This type
      is compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      #CISSTNETLIB_INTEGER. */
    typedef vct::size_type size_type;

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

    /*! Stores M, N, and StorageOrder which are needed to check if A
       matrix passed to solve method matches the allocated size. */
    //@{
    size_type MMember;
    size_type NMember;
    bool StorageOrderMember;
    //@}

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
      (nmrPInverseDynamicData) does not keep any memory allocated.
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
        UReference.SetRef(MMember, MMember,
                          (StorageOrderMember) ? MMember : 1,
                          (StorageOrderMember) ? 1 : MMember,
                          workspace.Pointer(current));
        current += (MMember * MMember);
        VtReference.SetRef(NMember, NMember,
                           (StorageOrderMember) ? NMember : 1,
                           (StorageOrderMember) ? 1 : NMember,
                           workspace.Pointer(current));
        current += (NMember * NMember);
        SReference.SetRef(minmn,
                          workspace.Pointer(current),
                          1);
        current += minmn;
        WorkspaceReference.SetRef(nmrSVDDynamicData::WorkspaceSize(MMember, NMember),
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
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Size of matrix pInverse is incorrect."));
        }
        if (pInverse.StorageOrder() != StorageOrderMember) {
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Storage order of pInverse is incorrect."));
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
        const size_type lwork = nmrSVDDynamicData::WorkspaceSize(MMember, NMember);
        if (lwork > workspace.size()) {
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Workspace is too small."));
        }
        if (!workspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Workspace must be compact."));
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
        //     u       vt      s       workspace   p = s^+ ut
        return m * m + n * n + minmn + lwork + n * m;
    }

    /*! Helper method to determine the min working space required by LAPACK
      SVD routine plus memory needed for S, U & Vt matrices used to compute
      PInverse.
      \param A The matrix whose SVD needs to be computed
     */
    template <class _matrixOwnerTypeA>
    static inline size_type WorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        return nmrPInverseDynamicData::WorkspaceSize(A.rows(), A.cols());
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
        nmrPInverseDynamicData & Data;
    public:
        Friend(nmrPInverseDynamicData &data): Data(data) {
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
#endif // #ifndef SWIG

    /*! The default constuctor.
      For dynamic size, there are assigned default values,
      which MUST be changed by calling appropriate methods.
      (See nmrPInverseDynamicData::Allocate and
      nmrPInverseDynamicData::SetRef)
     */
    nmrPInverseDynamicData():
        MMember(static_cast<size_type>(0)),
        NMember(static_cast<size_type>(0)),
        StorageOrderMember(VCT_COL_MAJOR)
    {
        AllocateOutputWorkspace(false, false);
    };


    /*! Constructor where user provides the input matrix to specify size,
      Memory allocation is done for output matrices and vectors as well as
      Workspace used by LAPACK. This case covers the scenario when user
      wants to make all system calls for memory allocation before entrying
      time critical code sections.
      \param A input matrix
    */
    template <class _matrixOwnerTypeA>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        this->Allocate(A.rows(), A.cols(), A.StorageOrder());
    }

    /*! Constructor where user provides the input matrix to specify size,
      Memory allocation is done for output matrices and vectors.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing her to share the workspace for LAPACK.
      \param A Input matrix
      \param workspace Workspace for SVD
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                           vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
    {
        this->SetRefWorkspace(A, workspace);
    }

    /*! Constructor where user provides the size and storage order of the input matrix,
      along with matrix AP and workspace.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks are made on the
      validity of the input and its consitency with the size of input matrix.
      \param A The input matrix
      \param pInverse The output matrix for PInverse
      \param workspace The workspace for LAPACK.
    */
    template <class _matrixOwnerTypeA,
              class _matrixOwnerTypePInverse,
              class _vectorOwnerTypeWorkspace>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & CMN_UNUSED(A),
                           vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> & pInverse,
                           vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
    {
        this->SetRef(pInverse, workspace);
    }
    /*! Constructor where user provides the size and storage order of the input matrix,
      along with matrix PInverse.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for SVD algorithm. Checks are made on thec
      validity of the input and its consitency with the size of input matrix.
      Memory allocation for workspace is done by the method.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using the PInverse matrix elsewhere in the *same* thread.
      \param A The input matrix
      \param pInverse The output matrix for PInverse
    */
    template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & CMN_UNUSED(A),
                           vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> & pInverse)
    {
        this->SetRefOutput(pInverse);
    }


    /*! This method allocates memory of output matrices and vector
      as well as the workspace.
      This method should be called before the nmrPInverseDynamicData
      object is passed on to nmrPInverse function, as the memory
      required for output matrices and workspace are allocated
      here or to reallocate memory previously allocated by constructor.
      Typically this method is called from a code segment
      where it is safe to allocate memory and use
      the data and workspace space later.
      \param A The matrix for which SVD needs to be computed, size MxN
    */
    template <class _matrixOwnerTypeA>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A)
    {
        this->SetDimension(A.rows(), A.cols(), A.StorageOrder());
        this->AllocateOutputWorkspace(true, true);
    }


    /*! This method allocates memory of output matrices and vector
      and uses the memory provided by user for workspace.
      Check is made to ensure that memory provided by user is sufficient
      for SVD routine of LAPACK.
      This method should be called before the nmrPInverseDynamicData
      object is passed on to nmrPInverse function, as the memory
      required for output matrices and workspace are allocated
      here or to reallocate memory previously allocated by constructor.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing her to share the workspace for LAPACK.
      Typically this method is called from a code segment
      where it is safe to allocate memory and use
      the data and workspace space later.
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

    /*! This method must be called before the data object
      is passed to nmrPInverse function.
      The user provides the input matrix to specify size,
      along with matrix PInverse and workspace.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks are made on the
      validity of the input and its consitency with the size of input matrix.
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

    /*! This method must be called before the data object
      is passed to nmrPInverse function.
      The user provides the input matrix to specify size,
      along with matrix PInverse.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks are made on thec
      validity of the input and its consitency with the size of input matrix.
      Memory allocation for workspace is done by the method.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using the PInverse matrix elsewhere in the *same* thread.

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
      \name Retrieving results

      In order to get access to U, V^t and S, after the have been
      computed by calling nmrPInverse function, use the following
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


#ifndef SWIG
/*
   ****************************************************************************
                                  FIXED SIZE
   ****************************************************************************
 */

/*! This provides the data class for fixed size matrices
  and provides a easy to use template. That is
  nmrPInverseFixedSizeData<4, 3, VCT_COL_MAJOR> vs.
  nmrPInverseDataBase<vctFixedSizeMatrix<4, 3, VCT_COL_MAJOR>
  No extra workspace of allocation etc is required for fixed size.
 */
template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder>
class nmrPInverseFixedSizeData
{
public:
    typedef vct::size_type size_type;
#ifndef DOXYGEN
    enum {MIN_MN = (_rows < _cols) ? _rows : _cols};
    enum {LWORK_1 = (3 * MIN_MN + (_rows > _cols)) ? _rows : _cols};
    enum {LWORK_2 = 5 * MIN_MN};
    enum {LWORK_3 =
          (static_cast<size_type>(LWORK_1) > static_cast<size_type>(LWORK_2))
          ?
          static_cast<size_type>(LWORK_1)
          :
          static_cast<size_type>(LWORK_2)};
    enum {LWORK = _rows * _rows + _cols * _cols + static_cast<size_type>(MIN_MN) + static_cast<size_type>(LWORK_3) + _rows * _cols};
#endif // DOXYGEN

    /*! Type of the input matrix */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> MatrixTypeA;
    /*! Type of the pseudo inverse matrix */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _cols, _rows, _storageOrder> MatrixTypePInverse;
    /*! Type used for the workspace */
    typedef vctFixedSizeVector<CISSTNETLIB_DOUBLE, LWORK> VectorTypeWorkspace;
    /*! Type used for the U matrix of the SVD */
    typedef vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _rows, _rows, _storageOrder ? _rows : 1, _storageOrder ? 1 : _rows> MatrixTypeU;
    /*! Type used for the Vt matrix of the SVD */
    typedef vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _cols, _cols, _storageOrder ? _cols : 1, _storageOrder ? 1 : _cols> MatrixTypeVt;
    /*! Type used for the S vector of the SVD */
    typedef vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, MIN_MN, 1> VectorTypeS;
    /*! Type used for the workspace of SVD */
    typedef vctFixedSizeVectorRef<CISSTNETLIB_DOUBLE, LWORK_3, 1> VectorTypeSVDWorkspace;
    typedef vctFixedSizeMatrixRef<CISSTNETLIB_DOUBLE, _cols, _rows, _storageOrder? _cols : 1, _storageOrder ? 1 : _rows> MatrixTypeP;
private:
    /*! Matrix for pseudo inverse */
    MatrixTypePInverse PInverseMember;

    /*! Workspace, including memory for output of SVD
      Order is U, Vt, S, workspace for SVD
    */
    VectorTypeWorkspace WorkspaceMember;

    /*! References to U, Vt, S and workspace in vector Workspace */
    //@{
    MatrixTypeU UReference;
    MatrixTypeVt VtReference;
    VectorTypeS SReference;
    VectorTypeSVDWorkspace SVDWorkspaceReference;
    MatrixTypeP PReference;
    //*}

public:

#ifndef DOXYGEN
    /* This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated PInverse function as a
      friend of this class, which turns out to be not so easy with
      Visual C++ 7. Instead the Friend class provides a cumbersome way
      to get non-const references to the private data.  In order to
      get non-const references the user has to first create a object
      of nmrPInverseFixedSizeData::Friend and then user get* method on
      that object. Our philosophy here is that this should be deterent
      for a general user and should ring alarm bells in a reasonable
      programmer.
    */
    class Friend {
    private:
        nmrPInverseFixedSizeData<_rows, _cols, _storageOrder> & Data;
    public:
        Friend(nmrPInverseFixedSizeData<_rows, _cols, _storageOrder> &data): Data(data) {
        }
        inline MatrixTypePInverse & PInverse(void) {
            return Data.PInverseMember;
        }
        inline VectorTypeS & S(void) {
            return Data.SReference;
        }
        inline MatrixTypeU & U(void) {
            return Data.UReference;
        }
        inline MatrixTypeVt & Vt(void) {
            return Data.VtReference;
        }
        inline VectorTypeWorkspace & Workspace(void) {
            return Data.WorkspaceMember;
        }
        inline MatrixTypeP & P(void) {
            return Data.PReference;
        }
    };
    friend class Friend;
#endif

    /*! Default constructor.  This constructor sets all the references
        used to call the SVD routines using a contiguous block of
        memory (aka workspace).  The memory is stack allocated but the
        references are computed at runtime. */
    nmrPInverseFixedSizeData():
        UReference(WorkspaceMember.Pointer(0)),
        VtReference(WorkspaceMember.Pointer(_rows * _rows)),
        SReference(WorkspaceMember.Pointer(_rows * _rows + _cols * _cols)),
        SVDWorkspaceReference(WorkspaceMember.Pointer(_rows * _rows + _cols * _cols + MIN_MN)),
        PReference(WorkspaceMember.Pointer(_rows * _rows + _cols * _cols + MIN_MN + LWORK_3))
    {};

    /*!
      \name Retrieving results

      In order to get access to U, V^t and S, after the have been
      computed by calling nmrPInverse function, use the following
      methods.
    */
    //@{
    inline const MatrixTypePInverse & PInverse(void) const {
        return PInverseMember;
    }
    inline const VectorTypeS & S(void) const {
        return SReference;
    }
    inline const MatrixTypeU & U(void) const {
        return UReference;
    }
    inline const MatrixTypeVt & Vt(void) const {
        return VtReference;
    }
    inline const MatrixTypeP & P(void) const {
        return PReference;
    }
    //@}
};
#endif  // #ifndef SWIG


/*!
  \name Algorithm P-Inverse: Moore-Penrose pseudo-inverse.

  Calculates the Moore-Penrose pseudo-inverse of the M by N matrix A,
  and stores the result in PInverse.  The singular values of A are
  returned in S.  The left singular vectors are returned in U, and the
  right singular vectors are returned in V.

  \f$ A^{+} = V * \Sigma^{+} * U^{T}\f$

  where \f$ \Sigma^{+} \f$ is a \f$ N \times M \f$ matrix which is
  zero except for its min(m,n) diagonal elements, U is a \f$ M \times
  M \f$ orthogonal matrix, and V is a \f$ N \times N \f$ orthogonal
  matrix.  The diagonal elements of \f$ \Sigma^{+} \f$ are the
  reciprocal ofc non-zero singular values of A; they are real and
  non-negative, andc are returned in descending order.  The first \f$
  \mbox{min}(m,n) \f$ columns of U and V are the left and right
  singular vectors of A.

  \note This routine returns \f$ V^{T}\f$, not \f$V\f$.

  \note On exit, the content of A is altered.

  There are three ways to call this method to compute the
  pseudo-inverse of the matrix A.
  <ol>

  <li>User provides matrices \f$A\f$ and \f$A^{+}\f$.  The user first
  allocates memory for these matrices and vector.

  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> PInverse(4, 5);
  \endcode

  The user can then call the P-Inverse routine

  \code
  nmrPInverse(A, PInverse);
  \endcode

  The PInverse method verifies that the size of the data objects
  matches the input, and allocates workspace memory, which is
  deallocated when the function ends.  Remember that nmrPInverse
  alters the content of matrix A.  For fixed size the function call is
  templated by size and row/column major, e.g.

  \code
  nmrPInverse<4, 3, VCT_COL_MAJOR>(A, PInverse);
  \endcode

  <li>Using a preallocated data object.  The user first creates the
  input matrix.

  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> input(rows, cols , VCT_ROW_MAJOR);
  \endcode

  The user creates a data object which could be of type
  nmrPInverseFixedSizeData or nmrPInverseDynamicData corresponding to
  fixed size, dynamic matrix.

  \code
  nmrPInverseDynamicData data(input);
  \endcode

  Finally, call the nmrPInverse function:

  \code
  nmrPInverse(input, data);
  \endcode

  The contents of input matrix is modified by this routine.  The
  matrices U, Vt and vector S are available through the following
  methods:

  \code
  std::cout << data.U()
            << data.S()
            << data.Vt()
            << data.PInverse()
            << std::endl;
  \endcode

  <li>User provides matrix PInverse with workspace required by pseudo-inverse routine of LAPACK.
  User creates matrices:

  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> PInverse(4, 5);
  \endcode

  User also needs to allocate memory for workspace. This method is particularly
  useful when the user is using more than one numerical methods from the library
  and is willing to share the workspace between them. In such as case, the user
  can allocate the a memory greater than the maximum required by different methods.
  To aid the user determine the minimum workspace required (and not spend time digging
  LAPACK documentation) the library provides helper function
  nmrPInverseDynamicData::WorkspaceSize(input).

  \code
  vctDynamicVector<CISSTNETLIB_DOUBLE> Work(nmrPInverseDynamicData::WorkspaceSize(A));
  \endcode

  Call the SVD function
  \code
  nmrPInverse(A, PInverse, Work);
  \endcode

  For fixed size the above two steps are replaced by
  \code
  nmrPInverseFixedSizeData<4, 3, VCT_COL_MAJOR>::TypeWorkspace Work;
  nmrPInverse<4, 3, VCT_COL_MAJOR>(A, PInverse, Work);
  \endcode

  </ol>

  nmrPInverse uses ::nmrSVD and returns the error code of ::nmrSVD.
  See ::nmrSVD for further details.

  \note The PInverse functions make use of LAPACK routines.  To
  activate this code, set the CISST_HAS_CISSTNETLIB flag to ON during
  the configuration of cisst with CMake.

  \note The general rule for numerical functions which depend on
  LAPACK is that column-major matrices should be used everywhere, and
  that all matrices should be compact.

  \note For the specific case of PInverse, a valid result is also
  obtained if all the matrices are stored in row-major order.  This is
  an exeption to the general rule.  However, mixed-order is not
  tolerated.
 */
//@{

/*! This function checks for valid input and calls the LAPACK
  function. The approach behind this defintion of the function is that
  the user creates a data object from a code wherein it is safe to do
  memory allocation. This data object is then passed on to this method
  along with the matrix whose PInverse is to be computed. The data
  object has members S, U, Vt and PInverse, which can be accessed
  through calls to method get*() along with adequate workspace for
  LAPACK.  This function modifies the contents of matrix A.  For
  details about nature of the data matrices see text above.

  \param A A matrix of size MxN, of one of vctDynamicMatrix or vctDynamicMatrixRef
  \param data A data object of one of the types corresponding to
  input matrix

  \test nmrPInverseTest::TestDynamicColumnMajor
        nmrPInverseTest::TestDynamicRowMajor
        nmrPInverseTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseTest::TestDynamicRowMajorUserAlloc
*/
template <class _matrixOwnerType>
inline CISSTNETLIB_INTEGER nmrPInverse(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> &A, nmrPInverseDynamicData &data) CISST_THROW(std::runtime_error)
{
    typedef vct::size_type size_type;

    typename nmrPInverseDynamicData::Friend dataFriend(data);
    CISSTNETLIB_INTEGER ret_value;
    /* check that the size and storage order matches with Allocate() */
    if (A.StorageOrder() != dataFriend.StorageOrder()) {
        cmnThrow(std::runtime_error("nmrPInverse Solve: Storage order used for Allocate was different"));
    }
    if ((A.rows() != dataFriend.M()) || (A.cols() != dataFriend.N())) {
        cmnThrow(std::runtime_error("nmrPInverse Solve: Size used for Allocate was different"));
    }
    const size_type rows = A.rows();
    const size_type cols = A.cols();
    const size_type minmn = (rows < cols) ? rows : cols;
    const size_type maxmn = (rows > cols) ? rows : cols;

    ret_value = nmrSVD(A, dataFriend.U(), dataFriend.S(),
                       dataFriend.Vt(), dataFriend.Workspace());
    const CISSTNETLIB_DOUBLE eps = cmnTypeTraits<CISSTNETLIB_DOUBLE>::Tolerance() * dataFriend.S().at(0) * maxmn;

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


/*! Basic version of PInverse where user provides the input matrix as
  well as storage for output and workspace needed by LAPACK.  No mem
  allocation is done in this function, user allocates everything
  inlcuding workspace. The PInverse method verifies that the size of
  the data objects matchesc the input. See static methods in
  nmrPInverseDynamicData for helper functions that help determine the
  min workspace required.  This function modifies the contents of
  matrix A.  For sizes of other matrices see text above.

  \param A is a reference to a dynamic matrix of size MxN
  \param PInverse The output matrix for PInverse
  \param Workspace The workspace for LAPACK.

  \test nmrPInverseTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse, class _vectorOwnerTypeWorkspace>
inline CISSTNETLIB_INTEGER nmrPInverse(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> &PInverse, vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> &Workspace)
{
    nmrPInverseDynamicData svdData(A, PInverse, Workspace);
    CISSTNETLIB_INTEGER ret_value = nmrPInverse(A, svdData);
    return ret_value;
}


/*! Basic version of PInverse where user provides the input matrix as
  well as storage for output.  The PInverse method verifies that the
  size of the data objects matches the input and allocates workspace
  memory, which is deallocated when the function ends.  This function
  modifies the contents of matrix A.  For sizes of other matrices see
  text above.

  \param A is a reference to a dynamic matrix of size MxN
  \param PInverse The output matrices and vector for PInverse

  \test nmrPInverseTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseTest::TestDynamicRowMajorUserAlloc
 */

template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse>
inline CISSTNETLIB_INTEGER nmrPInverse(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypePInverse, CISSTNETLIB_DOUBLE> &PInverse)
{
    nmrPInverseDynamicData svdData(A, PInverse);
    CISSTNETLIB_INTEGER ret_value = nmrPInverse(A, svdData);
    return ret_value;
}


#ifndef SWIG
/*!  Basic version of pseudo inverse where user provides the input
  matrix as well as storage for output and workspace needed by LAPACK.
  No memory allocation is done in this function, user allocates
  everything including workspace. The nmrPInverse function verifies
  that the size of the S and workspace vector objects match the input.
  This function modifies the contents of matrix A.  For sizes of other
  matrices see text above.

  \param A Fixed size matrix of size MxN
  \param pInverse The output matrix for pseudo inverse
  \param workspace The workspace for LAPACK.

  \test nmrPInverseTest::TestFixedSizeColumnMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeColumnMajorMGeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMGeqN_T2
 */
template <vct::size_type _rows, vct::size_type _cols, vct::size_type _work, bool _storageOrder, class _dataPtrType>
inline CISSTNETLIB_INTEGER nmrPInverse(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> & A,
                             vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _cols, _rows, _storageOrder> & pInverse,
                             vctFixedSizeVectorBase<_work, 1, CISSTNETLIB_DOUBLE, _dataPtrType> & workspace)
{
    typedef vct::size_type size_type;
#if CMN_ASSERT_IS_DEFINED
    const size_type lwork = nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::LWORK;
#endif
    const size_type lwork_3 = nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::LWORK_3;
    const size_type minmn = nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::MIN_MN;
    const size_type maxmn = (_rows > _cols) ? _rows : _cols;
    //Assert if requirement is greater than size provided!
    CMN_ASSERT(lwork <= _work);
    // split Work into submatrices and vectors
    // for this we simply use the nmrSVD with dynamic ref as parameters as we dont have
    // nmrSVD with fixedsizeref as parameters
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> ARef(A);
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> URef(_rows, _rows,
                                     (_storageOrder) ? _rows : 1,
                                     (_storageOrder) ? 1 : _rows,
                                     workspace.Pointer(0));
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> VtRef(_cols, _cols,
                                      (_storageOrder) ? _cols : 1,
                                      (_storageOrder) ? 1 : _cols,
                                      workspace.Pointer(_rows*_rows));
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> SRef(minmn,
                                     workspace.Pointer(_rows * _rows + _cols * _cols));
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> SVDWorkspaceRef(lwork_3,
                                                workspace.Pointer(_rows * _rows + _cols * _cols + minmn));
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> PRef(_cols, _rows, (_storageOrder) ? _rows : 1, (_storageOrder) ? 1 : _cols,
                                     workspace.Pointer(_rows * _rows + _cols * _cols + minmn + lwork_3));
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> pInverseRef(pInverse);
    CISSTNETLIB_INTEGER ret_value;
    ret_value = nmrSVD(ARef, URef, SRef, VtRef, SVDWorkspaceRef);
    const CISSTNETLIB_DOUBLE eps = cmnTypeTraits<CISSTNETLIB_DOUBLE>::Tolerance() * SRef(0) * maxmn;
    /*
    vctDynamicMatrix<CISSTNETLIB_DOUBLE> SM (_cols, _rows, _storageOrder);
    SM.SetAll(0.);
    for (size_type irank = 0; irank < minmn; irank++) {
        if (SRef(irank) > eps) {
            SM(irank, irank) = 1.0/SRef(irank);
        }
    }
    PRef.ProductOf(SM, URef.Transpose());
    */
    size_type irank;
    PRef.SetAll(0.);
    for (irank = 0; irank < minmn; irank++) {
        if (SRef(irank) > eps) {
            PRef.Row(irank).ProductOf(URef.Transpose().Row(irank), 1.0/SRef(irank));
        } else {
            PRef.Row(irank).SetAll(0.);
        }
    }
    pInverseRef.ProductOf(VtRef.Transpose(), PRef);

    return ret_value;
}

/*! Basic version of pseudo-inverse where user provides the input
  matrix as well as storage for output.  The nmrPInverse function
  verifies that the size of the S vector object matches the input and
  allocates workspace memory, which is deallocated when the function
  ends.  This function modifies the contents of matrix A.  For sizes
  of other matrices see text above.

  \param A Fixed size matrix of size MxN
  \param pInverse The output matrix for pseudo inverse

  \test nmrPInverseTest::TestFixedSizeColumnMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeColumnMajorMGeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMGeqN_T2
 */
template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder>
inline CISSTNETLIB_INTEGER nmrPInverse(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> &A,
                             vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _cols, _rows, _storageOrder> &pInverse)
{
    typename nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::VectorTypeWorkspace workspace;
    CISSTNETLIB_INTEGER ret_value = nmrPInverse(A, pInverse, workspace);
    return ret_value;
}

/*! Specialized version of pseudo-inverse for Fixed Size Matrix.
  since all error checking is done at compile time.  The approach
  behind this defintion of the function is that the user creates a
  data object from a code wherein it is safe to do memory
  allocation. This data object is then passed on to this method along
  with the matrix whose SVD is to be computed. The data object has
  members S, U, and Vt, which can be accessed through calls to method
  get*() along with adequate workspace for LAPACK.  This function
  modifies the contents of matrix A.  For details about nature of the
  data matrices see text above.

  \param A A matrix of size MxN
  \param data A data object of one of the types corresponding to
  input matrix

  \test nmrPInverseTest::TestFixedSizeColumnMajorMLeqN
        nmrPInverseTest::TestFixedSizeRowMajorMLeqN
        nmrPInverseTest::TestFixedSizeColumnMajorMGeqN
        nmrPInverseTest::TestFixedSizeRowMajorMGeqN
 */
template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder>
inline CISSTNETLIB_INTEGER nmrPInverse(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> &A,
                             nmrPInverseFixedSizeData<_rows, _cols, _storageOrder> &data)
{
    typename nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::Friend dataFriend(data);
    /* all the checking is done by SVD */
    CISSTNETLIB_INTEGER ret_value = nmrPInverse(A, dataFriend.PInverse(), dataFriend.Workspace());
    return ret_value;
}
#endif  // #ifndef SWIG

//@}


#endif
