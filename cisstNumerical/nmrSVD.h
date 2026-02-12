/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Anton Deguet
  Created on: 2005-10-18

  (C) Copyright 2005-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrSVD
*/


#ifndef _nmrSVD_h
#define _nmrSVD_h

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
  user can rely on the nmrSVDDynamicData class to perform the
  required memory allocation.  Furthermore, the underlying Fortran
  routine from LAPACK requires a workspace (aka a scratch space).
  This workspace can also be allocated by the nmrSVDDynamicData.

  Another good reason to use a "data" object is that the memory
  allocation can be performed once during an initialization phase
  while the function nmrSVD can be called numerous times later on
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

  The nmrSVDDynamicData class allows 4 different configurations:

  - Allocate automatically everything, i.e. the output (U, S and Vt)
    as well as the workspace.  This can be performed using either the
    constructor from input matrix (i.e. nmrSVDDynamicData(A)) or
    using the method Allocate(A).

  - Automatically allocate the workspace but rely on user allocated U,
    S and Vt.  The size of the problem, i.e. the input matrix A as
    well as the storage order will be deduced from U, S and Vt.  This
    can be performed using either the constructor from U, S and Vt
    (i.e. nmrSVDDynamicData(U, S, Vt)) or the method
    SetRefOutput(U, S, Vt).

  - Automatically allocate the output but rely on user allocated
    workspace.  This can be useful if the user decides to create a
    large workspace for multiple numerical routines.  The user will
    have to make sure his program is thread-safe.  This can be
    performed using either the constructor from A and the workspace
    (i.e. nmrSVDDynamicData(A, workspace)) or the method
    SetRefWorkspace(A, workspace).  Please note that the matrix A is
    required to compute the dimension of the problem since the only
    requirement or the workspace is that it must be large enough.

  - Don't allocate anything.  The user has to provide the containers
    he wants to use for U, S, Vt and the workspace.  In this case, the
    "data" is used mostly to check that all the containers are valid
    in terms of size and storage order.  This can be performed using
    either the constructor from U, S, Vt and workspace
    (i.e. nmrSVDDynamicData(U, S, Vt, workspace)) or the method
    SetRef(U, S, Vt, workspace).

  \sa nmrSVD
 */
