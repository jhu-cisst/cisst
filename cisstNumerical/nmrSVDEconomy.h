/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Ankur Kapoor, Anton Deguet
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
  \brief Declaration of nmrSVDEconomy
*/


#ifndef _nmrSVDEconomy_h
#define _nmrSVDEconomy_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

// Always include last
#include <cisstNumerical/nmrExport.h>


/*!
  \ingroup cisstNumerical

  \brief Data for SVD problem (Dynamic).

  The result of an SVD decomposition is composed of three different
  containers, the matrices U and V and the vector S defined by \f$ A =
  U * \Sigma * V^{T} \f$.  The sizes of these components must match
  exactly the input matrix A.  To ease the use of the SVD routine, the
  user can rely on the nmrSVDEconomyDynamicData class to perform the
  required memory allocation.  Furthermore, the underlying Fortran
  routine from LAPACK requires a workspace (aka a scratch space).
  This workspace can also be allocated by the nmrSVDEconomyDynamicData.

  Another good reason to use a "data" object is that the memory
  allocation can be performed once during an initialization phase
  while the function nmrSVDEconomy can be called numerous times later on
  without any new dynamic memory allocation.  This is crucial for such
  things as real time tasks.

  The SVD routine is somewhat specific in the sens that is can be used
  on either storage order, row major or column major, without any copy
  or transpose.  Nevertheless, the current implementation requires all
  the matrices to use the same storage order, i.e. if A is row major,
  both U and Vt must be stored row first.  Matrices and vectors must
  also be compact, i.e. use a contiguous block of memory.

  Any size or storage order mismatch will lead to an exception thrown
  (std::runtime_error).  Since we are using cmnThrow, it is possible
  to configure cisst (at compilation time) to abort the program
  instead of throwing an exception.

  The nmrSVDEconomyDynamicData class allows 4 different configurations:

  - Allocate automatically everything, i.e. the output (U, S and Vt)
    as well as the workspace.  This can be performed using either the
    constructor from input matrix (i.e. nmrSVDEconomyDynamicData(A)) or
    using the method Allocate(A).

  - Automatically allocate the workspace but rely on user allocated U,
    S and Vt.  The size of the problem, i.e. the input matrix A as
    well as the storage order will be deduced from U, S and Vt.  This
    can be performed using either the constructor from U, S and Vt
    (i.e. nmrSVDEconomyDynamicData(U, S, Vt)) or the method
    SetRefOutput(U, S, Vt).

  - Automatically allocate the output but rely on user allocated
    workspace.  This can be useful if the user decides to create a
    large workspace for multiple numerical routines.  The user will
    have to make sure his program is thread-safe.  This can be
    performed using either the constructor from A and the workspace
    (i.e. nmrSVDEconomyDynamicData(A, workspace)) or the method
    SetRefWorkspace(A, workspace).  Please note that the matrix A is
    required to compute the dimension of the problem since the only
    requirement or the workspace is that it must be large enough.

  - Don't allocate anything.  The user has to provide the containers
    he wants to use for U, S, Vt and the workspace.  In this case, the
    "data" is used mostly to check that all the containers are valid
    in terms of size and storage order.  This can be performed using
    either the constructor from U, S, Vt and workspace
    (i.e. nmrSVDEconomyDynamicData(U, S, Vt, workspace)) or the method
    SetRef(U, S, Vt, workspace).

  \sa nmrSVDEconomy
 */
class nmrSVDEconomyDynamicData {

public:
    /*! Type used for sizes within nmrSVDEconomyDynamicData.  This type
      is compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      #CISSTNETLIB_INTEGER. */
    typedef unsigned int size_type;

    /*! Matrix size type, i.e. vector of two elements: number of rows
        and columns. */
    typedef vctFixedSizeVector<size_type, 2> nsize_type;

protected:
    /*! Memory allocated for Workspace matrices if needed. */
    vctDynamicVector<CISSTNETLIB_DOUBLE> WorkspaceMemory;

