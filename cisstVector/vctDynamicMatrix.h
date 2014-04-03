/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicMatrix_h
#define _vctDynamicMatrix_h

/*!
  \file
  \brief Declaration of vctDynamicMatrix
*/

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrixBase.h>
#include <cisstVector/vctDynamicMatrixRef.h>
#include <cisstVector/vctDynamicMatrixOwner.h>

/*!
  \ingroup cisstVector

  \brief A matrix object of dynamic size.

  This class defines a matrix object of dynamic size with memory
  allocation.

  The algebraic operations are mostly inherited from the base classes
  vctDynamicMatrixBase and vctDynamicConstMatrixBase.  Here, we will
  briefly describe the specific properties of vctDynamicMatrix, with a
  few usage examples.

  <ol>
  <li> The class is templated by its element type, that is, the matrix
  element.  Normally, the element should be an arithmetic type, that
  is, support all the standard arithmetic operations: +, -, *, /, =,
  ==, <, >, <=, >=, ...
  <li> The class uses dynamically allocated memory, and, more
  importantly, \em owns the memory.  That is, a vctDynamicMatrix
  object automatically frees the allocated memory it owns when it is
  destroyed.
  <li>To allocate the memory, use one of the following operations.
  \code
  // define a typical element type
  typedef double ElementType;

  // the matrixRows and matrixCols variables can be set to any value at
  // any time before creating the matrix.
  size_t matrixRows = 12;
  size_t matrixCols = 9;

  // constructor allocation
  vctDynamicMatrix<ElementType> m1(matrixRows, matrixCols);

  // Create an empty matrix and later allocate memory.
  vctDynamicMatrix<ElementType> m2;
  m2.SetSize(matrixRows, matrixCols);

  // Create a dynamic matrix of some size and then change it.
  // This operation does not preserve any elements in the resized
  // matrix
  vctDynamicMatrix<Elements> m3(3 * matrixRows, 3 * matrixCols);
  m3.SetSize(2 * matrixRows, 2 * matrixCols);

  // resize a matrix and keep as many elements as possible.
  m3.resize(matrixRows, matrixCols);

  // Store an algebraic result to a new matrix.  In this case,
  // memory is allocated by the algebraic operation, and then
  // attached to the matrix object.
  vctDynamicMatrix<double> m4 = m3 - m2;
  \endcode
  <li> The default storage order is row first.  This can be modified
  using the different constructors as well as the method SetSize with
  the flags #VCT_ROW_MAJOR or #VCT_COL_MAJOR.
  \code
  // 12 by 7 matrix stored column first
  vctDynamicMatrix<double> m1(12, 7, VCT_COL_MAJOR);
  // a similar matrix filled with zeroes
  vctDynamicMatrix<double> m1(12, 7, 0.0, VCT_COL_MAJOR);
  // resize the matrix and change its storage order
  m1.SetSize(5, 7, VCT_ROW_MAJOR);
  \endcode
  <li> Matrix assignment can be facilitated through the Assign method
  (defined in the base class) or as follows.
  \code
  // Initialize all elements to the same value
  vctDynamicMatrix<ElementType> v5(matrixRows, matrixCols, 2.0);

  // Initialize the elements by specific values.  NOTE: All the
  // arguments MUST be of type ElementType
  vctDynamicMatrix<ElementType> matrix(2, 4);
  matrix.Assign(7.0, 1.0, 2.0, 3.0,
                4.0, 5.0, 6.0, 7.0); // correct
  matrix.Assign(7, 1, 2, 3,
                4, 5, 6, 7); // WRONG, missing dot

  // Assign one matrix to another.
  vctDynamicMatrix<int> matrixInt;
  matrixInt.Assign(matrix);
  matrixInt = matrix; // same operation
  \endcode
  </ol>

  A few more notes.
  <ul>
  <li> The elements lie in contiguous increasing memory addresses.
  <li> There is no direct way of ``disowning'' a vctDynamicMatrix,
  that is taking ownership of its memory.
  <li> When a function returns an allocating dynamic matrix object, it
  is better to return a vctReturnDynamicMatrix, which is a helper
  class designed for transfering the ownership of the allocated memory
  without reallocation.
  </ul>

  \param _elementType the type of an element in the matrix

  \sa vctDynamicMatrixBase vctDynamicConstMatrixBase
*/
template <class _elementType>
class vctDynamicMatrix : public vctDynamicMatrixBase<vctDynamicMatrixOwner<_elementType>, _elementType>
{

