/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Ofri Sadowsky
  Created on:	2003-11-04

  (C) Copyright 2003-2020 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeMatrix_h
#define _vctFixedSizeMatrix_h

/*!
  \file
  \brief Declaration of vctFixedSizeMatrix
 */

#include <cisstVector/vctFixedSizeConstMatrixRef.h>
#include <cisstVector/vctFixedSizeMatrixRef.h>


/*! \brief Implementation of a fixed-size matrix using template
  metaprogramming.

  \ingroup cisstVector

  The matrix type is stored as a contiguous array of a fixed size
  (stack allocation).  It provides methods for operations which are
  implemented using template metaprogramming.  See
  vctFixedSizeConstMatrixBase and vctFixedSizeMatrixBase for more
  implementation details.

  \param _elementType the type of an element in the matrix
  \param _rows the number of rows of the matrix
  \param _cols the number of columns of the matrix
  \param _rowMajor the storage order, either #VCT_ROW_MAJOR (default) or #VCT_COL_MAJOR.
*/

template<class _elementType, vct::size_type _rows, vct::size_type _cols, bool _rowMajor>
class vctFixedSizeMatrix : public vctFixedSizeMatrixBase
<_rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows, _elementType,
 typename vctFixedSizeMatrixTraits<_elementType, _rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows>::array >
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    typedef vctFixedSizeMatrixTraits<_elementType, _rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows> MatrixTraits;

    /*! Type of the base class */
    typedef vctFixedSizeMatrixBase<_rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows, _elementType,
        typename vctFixedSizeMatrixTraits<_elementType, _rows, _cols,
        _rowMajor?_cols:1, _rowMajor?1:_rows>::array >
        BaseType;
    typedef vctFixedSizeMatrix<_elementType, _rows, _cols, _rowMajor> ThisType;

    typedef typename MatrixTraits::iterator iterator;
    typedef typename MatrixTraits::const_iterator const_iterator;
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;

    typedef class cmnTypeTraits<value_type> TypeTraits;
    typedef typename TypeTraits::VaArgPromotion ElementVaArgPromotion;

    /*! Default constructor.  Do nothing. */
    inline vctFixedSizeMatrix() {}

    /*! Copy constructor */
    inline vctFixedSizeMatrix(const ThisType & other) {
        this->Assign(other);
    }

    /*! Initialize all the elements to the given value.
      \param value the value used to set all the elements of the matrix
    */
    explicit inline vctFixedSizeMatrix(const value_type & value) {
        this->SetAll(value);
    }

    /*! Initialize the elements of the matrix with a set of given
      values.  The assignment is done by the order of the iterators,
      that is, row by row from the low column to the high column in
      each row.

      \note This constructor doesn't check that the correct number of
      elements have been provided.  Use with caution.

      \note Since this method relies on the standard \c va_arg, the
      parameters must be correctly casted.  E.g, \c 3.14 will be
      treated as a double while \c 3.14f will be treated as a float
      and \c 3 will be handled like an integer.
    */
    inline vctFixedSizeMatrix(const value_type element0, const value_type element1, ...)
    {
        CMN_ASSERT( this->size() > 1 );
        (*this).at(0) = element0;
        (*this).at(1) = element1;
        index_type elementIndex = 2;
        const size_type numElements = this->size();
        va_list nextArg;
        va_start(nextArg, element1);
        for (; elementIndex < numElements; ++elementIndex) {
            (*this).at(elementIndex) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
    }

    /*! Initialize the elements of this matrix with values from
      another matrix.  The other matrix can include elements of any
      type, which will be converted using standard conversion to
      elements of this matrix.
    */
    template <class __elementType, stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    explicit inline
    vctFixedSizeMatrix(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, __elementType, __dataPtrType> & matrix) {
        this->Assign(matrix);
    }

    /*! Initialize the matrix from a dynamic matrix with elements of
      the same type.

      \note Since the size of the dynamic matrix might not match the
      size of the fixed size one, this constructor can throw an
      exception.

      \sa Corresponding Assign()
    */
    template <class __matrixOwnerType>
    inline vctFixedSizeMatrix(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & matrix)
    {
        this->Assign(matrix);
    }

	template <stride_type __rowStride, stride_type __colStride, class __elementType, class __dataPtrType>
    inline ThisType & operator = (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, __elementType,__dataPtrType> & other) {
        // Experimental static_cast replacing reinterpret_cast.  Return the reinterpret_cast if you encounter
        // compilation errors.
        return static_cast<ThisType &>(this->Assign(other));
	}

	inline ThisType & operator = (const ThisType & other) {
		return static_cast<ThisType &>(this->Assign(other));
	}

	template <stride_type __rowStride, stride_type __colStride>
    inline ThisType & operator = (const vctFixedSizeConstMatrixRef<value_type, _rows, _cols, __rowStride, __colStride> & other)
	{
		return static_cast<ThisType &>(this->Assign(other));
	}

	template <stride_type __rowStride, stride_type __colStride, class __elementType>
    inline ThisType & operator = (const vctFixedSizeConstMatrixRef<__elementType, _rows, _cols, __rowStride, __colStride> & other)
	{
		this->Assign(other);
		return *this;
	}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

};




