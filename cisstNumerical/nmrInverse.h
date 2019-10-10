/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2006-01-27

  (C) Copyright 2006-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of nmrInverse
*/


#ifndef _nmrInverse_h
#define _nmrInverse_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

// Always include last
#include <cisstNumerical/nmrExport.h>


/*!
  \ingroup cisstNumerical

  \brief Data for Inverse problem (Dynamic).

  To ease the use of the Inverse routine, the user can rely on the
  nmrInverseDynamicData class to perform the required memory
  allocation.

  Another good reason to use a data object is that the memory
  allocation can be performed once during an initialization phase
  while the function nmrInverse can be called numerous times later on
  without any new dynamic memory allocation.  This is crucial for such
  things as real time tasks.

  The Inverse routine can be used for different storage orders,
  i.e. either #VCT_ROW_MAJOR or #VCT_COL_MAJOR.  The only restriction
  is that all matrices and vectors must also be compact, i.e. use a
  contiguous block of memory.

  Any size or storage order mismatch will lead to an exception thrown
  (std::runtime_error).  Since we are using cmnThrow, it is possible
  to configure cisst (at compilation time) to abort the program
  instead of throwing an exception.

  The nmrInverseDynamicData class allows 2 different configurations:

  - Allocate automatically the workspace (PivotIndices vector and
    Workspace).  This can be performed using either the constructor
    from input matrix (i.e. nmrInverseDynamicData(A)) or using the
    method Allocate(A).

  - Don't allocate anything.  The user has to provide the container he
    wants to use for the workspace and the vector of pivot indices.
    In this case, the data is used mostly to check that the container
    is valid in terms of size.  This can be performed using either the
    constructor from PivotIndices (i.e. nmrSVDDynamicData(A,
    pivotIndices, workspace)) or the method SetRef(A, pivotIndices,
    workspace).

  \sa nmrInverse
 */
class nmrInverseDynamicData {

public:
    /*! Type used for sizes within nmrInverseDynamicData.  This type
      is compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      CISSTNETLIB_INTEGER. */
    typedef vct::size_type size_type;

    enum {NB = 64};

protected:
    /*! Memory allocated for pivot indices vector if needed. */
    vctDynamicVector<CISSTNETLIB_INTEGER> PivotIndicesMemory;

    /*! Memory allocated for the workspace if needed. */
    vctDynamicVector<CISSTNETLIB_DOUBLE> WorkspaceMemory;

    /*! Reference return type, this points either to user allocated
      memory or our memory chunk if needed.
     */
    //@{
    vctDynamicVectorRef<CISSTNETLIB_INTEGER> PivotIndicesReference;
    vctDynamicVectorRef<CISSTNETLIB_DOUBLE> WorkspaceReference;
    //@}

    /*! Store Size and StorageOrder which are needed to check if A
       matrix passed ::nmrInverse matches the allocated size. */
    //@{
    size_type SizeMember;
    bool StorageOrderMember;
    //@}

    /*! Private method to set the data members SizeMember and
      StorageOrderMember.  This method must be called before
      AllocatePivotIndicesWorkspace, ThrowUnlessOutputSizeIsCorrect or
      ThrowUnlessWorkspaceSizeIsCorrect. */
    inline void SetDimension(size_type size, bool storageOrder)
    {
        SizeMember = size;
        StorageOrderMember = storageOrder;
    }

    /*! Private method to allocate memory for the the pivot indices
      and the workspace if needed.  This method assumes that the
      dimension m and n as well as the storage order are already set.
      It is important to use this method in all the methods provided
      in the user API, even if all the memory is provided by the user
      since this method will ensure that the data
      (nmrInverseDynamicData) does not keep any memory allocated.
      This is for the case where a single data object is used first to
      allocate everything and, later on, used with user allocated
      memory (for the output).

      \note The method SetDimension must have been called before.
    */
    inline void AllocatePivotIndicesWorkspace(bool allocatePivotIndices, bool allocateWorkspace)
    {
        const size_type maxSize1 = (SizeMember > 1) ? SizeMember : 1;

        // allocate PivotIndices
        if (allocatePivotIndices) {
            this->PivotIndicesMemory.SetSize(maxSize1);
            this->PivotIndicesReference.SetRef(this->PivotIndicesMemory);
        } else {
            this->PivotIndicesMemory.SetSize(0);
        }

        // allocate Workspace
        if (allocateWorkspace) {
            this->WorkspaceMemory.SetSize(maxSize1 * NB);
            this->WorkspaceReference.SetRef(this->WorkspaceMemory);
        } else {
            this->WorkspaceMemory.SetSize(0);
        }
    }