    friend class vctReturnDynamicMatrix<_elementType>;

public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 2};
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    typedef vctDynamicMatrixBase<vctDynamicMatrixOwner<_elementType>, _elementType> BaseType;
    typedef vctDynamicMatrix<_elementType> ThisType;


    /*! Default constructor. Initialize an empty matrix. */
    vctDynamicMatrix()
        // The default initialization of vctDynamicMatrixOwner is empty.
    {}


    /* Constructor: Create a matrix of the specified size.  Elements
       initialized with default constructor.  The storage order can be
       either #VCT_ROW_MAJOR or #VCT_COL_MAJOR. */
    //@{
    vctDynamicMatrix(size_type rows, size_type cols, bool storageOrder = VCT_DEFAULT_STORAGE) {
        this->SetSize(rows, cols, storageOrder);
    }

    vctDynamicMatrix(const nsize_type & matrixSize, bool storageOrder = VCT_DEFAULT_STORAGE) {
        this->SetSize(matrixSize, storageOrder);
    }
    //@}

    /*! Constructor: Create a matrix of the specified size and assign all
      elements a specific value. The storage order can be
       either #VCT_ROW_MAJOR or #VCT_COL_MAJOR.*/
    //@{
    vctDynamicMatrix(size_type rows, size_type cols, value_type value, bool storageOrder = VCT_DEFAULT_STORAGE) {
        this->SetSize(rows, cols, storageOrder);
        this->SetAll(value);
    }

    vctDynamicMatrix(const nsize_type & matrixSize, value_type value, bool storageOrder = VCT_DEFAULT_STORAGE) {
        this->SetSize(matrixSize, storageOrder);
        this->SetAll(value);
    }
    //@}

    /*! Special copy constructor: Take ownership of the data of a
      temporary matrix object of type vctReturnDynamicMatrix.  Disown
      the other matrix.
    */
    vctDynamicMatrix(const vctReturnDynamicMatrix<value_type> & otherMatrix);


    /*! Copy constructor: Allocate memory to store a copy of the other
      matrix, and copy the elements of the other matrix to this
      matrix.
    */
    vctDynamicMatrix(const ThisType & otherMatrix):
        BaseType()
    {
        this->SetSize(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.StorageOrder());
        this->Assign(otherMatrix);
    }


    /*! Copy constructor: Allocate memory and copy all the elements
      from the other matrix. The storage order can be either
      #VCT_ROW_MAJOR or #VCT_COL_MAJOR.*/
    template <class __matrixOwnerType, typename __otherMatrixElementType>
    vctDynamicMatrix(const vctDynamicConstMatrixBase<__matrixOwnerType, __otherMatrixElementType> & otherMatrix, bool storageOrder) {
        this->SetSize(otherMatrix.rows(), otherMatrix.cols(), storageOrder);
        this->Assign(otherMatrix);
    }


    /*! Copy constructor: Allocate memory and copy all the elements
      from the other matrix. The storage order of the copied matrix is
      defined by the source matrix.
    */
    template <class __matrixOwnerType>
    vctDynamicMatrix(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & otherMatrix) {
        this->SetSize(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.StorageOrder());
        this->Assign(otherMatrix);
    }


    /*! Copy constructor: Allocate memory and copy all the elements
      from the other matrix. The storage order of the copied matrix is
      defined by the source matrix.  This constructor can also be used
      for type conversions.
    */
    template <class __matrixOwnerType, typename __otherMatrixElementType>
    explicit vctDynamicMatrix(const vctDynamicConstMatrixBase<__matrixOwnerType, __otherMatrixElementType> & otherMatrix) {
        this->SetSize(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.StorageOrder());
        this->Assign(otherMatrix);
    }


    /*! Constructor from a fixed size matrix. */
    template <size_type __rows, size_type __cols,
              stride_type __rowStride, stride_type __colStride,
              class __elementType, class __dataPtrType>
    explicit vctDynamicMatrix(const vctFixedSizeConstMatrixBase<__rows, __cols,
                                                                __rowStride, __colStride,
                                                                __elementType, __dataPtrType>  & other) {
        this->ForceAssign(other);
    }


    /*!  Assignment from a dynamic matrix to a matrix.  The
      operation discards the old memory allocated for this matrix, and
      allocates new memory the size of the input matrix.  Then the
      elements of the input matrix are copied into this matrix.

      \todo This assumes a row major storage.  Needs more work.
    */
    template <class __matrixOwnerType, typename __elementType>
    ThisType & operator = (const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & otherMatrix) {
        this->SetSize(otherMatrix.rows(), otherMatrix.cols());
        this->Assign(otherMatrix);
		return *this;
    }


    /*!  Assignment from a dynamic matrix to this matrix.  The
      operation discards the old memory allocated for this matrix, and
      allocates new memory the size of the input matrix.  Then the
      elements of the input matrix are copied into this matrix.
    */
    ThisType & operator = (const ThisType & otherMatrix) {
        this->SetSize(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.StorageOrder());
        this->Assign(otherMatrix);
		return *this;
    }

    /*! Assignment from a fixed size matrix.  This operator will
      resize the left side dynamic matrix to match the right side
      fixed size matrix. */
    template <size_type __rows, size_type __cols,
              stride_type __rowStride, stride_type __colStride,
              class __elementType, class __dataPtrType>
    ThisType & operator = (const vctFixedSizeConstMatrixBase<__rows, __cols,
                                                             __rowStride, __colStride,
                                                             __elementType, __dataPtrType>  & other) {
        this->ForceAssign(other);
        return *this;
    }


    /*!  Assignement from a transitional vctReturnDynamicMatrix to a
      vctDynamicMatrix variable.  This specialized operation does not
      perform any element copy.  Instead it transfers ownership of the
      data from the other matrix to this matrix, and disowns the other
      matrix.  The right hand side operand must be a temporary object
      returned, e.g., from a function or overloaded operator.

      \todo This operator needs some revisions.
    */
    ThisType & operator = (const vctReturnDynamicMatrix<value_type> & otherMatrix);

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

    // documented in base class
    template <class __matrixOwnerType, typename __elementType>
    inline ThisType & ForceAssign(const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & other) {
        this->SetSize(other.sizes(), other.StorageOrder());
        this->Assign(other);
        return *this;
    }

    // documented in base class
    template <size_type __rows, size_type __cols,
              stride_type __rowStride, stride_type __colStride,
              class __elementType, class __dataPtrType>
    inline ThisType & ForceAssign(const vctFixedSizeConstMatrixBase<__rows, __cols,
                                                                    __rowStride, __colStride,
                                                                    __elementType, __dataPtrType>  & other) {
        this->SetSize(other.sizes(), other.StorageOrder());
        this->Assign(other);
        return *this;
    }

    /*! Non-destructive size change.  Change the size to the specified
      size, and preserve as many rows and columns as possible from the
      former matrix.

      \note If the storage order and the sizes (both rows and columns)
      are unchanged, this method does nothing.

      \note This method doesn't allow to change the storage order of
      the elements (i.e. stays either row or column major).

      \note If the size is set to zero, the data pointer is set to
      null (0).
    */
    //@{
    void resize(size_type rows, size_type cols) {
        this->resize(nsize_type(rows, cols));
    }

    void resize(const nsize_type & newSizes) {
        if (newSizes == this->sizes())
            return;

        const bool isRowMajor = this->IsRowMajor();
        ThisType newData(newSizes, isRowMajor);
        const nsize_type corner(0);
        nsize_type minSizes;
        minSizes.ElementwiseMinOf(this->sizes(), newSizes);
        vctDynamicConstMatrixRef<value_type> myDataMinSpaceRef(*this, corner, minSizes);
        vctDynamicMatrixRef<value_type> newDataMinSpaceRef(newData, corner, minSizes);
        newDataMinSpaceRef.Assign(myDataMinSpaceRef);
        this->Matrix.Disown();
        this->Matrix.Own(newSizes, isRowMajor, newData.Matrix.Release());
    }
    //@}

    /*! DESTRUCTIVE size change.  Change the size to the specified
      size.  Discard of all the old values. The storage order can be
      either #VCT_ROW_MAJOR or #VCT_COL_MAJOR.  If the storage order
      is not specified, it is not modified. */
    //@{
    void SetSize(size_type rows, size_type cols, bool storageOrder) {
        this->Matrix.SetSize(rows, cols, storageOrder);
    }

    void SetSize(const nsize_type & matrixSize, bool storageOrder) {
        this->Matrix.SetSize(matrixSize, storageOrder);
    }

    void SetSize(size_type rows, size_type cols) {
        this->Matrix.SetSize(rows, cols, this->StorageOrder());
    }

    void SetSize(const nsize_type & matrixSize) {
        this->Matrix.SetSize(matrixSize, this->StorageOrder());
    }
    //@}

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream)
    {
        // get and set size
        size_type myRows = 0;
        size_type myCols = 0;
        cmnDeSerializeSizeRaw(inputStream, myRows);
        cmnDeSerializeSizeRaw(inputStream, myCols);
        this->SetSize(myRows, myCols);

        // get data
        size_type indexRow, indexCol;
        for (indexRow = 0; indexRow < myRows; ++indexRow) {
            for (indexCol = 0; indexCol < myCols; ++indexCol) {
                cmnDeSerializeRaw(inputStream, this->Element(indexRow, indexCol));
            }
        }
    }

};