/*! \name Elementwise operations between fixed size matrices. */
//@{
/*! Operation between fixed size matrices (same size).
  \param matrix1 The first operand of the binary operation.
  \param matrix2 The second operand of the binary operation.
  \return The matrix result of \f$op(matrix1, matrix2)\f$. */
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _input1RowStride, vct::stride_type _input1ColStride, class _input1Data,
          vct::stride_type _input2RowStride, vct::stride_type _input2ColStride, class _input2Data, class _elementType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator + (const vctFixedSizeConstMatrixBase<_rows, _cols, _input1RowStride, _input1ColStride, _elementType, _input1Data> & matrix1,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _input2RowStride, _input2ColStride, _elementType, _input2Data> & matrix2) {
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.SumOf(matrix1, matrix2);
    return result;
}

/* documented above */
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _input1RowStride, vct::stride_type _input1ColStride, class _input1Data,
          vct::stride_type _input2RowStride, vct::stride_type _input2ColStride, class _input2Data, class _elementType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator - (const vctFixedSizeConstMatrixBase<_rows, _cols, _input1RowStride, _input1ColStride, _elementType, _input1Data> & matrix1,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _input2RowStride, _input2ColStride, _elementType, _input2Data> & matrix2) {
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.DifferenceOf(matrix1, matrix2);
    return result;
}
//@}


/*! \name Elementwise operations between a matrix and a scalar. */
//@{
/*! Operation between a matrix and a scalar.
 \param matrix The first operand of the binary operation.
 \param scalar The second operand of the binary operation.
 \return The matrix result of \f$op(matrix, scalar)\f$. */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator + (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.SumOf(matrix, scalar);
    return result;
}

/* documented above */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator - (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.DifferenceOf(matrix, scalar);
    return result;
}

/* documented above */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator * (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.ProductOf(matrix, scalar);
    return result;
}

/* documented above */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator / (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.RatioOf(matrix, scalar);
    return result;
}
//@}


/*! \name Elementwise operations between a scalar and a matrix. */
//@{
/*! Operation between a scalar and a matrix.
 \param scalar The first operand of the binary operation.
 \param matrix The second operand of the binary operation.
 \return The matrix result of \f$op(scalar, matrix)\f$. */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator + (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.SumOf(scalar, matrix);
    return result;
}

/* documented above */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator - (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.DifferenceOf(scalar, matrix);
    return result;
}

/* documented above */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator * (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.ProductOf(scalar, matrix);
    return result;
}

/* documented above */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator / (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.RatioOf(scalar, matrix);
    return result;
}
//@}



/*! \name Elementwise operations on a matrix. */
//@{
/*! Unary operation on a matrix.
  \param matrix The operand of the unary operation
  \return The matrix result of \f$op(matrix)\f$.
*/
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator - (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.NegationOf(matrix);
    return result;
}
//@}