    /*! Verifies that the user provided reference for the pivot
      indices match the size of the data object as set by
      SetDimension.  This method also checks that all containers are
      compact.

      \note The method SetDimension must have been called before.
    */
    template <class _vectorOwnerTypePivotIndices>
    inline void ThrowUnlessPivotIndicesSizeIsCorrect(vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices)
        CISST_THROW(std::runtime_error)
    {
        // check sizes and compacity
        const size_type maxSize1 = (SizeMember > 1) ? SizeMember : 1;
        if (maxSize1 > pivotIndices.size()) {
            cmnThrow(std::runtime_error("nmrInverseDynamicData: Size of vector pivotIndices is incorrect."));
        }
        if (!pivotIndices.IsCompact()) {
            cmnThrow(std::runtime_error("nmrInverseDynamicData: Vector pivotIndices must be compact."));
        }
    }


    /*! Verifies that the user provided reference for the workspace
      match the size of the data object as set by SetDimension.  This
      method also checks that all containers are compact.

      \note The method SetDimension must have been called before.
    */
    template <class _vectorOwnerTypeWorkspace>
    inline void ThrowUnlessWorkspaceSizeIsCorrect(vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
        CISST_THROW(std::runtime_error)
    {
        // check sizes and compacity
        const size_type maxSize1 = (SizeMember > 1) ? SizeMember : 1;
        if (maxSize1 * NB > workspace.size()) {
            cmnThrow(std::runtime_error("nmrInverseDynamicData: Size of vector workspace is incorrect."));
        }
        if (!workspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrInverseDynamicData: Vector workspace must be compact."));
        }
    }


public:

    /*! Helper method to compute the size of the pivot indices vector.

      \param A The matrix to be used by ::nmrInverse (it is used only
      to determine the size).
    */
    template <class _matrixOwnerTypeA>
    static inline
    size_type PivotIndicesSize(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        return ((A.rows() > 1) ? A.rows() : 1);
    }


    /*! Helper method to compute the size of the workspace vector.

      \param A The matrix to be used by ::nmrInverse (it is used only
      to determine the size).
    */
    template <class _matrixOwnerTypeA>
    static inline
    size_type WorkspaceSize(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        return ((A.rows() > 1) ? A.rows() : 1) * NB;
    }


#ifndef SWIG
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated Inverse function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrInverseDynamicData & Data;
    public:
        Friend(nmrInverseDynamicData &data): Data(data) {
        }
        inline vctDynamicVectorRef<CISSTNETLIB_INTEGER> & PivotIndices(void) {
            return Data.PivotIndicesReference;
        }
        inline vctDynamicVectorRef<CISSTNETLIB_DOUBLE> & Workspace(void) {
            return Data.WorkspaceReference;
        }
        inline size_type Size(void) {
            return Data.SizeMember;
        }
        inline size_type StorageOrder(void) {
            return Data.StorageOrderMember;
        }
    };
    friend class Friend;
#endif // DOXYGEN
#endif // SWIG

    /*! The default constuctor.  For dynamic size, there are assigned
      default values, i.e. sets all the dimensions to zero.  These
      MUST be changed by calling the appropriate method.

      \sa nmrInverseDynamicData::Allocate
      nmrInverseDynamicData::SetRef
    */
    nmrInverseDynamicData():
        SizeMember(static_cast<size_type>(0)),
        StorageOrderMember(VCT_ROW_MAJOR)
    {
        AllocatePivotIndicesWorkspace(false, false);
    }

    /*! Constructor where the user specifies the size.  Memory
      allocation is performed for pivot indices vector and
      workspace. This should be used when the user doesn't care much
      about where the memory management.

      \param size Dimension of the square matrix.
      \param storageOrder Storage order of the matrix

      \sa nmrInverseDynamicData::Allocate
    */
    nmrInverseDynamicData(size_type size, bool storageOrder)
    {
        this->Allocate(size, storageOrder);
    }