    /*! Memory allocated for U, Vt Matrices and Vector S if needed.
       This method allocates a single block of memory for these 3
       containers; m x m elements of U followed by n x n elements of
       Vt followed by min (m, n) elements of S.
      */
    vctDynamicVector<CISSTNETLIB_DOUBLE> OutputMemory;

    /*! References to workspace or return types, these point either to
      user allocated memory or our memory chunks if needed.
     */
    //@{
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> UReference;
    vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> VtReference;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> SReference;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> WorkspaceReference;
    //@}

    /*! Just store M, N, and StorageOrder which are needed
       to check if A matrix passed to solve method matches
       the allocated size. */
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
      this method will ensure that the "data"
      (nmrSVDEconomyDynamicData) does not keep any memory allocated.
      This is for the case where a single "data" is used first to
      allocate everything and, later on, used with user allocated
      memory (for either the workspace or the output).  For example:
      \code
      vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(20, 20);
      vctRandom(A, 10, 10);
      nmrSVDEconomyDynamicData data(A); // allocate output AND workspace
      vctDynamicVector<CISSTNETLIB_DOUBLE> workspace(nmrSVDEconomyDynamicData::WorkspaceSize(A));
      data.SetRefWorkspace(workspace); // after all, use my own workspace
      \endcode
      \note The method SetDimension must have been called before.
    */
    inline void AllocateOutputWorkspace(bool allocateOutput, bool allocateWorkspace)
    {
        // allocate output
        if (allocateOutput) {
            const size_type minmn = (MMember < NMember) ? MMember : NMember;
            const size_type outputLength = MMember * minmn + NMember * NMember + minmn;
            this->OutputMemory.SetSize(outputLength);
            this->UReference.SetRef(MMember, minmn,
                                    (StorageOrderMember) ? minmn : 1,
                                    (StorageOrderMember) ? 1 : MMember,
                                    this->OutputMemory.Pointer(0));
            this->VtReference.SetRef(NMember, NMember,
                                     (StorageOrderMember) ? NMember : 1,
                                     (StorageOrderMember) ? 1 : NMember,
                                     this->OutputMemory.Pointer(MMember * minmn));
            this->SReference.SetRef(minmn,
                                    this->OutputMemory.Pointer(MMember * minmn + NMember * NMember), 1);
        } else {
            this->OutputMemory.SetSize(0);
        }
        // allocate workspace
        if (allocateWorkspace) {
            this->WorkspaceMemory.SetSize(WorkspaceSize(MMember, NMember));
            this->WorkspaceReference.SetRef(this->WorkspaceMemory);
        } else {
            this->WorkspaceMemory.SetSize(0);
        }
    }


    /*!  Verifies that the user provided references for the output
      match the size of the "data" as set by SetDimension.  This
      method also checks that the storage orders are consistent across
      the provided matrices and that all containers are compact.

      \note The method SetDimension must have been called before.
    */
    template <typename _matrixOwnerTypeU,
              typename _vectorOwnerTypeS,
              typename _matrixOwnerTypeVt>
    inline void ThrowUnlessOutputSizeIsCorrect(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & inU,
                                               vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & inS,
                                               vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & inVt) const
        CISST_THROW(std::runtime_error)
    {
         // check sizes and storage order
        const size_type minmn = (MMember < NMember) ? MMember : NMember;
        if ((inU.rows() != MMember ) || (inU.cols() != minmn)) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Size of matrix U is incorrect."));
        }
        if (inU.StorageOrder() != StorageOrderMember) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Storage order of U is incorrect."));
        }
        if (!inU.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Matrix U must be compact."));
        }
        if (! inVt.IsSquare(NMember)) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Size of matrix Vt is incorrect."));
        }
        if (inVt.StorageOrder() != StorageOrderMember) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Storage order of Vt is incorrect."));
        }
        if (!inVt.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Matrix Vt must be compact."));
        }
        if (minmn != inS.size()) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Size of vector S is incorrect."));
        }
        if (!inS.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Vector S must be compact."));
        }
    }


    /*! Verifies that the user provided references for the workspace
      match (or is greated than) the size of the "data" as set by
      SetDimension.  This method also checks that the workspace is
      compact.

      \note The method SetDimension must have been called before.
    */
    template <typename _vectorOwnerTypeWorkspace>
    inline void
    ThrowUnlessWorkspaceSizeIsCorrect(vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & inWorkspace) const
        CISST_THROW(std::runtime_error)
    {
        const size_type lwork = nmrSVDEconomyDynamicData::WorkspaceSize(MMember, NMember);
        if (lwork > inWorkspace.size()) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Workspace is too small."));
        }
        if (!inWorkspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData: Workspace must be compact."));
        }
    }