class nmrSVDDynamicData {

public:
    /*! Type used for sizes within nmrSVDDynamicData.  This type
      is compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      #CISSTNETLIB_INTEGER. */
    typedef vct::size_type size_type;

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
    inline void SetDimension(size_type m,
                             size_type n,
                             bool storageOrder)
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
      (nmrSVDDynamicData) does not keep any memory allocated.
      This is for the case where a single "data" is used first to
      allocate everything and, later on, used with user allocated
      memory (for either the workspace or the output).  For example:
      \code
      vctDynamicMatrix<double> A(20, 20);
      vctRandom(A, 10, 10);
      nmrSVDDynamicData data(A); // allocate output AND workspace
      vctDynamicVector<double> workspace(nmrSVDDynamicData::WorkspaceSize(A));
      data.SetRefWorkspace(workspace); // after all, use my own workspace
      \endcode
      \note The method SetDimension must have been called before.
    */
    inline void AllocateOutputWorkspace( bool allocateOutput,
                                         bool allocateWorkspace)
    {

        // allocate output
        if (allocateOutput) {
            const size_type minmn =(MMember<NMember)? MMember: NMember;
            const size_type outputLength = ( MMember * MMember +
                                             NMember * NMember +
                                             minmn );
            this->OutputMemory.SetSize(outputLength);
            this->UReference.SetRef(MMember, MMember,
                                    (StorageOrderMember) ? MMember : 1,
                                    (StorageOrderMember) ? 1 : MMember,
                                    this->OutputMemory.Pointer(0));
            this->VtReference.SetRef(NMember, NMember,
                                     (StorageOrderMember)? NMember: 1,
                                     (StorageOrderMember)? 1: NMember,
                                     this->OutputMemory.Pointer(MMember * MMember));
            this->SReference.SetRef((MMember<NMember)? MMember:NMember,
                                    this->OutputMemory.Pointer(MMember*MMember + NMember*NMember),
                                    1);
        }
        else {
            this->OutputMemory.SetSize(0);
        }
        // allocate workspace
        if (allocateWorkspace) {
            this->WorkspaceMemory.SetSize( WorkspaceSize( MMember,
                                                          NMember ) );
            this->WorkspaceReference.SetRef(this->WorkspaceMemory);
        }
        else {
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
    inline
    void ThrowUnlessOutputSizeIsCorrect(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE>& inU,
                                        vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE>& inS,
                                        vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE>& inVt) const
        CISST_THROW(std::runtime_error)
    {

        // check sizes and storage order
        if (! inU.IsSquare(MMember)) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Size of matrix U is incorrect."));
        }
        if (inU.StorageOrder() != StorageOrderMember) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Storage order of U is incorrect."));
        }
        if (!inU.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Matrix U must be compact."));
        }
        if (! inVt.IsSquare(NMember)) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Size of matrix Vt is incorrect."));
        }
        if (inVt.StorageOrder() != StorageOrderMember) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Storage order of Vt is incorrect."));
        }
        if (!inVt.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Matrix Vt must be compact."));
        }
        const size_type minmn = (MMember < NMember)? MMember: NMember;
        if (minmn != inS.size()) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Size of vector S is incorrect."));
        }
        if (!inS.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Vector S must be compact."));
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
    ThrowUnlessWorkspaceSizeIsCorrect(vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE>& inWorkspace) const
        CISST_THROW(std::runtime_error)
    {

        const size_type lwork =
            nmrSVDDynamicData::WorkspaceSize(MMember, NMember);

        if (lwork > inWorkspace.size()) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Workspace is too small."));
        }

        if (!inWorkspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData: Workspace must be compact."));
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
    static inline
    size_type WorkspaceSize( vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE>& inA)
    {
        return nmrSVDDynamicData::WorkspaceSize(inA.rows(), inA.cols());
    }


    /*! Helper method to compute the size of the matrix S.  This
      method can be used before UpdateMatrixS to make sure that the
      size of S is correct.

      \param A The matrix to be decomposed using ::nmrSVD (it is used
      only to determine the sizes).
    */
    template <class _matrixOwnerTypeA>
    static inline
    nsize_type MatrixSSize(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        nsize_type matrixSize(A.rows(), A.cols());
        return matrixSize;
    }


    /*! Fill a matrix from the singular values.  Sets all the elements
      to zero and then replace the diagonal by the singular values
      (provided by vectorS).

      \param A Matrix decomposed using nmrSVD.  This is required to check the dimension of matrixS.
      \param vectorS Vector of singular values as computed by nmrSVD.
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
        if ((A.rows() != matrixS.rows()) || (A.cols() != matrixS.cols())) {
            cmnThrow(std::runtime_error("nmrSVDDynamicData::UpdateMatrixS: Size of matrix S is incorrect."));
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
        nmrSVDDynamicData & Data;
    public:
        Friend(nmrSVDDynamicData &inData): Data(inData) {
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
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE>& Workspace(void){
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

      \sa nmrSVDDynamicData::Allocate
      nmrSVDDynamicData::SetRefOutput
      nmrSVDDynamicData::SetRefWorkspace
      nmrSVDDynamicData::SetRef
    */
    nmrSVDDynamicData():
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

      \sa nmrSVDDynamicData::Allocate
    */
    nmrSVDDynamicData(size_type m, size_type n, bool storageOrder)
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

      \sa nmrSVDDynamicData::Allocate
    */
    template <class _matrixOwnerTypeA>
    nmrSVDDynamicData(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
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

      \sa nmrSVDDynamicData::SetRefWorkspace
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    nmrSVDDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE>& A,
                      vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE>& inWorkspace)
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

      \sa nmrSVDDynamicData::SetRef
    */
    template <typename _matrixOwnerTypeU,
              typename _vectorOwnerTypeS,
              typename _matrixOwnerTypeVt,
              typename _vectorOwnerTypeWorkspace>
    nmrSVDDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & inU,
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

      \sa nmrSVDDynamicData::SetRefOutput
    */
    template <typename _matrixOwnerTypeU,
              typename _vectorOwnerTypeS,
              typename _matrixOwnerTypeVt>
    nmrSVDDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & inU,
                      vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & inS,
                      vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & inVt)
    {
        this->SetRefOutput(inU, inS, inVt);
    }


    /*! This method allocates memory for the output matrices and
      vector as well as the workspace.  The input matrix is used only
      to determine the size of the problem as well as the storage
      order (i.e. #VCT_ROW_MAJOR or #VCT_COL_MAJOR).

      This method should be called before the nmrSVDDynamicData
      object is passed on to nmrSVD function.

      \param A The matrix for which SVD needs to be computed, size MxN
    */
    template <class _matrixOwnerTypeA>
    inline
    void Allocate(const vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
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
    inline
    void SetRefWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
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
        this->SetDimension(inU.rows(),inVt.rows(),inU.StorageOrder());
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
              typename _vectorOwnerTypeS,
              typename _matrixOwnerTypeVt>
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
      called after the data has been computed by the nmrSVD
      function. */
    inline
    const vctDynamicVectorRef<CISSTNETLIB_DOUBLE>& S(void) const
    { return SReference; }

    /*! Const reference to the result matrix U.  This method must be
      called after the data has been computed by the nmrSVD
      function. */
    inline
    const vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> & U(void) const
    { return UReference; }
    /*! Const reference to the result matrix Vt (V transposed).  This
      method must be called after the data has been computed by
      the nmrSVD function. */
    inline
    const vctDynamicMatrixRef<CISSTNETLIB_DOUBLE> & Vt(void) const
    { return VtReference; }

};





/*!
  \ingroup cisstNumerical

  \brief Data of SVD problem (Fixed size).

  This class is similar to nmrSVDDynamicData except that it is
  dedicated to fixed size containers.  While nmrSVDDynamicData is
  designed to be modified dynamically, nmrSVDFixedSizeData is
  fully defined at compilation time using template parameters.  The
  required parameters are the dimension of the input matrix and its
  storage order:
  \code
  nmrSVDFixedSizeData<4, 3, VCT_COL_MAJOR> data;
  \endcode

  \note An object of type nmrSVDFixedSizeData contains the memory
  required for the output and the workspace, i.e. its actual size will
  be equal to the memory required to store the matrices U and Vt as
  the vectors S and workspace.

  \note There is no dynamic memory allocation (no \c new) and the
  memory can not be used by reference.  To use memory by reference,
  one must use nmrSVDDynamicData with vctDynamicMatrixRef and
  vctDynamicVectorRef (these dynamic references can actually be used
  to overlay a fixed size container).
 */
#ifndef SWIG
template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder = VCT_ROW_MAJOR>
class nmrSVDFixedSizeData
{
public:
#ifndef DOXYGEN
    typedef vct::size_type size_type;

    enum {M = (_storageOrder == VCT_COL_MAJOR) ? _rows : _cols};
    enum {N = (_storageOrder == VCT_COL_MAJOR) ? _cols : _rows};
    enum {MIN_MN =
          (static_cast<size_type>(M) < static_cast<size_type>(N))
          ?
          static_cast<size_type>(M)
          :
          static_cast<size_type>(N)};
    enum {LWORK_1 =
          (3 * static_cast<size_type>(MIN_MN) + (static_cast<size_type>(M)) > static_cast<size_type>(N))
          ?
          static_cast<size_type>(M)
          :
          static_cast<size_type>(N)};
    enum {LWORK_2 = 5 * MIN_MN};
    enum {LWORK =
          (static_cast<size_type>(LWORK_1) > static_cast<size_type>(LWORK_2))
          ?
          static_cast<size_type>(LWORK_1)
          :
          static_cast<size_type>(LWORK_2)};
#endif // DOXYGEN
    /*! Type of the input matrix A (size and storage order computed
      from the data template parameters). */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> MatrixTypeA;
    /*! Type of the output matrix U (size and storage order computed
      from the data template parameters). */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _rows, _storageOrder> MatrixTypeU;
    /*! Type of the output vector S (size and storage order computed
      from the data template parameters). */
    typedef vctFixedSizeVector<CISSTNETLIB_DOUBLE, MIN_MN> VectorTypeS;
    /*! Type of matrix to be filled from vector S (size and storage
      order computed from the data template parameters). */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> MatrixTypeS;
    /*! Type of the output matrix Vt (size and storage order computed
      from the data template parameters). */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _cols, _cols, _storageOrder> MatrixTypeVt;
    /*! Type of the workspace vector (size and storage order computed
      from the data template parameters). */
    typedef vctFixedSizeVector<CISSTNETLIB_DOUBLE, LWORK> VectorTypeWorkspace;

protected:
    MatrixTypeU UMember;                 /*!< Data member used to store the output matrix U. */
    MatrixTypeVt VtMember;               /*!< Data member used to store the output matrix Vt. */
    VectorTypeS SMember;                 /*!< Data member used to store the output vector S. */
    VectorTypeWorkspace WorkspaceMember; /*!< Data member used to store the workspace vector. */

public:
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated SVD function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrSVDFixedSizeData<_rows, _cols, _storageOrder>& Data;
    public:
        Friend(nmrSVDFixedSizeData<_rows, _cols, _storageOrder> &inData): Data(inData)
        {}

        inline VectorTypeS & S(void)
        { return Data.SMember; }

        inline MatrixTypeU & U(void)
        { return Data.UMember; }

        inline MatrixTypeVt & Vt(void)
        { return Data.VtMember; }

        inline VectorTypeWorkspace & Workspace(void)
        { return Data.WorkspaceMember; }
    };
    friend class Friend;
#endif // DOXYGEN

    /*! Default constructor.  Does nothing since the allocation is
      performed on the stack. */
    nmrSVDFixedSizeData() {};

    /*! Const reference to the result vector S.  This method must be
      called after the data has been computed by the nmrSVD
      function. */
    inline const VectorTypeS & S(void) const
    { return SMember; }

    /*! Const reference to the result matrix U.  This method must be
      called after the data has been computed by the nmrSVD
      function. */
    inline const MatrixTypeU & U(void) const
    { return UMember; }

    /*! Const reference to the result matrix Vt (V transposed).  This
      method must be called after the data has been computed by
      the nmrSVD function. */
    inline const MatrixTypeVt & Vt(void) const
    { return VtMember; }

    /*! Fill a matrix from the singular values.  Sets all the elements
      to zero and then replace the diagonal by the singular values
      (provided by vectorS). */
    static inline
    MatrixTypeS & UpdateMatrixS( const VectorTypeS & vectorS,
                                 MatrixTypeS & matrixS )
    {
        matrixS.SetAll(0.0);
        matrixS.Diagonal().Assign(vectorS);
        return matrixS;
    }
};
#endif // SWIG