    /*! Constructor where the user provides the input matrix to
      specify the size and storage order.  Memory allocation is
      performed for pivot indices vector and workspace. This should be
      used when the user doesn't care much about memory management.

      \param A input matrix.

      \sa nmrInverseDynamicData::Allocate
    */
    template <class _matrixOwnerTypeA>
    nmrInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        this->Allocate(A);
    }

    /*! Constructor where the user provides the vector to store the
      pivot indices and the workspace.  The data object now acts as a
      composite container to hold, pass and manipulate a convenient
      storage for ::nmrInverse algorithm. Checks are made on the
      validity of the input and its consitency in terms of size.

      \param A The matrix to be inversed, used to verify the sizes.
      \param pivotIndices Vector created by the user to store the pivot indices.
      \param workspace Vector created by the user for the workspace.

      \sa nmrInverseDynamicData::SetRef
    */
    template <class _matrixOwnerTypeA,
              class _vectorOwnerTypePivotIndices,
              class _vectorOwnerTypeWorkspace>
    nmrInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                          vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices,
                          vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
    {
        this->SetRef(A, pivotIndices, workspace);
    }

    /*! This method allocates memory for the pivot indices vector and
      the workspace.  The input matrix is used only to determine the
      size of these vectors.

      This method should be called before the nmrInverseDynamicData
      object is passed on to the ::nmrInverse function.

      \param A The square matrix for which inverse needs to be
      computed.
    */
    template <class _matrixOwnerTypeA>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
    {
        this->Allocate(A.rows(), A.StorageOrder());
    }

    /*! This method allocates the memory for the pivot indices and the
      workspace.  This method is not meant to be a top-level user API,
      but is used by other overloaded Allocate methods.

      \param size Size of the square input matrix A.
      \param storageOrder Storage order of the input matrix.
    */
    void Allocate(size_type size, bool storageOrder)
    {
        this->SetDimension(size, storageOrder);
        this->AllocatePivotIndicesWorkspace(true, true);
    }

    /*! This method doesn't allocate any memory as it relies on the
      user provided vectors (pivotIndices and workspace).

      The data object now acts as a composite container to hold,
      pass and manipulate a convenient storage for Inverse algorithm.  The
      method tests that all the containers provided by the user have
      the correct size and are compact.

      \param A The matrix to be inversed, used to verify the sizes.
      \param pivotIndices Vector created by the user to store the pivot indices.
      \param workspace Vector created by the user for the workspace.
    */
    template <class _matrixOwnerTypeA,
              class _vectorOwnerTypePivotIndices,
              class _vectorOwnerTypeWorkspace>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
        CISST_THROW(std::runtime_error)
    {
        this->SetDimension(A.rows(), A.StorageOrder());
        this->AllocatePivotIndicesWorkspace(false, false);
        this->ThrowUnlessPivotIndicesSizeIsCorrect(pivotIndices);
        this->PivotIndicesReference.SetRef(pivotIndices);
        this->ThrowUnlessWorkspaceSizeIsCorrect(workspace);
        this->WorkspaceReference.SetRef(workspace);

    }
};



/*!
  \ingroup cisstNumerical

  \brief Data for Inverse problem (Fixed size).

  This class is similar to nmrInverseDynamicData except that it is
  dedicated to fixed size containers.  While nmrInverseDynamicData is
  designed to be modified dynamically, nmrInverseFixedSizeData is
  fully defined at compilation time using template parameters.  The
  required parameters are the dimensions of the input matrix:
  \code
  nmrInverseFixedSizeData<4, VCT_COL_MAJOR> data;
  \endcode

  \note An object of type nmrInverseFixedSizeData contains the memory
  required for the pivot indices and the workspace, i.e. its actual
  size will be equal to the memory required to store these vectors.

  \note There is no dynamic memory allocation (no \c new) and the
  memory can not be used by reference.  To use memory by reference,
  one must use nmrInverseDynamicData with vctDynamicMatrixRef and
  vctDynamicVectorRef (these dynamic references can actually be used
  to overlay a fixed size container).
 */
