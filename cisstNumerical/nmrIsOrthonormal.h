/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on: 2005-07-27

  (C) Copyright 2005-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of functions nmrIsOrthonormal
*/


#ifndef _nmrIsOrthonormal_h
#define _nmrIsOrthonormal_h


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>



/*!
  \ingroup cisstNumerical

  \brief Data (workspace) for ::nmrIsOrthonormal (Dynamic).

  Any size mismatch will lead to an exception thrown
  (std::runtime_error).  Since we are using cmnThrow, it is possible
  to configure cisst (at compilation time) to abort the program
  instead of throwing an exception.

  The nmrIsOrthonormalDynamicData class allows 2 different configurations:

  - Allocate automatically the workspace.  This can be performed using
    either the constructor from input matrix
    (i.e. nmrIsOrthonormalDynamicData(A)) or using the method
    Allocate(A).

  - Don't allocate anything.  The user has to provide the container he
    wants to use for the workspace.  In this case, the data object is
    used mostly to check that all the workspace is large enough.  This
    can be performed using either the constructor from workspace
    (i.e. nmrIsOrthonormalDynamicData(A, workspace)) or the method
    SetRef(A, workspace).

  \sa nmrIsOrthonormal
 */
template <class _elementType>
class nmrIsOrthonormalDynamicData {

public:
    /*! Type used for sizes within nmrIsOrthonormalDynamicData. */
    typedef vct::size_type size_type;

protected:
    /*! Memory allocated for Workspace matrices if needed. */
    vctDynamicVector<_elementType> WorkspaceMemory;

    /*! Private method to set the data member Size.  This method must
      be called before AllocateWorkspace or
      ThrowUnlessWorkspaceSizeIsCorrect. */
    inline void SetDimension(size_type m)
    {
        Size = m * m;
    }

    /*! Private method to allocate memory for the workspace if needed.
      This method assumes that the size of the problem has been set.
      It is important to use this method in all the methods provided
      in the user API, even if all the memory is provided by the user
      since this method will ensure that the data
      (nmrIsOrthonormalDynamicData) does not keep any memory
      allocated.  This is for the case where a single data object is used
      first to allocate everything and, later on, used with user
      allocated memory (for either the workspace or the output).  For
      example:
      \code
      vctDynamicMatrix<double> A(20, 20);
      vctRandom(A, 10, 10);
      nmrIsOrthonormalDynamicData data(A); // allocate workspace
      vctDynamicVector<double> workspace(nmrIsOrthonormalDynamicData::WorkspaceSize(A));
      data.SetRef(A, workspace); // after all, use my own workspace
      \endcode

      \note The method SetDimension must have been called before.
    */
    inline void AllocateWorkspace(bool allocateWorkspace)
    {
        if (allocateWorkspace) {
            this->WorkspaceMemory.SetSize(Size);
            this->WorkspaceReference.SetRef(this->WorkspaceMemory);
        } else {
            this->WorkspaceMemory.SetSize(0);
        }
    }

    /*! Verifies that the user provided references for the workspace
      match (or is greated than) the size of the data object as set by
      SetDimension.  This method also checks that the workspace is
      compact.

      \note The method SetDimension must have been called before.
    */
    template <typename __vectorOwnerTypeWorkspace>
    inline void
    ThrowUnlessWorkspaceSizeIsCorrect(vctDynamicVectorBase<__vectorOwnerTypeWorkspace, _elementType> & inWorkspace) const
        CISST_THROW(std::runtime_error)
    {
        if (Size > inWorkspace.size()) {
            cmnThrow(std::runtime_error("nmrIsOrthonormalDynamicData: Workspace is too small."));
        }
        if (!inWorkspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrIsOrthonormalDynamicData: Workspace must be compact."));
        }
    }

    /*! References to work or return types, these point either to user
      allocated memory or our memory chunks if needed.
     */
    vctDynamicVectorRef<_elementType> WorkspaceReference;

    /*! Just store the size of the matrix, i.e. M * N. */
    size_type Size;

public:

    /*! Helper methods for user to set minimum working space required
      for ::nmrIsOrthonormal.

      \param m The number or rows of matrix to be tested.  The matrix
      must be square.
     */
    static inline size_type WorkspaceSize(size_type m)
    {
        return m * m;
    }