public:

    /*! Helper methods for user to set minimum working space required
      by LAPACK SVD routine.

      \param m, n The size of matrix whose SVD needs to be computed.
     */
    static inline size_type WorkspaceSize(size_type m, size_type n)
    {
        const size_type minmn = (m < n) ? m : n;
        const size_type maxmn = (m > n) ? m : n;
        const size_type lwork_1 = 3 * minmn + maxmn;
        const size_type lwork_2 = 5 * minmn;
        return (lwork_1 > lwork_2) ? lwork_1 : lwork_2;
    }

    /*! Helper method to determine the minimum working space required
      by LAPACK SVD routine.

      \param inA The matrix whose SVD needs to be computed
    */
    template <class _matrixOwnerTypeA>
    static inline size_type WorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & inA)
    {
        return nmrSVDEconomyDynamicData::WorkspaceSize(inA.rows(), inA.cols());
    }


    /*! Helper method to compute the size of the matrix S.  This
      method can be used before UpdateMatrixS to make sure that the
      size of S is correct.

      \param A The matrix to be decomposed using ::nmrSVDEconomy (it is used
      only to determine the sizes).
    */
    template <class _matrixOwnerTypeA>
    static inline
    nsize_type MatrixSSize(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        nsize_type matrixSize(A.rows(), (A.rows() < A.cols()) ? A.row() : A.cols());
        return matrixSize;
    }


    /*! Fill a matrix from the singular values.  Sets all the elements
      to zero and then replace the diagonal by the singular values
      (provided by vectorS).

      \param A Matrix decomposed using nmrSVDEconomy.  This is required to check the dimension of matrixS.
      \param vectorS Vector of singular values as computed by nmrSVDEconomy.
      \param matrixS Matrix with storage provided by the user.  It must have the same size as A.
    */
    template <class _matrixOwnerTypeA, class _matrixOwnerTypeS, class _vectorOwnerTypeS>
    static inline
    vctDynamicMatrixBase<_matrixOwnerTypeS, CISSTNETLIB_DOUBLE> &
    UpdateMatrixS(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                  const vctDynamicConstVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & vectorS,
                  vctDynamicMatrixBase<_matrixOwnerTypeS, CISSTNETLIB_DOUBLE> & matrixS)
        CISST_THROW(std::runtime_error)
    {
        const size_type minmn = (A.rows() < A.cols()) ? A.rows() : A.cols();
        if ((minmn != matrixS.rows()) || (minmn != matrixS.cols())) {
            cmnThrow(std::runtime_error("nmrSVDEconomyDynamicData::UpdateMatrixS: Size of matrix S is incorrect."));
        }
        matrixS.SetAll(0.0);
        matrixS.Diagonal().Assign(vectorS);
        return matrixS;
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
        nmrSVDEconomyDynamicData & Data;
    public:
        Friend(nmrSVDEconomyDynamicData &inData): Data(inData) {
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & S(void) {
            return Data.SReference;
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> & U(void) {
            return Data.UReference;
        }
        inline vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> & Vt(void) {
            return Data.VtReference;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & Workspace(void) {
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

    /*! The default constuctor.  For dynamic size, there are assigned
      default values, i.e. sets all the dimensions to zero.  These
      MUST be changed by calling the appropriate method.

      \sa nmrSVDEconomyDynamicData::Allocate
      nmrSVDEconomyDynamicData::SetRefOutput
      nmrSVDEconomyDynamicData::SetRefWorkspace
      nmrSVDEconomyDynamicData::SetRef
    */
    nmrSVDEconomyDynamicData():
        MMember(static_cast<size_type>(0)),
        NMember(static_cast<size_type>(0)),
        StorageOrderMember(VCT_COL_MAJOR)
    {
        AllocateOutputWorkspace(false, false);
    }

    /*! Constructor where the user specifies the size and storage
      order.  Memory allocation is performed for the output matrices
      and vectors as well as Workspace used by LAPACK. This should be
      used when the user doesn't care much about where the output
      should be stored and doesn't need to share the workspace between
      different algorithms.

      \param m, n Dimension of the matrix to be decomposed.
      \param storageOrder Storage order used for all matrices.

      \sa nmrSVDEconomyDynamicData::Allocate
    */
    nmrSVDEconomyDynamicData(size_type m, size_type n, bool storageOrder)
    {
        this->Allocate(m, n, storageOrder);
    }

    /*! Constructor where the user provides the input matrix to
      specify the size and storage order.  Memory allocation is
      performed for the output matrices and vectors as well as
      Workspace used by LAPACK. This should be used when the user
      doesn't care much about where the output should be stored and
      doesn't need to share the workspace between different
      algorithms.

      \param A input matrix

      \sa nmrSVDEconomyDynamicData::Allocate
    */
    template <class _matrixOwnerTypeA>
    nmrSVDEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        this->Allocate(A);
    }

    /*! Constructor where the user provides the input matrix to
      specify the size and storage order.  Memory allocation is
      performed for the output matrices and vectors only.  This
      constructor should be used when the user cares wants to avoid
      allocating different workspaces for different numerical
      routines.  Please note that since multiple routines can share
      the workspace, these routines must be called in a thread safe
      manner.

      \param A input matrix
      \param inWorkspace workspace

      \sa nmrSVDEconomyDynamicData::SetRefWorkspace
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    nmrSVDEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                      vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & inWorkspace)
    {
        this->SetRefWorkspace(A, inWorkspace);
    }

    /*! Constructor where the user provides the matrices U, Vt and
      vectors S as well as the workspace.  The data object now
      acts as a composite container to hold, pass and manipulate a
      convenient storage for SVD algorithm. Checks are made on the
      validity of the input and its consitency in terms of size ans
      storage order.  Please note that since the workspace and the
      input are now created by the user, special attention must be
      given to thread safety issues.

      \param inU, inS, inVt The output matrices and vector
      \param inWorkspace The workspace for LAPACK.

      \sa nmrSVDEconomyDynamicData::SetRef
    */
    template <typename _matrixOwnerTypeU,
              typename _vectorOwnerTypeS,
              typename _matrixOwnerTypeVt,
              typename _vectorOwnerTypeWorkspace>
    nmrSVDEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & inU,
                      vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & inS,
                      vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & inVt,
                      vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & inWorkspace)
    {
        this->SetRef(inU, inS, inVt, inWorkspace);
    }

    /*! Constructor where the user provides the matrices U, Vt and
      vectors S.  The workspace will be allocated and managed by the
      "data".  This constructor should be used when the user already
      has a storage for the data but doesn't care much about the
      workspace.

      \param inU, inS, inVt The output matrices and vector

      \sa nmrSVDEconomyDynamicData::SetRefOutput
    */
    template <typename _matrixOwnerTypeU,
              typename _vectorOwnerTypeS,
              typename _matrixOwnerTypeVt>
    nmrSVDEconomyDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & inU,
                      vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & inS,
                      vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & inVt)
    {
        this->SetRefOutput(inU, inS, inVt);
    }


    /*! This method allocates memory for the output matrices and
      vector as well as the workspace.  The input matrix is used only
      to determine the size of the problem as well as the storage
      order (i.e. #VCT_ROW_MAJOR or #VCT_COL_MAJOR).

      This method should be called before the nmrSVDEconomyDynamicData
      object is passed on to nmrSVDEconomy function.

      \param A The matrix for which SVD needs to be computed, size MxN
    */
    template <class _matrixOwnerTypeA>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        this->Allocate(A.rows(), A.cols(), A.StorageOrder());
    }

    /*! This method allocates the memory for the output matrices and
      vector and uses the memory provided by user for workspace.  The
      input matrix A is used to determine the size of the problem as
      well as the storage order.

      This method verifies that the workspace provided by the user is
      large enough and is compact.

      \param A The matrix for which SVD needs to be computed, size MxN.
      \param inWorkspace The vector used for workspace by LAPACK.
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    inline void SetRefWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & inWorkspace)
    {
        this->SetDimension(A.rows(), A.cols(), A.StorageOrder());

        // allocate output and set references
        this->AllocateOutputWorkspace(true, false);

        // set reference on user provided workspace
        this->ThrowUnlessWorkspaceSizeIsCorrect(inWorkspace);
        this->WorkspaceReference.SetRef(inWorkspace);
    }

    /*! This method allocates the memory for the output matrices and
      vector as well as the workspace.  This method is not meant to be
      a top-level user API, but is used by other overloaded Allocate
      methods.

      \param m Number of rows of input matrix A.
      \param n Number of cols of input matrix A.
      \param storageOrder Storage order of input matrix. One of #VCT_COL_MAJOR
      or #VCT_ROW_MAJOR.
    */
    void Allocate(size_type m, size_type n, bool storageOrder)
    {
        this->SetDimension(m, n, storageOrder);
        this->AllocateOutputWorkspace(true, true);
    }

    /*! This method doesn't allocate any memory as it relies on user
      provided matrices and vectors for the output as well as the
      workspace.

      The data object now acts as a composite container to hold,
      pass and manipulate a convenient storage for SVD algorithm.  The
      method tests that all the containers provided by the user have
      the correct size, storage order and are compact.

      \param inU, inS, inVt The output matrices and vector.
      \param inWorkspace The workspace.
    */
    template <typename _matrixOwnerTypeU,
              typename _vectorOwnerTypeS,
              typename _matrixOwnerTypeVt,
              typename _vectorOwnerTypeWorkspace>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & inU,
                vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & inS,
                vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & inVt,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & inWorkspace)
        CISST_THROW(std::runtime_error)
    {
        this->SetDimension(inU.rows(), inVt.rows(), inU.StorageOrder());
        this->AllocateOutputWorkspace(false, false);
        this->ThrowUnlessOutputSizeIsCorrect(inU, inS, inVt);
        this->ThrowUnlessWorkspaceSizeIsCorrect(inWorkspace);

        this->SReference.SetRef(inS);
        this->UReference.SetRef(inU);
        this->VtReference.SetRef(inVt);
        this->WorkspaceReference.SetRef(inWorkspace);
    }


    /*! This method allocates the memory for the workspace.  The
      output memory is provided by the user.  The method computes the
      size of the problem based on the user provided output and
      verifies that the output components (inU, inS, and inVt) are
      consistent with respect to their size and storage order.

      \param inU, inS, inVt The output matrices and vector.
    */
    template <typename _matrixOwnerTypeU,
              typename _vectorOwnerTypeS, typename _matrixOwnerTypeVt>
    void SetRefOutput(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & inU,
                      vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & inS,
                      vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & inVt)
        CISST_THROW(std::runtime_error)
    {
        this->SetDimension(inU.rows(), inVt.rows(), inU.StorageOrder());
        this->ThrowUnlessOutputSizeIsCorrect(inU, inS, inVt);

        this->SReference.SetRef(inS);
        this->UReference.SetRef(inU);
        this->VtReference.SetRef(inVt);

        AllocateOutputWorkspace(false, true);
    }

    /*! Const reference to the result vector S.  This method must be
      called after the data has been computed by the nmrSVDEconomy
      function. */
    inline const vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & S(void) const {
        return SReference;
    }

    /*! Const reference to the result matrix U.  This method must be
      called after the data has been computed by the nmrSVDEconomy
      function. */
    inline const vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> & U(void) const {
        return UReference;
    }
    /*! Const reference to the result matrix Vt (V transposed).  This
      method must be called after the data has been computed by
      the nmrSVDEconomy function. */
    inline const vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> & Vt(void) const {
        return VtReference;
    }
};