#ifndef SWIG
template <vct::size_type _size, bool _storageOrder>
class nmrInverseFixedSizeData
{
public:
#ifndef DOXYGEN
    typedef vct::size_type size_type;
    enum {MAX_SIZE_1 = (_size > 1) ? _size : 1};
    enum {NB = 64};
    enum {LWORK = _size * NB};

#endif // DOXYGEN
    /*! Type of the input matrix A (size computed from the data
      template parameters). */
    typedef vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _size, _size, _storageOrder> MatrixTypeA;
    /*! Type of the output vector PivotIndices (size computed from the
      template parameters). */
    typedef vctFixedSizeVector<CISSTNETLIB_INTEGER, MAX_SIZE_1> VectorTypePivotIndices;
    /*! Type used to create the workspace (size computed from the
      template parameters). */
    typedef vctFixedSizeVector<CISSTNETLIB_DOUBLE, LWORK> VectorTypeWorkspace;

protected:
    VectorTypePivotIndices PivotIndicesMember; /*!< Data member used to store the vector pivotIndices. */
    VectorTypeWorkspace WorkspaceMember; /*!< Data member used to store the workspace vector. */

public:
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated Inverse function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
    */
    class Friend {
    private:
        nmrInverseFixedSizeData<_size, _storageOrder> & Data;
    public:
        Friend(nmrInverseFixedSizeData<_size, _storageOrder> &data): Data(data) {
        }
        inline VectorTypePivotIndices & PivotIndices(void) {
            return Data.PivotIndicesMember;
        }
        inline VectorTypeWorkspace & Workspace(void) {
            return Data.WorkspaceMember;
        }
    };
    friend class Friend;
#endif // DOXYGEN

    /*! Default constructor.  Does nothing since the allocation is
      performed on the stack. */
    nmrInverseFixedSizeData() {};
};
#endif // SWIG


/*!  \name Algorithm Inverse: Compute the inverse of a square matrix
  using Lower Upper Decomposition

  These functions are different wrappers for the LAPACK function
  dgetrf and dgetri.  They compute the inverse factorization of a
  square matrix A.

  These functions are wrappers around the LAPACK routine dgetrf and dgetri,
  therefore they share some features with the LAPACK routine:
  <ol>
  <li>On exit, the content of A is altered.
  <li>The vectors and matrices must be compact, i.e. use a contiguous
  block of memory.
  </ol>

  The ::nmrInverse functions add the following features:
  <ol>
  <li>A simplified interface to the cisstVector matrices, either
  vctDynamicMatrix or vctFixedSizeMatrix.
  <li>Input validation checks are performed, i.e. an
  std::runtime_error exception will be thrown if the sizes or storage
  order don't match or if the containers are not compact.
  <li>Helper classes to allocate memory for the output and workspace:
  nmrInverseFixedSizeData and nmrInverseDynamicData.
  </ol>

  There are different ways to call this function to compute the Inverse of
  the matrix A.  These correspond to different overloaded ::nmrInverse
  functions:
  <ol>
  <li>Using a preallocated data object.

  The user creates the input matrix A:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(12, 24 , VCT_COL_MAJOR); // 12 x 24 matrix
  vctRandom(A, -10.0, 10.0);
  \endcode
  The user allocates a data object which could be of
  type nmrInverseFixedSizeData or nmrInverseDynamicData.
  corresponding to fixed size or dynamic matrix A:
  \code
  nmrInverseDynamicData data(A);
  \endcode
  Call the nmrInverse function:
  \code
  nmrInverse(A, data);
  \endcode
  The content of input matrix A is modified by this routine.

  <li>The user provides the vector pivotIndices and workspace.

  The User allocates memory for this vector:
  \code
  vctDynamicMatrix<CISSTNETLIB_DOUBLE> A(5, 5, VCT_COL_MAJOR);
  vctRandom(A, -10.0, 10.0);
  vctDynamicVector<CISSTNETLIB_INTEGER>
      pivotIndices(nmrInverseDynamicData::PivotIndicesSize(A));
  vctDynamicVector<CISSTNETLIB_DOUBLE>
      workspace(nmrInverseDynamicData::WorkspaceSize(A));
  \endcode
  Call the Inverse routine:
  \code
  nmrInverse(A, pivotIndices, workspace);
  \endcode
  The Inverse function verifies that the size of the data objects
  matches the input.

  <li>Using a data for fixed size containers.

  \code
  vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, 5, 5, VCT_COL_MAJOR> A;
  vctRandom(A, -10.0, 10.0);
  typedef nmrInverseFixedSizeData<5, VCT_COL_MAJOR> DataType;
  DataType data;
  nmrInverse(A, data);
  \endcode

  </ol>

  \note The Inverse functions make use of LAPACK routines.  To
  activate this code, set the CISST_HAS_CISSTNETLIB flag to ON during
  the configuration of cisst with CMake.
 */
//@{


/*! This function solves the Inverse problem for a dynamic matrix using an
  nmrInverseDynamicData.

  This function checks for valid input (size and compact) and calls
  the LAPACK function.  If the input doesn't match the data, an
  exception is thrown (\c std::runtime_error).

  This function modifies the input matrix A and stores the results in
  the data.

  \param A A square matrix, either vctDynamicMatrix or vctDynamicMatrixRef.
  \param data A data object corresponding to the input matrix.

  \test nmrInverseTest::TestDynamicColumnMajor
        nmrInverseTest::TestDynamicRowMajor
 */