/*!
  \name Algorithm SVD: Singular Value Decomposition

  The ::nmrSVD functions compute the singular value decomposition
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

  The ::nmrSVD functions add the following features:
  <ol>
  <li>A simplified interface to the cisstVector matrices, either
  vctDynamicMatrix or vctFixedSizeMatrix.
  <li>The possibility to use any storage order, i.e. #VCT_ROW_MAJOR or
  #VCT_COL_MAJOR.  In the specific case of SVD, the storage order can
  be treated as a transpose and the properties of the decomposition
  are preserved (::nmrSVD will swap U and Vt pointers).  This is
  performed at no extra cost, i.e. no copy back and forth of the
  elements nor extra memory allocation.
  <li>Input validation checks are performed, i.e. an
  std::runtime_error exception will be thrown if the sizes or storage
  order don't match or if the containers are not compact.
  <li>Helper classes to allocate memory for the output and workspace:
  nmrSVDFixedSizeData and nmrSVDDynamicData.
  </ol>

  There are different ways to call this function to compute the SVD of
  the matrix A.  These correspond to different overloaded ::nmrSVD
  functions:
  <ol>
  <li>Using a preallocated data object.

  The user creates the input matrix A:
  \code
  vctDynamicMatrix<double> A(12, 24 , VCT_ROW_MAJOR); // 12 x 24 matrix
  vctRandom(A, -10.0, 10.0);
  \endcode
  The user allocates a data object which could be of
  type nmrSVDFixedSizeData or nmrSVDDynamicData.
  corresponding to fixed size or dynamic matrix A:
  \code
  nmrSVDDynamicData data(A);
  \endcode
  Call the nmrSVD function:
  \code
  nmrSVD(A, data);
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
  vctDynamicMatrix<double> A(5, 4);
  vctRandom(A, -10.0, 10.0);
  vctDynamicMatrix<double> U(5, 5);
  vctDynamicMatrix<double> Vt(4, 4);
  vctDynamicVector<double> S(4);
  \endcode
  Call the SVD routine:
  \code
  nmrSVD(A, U, S, Vt);
  \endcode
  The SVD function verifies that the size of the data objects
  matches the input, and allocates workspace memory, which is
  deallocated when the function ends.

  <li>The user provides the matrices U, Vt, and vector S along with
  workspace required by SVD routine of LAPACK.

  Create matrices and vector:
  \code
  vctDynamicMatrix<double> A(5, 4);
  vctRandom(A, -10.0, 10.0);
  vctDynamicMatrix<double> U(5, 5);
  vctDynamicMatrix<double> Vt(4, 4);
  vctDynamicVector<double> S(4);
  \endcode
  One also needs to allocate memory the for workspace. This approach is
  particularly useful when the user is using more than one numerical
  method from the library and is willing or need to share the workspace
  between them. In such as case, the user can allocate a block of memory
  greater than the minimum required by different methods.  To help the
  user determine the minimum workspace required the library provides
  the helper function nmrSVDDynamicData::WorkspaceSize().
  \code
  vctDynamicVector<double> workspace(nmrSVDDynamicData::WorkspaceSize(A));
  \endcode
  Call the SVD function:
  \code
  nmrSVD(A, U, S, Vt, workspace);
  \endcode

  For fixed size containers, the example above would be:
  \code
  vctFixedSizeMatrix<double, 5, 4> A;
  vctRandom(A, -10.0, 10.0);
  vctFixedSizeMatrix<double, 5, 5> U;
  vctFixedSizeMatrix<double, 4, 4> Vt;
  vctFixedVector<double, 4> S;
  nmrSVDFixedSizeData<4, 3, VCT_COL_MAJOR>::VectorTypeWorkspace Workspace;
  nmrSVD(A, U, S, Vt, Workspace);
  \endcode

  </ol>

  The functions ::nmrSVD return the status code as defined by LAPACK, i.e.:
  <ol>

  <li>0: Successful exit.

  <li>Greater than 0: If SBDSQR did not converge, return value
      specifies how many superdiagonals of an intermediate bidiagonal
      form B did not converge to zero. See the description of
      workspace above for details.

      In this case Workspace(2:MIN(M,N)) contains the unconverged
      superdiagonal elements of an upper bidiagonal matrix \f$B\f$
      whose diagonal is in \f$S\f$ (not necessarily sorted). \f$B\f$
      satisfies \f$A = U * B * V^{T}\f$, so it has the same singular
      values as \f$A\f$, and singular vectors related by \f$U\f$ and
      \f$V^T\f$.

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
  nmrSVDDynamicData.

  This function checks for valid input (size, storage order and
  compact) and calls the LAPACK function.  If the input doesn't match
  the data, an exception is thrown (\c std::runtime_error).

  This function modifies the input matrix A and stores the results in
  the data.  Each component of the result can be obtained via the
  const methods nmrSVDDynamicData::U(), nmrSVDDynamicData::S()
  and nmrSVDDynamicData::Vt().

  \param A A matrix of size MxN, either vctDynamicMatrix or vctDynamicMatrixRef.
  \param data A data object corresponding to the input matrix.

  \test nmrSVDTest::TestDynamicColumnMajor
        nmrSVDTest::TestDynamicRowMajor
        nmrSVDTest::TestDynamicColumnMajorUserAlloc
        nmrSVDTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerType>
inline
CISSTNETLIB_INTEGER nmrSVD(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> & A,
                           nmrSVDDynamicData & data)
    CISST_THROW(std::runtime_error)
{
    typename nmrSVDDynamicData::Friend dataFriend(data);
    CISSTNETLIB_INTEGER Info;
    char m_Jobu = 'A';
    char m_Jobvt = 'A';
    CISSTNETLIB_INTEGER m_Lwork =
        static_cast<CISSTNETLIB_INTEGER>(nmrSVDDynamicData::WorkspaceSize(dataFriend.M(),
                                                                          dataFriend.N()));

    /* check that storage order matches with Allocate() */
    if (A.StorageOrder() != dataFriend.StorageOrder()) {
        cmnThrow(std::runtime_error("nmrSVD: Storage order used for Allocate was different"));
    }
    /* check sizes */
    if ((dataFriend.M() != A.rows()) || (dataFriend.N() != A.cols())) {
        cmnThrow(std::runtime_error("nmrSVD: Size used for Allocate was different"));
    }
    /* check that the matrices are compact */
    if (! A.IsCompact()) {
        cmnThrow(std::runtime_error("nmrSVD: Requires a compact matrix"));
    }

    /* Based on storage order, permute U and Vt as well as dimension */
    CISSTNETLIB_DOUBLE *UPtr, *VtPtr;
    CISSTNETLIB_INTEGER m_Lda, m_Ldu, m_Ldvt;

    CISSTNETLIB_INTEGER dfM = static_cast<CISSTNETLIB_INTEGER>(dataFriend.M());
    CISSTNETLIB_INTEGER dfN = static_cast<CISSTNETLIB_INTEGER>(dataFriend.N());
    if (A.IsColMajor()) {
        m_Lda = (1 > dfM) ? 1 : dfM;
        m_Ldu = dfM;
        m_Ldvt = dfN;
        UPtr = dataFriend.U().Pointer();
        VtPtr = dataFriend.Vt().Pointer();
    } else {
        m_Lda = (1 > dfN) ? 1 : dfN;
        m_Ldu = dfN;
        m_Ldvt = dfM;
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
  user (see nmrSVDDynamicData::SetRef).  While the data is
  being build, the consistency of the output and workspace is checked.
  Then, the nmrSVD(A, data) function can be used safely.

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD
  \param Workspace The workspace for LAPACK.

  \test nmrSVDTest::TestDynamicColumnMajorUserAlloc
        nmrSVDTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA,
          class _matrixOwnerTypeU,
          class _vectorOwnerTypeS,
          class _matrixOwnerTypeVt,
          class _vectorOwnerTypeWorkspace>
inline
CISSTNETLIB_INTEGER nmrSVD(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                           vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE> & U,
                           vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE> & S,
                           vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE> & Vt,
                           vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & Workspace)
{
    nmrSVDDynamicData svdData(U, S, Vt, Workspace);
    CISSTNETLIB_INTEGER ret_value = nmrSVD(A, svdData);
    return ret_value;
}

/*! This function solves the SVD problem for a dynamic matrix using
  the storage provided by the user for the output (U, S, Vt).  A
  workspace will be dynamically allocated.

  Internally, a data is created using the storage provided by the
  user (see nmrSVDDynamicData::SetRefOutput).  While the data
  is being build, the consistency of the output is checked.  Then, the
  nmrSVD(A, data) function can be used safely.

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD

  \test nmrSVDTest::TestDynamicColumnMajorUserAlloc
        nmrSVDTest::TestDynamicRowMajorUserAlloc

  \warning Again, this method will dynamically allocate a workspace at
  each call.  This is somewhat inefficient if the method is to be
  called many times.  For a real-time task, the dynamic allocation
  might even break your application.
 */
template <class _matrixOwnerTypeA,
          class _matrixOwnerTypeU,
          class _vectorOwnerTypeS,
          class _matrixOwnerTypeVt>
inline
CISSTNETLIB_INTEGER nmrSVD(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE>& A,
                           vctDynamicMatrixBase<_matrixOwnerTypeU, CISSTNETLIB_DOUBLE>& U,
                           vctDynamicVectorBase<_vectorOwnerTypeS, CISSTNETLIB_DOUBLE>& S,
                           vctDynamicMatrixBase<_matrixOwnerTypeVt, CISSTNETLIB_DOUBLE>& Vt)
{
    nmrSVDDynamicData svdData(U, S, Vt);
    CISSTNETLIB_INTEGER ret_value = nmrSVD(A, svdData);
    return ret_value;
}


#ifndef SWIG // don't have fixed size containers in Python

/*!  This function solves the SVD problem for a fixed size matrix
  using the storage provided by the user for both the output (U, S,
  Vt) and the workspace.

  The storage order of the matrices and their sizes must match at
  compilation time.  This is enforced by the template parameters and
  matching problems will lead to compilation errors.  Since there is
  no easy way to enforece the size of the vectors S and workspace with
  template parameters, a runtime check is performed.  The test uses
  CMN_ASSERT to determine what to do if the sizes don't match.  By
  default CMN_ASSERT calls \c abort() but it can be configured to be
  ignored or to throw an exception (see #CMN_ASSERT for details).

  This function modifies the input matrix A and the workspace.  It
  stores the results in U, S and Vt.

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD
  \param workspace The workspace for LAPACK.

  \test nmrSVDTest::TestFixedSizeColumnMajorMLeqN_T2
        nmrSVDTest::TestFixedSizeRowMajorMLeqN_T2
        nmrSVDTest::TestFixedSizeColumnMajorMGeqN_T2
        nmrSVDTest::TestFixedSizeRowMajorMGeqN_T2
 */
template <vct::size_type _rows, vct::size_type _cols, vct::size_type _minmn,
          vct::size_type _work, bool _storageOrder>
inline
CISSTNETLIB_INTEGER nmrSVD(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> & A,
                           vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _rows, _storageOrder> & U,
                           vctFixedSizeVector<CISSTNETLIB_DOUBLE, _minmn> & S,
                           vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _cols, _cols, _storageOrder> & Vt,
                           vctFixedSizeVector<CISSTNETLIB_DOUBLE, _work> & workspace)
{
#if CMN_ASSERT_IS_DEFINED
    const CISSTNETLIB_INTEGER minmn =
        static_cast<CISSTNETLIB_INTEGER>(nmrSVDFixedSizeData<_rows, _cols, _storageOrder>::MIN_MN);
#endif
    //Assert if requirement is greater than size provided!
    CMN_ASSERT(minmn <= static_cast<CISSTNETLIB_INTEGER>(_minmn));
    CISSTNETLIB_INTEGER ldu = (_storageOrder == VCT_COL_MAJOR) ? _rows : _cols;
    CISSTNETLIB_INTEGER lda = (1 > ldu) ? 1 : ldu;
    CISSTNETLIB_INTEGER ldvt = (_storageOrder == VCT_COL_MAJOR) ? _cols : _rows;
    CISSTNETLIB_INTEGER lwork =
        static_cast<CISSTNETLIB_INTEGER>(nmrSVDFixedSizeData<_rows, _cols, _storageOrder>::LWORK);
    //Assert if requirement is greater than size provided!
    CMN_ASSERT(lwork <= static_cast<CISSTNETLIB_INTEGER>(_work));
    char jobu = 'A';
    char jobvt = 'A';
    CISSTNETLIB_INTEGER info;
    CISSTNETLIB_DOUBLE *UPtr, *VtPtr;
    if (_storageOrder == VCT_COL_MAJOR) {
        UPtr = U.Pointer();
        VtPtr = Vt.Pointer();
    } else {
        UPtr = Vt.Pointer();
        VtPtr = U.Pointer();
    }

    // for versions based on gfortran/lapack, CISSTNETLIB_VERSION is
    // defined
#if defined(CISSTNETLIB_VERSION)
    /* call the LAPACK C function */
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgesvd_(&jobu, &jobvt, &ldu, &ldvt,
                        A.Pointer(), &lda, S.Pointer(),
                        UPtr, &ldu,
                        VtPtr, &ldvt,
                        workspace.Pointer(), &lwork, &info);
#endif
#else // no major version
      dgesvd_(&jobu, &jobvt, &ldu, &ldvt,
              A.Pointer(), &lda, S.Pointer(),
              UPtr, &ldu,
              VtPtr, &ldvt,
              workspace.Pointer(), &lwork, &info);
#endif // CISSTNETLIB_VERSION
#else
    ftnlen jobu_len = (ftnlen)1, jobvt_len = (ftnlen)1;
    la_dzlapack_MP_sgesvd_nat(&jobu, &jobvt, &ldu, &ldvt,
                              A.Pointer(), &lda, S.Pointer(),
                              UPtr, &ldu,
                              VtPtr, &ldvt,
                              workspace.Pointer(), &lwork, &info,
                              jobu_len, jobvt_len);
#endif

    return info;
}

/*! This function solves the SVD problem for a fixed size matrix using
  the storage provided by the user for the output (U, S, Vt).  The
  workspace will be automatically allocated (stack allocation of a
  fixed size vector).

  This method calls nmrSVD(A, U, S, Vt, workspace).

  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD

  \test nmrSVDTest::TestFixedSizeColumnMajorMLeqN_T2
        nmrSVDTest::TestFixedSizeRowMajorMLeqN_T2
        nmrSVDTest::TestFixedSizeColumnMajorMGeqN_T2
        nmrSVDTest::TestFixedSizeRowMajorMGeqN_T2
 */
template <vct::size_type _rows, vct::size_type _cols, vct::size_type _minmn, bool _storageOrder>
inline
CISSTNETLIB_INTEGER nmrSVD(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> & A,
                           vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _rows, _storageOrder> & U,
                           vctFixedSizeVector<CISSTNETLIB_DOUBLE, _minmn> & S,
                           vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _cols, _cols, _storageOrder> & Vt)
{
    typename nmrSVDFixedSizeData<_rows, _cols, _storageOrder>::VectorTypeWorkspace workspace;
    const CISSTNETLIB_INTEGER ret_value = nmrSVD(A, U, S, Vt, workspace);
    return ret_value;
}


/*! This function solves the SVD problem for a fixed size matrix using
  the storage provided by the user for both the output (U, S, Vt) and
  the workspace.  This function allows to use the very convenient
  nmrSVDFixedSizeData to allocate the memory required for U, S, Vt
  and the workspace:
  \code
  vctFixedSizeMatrix<double, 12, 7> A;
  vctRandom(A, -10.0, 10.0);
  nmrSVDFixedSizeData<12, 7> data;
  nmrSVD(A, data);
  std::cout << "U: " << data.U() << std::endl
            << "S: " << data.S() << std::endl
            << "V: " << data.Vt().Transpose() << std::endl;
  \endcode

  This method calls nmrSVD(A, U, S, Vt, workspace).

  \param A A fixed size matrix of size MxN.
  \param data A data object.

  \test nmrSVDTest::TestFixedSizeColumnMajorMLeqN
        nmrSVDTest::TestFixedSizeRowMajorMLeqN
        nmrSVDTest::TestFixedSizeColumnMajorMGeqN
        nmrSVDTest::TestFixedSizeRowMajorMGeqN
 */
template <vct::size_type _rows, vct::size_type _cols, bool _storageOrder>
inline
CISSTNETLIB_INTEGER nmrSVD(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _rows, _cols, _storageOrder> & A,
                           nmrSVDFixedSizeData<_rows, _cols, _storageOrder> & data)
{
    typename nmrSVDFixedSizeData<_rows, _cols, _storageOrder>::Friend dataFriend(data);
    CISSTNETLIB_INTEGER ret_value = nmrSVD( A, dataFriend.U(), dataFriend.S(),
                                            dataFriend.Vt(), dataFriend.Workspace() );
    return ret_value;
}
#endif // SWIG

//@}

#endif