/*!
  \name Algorithm SVD: Singular Value Decomposition

  The ::nmrSVDEconomy functions compute the singular value decomposition
  (SVD) of a real \f$ M \times N \f$ matrix A, optionally computing
  the left and/or right singular vectors. The SVD problem is written:

  \f$ A = U * \Sigma * V^{T} \f$

  where \f$ \Sigma \f$ is a \f$ M \times N \f$ matrix which is zero
  except for its min(m,n) diagonal elements, U is a \f$ M \times M \f$
  orthogonal matrix, and V is a \f$ N \times N \f$ orthogonal matrix.
  The diagonal elements of \f$ \Sigma \f$ are the singular values of
  A; they are real and non-negative, and are returned in descending
  order.  The first \f$ \mbox{min}(m,n) \f$ columns of U and V are the
  left and right singular vectors of A.

  These functions are wrappers around the LAPACK routine dgesvd,
  therefore they share some features with the LAPACK routine:
  <ol>
  <li>They return \f$ V^{T} \f$, not \f$ V \f$.
  <li>On exit, the content of A is altered.
  <li>The vectors and matrices must be compact, i.e. use a contiguous
  block of memory.
  </ol>

  The ::nmrSVDEconomy functions add the following features:
  <ol>
  <li>A simplified interface to the cisstVector matrices, either
  vctDynamicMatrix
  <li>The possibility to use any storage order, i.e. #VCT_ROW_MAJOR or
  #VCT_COL_MAJOR.  In the specific case of SVD, the storage order can
  be treated as a transpose and the properties of the decomposition
  are preserved (::nmrSVDEconomy will swap U and Vt pointers).  This is
  performed at no extra cost, i.e. no copy back and forth of the
  elements nor extra memory allocation.
  <li>Input validation checks are performed, i.e. an
  std::runtime_error exception will be thrown if the sizes or storage
  order don't match or if the containers are not compact.
  <li>Helper classes to allocate memory for the output and workspace:
  nmrSVDEconomyDynamicData.
  </ol>

  There are different ways to call this function to compute the SVD of
  the matrix A.  These correspond to different overloaded ::nmrSVDEconomy
  functions:
  <ol>
  <li>Using a preallocated data object.

  The user creates the input matrix A:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(12, 24 , VCT_ROW_MAJOR); // 12 x 24 matrix
  vctRandom(A, -10.0, 10.0);
  \endcode
  The user allocates a data object which could be of
  type nmrSVDEconomyDynamicData.
  corresponding to fixed size or dynamic matrix A:
  \code
  nmrSVDEconomyDynamicData data(A);
  \endcode
  Call the nmrSVDEconomy function:
  \code
  nmrSVDEconomy(A, data);
  \endcode
  The content of input matrix A is modified by this routine.  The
  matrices U, Vt and vector S are available through the following
  methods
  \code
  std::cout << "U: " << data.U() << std::endl
            << "S: " << data.S() << std::endl
            << "V: " << data.Vt().Transpose() << std::endl;
  \endcode

  <li>The user provides the matrices U, Vt, and vector S.

  The User allocates memory for these matrices and vector:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
  vctRandom(A, -10.0, 10.0);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> U(5, 5);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> Vt(4, 4);
  vctDynamicVector<CISSTNETLIB_DOUBLE> S(4);
  \endcode
  Call the SVD routine:
  \code
  nmrSVDEconomy(A, U, S, Vt);
  \endcode
  The SVD function verifies that the size of the data objects
  matches the input, and allocates workspace memory, which is
  deallocated when the function ends.

  <li>The user provides the matrices U, Vt, and vector S along with
  workspace required by SVD routine of LAPACK.

  Create matrices and vector:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 4);
  vctRandom(A, -10.0, 10.0);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> U(5, 5);
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> Vt(4, 4);
  vctDynamicVector<CISSTNETLIB_DOUBLE> S(4);
  \endcode
  One also needs to allocate memory the for workspace. This approach is
  particularly useful when the user is using more than one numerical
  method from the library and is willing or need to share the workspace
  between them. In such as case, the user can allocate a block of memory
  greater than the minimum required by different methods.  To help the
  user determine the minimum workspace required the library provides
  the helper function nmrSVDEconomyDynamicData::WorkspaceSize().
  \code
  vctDynamicVector<CISSTNETLIB_DOUBLE> workspace(nmrSVDEconomyDynamicData::WorkspaceSize(A));
  \endcode
  Call the SVD function:
  \code
  nmrSVDEconomy(A, U, S, Vt, workspace);
  \endcode

  </ol>

  \note The SVD functions make use of LAPACK routines.  To activate
  this code, set the CISST_HAS_CISSTNETLIB flag to ON during the
  configuration of cisst with CMake.

  \note The general rule for numerical functions which depend on
  LAPACK is that column major matrices should be used everywhere, and
  that all matrices should be compact.  In this case, both row major
  and column major are allowed but they must not be mixed in a
  data object.
 */