template <class _matrixOwnerType>
inline CISSTNETLIB_INTEGER nmrInverse(vctDynamicMatrixBase<_matrixOwnerType, CISSTNETLIB_DOUBLE> & A,
                            nmrInverseDynamicData & data)
    CISST_THROW(std::runtime_error)
{
    typename nmrInverseDynamicData::Friend dataFriend(data);
    CISSTNETLIB_INTEGER info;

    /* check that the matrix is square */
    if (!A.IsSquare()) {
        cmnThrow(std::runtime_error("nmrInverse: Input must be a square matrix."));
    }
    /* check sizes */
    if (dataFriend.Size() != A.rows()) {
        cmnThrow(std::runtime_error("nmrInverse: Size used for Allocate was different."));
    }
    /* check that the matrices are compact */
    if (! A.IsCompact()) {
        cmnThrow(std::runtime_error("nmrInverse: Requires a compact matrix."));
    }

    CISSTNETLIB_INTEGER size = dataFriend.Size();
    CISSTNETLIB_INTEGER lda = (size > 1) ? size : 1;
    CISSTNETLIB_INTEGER lwork = dataFriend.Workspace().size();
    /* call the LAPACK C function */
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgetrf_(&size, &size,
                        A.Pointer(), &lda,
                        dataFriend.PivotIndices().Pointer(),
                        &info);
    cisstNetlib_dgetri_(&size,
                        A.Pointer(), &lda,
                        dataFriend.PivotIndices().Pointer(),
                        dataFriend.Workspace().Pointer(),
                        &lwork,
                        &info);
#endif
#else // no major version
    dgetrf_(&size, &size,
            A.Pointer(), &lda,
            dataFriend.PivotIndices().Pointer(),
            &info);
    dgetri_(&size,
            A.Pointer(), &lda,
            dataFriend.PivotIndices().Pointer(),
            dataFriend.Workspace().Pointer(),
            &lwork,
            &info);
#endif // CISSTNETLIB_VERSION
    return info;
}



/*! This function solves the Inverse problem for a dynamic matrix
  using the storage provided by the user (pivotIndices and workspace).

  Internally, a data is created using the storage provided by the
  user (see nmrInverseDynamicData::SetRef).  While the data is
  being build, the consistency of the output is checked.  Then, the
  nmrInverse(A, data) function can be used safely.

  \param A is a reference to a dynamic square matrix
  \param pivotIndices Vector created by the user to store the pivot indices.
  \param workspace Vector created by the user for the workspace.

  \test nmrInverseTest::TestDynamicColumnMajorUserAlloc
        nmrInverseTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA,
          class _vectorOwnerTypePivotIndices,
          class _vectorOwnerTypeWorkspace>
inline CISSTNETLIB_INTEGER nmrInverse(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A,
                            vctDynamicVectorBase<_vectorOwnerTypePivotIndices, CISSTNETLIB_INTEGER> & pivotIndices,
                            vctDynamicVectorBase<_vectorOwnerTypeWorkspace, CISSTNETLIB_DOUBLE> & workspace)
{
    nmrInverseDynamicData data(A, pivotIndices, workspace);
    return nmrInverse(A, data);
}


/*! This function solves the Inverse problem for a dynamic matrix.

  Internally, a data object is created.  This forces the dynamic
  allocation of the pivot indices vector as well as the workspace
  vector.  Then, the nmrInverse(A, data) function can be used safely.

  \param A is a reference to a dynamic square matrix
*/
template <class _matrixOwnerTypeA>
inline CISSTNETLIB_INTEGER nmrInverse(vctDynamicMatrixBase<_matrixOwnerTypeA, CISSTNETLIB_DOUBLE> & A)
{
    nmrInverseDynamicData data(A);
    return nmrInverse(A, data);
}


#ifndef SWIG // don't have fixed size containers in Python

/*!  This function solves the Inverse problem for a fixed size matrix
  using the storage provided by the user for the pivot indices vector
  as well as the workspace vector.

  The sizes of the matrices must match at compilation time.  This is
  enforced by the template parameters and matching problems will lead
  to compilation errors.  Since there is no easy way to enforce the
  size of the vector pivotIndices with template parameters, a runtime
  check is performed.  The test uses CMN_ASSERT to determine what to
  do if the sizes don't match.  By default CMN_ASSERT calls \c abort()
  but it can be configured to be ignored or to throw an exception (see
  #CMN_ASSERT for details).

  This function modifies the input matrix A.

  \param A is a fixed size square matrix.
  \param pivotIndices Vector created by the caller for the pivot indices.
  \param workspace Vector created by the caller for the workspace.

  \test TestFixedSizeColumnMajorUserAlloc
        TestFixedSizeRowMajorUserAlloc
 */
