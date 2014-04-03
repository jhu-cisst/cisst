/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky
  Created on:	2003-11-04

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeConstMatrixRef_h
#define _vctFixedSizeConstMatrixRef_h

/*!
  \file
  \brief Declaration of vctFixedSizeConstMatrixRef
 */

#include <cisstVector/vctFixedSizeMatrixBase.h>

/*!  \brief An implementation of the ``abstract''
  vctFixedSizeConstMatrixBase.

  \ingroup cisstVector

  This implementations uses a pointer to the matrix beginning as the
  matrix defining data member.  An instantiation of this type can be
  used as a matrix reference with TransposeRef().

  Note that the class provides only const operations, except for
  assigning the matrix start, which does not affect the matrix.
  This is despite the fact that the stored pointer is non-const.

  See the base class (vctFixedSizeConstMatrixBase) for template
  parameter details.
*/
template <class _elementType, vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
class vctFixedSizeConstMatrixRef : public vctFixedSizeConstMatrixBase
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

    typedef vctFixedSizeConstMatrixRef<value_type, _rows, _cols,
                                       _rowStride, _colStride> ThisType;
    typedef vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, value_type,
                                        typename MatrixTraits::pointer> BaseType;

    /*! Default constructor: create an uninitialized matrix */
    inline vctFixedSizeConstMatrixRef()
    {}

    /*! Initialize the matrix with a (non-const) pointer */
    inline vctFixedSizeConstMatrixRef(pointer p) {
        SetRef(p);
    }

    /*! Convenience constructor to initialize a reference to a fixed-size matrix object.
      Since the argument is const, there is no worry about explicit instantiation.
    */
    template <size_type __rows, size_type __cols, class __dataPtrType>
    inline vctFixedSizeConstMatrixRef(const vctFixedSizeConstMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType> & matrix)
    {
        SetRef(matrix, 0, 0);
    }

    /*! Convenience constructor to initialize a reference to a fixed-size matrix object.
    */
    template <size_type __rows, size_type __cols, class __dataPtrType>
    inline vctFixedSizeConstMatrixRef(const vctFixedSizeConstMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType> & matrix,
                                      size_type startRow, size_type startCol)
    {
        SetRef(matrix, startRow, startCol);
    }

    /*! Convenience constructor to initialize a fixed-size reference to a dynamic matrix.
      The involves assertion that the sizes and strides match */
    template <class __matrixOwnerType>
    inline vctFixedSizeConstMatrixRef(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                      size_type startRow, size_type startCol)
    {
        SetRef(matrix, startRow, startCol);
    }

    /*! Initialize the matrix with a const pointer.  This requires
      const_cast. */
    inline vctFixedSizeConstMatrixRef(const_pointer p) {
        SetRef(p);
    }

    /*! Assign the matrix start with a (non-const) pointer */
    inline void SetRef(pointer p) {
        this->Data = p;
    }

    /*! Assign the matrix start with a const pointer.  This requires const_cast. */
    inline void SetRef(const_pointer p) {
        this->Data = const_cast<pointer>(p);
    }

    /*! Convenience method to set a reference to a fixed-size matrix object.
      \param matrix the fixed size matrix to be referenced
      \param startRow the row index from which reference will start
      \param startCol the column index from which reference will start
      The strides of this matrix must be identical to the strides of the other
      matrix (this is enforced by the template parameters).
      \note This method asserts that the size of this matrix does not exceed the
      size of the input matrix (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <size_type __rows, size_type __cols, class __dataPtrType>
    inline void SetRef(const vctFixedSizeConstMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType> & matrix,
                       size_type startRow, size_type startCol)
    {
        if ((startRow + this->rows() > matrix.rows()) || (startCol + this->cols() > matrix.cols())) {
            cmnThrow(std::out_of_range("vctFixedSizeConstMatrixRef SetRef out of range"));
        }
        SetRef(matrix.Pointer(startRow, startCol));
    }

    /*! Convenience method to set a reference to a dynamic matrix object.
      \param matrix the fixed size matrix to be referenced
      \param startRow the row index from which reference will start
      \param startCol the column index from which reference will start
      The strides of this matrix must be identical to the strides of the other
      matrix (otherwise cmnThrow is used to throw std::runtime_error).
      \note This method asserts that the size of this matrix does not exceed the
      size of the input matrix (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <class __matrixOwnerType>
    inline void SetRef(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                       size_type startRow, size_type startCol)
    {
        if ((this->row_stride() != matrix.row_stride()) || (this->col_stride() == matrix.col_stride())) {
            cmnThrow(std::runtime_error("vctFixedSizeconstMatrixRef SetRef with incompatible stride(s)"));
        }
        if ((startRow + this->rows() > matrix.rows()) || (startCol + this->cols() > matrix.cols())) {
            cmnThrow(std::out_of_range("vctFixedSizeConstMatrixRef SetRef out of range"));
        }
        SetRef(matrix.Pointer(startRow, startCol));
    }

};


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
inline vctFixedSizeConstMatrixRef<_elementType, _cols, _rows, _colStride, _rowStride>
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                            _elementType, _dataPtrType>::TransposeRef() const {
    return ConstRefTransposeType(Data);
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
typename vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::RefTransposeType
inline vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride,
                              _elementType, _dataPtrType>::TransposeRef(void) {
    return RefTransposeType(this->Data);
}


#endif  // _vctFixedSizeConstMatrixRef_h