//@{


/*! This function solves the SVD problem for a dynamic matrix using an
  nmrSVDEconomyDynamicData.

  This function checks for valid input (size, storage order and
  compact) and calls the LAPACK function.  If the input doesn't match
  the data, an exception is thrown (\c std::runtime_error).

  This function modifies the input matrix A and stores the results in
  the data.  Each component of the result can be obtained via the
  const methods nmrSVDEconomyDynamicData::U(), nmrSVDEconomyDynamicData::S()
  and nmrSVDEconomyDynamicData::Vt().

  \param A A matrix of size MxN, either vctDynamicMatrix or vctDynamicMatrixRef.
  \param data A data object corresponding to the input matrix.

  \test nmrSVDEconomyTest::TestDynamicColumnMajor
        nmrSVDEconomyTest::TestDynamicRowMajor
        nmrSVDEconomyTest::TestDynamicColumnMajorUserAlloc
        nmrSVDEconomyTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerType>
inline CISSTNETLIB_INTEGER nmrSVDEconomy(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> & A,
                                         nmrSVDEconomyDynamicData & data)
    CISST_THROW(std::runtime_error)
{
    typename nmrSVDEconomyDynamicData::Friend dataFriend(data);
    CISSTNETLIB_INTEGER Info;
    char m_Jobu = 'S';
    char m_Jobvt = 'A';
    CISSTNETLIB_INTEGER m_Lwork = static_cast<CISSTNETLIB_INTEGER>(nmrSVDEconomyDynamicData::WorkspaceSize(dataFriend.M(),
                                                                                                           dataFriend.N()));
    /* check that storage order matches with Allocate() */
    if (A.StorageOrder() != dataFriend.StorageOrder()) {
        cmnThrow(std::runtime_error("nmrSVDEconomy: Storage order used for Allocate was different"));
    }
    /* check sizes */
    if ((dataFriend.M() != A.rows()) || (dataFriend.N() != A.cols())) {
        cmnThrow(std::runtime_error("nmrSVDEconomy: Size used for Allocate was different"));
    }
    /* check that the matrices are compact */
    if (! A.IsCompact()) {
        cmnThrow(std::runtime_error("nmrSVDEconomy: Requires a compact matrix"));
    }

    /* Based on storage order, permute U and Vt as well as dimension */
    CISSTNETLIB_DOUBLE *UPtr, *VtPtr;
    CISSTNETLIB_INTEGER m_Lda, m_Ldu, m_Ldvt;

    if (A.IsColMajor()) {
        m_Lda = (1 > dataFriend.M()) ? 1 : dataFriend.M();
        m_Ldu = dataFriend.M();
        m_Ldvt = dataFriend.N();
        UPtr = dataFriend.U().Pointer();
        VtPtr = dataFriend.Vt().Pointer();
    } else {
        m_Lda = (1 > dataFriend.N()) ? 1 : dataFriend.N();
        m_Ldu = dataFriend.N();
        m_Ldvt = dataFriend.M();
        UPtr = dataFriend.Vt().Pointer();
        VtPtr = dataFriend.U().Pointer();
    }

    // for versions based on gfortran/lapack, CISSTNETLIB_VERSION is
    // defined