    /*! Helper methods for user to set minimum working space required
      for ::nmrIsOrthonormal.

      \param A The matrix to be tested.  A must be square.
    */
    template <class __matrixOwnerTypeA>
    static inline size_type WorkspaceSize(const vctDynamicConstMatrixBase<__matrixOwnerTypeA, _elementType> & A)
    {
        return nmrIsOrthonormalDynamicData::WorkspaceSize(A.rows());
    }


#ifndef SWIG
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated IsOrthonormal function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrIsOrthonormalDynamicData & Data;
    public:
        Friend(nmrIsOrthonormalDynamicData &data): Data(data) {
        }
        inline vctDynamicVectorRef<_elementType> & Workspace(void) {
            return Data.WorkspaceReference;
        }
        inline size_type Size(void) {
            return Data.Size;
        }
    };
    friend class Friend;
#endif // DOXYGEN
#endif // SWIG

    /*! The default constuctor.  For dynamic size, there are assigned
      default values, i.e. sets all the dimensions to zero.  These
      MUST be changed by calling the appropriate method.

      \sa nmrIsOrthonormalDynamicData::Allocate
      nmrIsOrthonormalDynamicData::SetRef
    */
    nmrIsOrthonormalDynamicData():
        Size(static_cast<size_type>(0))
    {
        AllocateWorkspace(false);
    }

    /*! Constructor where the user specifies the size.  Memory
      allocation is performed for the workspace. This should be used
      when the user doesn't need to share the workspace between
      different algorithms.

      \param m Number of rows of the matrix to be decomposed.  The
      matrix must be square.

      \sa nmrIsOrthonormalDynamicData::Allocate
    */
    nmrIsOrthonormalDynamicData(size_type m)
    {
        this->Allocate(m);
    }

    /*! Constructor where the user provides the input matrix to
      specify the size.  Memory allocation is performed for the
      workspace. This should be used when the user doesn't need to
      share the workspace between different algorithms.

      \param A input matrix

      \sa nmrIsOrthonormalDynamicData::Allocate
    */
    template <class __matrixOwnerTypeA>
    nmrIsOrthonormalDynamicData(const vctDynamicConstMatrixBase<__matrixOwnerTypeA, _elementType> & A)
    {
        this->Allocate(A);
    }

    /*! Constructor where the user provides the input matrix to
      specify the size and storage order.  There is no memory
      allocation performed in this case.  This constructor should be
      used when the user cares wants to avoid allocating different
      workspaces for different numerical routines.  Please note that
      since multiple routines can share the workspace, these routines
      must be called in a thread safe manner.

      \param A input matrix
      \param inWorkspace workspace

      \sa nmrIsOrthonormalDynamicData::SetRefWorkspace
    */
    template <class __matrixOwnerTypeA, class __vectorOwnerTypeWorkspace>
    nmrIsOrthonormalDynamicData(const vctDynamicConstMatrixBase<__matrixOwnerTypeA, _elementType> & A,
                                vctDynamicVectorBase<__vectorOwnerTypeWorkspace, _elementType> & inWorkspace)
    {
        this->SetRef(A, inWorkspace);
    }


    /*! This method allocates memory for the workspace.  The input matrix is used only
      to determine the size of the problem.

      This method should be called before the nmrIsOrthonormalDynamicData
      object is passed on to ::nmrIsOrthonormal function.

      \param A The matrix to be used with nmrIsOrthonormal.
    */
    template <class __matrixOwnerTypeA>
    inline void Allocate(const vctDynamicConstMatrixBase<__matrixOwnerTypeA, _elementType> & A)
    {
        this->Allocate(A.rows());
    }

    /*! This method uses the memory provided by user for workspace.  The
      input matrix A is used to determine the size of the problem.

      This method verifies that the workspace provided by the user is
      large enough and is compact.

      \param A The matrix to be used with IsOrthonormal.
      \param inWorkspace The vector used for workspace.
    */
    template <class __matrixOwnerTypeA, class __vectorOwnerTypeWorkspace>
    inline void SetRef(const vctDynamicConstMatrixBase<__matrixOwnerTypeA, _elementType> & A,
                       vctDynamicVectorBase<__vectorOwnerTypeWorkspace, _elementType> & inWorkspace)
    {
        this->SetDimension(A.rows());

        // allocate output and set references
        this->AllocateWorkspace(false);

        // set reference on user provided workspace
        this->ThrowUnlessWorkspaceSizeIsCorrect(inWorkspace);
        this->WorkspaceReference.SetRef(inWorkspace);
    }

    /*! This method allocates the memory for the workspace.  This
      method is not meant to be a top-level user API, but is used by
      other overloaded Allocate methods.

      \param m Number of rows of input matrix A.  A must be a square matrix.
    */
    void Allocate(size_type m)
    {
        this->SetDimension(m);
        this->AllocateWorkspace(true);
    }
};