/*!
  Class vctReturnDynamicMatrix is specialized to store a temporary
  matrix object by transfering ownership.  An object of this class has
  all the methods inherited from vctDynamicMatrix, but can only be
  constructed in one way -- taking the ownership from another
  vctDynamicMatrix (or vctReturnDynamicMatrix) object.  In turn, when
  an assignment occurs from a vctReturnDynamicMatrix to a
  vctDynamicMatrix (or likewise construction), the lhs
  vctDynamicMatrix strips this object from the ownership of the data.

  Use this class only when you want to return a newly created dynamic
  matrix from a function, without going through memory allocation and
  deallocation.  Never use it on an object that is going to remain
  in scope after constructing the vctReturnDynamicMatrix.
*/
template <class _elementType>
class vctReturnDynamicMatrix : public vctDynamicMatrix<_elementType> {
public:
    /*! Base type of vctReturnDynamicMatrix. */
    typedef vctDynamicMatrix<_elementType> BaseType;

    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    explicit vctReturnDynamicMatrix(const BaseType & other)
    {
        BaseType & nonConstOther = const_cast<BaseType &>(other);
        // if we don't save it in a variable, it will be destroyed in the Release operation
        const size_type rows = other.rows();
        const size_type cols = other.cols();
        const bool storageOrder = other.StorageOrder();
        this->Matrix.Own(rows, cols, storageOrder, nonConstOther.Matrix.Release());
    }
};