template <vct::size_type _rows, vct::size_type _cols, vct::size_type _input1Cols,
          vct::stride_type _input1RowStride, vct::stride_type _input1ColStride, class _input1DataPtrType,
          vct::stride_type _input2RowStride, vct::stride_type _input2ColStride, class _input2DataPtrType,
          class _elementType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator * (const vctFixedSizeConstMatrixBase<_rows, _input1Cols, _input1RowStride, _input1ColStride, _elementType, _input1DataPtrType> & input1Matrix,
            const vctFixedSizeConstMatrixBase<_input1Cols, _cols, _input2RowStride, _input2ColStride, _elementType, _input2DataPtrType> & input2Matrix) {
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.ProductOf(input1Matrix, input2Matrix);
    return result;
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _inputMatrixRowStride, vct::stride_type _inputMatrixColStride, class _inputMAtrixDataPtrType,
          vct::stride_type _inputVectorStride, class _inputVectorDataPtrType,
          class _elementType>
inline vctFixedSizeVector<_elementType, _rows>
operator * (const vctFixedSizeConstMatrixBase<_rows, _cols, _inputMatrixRowStride, _inputMatrixColStride, _elementType, _inputMAtrixDataPtrType> & inputMatrix,
            const vctFixedSizeConstVectorBase<_cols, _inputVectorStride, _elementType, _inputVectorDataPtrType> & inputVector)
{
    vctFixedSizeVector<_elementType, _rows> result;
    result.ProductOf(inputMatrix, inputVector);
    return result;
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _inputVectorStride, class _inputVectorDataPtrType,
          vct::stride_type _inputMatrixRowStride, vct::stride_type _inputMatrixColStride, class _inputMatrixDataPtrType,
          class _elementType>
inline vctFixedSizeVector<_elementType, _cols>
operator * (const vctFixedSizeConstVectorBase<_rows, _inputVectorStride, _elementType, _inputVectorDataPtrType> & inputVector,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _inputMatrixRowStride, _inputMatrixColStride, _elementType, _inputMatrixDataPtrType> & inputMatrix)
{
    vctFixedSizeVector<_elementType, _cols> result;
    result.ProductOf(inputVector, inputMatrix);
    return result;
}


/*
  Methods declared previously and implemented here because they require vctFixedSizeVector
*/
#ifndef DOXYGEN

/* documented in class vctFixedSizeConstMatrixBase */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Abs(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::AbsValue>::
        Run(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Negation(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::Negation>::
        Run(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Floor(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::Floor>::
        Run(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Ceil(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::Ceil>::
        Run(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template <vct::size_type _rows, vct::size_type _cols, vct::stride_type _rowStride, vct::stride_type _colStride, class _elementType, class _dataPtrType>
const typename vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType &
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Eye(void) {
    static MatrixValueType result(_elementType(0));
#if (CISST_COMPILER == CISST_GCC) || (CISST_COMPILER == CISST_CLANG)
    result.Diagonal().SetAll(_elementType(1));
#else
    static _elementType value(result.Diagonal().SetAll(_elementType(1)));
#endif
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride, class _dataPtrType,
          vct::stride_type __rowStride, vct::stride_type __colStride, class __dataPtrType,
          class _elementType,
          class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareMatrix(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix1,
                                           const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                                           _elementType, __dataPtrType> & matrix2) {
    vctFixedSizeMatrix<bool, _rows, _cols> result;
    vctFixedSizeMatrixLoopEngines::
        MoMiMi<_elementOperationType>::Run(result, matrix1, matrix2);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride, class _dataPtrType,
          class _elementType,
          class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareScalar(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix,
                                           const _elementType & scalar) {
    vctFixedSizeMatrix<bool, _rows, _cols> result;
    vctFixedSizeMatrixLoopEngines::
        MoMiSi<_elementOperationType>::Run(result, matrix, scalar);
    return result;
}
#endif // DOXYGEN


#endif // _vctFixedSizeMatrix_h