/*!
  \ingroup cisstNumerical

  \brief Data for ::nmrIsOrthonormal (Fixed size).

  This class is similar to nmrIsOrthonormalDynamicData except that it is
  dedicated to fixed size containers.  While nmrIsOrthonormalDynamicData is
  designed to be modified dynamically, nmrIsOrthonormalFixedSizeData is
  fully defined at compilation time using template parameters.  The
  required parameter is the dimension of the input matrix:
  \code
  nmrIsOrthonormalFixedSizeData<4> data;
  \endcode

  \note An object of type nmrIsOrthonormalFixedSizeData contains the memory
  required for the output and the workspace, i.e. its actual size will
  be equal to the memory required to store the workspace.

  \note There is no dynamic memory allocation (no \c new) and the
  memory can not be used by reference.  To use memory by reference,
  one must use nmrIsOrthonormalDynamicData with
  vctDynamicMatrixRef and vctDynamicVectorRef (these dynamic
  references can actually be used to overlay a fixed size container).

  \sa nmrIsOrthonormal
 */
#ifndef SWIG
template <class _elementType, vct::size_type _rows>
class nmrIsOrthonormalFixedSizeData
{
public:
#ifndef DOXYGEN
    enum {SIZE = _rows * _rows};
#endif // DOXYGEN
    /*! Type of the workspace vector (size and storage order computed
      from the data object template parameters). */
    typedef vctFixedSizeVector<_elementType, SIZE> VectorTypeWorkspace;

protected:
    VectorTypeWorkspace WorkspaceMember; /*!< Data member used to store the workspace vector. */

public:
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated IsOrthonormal function as
      a friend of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrIsOrthonormalFixedSizeData<_elementType, _rows> & Data;
    public:
        Friend(nmrIsOrthonormalFixedSizeData<_elementType, _rows> &data): Data(data) {
        }
        inline VectorTypeWorkspace & Workspace(void) {
            return Data.WorkspaceMember;
        }
    };
    friend class Friend;
#endif // DOXYGEN

    /*! Default constructor.  Does nothing since the allocation is
      performed on the stack. */
    nmrIsOrthonormalFixedSizeData() {};
};
#endif // SWIG



/*!
  \name nmrIsOrthonormal: Check if a matrix is orthonormal

  This function computes the product of the matrix with its transpose
  then subtracts the identity and verifies that all elements are null
  within some tolerance.

  This function is overloaded to be used with either a fixed size or a
  dynamic matrix (see vctFixedSizeMatrix and vctDynamicMatrix).
  Furthermore, it is possible to provide a workspace to store the
  product of the matrix and its transpose and avoid any undesired
  memory allocation:
  <ol>
  <li>The workspace can be provided using the dedicated classes
  nmrIsOrthonormalDynamicData and
  nmrIsOrthonormalFixedSizeData.  The user has to create the
  data based on the matrix to be tested and then pass it to
  nmrIsOrthonormal:
  \code
  vctDynamicMatrix<double> A(9, 9);
  nmrIsOrthonormalDynamicData<double> data(A);
  int i; bool result;
  for (i = 0; i < 100; i++) {
      vctRandom(A, -10.0, 10.0);
      result = nmrIsOrthonormal(A, data);
      ...
  }
  \endcode

  <li>The workspace can be provided as a dynamic vector
  (vctDynamicVector) large enough to store the product of the matrix
  with its transpose:
  \code
  vctDynamicMatrix<double> A(12, 12);
  vctRandom(A, -10.0, 10.0);
  vctDynamicVector<double> workspace(500); // we need at least 12x12
  bool result = nmrIsOrthonormal(A, workspace);
  \endcode

  <li> If no workspace is provided the nmrIsOrthonormal function will
  allocate one for you.  This might be more convenient for most users
  but one must be aware that a memory allocation has to be performed
  (stack allocation for fixed size matrices, dynamic allocation for
  dynamic matrices):
  \code
  vctFixedSizeMatrix<double, 5, 5> A;
  vctRandom(A, -10.0, 10.0);
  bool result = nmrIsOrthonormal(A);
  \endcode
  </ol>

  \note For a dynamic matrix, the function will throw an exception is
  the matrix is not square (std::runtime_error).

  \note The workspace needs to be compact and large enough otherwise
  nmrIsOrthonormal will throw an exception (std::runtime_error).

  \sa nmrIsOrthonormalDynamicData nmrIsOrthonormalFixedSizeData
*/
//@{

/*! This function checks if a fixed size matrix is orthonormal.  It
  uses a data (see nmrIsOrthonormalFixedSizeData) to store the
  temporary product of the matrix with its transpose.

  \param A A square fixed size matrix
  \param data Workspace used to compute the product
  \param tolerance Tolerance used to compare the product with identity
 */