#if defined(CISSTNETLIB_VERSION)
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgesvd_(&m_Jobu, &m_Jobvt, &m_Ldu, &m_Ldvt,
                        A.Pointer(), &m_Lda, dataFriend.S().Pointer(),
                        UPtr, &m_Ldu,
                        VtPtr, &m_Ldvt,
                        dataFriend.Workspace().Pointer(), &m_Lwork, &Info);
#endif
#else // no major version
    dgesvd_(&m_Jobu, &m_Jobvt, &m_Ldu, &m_Ldvt,
            A.Pointer(), &m_Lda, dataFriend.S().Pointer(),
            UPtr, &m_Ldu,
            VtPtr, &m_Ldvt,
            dataFriend.Workspace().Pointer(), &m_Lwork, &Info);
#endif // CISSTNETLIB_VERSION
#else
   ftnlen jobu_len = (ftnlen)1, jobvt_len = (ftnlen)1;
   la_dzlapack_MP_sgesvd_nat(&m_Jobu, &m_Jobvt, &m_Ldu, &m_Ldvt,
                             A.Pointer(), &m_Lda, dataFriend.S().Pointer(),
                             UPtr, &m_Ldu,
                             VtPtr, &m_Ldvt,
                             dataFriend.Workspace().Pointer(), &m_Lwork, &Info,
                             jobu_len, jobvt_len);