// implementation of the special copy constuctor of vctDynamicMatrix
template <class _elementType>
vctDynamicMatrix<_elementType>::vctDynamicMatrix(const vctReturnDynamicMatrix<_elementType> & other) {
    vctReturnDynamicMatrix<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicMatrix<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const size_type rows = other.rows();
    const size_type cols = other.cols();
    const bool storageOrder = other.StorageOrder();
    this->Matrix.Own(rows, cols, storageOrder, nonConstOther.Matrix.Release());
}


// implementation of the special assignment operator from vctReturnDynamicMatrix to vctDynamicMatrix
template <class _elementType>
vctDynamicMatrix<_elementType> &
vctDynamicMatrix<_elementType>::operator = (const vctReturnDynamicMatrix<_elementType> & other) {
    vctReturnDynamicMatrix<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicMatrix<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const size_type rows = other.rows();
    const size_type cols = other.cols();
    const bool storageOrder = other.StorageOrder();
    this->Matrix.Disown();
    this->Matrix.Own(rows, cols, storageOrder, nonConstOther.Matrix.Release());
    return *this;
}


/*! \name Elementwise operations between dynamic matrices. */
//@{
/*! Operation between dynamic matrices (same size).
  \param inputMatrix1 The first operand of the binary operation.
  \param inputMatrix2 The second operand of the binary operation.
  \return The matrix result of \f$op(matrix1, matrix2)\f$.
*/
template <class _matrixOwnerType1, class _matrixOwnerType2, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator + (const vctDynamicConstMatrixBase<_matrixOwnerType1, _elementType> & inputMatrix1,
            const vctDynamicConstMatrixBase<_matrixOwnerType2, _elementType> & inputMatrix2) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix1);
    resultStorage.Add(inputMatrix2);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template <class _matrixOwnerType1, class _matrixOwnerType2, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const vctDynamicConstMatrixBase<_matrixOwnerType1, _elementType> & inputMatrix1,
            const vctDynamicConstMatrixBase<_matrixOwnerType2, _elementType> & inputMatrix2) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix1);
    resultStorage.Subtract(inputMatrix2);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a dynamic matrix and a
  scalar. */