template <vct::size_type _size, vct::stride_type _rowStride, vct::stride_type _colStride, typename _elementType, class _dataPtrType>
bool nmrIsOrthonormal(const vctFixedSizeConstMatrixBase<_size, _size, _rowStride, _colStride, _elementType, _dataPtrType> & A,
                      nmrIsOrthonormalFixedSizeData<_elementType, _size> & data,
                      _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance()) {
    typename nmrIsOrthonormalFixedSizeData<_elementType, _size>::Friend dataFriend(data);
    vctFixedSizeMatrixRef<_elementType, _size, _size, _size, 1> product(dataFriend.Workspace().Pointer());
    product.ProductOf(A, A.TransposeRef());
    product.Diagonal().Subtract(_elementType(1));
    return (product.LinfNorm() <= tolerance);
}

/*! This function checks if a fixed size matrix is orthonormal.  It
  creates a data on the stack to store the temporary product of
  the matrix with its transpose.

  \param A A square fixed size matrix
  \param tolerance Tolerance used to compare the product with identity
 */
template <vct::size_type _size, vct::stride_type _rowStride, vct::stride_type _colStride, typename _elementType, class _dataPtrType>
bool nmrIsOrthonormal(const vctFixedSizeConstMatrixBase<_size, _size, _rowStride, _colStride, _elementType, _dataPtrType> & A,
                      _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance()) {
    typedef vctFixedSizeConstMatrixBase<_size, _size, _rowStride, _colStride, _elementType, _dataPtrType> InputType;
    typedef typename InputType::MatrixValueType MatrixValueType;
    MatrixValueType product;
    product.ProductOf(A, A.TransposeRef());
    product.Diagonal().Subtract(_elementType(1));
    return (product.LinfNorm() <= tolerance);
}

/*! This function checks if a dynamic matrix is orthonormal.  It uses
  a data (see nmrIsOrthonormalDynamicData) to store the
  temporary product of the matrix with its transpose.

  \param A A square dynamic matrix
  \param data Workspace used to compute the product
  \param tolerance Tolerance used to compare the product with identity
 */
template <class _matrixOwnerTypeA, class _elementType>
bool nmrIsOrthonormal(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, _elementType> & A,
                      nmrIsOrthonormalDynamicData<_elementType> & data,
                      _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance())
    CISST_THROW(std::runtime_error)
{
    typename nmrIsOrthonormalDynamicData<_elementType>::Friend dataFriend(data);

    /* check that this is a square matrix */
    if (A.cols() != A.rows()) {
        cmnThrow(std::runtime_error("nmrIsOrthonormal: Requires a square matrix."));
    }
    /* check that the workspace is large enough */
    if ((A.cols() * A.rows()) > dataFriend.Size()) {
        cmnThrow(std::runtime_error("nmrIsOrthonormal: The data provided is not large enough."));
    }
    vctDynamicMatrixRef<_elementType> product;
    product.SetRef(A.rows(), A.cols(), A.cols(), 1, dataFriend.Workspace().Pointer());
    product.ProductOf(A, A.TransposeRef());
    product.Diagonal().Subtract(_elementType(1));
    return (product.LinfNorm() <= tolerance);
}

/*! This function checks if a dynamic matrix is orthonormal.  It
  dynamically allocates a data to store the temporary product of
  the matrix with its transpose.

  \param A A square dynamic matrix
  \param tolerance Tolerance used to compare the product with identity
 */
template <class _matrixOwnerTypeA, typename _elementType>
bool nmrIsOrthonormal(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, _elementType> & A,
                      _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance())
    CISST_THROW(std::runtime_error)
{
    nmrIsOrthonormalDynamicData<_elementType> data;
    data.Allocate(A);
    return nmrIsOrthonormal(A, data, tolerance);
}

/*! This function checks if a dynamic matrix is orthonormal.  It uses
  a user allocated workspace to store the temporary product of the
  matrix with its transpose.

  \param A A square dynamic matrix
  \param workspace Workspace used to compute the product
  \param tolerance Tolerance used to compare the product with identity
 */
template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace, typename _elementType>
bool nmrIsOrthonormal(const vctDynamicConstMatrixBase<_matrixOwnerTypeA, _elementType> & A,
                      vctDynamicVectorBase<_vectorOwnerTypeWorkspace, _elementType> & workspace,
                      _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance())
    CISST_THROW(std::runtime_error)
{
    nmrIsOrthonormalDynamicData<_elementType> data;
    data.SetRef(A, workspace);
    return nmrIsOrthonormal(A, data, tolerance);
}
//@}

#endif // _nmrIsOrthonormal_h