template <vct::size_type _size, vct::size_type _maxSize1, vct::size_type _lWork, bool _storageOrder>
inline CISSTNETLIB_INTEGER nmrInverse(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _size, _size, _storageOrder> & A,
                            vctFixedSizeVector<CISSTNETLIB_INTEGER, _maxSize1> & pivotIndices,
                            vctFixedSizeVector<CISSTNETLIB_DOUBLE, _lWork> & workspace)
{
#if CMN_ASSERT_IS_DEFINED
    const CISSTNETLIB_INTEGER maxSize1 = static_cast<CISSTNETLIB_INTEGER>(nmrInverseFixedSizeData<_size, _storageOrder>::MAX_SIZE_1);
#endif
    const CISSTNETLIB_INTEGER lWork = static_cast<CISSTNETLIB_INTEGER>(nmrInverseFixedSizeData<_size, _storageOrder>::LWORK);
    //Assert if requirement is equal to size provided!
    CMN_ASSERT(maxSize1 == static_cast<CISSTNETLIB_INTEGER>(_maxSize1));
    CMN_ASSERT(lWork <= static_cast<CISSTNETLIB_INTEGER>(_lWork));

    CISSTNETLIB_INTEGER info;
    CISSTNETLIB_INTEGER lda = _maxSize1;
    CISSTNETLIB_INTEGER size = _size;
    CISSTNETLIB_INTEGER lwork = lWork;

    /* call the LAPACK C function */
#if defined(CISSTNETLIB_VERSION_MAJOR)
#if (CISSTNETLIB_VERSION_MAJOR >= 3)
    cisstNetlib_dgetrf_(&size, &size,
                        A.Pointer(), &lda,
                        pivotIndices.Pointer(),
                        &info);
    cisstNetlib_dgetri_(&size,
                        A.Pointer(), &lda,
                        pivotIndices.Pointer(),
                        workspace.Pointer(),
                        &lwork,
                        &info);
#endif
#else // no major version
    dgetrf_(&size, &size,
            A.Pointer(), &lda,
            pivotIndices.Pointer(),
            &info);
    dgetri_(&size,
            A.Pointer(), &lda,
            pivotIndices.Pointer(),
            workspace.Pointer(),
            &lwork,
            &info);
#endif // CISSTNETLIB_VERSION
    return info;
}


/*! This function solves the Inverse problem for a fixed size matrix using
  nmrInverseFixedSizeData to allocate the memory required for the pivot indices and the workspace:
  \code
  vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, 7, 7, VCT_ROW_MAJOR> A;
  vctRandom(A, -10.0, 10.0);
  nmrInverseFixedSizeData<7, VCT_ROW_MAJOR> data;
  nmrInverse(A, data);
  \endcode

  This method calls nmrInverse(A, pivotIndices, workspace).

  \param A A fixed size square matrix.
  \param data A data object.

  \test TestFixedSizeColumnMajor
        TestFixedSizeRowMajor
 */
template <vct::size_type _size, bool _storageOrder>
inline CISSTNETLIB_INTEGER nmrInverse(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _size, _size, _storageOrder> & A,
                            nmrInverseFixedSizeData<_size, _storageOrder> & data)
{
    typename nmrInverseFixedSizeData<_size, _storageOrder>::Friend dataFriend(data);
    return nmrInverse(A, dataFriend.PivotIndices(), dataFriend.Workspace());
}


/*! This function solves the Inverse problem for a fixed size matrix.

  Internally, a data object is created.  This forces the allocation
  (stack) of the pivot indices vector as well as the workspace vector.
  Then, the nmrInverse(A, data) function can be used safely.

  \param A A fixed size square matrix.
*/
template <vct::size_type _size, bool _storageOrder>
inline CISSTNETLIB_INTEGER nmrInverse(vctFixedSizeMatrix<CISSTNETLIB_DOUBLE, _size, _size, _storageOrder> & A)
{
    nmrInverseFixedSizeData<_size, _storageOrder> data;
    return nmrInverse(A, data);
}

#endif // SWIG

//@}


#endif // _nmrInverse_h
