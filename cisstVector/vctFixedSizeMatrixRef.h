/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky
  Created on: 2003-11-04

  (C) Copyright 2003-2015 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeMatrixRef_h
#define _vctFixedSizeMatrixRef_h

/*!
  \file
  \brief Declaration of vctFixedSizeMatrixRef
 */

#include <cisstVector/vctFixedSizeMatrixBase.h>

/*!  \brief An implementation of the ``abstract''
  vctFixedSizeMatrixBase.

  \ingroup cisstVector

  This implementations uses a pointer to the matrix beginning as the
  matrix defining data member.  An instantiation of this type can be
  used as a matrix reference with TransposeRef().

  See the base class (vctFixedSizeMatrixBase) for template
  parameter details.

  \sa vctFixedSizeConstMatrixRef
*/
template <class _elementType, vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
class vctFixedSizeMatrixRef : public vctFixedSizeMatrixBase
  <_rows, _cols,_rowStride, _colStride, _elementType,
   typename vctFixedSizeMatrixTraits<_elementType, _rows, _cols, _rowStride, _colStride>::pointer>
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    typedef vctFixedSizeMatrixTraits<_elementType, _rows, _cols,
                                     _rowStride, _colStride> MatrixTraits;

    typedef typename MatrixTraits::iterator iterator;
    typedef typename MatrixTraits::const_iterator const_iterator;
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;

    typedef vctFixedSizeMatrixRef<value_type, _rows, _cols,
                                  _rowStride, _colStride> ThisType;
    typedef vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, value_type,
                                   typename MatrixTraits::pointer> BaseType;

    /*! Default constructor: create an uninitialized matrix */
    vctFixedSizeMatrixRef()
        {}

    /*! Initialize the matrix with a (non-const) pointer */
    vctFixedSizeMatrixRef(pointer p) {
        SetRef(p);
    }

    /*! Convenience constructor to initialize a reference to a fixed-size matrix object.
      The constructor is declared as explicit, and care must be taken when a matrix
      object is passed as a function argument.
    */
    template <size_type __rows, size_type __cols, class __dataPtrType>
    inline explicit vctFixedSizeMatrixRef(vctFixedSizeMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType> & matrix)
    {
        SetRef(matrix, 0, 0);
    }


    /*! Convenience constructor to initialize a reference to a fixed-size matrix object.
    */
    template <size_type __rows, size_type __cols, class __dataPtrType>
    inline vctFixedSizeMatrixRef(
        vctFixedSizeMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType>
        & matrix, index_type startRow, index_type startCol )
    {
        SetRef(matrix, startRow, startCol);
    }

    /*! Convenience constructor to initialize a fixed-size reference
      to a dynamic matrix.  The constructor involves assertion that
      the sizes and strides match */
    template <class __matrixOwnerType>
    inline vctFixedSizeMatrixRef(vctDynamicMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                 index_type startRow, index_type startCol)
    {
        SetRef(matrix, startRow, startCol);
    }

    /*! Assign the matrix start with a (non-const) pointer */
    void SetRef(pointer p) {
        this->Data = p;
    }

    void SetRef(const ThisType & other) {
        this->SetRef(other.Data);
    }

    /*! Convenience method to set a reference to a fixed-size matrix object.
      \param matrix the fixed size matrix to be referenced
      \param startRow the row index from which reference will start
      \param startCol the column index from which reference will start
      The strides of this matrix must be identical to the strides of the other
      matrix (this is enforced by the template parameters).
      \note This method verifies that the size of this matrix does not exceed the
      size of the input matrix (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <size_type __rows, size_type __cols, class __dataPtrType>
    inline void SetRef(vctFixedSizeMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType> & matrix,
                       index_type startRow, index_type startCol)
    {
        if ((startRow + this->rows() > matrix.rows()) || (startCol + this->cols() > matrix.cols())) {
            cmnThrow(std::out_of_range("vctFixedSizeMatrixRef SetRef out of range"));
        }
        SetRef(matrix.Pointer(startRow, startCol));
    }

    template <class __dataPtrType>
    inline void SetRef(vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, __dataPtrType> & matrix)
    {
        SetRef(matrix.Pointer());
    }

    /*! Convenience method to set a reference to a dynamic matrix object.
      \param matrix the fixed size matrix to be referenced
      \param startRow the row index from which reference will start
      \param startCol the column index from which reference will start
      The strides of this matrix must be identical to the strides of the other
      matrix (otherwise cmnThrow is used to throw std::runtime_error).
      \note This method verifies that the size of this matrix does not exceed the
      size of the input matrix (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <class __matrixOwnerType>
    inline void SetRef(vctDynamicMatrixBase<__matrixOwnerType, _elementType> & matrix,
                       index_type startRow, index_type startCol)
    {
        if ((this->row_stride() != matrix.row_stride()) || (this->col_stride() == matrix.col_stride())) {
            cmnThrow(std::runtime_error("vctFixedSizeMatrixRef SetRef with incompatible stride(s)"));
        }
        if ((startRow + this->rows() > matrix.rows()) || (startCol + this->cols() > matrix.cols())) {
            cmnThrow(std::out_of_range("vctFixedSizeMatrixRef SetRef out of range"));
        }
        SetRef(matrix.Pointer(startRow, startCol));
    }

    /*!
      \name Assignment operation into a matrix reference

      \param other The matrix to be copied.
    */
    //@{
    inline ThisType & operator = (const ThisType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <stride_type __rowStride, stride_type __colStride>
    inline ThisType & operator = (const vctFixedSizeConstMatrixRef<_elementType, _rows, _cols, __rowStride, __colStride> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <stride_type __rowStride, stride_type __colStride, class __elementType, class __dataPtrType>
    inline ThisType & operator = (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, __elementType, __dataPtrType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <class __matrixOwnerType>
    inline ThisType & operator = (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }
    //@}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

};


#ifndef DOXYGEN

/* MultiplyMatrixVector function declaration is in
   vctFixedSizeVectorBase. This is an auxiliary function which
   multiplies matrix*vector and stored the result directly into a
   matrix */
template <vct::size_type _resultSize, vct::stride_type _resultStride, class _resultElementType, class _resultDataPtrType,
          vct::size_type _matrixCols, vct::stride_type _matrixRowStride, vct::stride_type _matrixColStride, class _matrixDataPtrType,
          vct::stride_type _vectorStride, class _vectorDataPtrType>
inline void MultiplyMatrixVector(
                                 // create matrix references to both vectors and use the matrix product
                                 vctFixedSizeVectorBase<_resultSize, _resultStride, _resultElementType, _resultDataPtrType> & result,
                                 const vctFixedSizeConstMatrixBase<_resultSize, _matrixCols, _matrixRowStride, _matrixColStride,
                                 _resultElementType, _matrixDataPtrType> & matrix,
                                 const vctFixedSizeConstVectorBase<_matrixCols, _vectorStride, _resultElementType, _vectorDataPtrType> & vector)
{
    const vctFixedSizeConstMatrixRef<_resultElementType, _matrixCols, 1, _vectorStride, 1>
        inputVectorRef(vector.Pointer());
    vctFixedSizeMatrixRef<_resultElementType, _resultSize, 1, _resultStride, 1> resultRef(result.Pointer());
    resultRef.ProductOf(matrix, inputVectorRef);
}

/* MultiplyVectorMatrix function declaration is in
   vctFixedSizeVectorBase. This is an auxiliary function which
   multiplies vector*matrix and stored the result directly into a
   vector */
template <vct::size_type _resultSize, vct::stride_type _resultStride, class _resultElementType, class _resultDataPtrType,
          vct::size_type _vectorSize, vct::stride_type _vectorStride, class _vectorDataPtrType,
          vct::stride_type _matrixRowStride, vct::stride_type _matrixColStride, class _matrixDataPtrType >
inline void MultiplyVectorMatrix(
                                 vctFixedSizeVectorBase<_resultSize, _resultStride, _resultElementType, _resultDataPtrType> & result,
                                 const vctFixedSizeConstVectorBase<_vectorSize, _vectorStride, _resultElementType, _vectorDataPtrType> & vector,
                                 const vctFixedSizeConstMatrixBase<_vectorSize, _resultSize, _matrixRowStride, _matrixColStride,
                                 _resultElementType, _matrixDataPtrType> & matrix
                                 )
{
    // create matrix references to both vectors and use the matrix product
    const vctFixedSizeConstMatrixRef<_resultElementType, 1, _vectorSize, (_vectorStride*_vectorSize), _vectorStride>
        inputVectorRef(vector.Pointer());
    vctFixedSizeMatrixRef<_resultElementType, 1, _resultSize, (_resultStride*_resultSize), _resultStride>
        resultRef(result.Pointer());
    resultRef.ProductOf(inputVectorRef, matrix);
}

#endif // DOXYGEN


#endif // _vctFixedSizeMatrixRef_h