//@{
/*! Operation between a dynamic matrix and a scalar.
  \param inputMatrix The first operand of the binary operation.
  \param inputScalar The second operand of the binary operation.
  \return The matrix result of \f$op(matrix, scalar)\f$.
*/
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator + (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Add(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Subtract(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator * (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Multiply(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator / (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Divide(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a scalar and a dynamic
  matrix. */
//@{
/*! Operation between a scalar and a dynamic matrix.
  \param inputScalar The first operand of the binary operation.
  \param inputMatrix The second operand of the binary operation.
  \return The matrix result of \f$op(scalar, matrix)\f$.
*/
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator + (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.SumOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.DifferenceOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator * (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.ProductOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator / (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.RatioOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}


/*! \name Elementwise operations on a dynamic matrix. */
//@{
/*! Unary operation on a dynamic matrix.
  \param inputMatrix The operand of the unary operation
  \return The matrix result of \f$op(matrix)\f$.
*/
template <class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.NegationOf(inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}



#ifndef DOXYGEN
template <class _matrixOwnerType1, class _matrixOwnerType2, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator * (const vctDynamicConstMatrixBase<_matrixOwnerType1, _elementType> & inputMatrix1,
            const vctDynamicConstMatrixBase<_matrixOwnerType2, _elementType> & inputMatrix2) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix1.rows(), inputMatrix2.cols());
    resultStorage.ProductOf(inputMatrix1, inputMatrix2);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}


template <class _matrixOwnerType, class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputMatrix.rows());
    resultStorage.ProductOf(inputMatrix, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}


template <class _vectorOwnerType, class _matrixOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputMatrix.cols());
    resultStorage.ProductOf(inputVector, inputMatrix);
    return vctReturnDynamicVector<value_type>(resultStorage);
}



/*
  Methods declared previously and implemented here because they require vctReturnDynamicMatrix
*/


/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Abs(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::AbsValue>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Negation(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::Negation>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Floor(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::Floor>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Ceil(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::Ceil>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Eye(size_type size) {
    vctDynamicMatrix<value_type> resultStorage(size, size);
    resultStorage.SetAll(_elementType(0));
    resultStorage.Diagonal().SetAll(_elementType(1));
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* Documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class __matrixOwnerType, class _elementType,
          class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareMatrix(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix1,
                                         const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix2) {
    vctDynamicMatrix<bool> result(matrix1.rows(), matrix1.cols());
    vctDynamicMatrixLoopEngines::
        MoMiMi<_elementOperationType>::Run(result, matrix1, matrix2);
    return vctReturnDynamicMatrix<bool>(result);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType,
          class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareScalar(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix,
                                         const _elementType & scalar) {
    vctDynamicMatrix<bool> result(matrix.rows(), matrix.cols());
    vctDynamicMatrixLoopEngines::
        MoMiSi<_elementOperationType>::Run(result, matrix, scalar);
    return vctReturnDynamicMatrix<bool>(result);
}

#endif // DOXYGEN


#endif // _vctDynamicMatrix_h