#endif
    return Info;
}

/*! This function solves the SVD problem for a dynamic matrix using
  the storage provided by the user for both the output (U, S, Vt) and
  the workspace.

  Internally, a data is created using the storage provided by the
  user (see nmrSVDEconomyDynamicData::SetRef).  While the data is
  being build, the consistency of the output and workspace is checked.
  Then, the nmrSVDEconomy(A, data) function can be used safely.

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD
  \param Workspace The workspace for LAPACK.

  \test nmrSVDEconomyTest::TestDynamicColumnMajorUserAlloc
        nmrSVDEconomyTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypeU,
          class _vectorOwnerTypeS, class _matrixOwnerTypeVt,
          class _vectorOwnerTypeWorkspace>
inline CISSTNETLIB_INTEGER nmrSVDEconomy(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                               vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & U,
                               vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & S,
                               vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & Vt,
                               vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & Workspace)
{
    nmrSVDEconomyDynamicData svdData(U, S, Vt, Workspace);
    CISSTNETLIB_INTEGER ret_value = nmrSVDEconomy(A, svdData);
    return ret_value;
}

/*! This function solves the SVD problem for a dynamic matrix using
  the storage provided by the user for the output (U, S, Vt).  A
  workspace will be dynamically allocated.

  Internally, a data is created using the storage provided by the
  user (see nmrSVDEconomyDynamicData::SetRefOutput).  While the data
  is being build, the consistency of the output is checked.  Then, the
  nmrSVDEconomy(A, data) function can be used safely.

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD

  \test nmrSVDEconomyTest::TestDynamicColumnMajorUserAlloc
        nmrSVDEconomyTest::TestDynamicRowMajorUserAlloc

  \warning Again, this method will dynamically allocate a workspace at
  each call.  This is somewhat inefficient if the method is to be
  called many times.  For a real-time task, the dynamic allocation
  might even break your application.
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypeU,
          class _vectorOwnerTypeS, class _matrixOwnerTypeVt>
inline CISSTNETLIB_INTEGER nmrSVDEconomy(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                               vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & U,
                               vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & S,
                               vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & Vt)
{
    nmrSVDEconomyDynamicData svdData(U, S, Vt);
    CISSTNETLIB_INTEGER ret_value = nmrSVDEconomy(A, svdData);
    return ret_value;
}


//@}


#endif
