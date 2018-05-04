/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-11-04

  (C) Copyright 2003-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeConstMatrixBase_h
#define _vctFixedSizeConstMatrixBase_h

/*!
  \file
  \brief Declaration of vctFixedSizeConstMatrixBase
 */

#include <cisstCommon/cmnSerializer.h>

#include <cisstVector/vctFixedSizeVectorRef.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrixTraits.h>
#include <cisstVector/vctFixedSizeMatrixLoopEngines.h>
#include <cisstVector/vctFastCopy.h>

#include <stdio.h>

/* Forward declarations */
#ifndef DOXYGEN
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride, class _dataPtrType,
          vct::stride_type __rowStride, vct::stride_type __colStride, class __dataPtrType,
          class _elementType,
          class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareMatrix(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix1,
                                           const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                                           _elementType, __dataPtrType> & matrix2);

template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride, class _dataPtrType,
          class _elementType,
          class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareScalar(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix,
                                           const _elementType & scalar);
#endif // DOXYGEN

/*!  \brief A template for a fixed size matrix with fixed spacing in
  memory.

  The matrix can be defined over an existing memory block, or define
  its own memory block.  The class's methods follow the STL
  specifications for Random Access Container.  It uses
  vctFixedSizeMatrixTraits to define the types required by the STL
  interface.

  This class defines a read-only memory matrix.  The read/write matrix
  is derived from this class (compare with
  vctFixedStrideMatrixConstIterator).  Therefore, all the methods of
  this class are const.  The non-const matrix and methods are
  defined in vctFixedSizeMatrixBase.

  Matrix indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _elementType the type of element in the matrix.

  \param _rows the number of rows that the matrix provides access to.

  \param _cols the number of columns that the matrix provides access
  to.

  \param _rowStride the spacing between the rows in the memory block.

  \param _colStride the spacing between the columns in the memory
  block.

  \param _dataPtrType the type of object that defines the matrix.
  This may be an element pointer of an acutal fixed-size C-style array
  type.

  \sa vctFixedStrideMatrixConstIterator vctFixedSizeMatrixTraits
*/
template<vct::size_type _rows, vct::size_type _cols,
         vct::stride_type _rowStride, vct::stride_type _colStride,
         class _elementType, class _dataPtrType>
class vctFixedSizeConstMatrixBase
{
 public:
    /* Declare the container-defined typed required by STL, plus the
       types completed by our traits class */

    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 2};
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    /*! Type of the matrix itself. */
    typedef vctFixedSizeConstMatrixBase<_rows, _cols,
        _rowStride, _colStride, _elementType, _dataPtrType> ThisType;

    /*! Traits used for all useful types related to a vctFixedSizeMatrix */
    typedef vctFixedSizeMatrixTraits<_elementType, _rows, _cols,
        _rowStride, _colStride> MatrixTraits;

    /*! Iterator on the elements of the matrix. */
    typedef typename MatrixTraits::iterator iterator;

    /*! Const iterator on the elements of the matrix. */
    typedef typename MatrixTraits::const_iterator const_iterator;

    /*! Reverse iterator on the elements of the matrix. */
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;

    /*! Const reverse iterator on the elements of the matrix. */
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;

    /*! Define the dimensions of the matrix */
    enum {ROWS = MatrixTraits::ROWS, COLS = MatrixTraits::COLS,
          LENGTH = MatrixTraits::LENGTH};
    /*! Define the strides between rows and columns of the matrix */
    enum {ROWSTRIDE = MatrixTraits::ROWSTRIDE,
          COLSTRIDE = MatrixTraits::COLSTRIDE};

    /*! Define properties of the main diagonal of the matrix */
    enum {DIAGONAL_LENGTH = (ROWS <= COLS) ? ROWS : COLS,
          DIAGONAL_STRIDE = ROWSTRIDE + COLSTRIDE};

    /*! The type indicating a row of this matrix accessed by (const) reference
     */
    typedef vctFixedSizeConstVectorRef<_elementType, COLS, COLSTRIDE>
        ConstRowRefType;
    /*! The type indicating a row of this matrix accessed by (non-const)
      reference */
    typedef vctFixedSizeVectorRef<_elementType, COLS, COLSTRIDE>
        RowRefType;
    /*! The type indicating a column of this matrix accessed by (const)
      reference */
    typedef vctFixedSizeConstVectorRef<_elementType, ROWS, ROWSTRIDE>
        ConstColumnRefType;
    /*! The type indicating a column of this matrix accessed by (non-const)
      reference */
    typedef vctFixedSizeVectorRef<_elementType, ROWS, ROWSTRIDE>
    ColumnRefType;
    /*! The type indicating the main diagonal of the matrix accessed
      by (const) reference */
    typedef vctFixedSizeConstVectorRef<_elementType, DIAGONAL_LENGTH, DIAGONAL_STRIDE>
    ConstDiagonalRefType;
    /*! The type indicating the main diagonal of the matrix accessed
      by (non-const) reference */
    typedef  vctFixedSizeVectorRef<_elementType, DIAGONAL_LENGTH, DIAGONAL_STRIDE>
        DiagonalRefType;

    /*! The type of vector object required to store a copy of a row of this
      matrix */
    typedef vctFixedSizeVector<_elementType, COLS> RowValueType;
    /*! The type of vector object required to store a copy of a column of this
      matrix */
    typedef vctFixedSizeVector<_elementType, ROWS> ColumnValueType;

    /*! The type of const reference to this matrix. */
    typedef vctFixedSizeConstMatrixRef<_elementType, _rows, _cols, _rowStride, _colStride> ConstRefType;

    /*! The type of reference to this matrix. */
    typedef vctFixedSizeMatrixRef<_elementType, _rows, _cols, _rowStride, _colStride> RefType;


    /*! The type of object representing this matrix accessed in transposed
      order.  Access is by (const) reference */
    typedef vctFixedSizeConstMatrixRef<_elementType, _cols, _rows,
                                       _colStride, _rowStride> ConstRefTransposeType;

    /*! The type of object representing this matrix accessed in transposed
      order.  Access is by (non-const) reference */
    typedef vctFixedSizeMatrixRef<_elementType, _cols, _rows,
                                  _colStride, _rowStride> RefTransposeType;

    /*! The type of object required to store a transposed copy of this
        matrix.  The storage order is determined by comparing the
        strides. */
    typedef vctFixedSizeMatrix<_elementType, COLS, ROWS,
                               COLSTRIDE <= ROWSTRIDE> TransposeValueType;

    /*! The type of a matrix returned by value from operations on this
        object.  The storage order is determined by comparing the
        strides. */
    typedef vctFixedSizeMatrix<_elementType, ROWS, COLS,
                               COLSTRIDE <= ROWSTRIDE> MatrixValueType;

    /*! The type of a matrix of booleans returned from operations on
      this object, e.g., ElementwiseEqual.  The storage order is
      determined by comparing the strides. */
    typedef vctFixedSizeMatrix<bool, ROWS, COLS,
                               COLSTRIDE <= ROWSTRIDE> BoolMatrixValueType;

    typedef _dataPtrType DataType;

 protected:
    /*! A declaration of the matrix-defining member object */
    DataType Data;


    /*! Check the validity of an index. */
    inline void ThrowUnlessValidIndex(size_type index) const CISST_THROW(std::out_of_range) {
        if (! ValidIndex(index)) {
            cmnThrow(std::out_of_range("vctFixedSizeMatrix: Invalid index"));
        }
    }


    /*! Check the validity of the row and column indices. */
    inline void ThrowUnlessValidIndex(size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        if (! ValidIndex(rowIndex, colIndex)) {
            cmnThrow(std::out_of_range("vctFixedSizeMatrix: Invalid indices"));
        }
    }


 public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    const_iterator begin() const {
        return const_iterator(Data, 0);
    }


    /*! Returns a const iterator on the past-the-last element (STL
      compatibility). */
    const_iterator end() const {
        return const_iterator(Data) + LENGTH;
    }


    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(Pointer(ROWS - 1, COLS - 1), 0);
    }


    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */
    const_reverse_iterator rend() const {
        return const_reverse_iterator(Data - ROWSTRIDE +
                                      COLSTRIDE * (COLS - 1), 0);
    }

    /*! Returns the size of the matrix or vector (STL
      compatibility). */
    size_type size() const {
        return LENGTH;
    }

    /*! Return a fixed size vector containing the number of rows and columns. */
    const nsize_type & sizes(void) const {
        static nsize_type staticSizes(ROWS, COLS);
        return staticSizes;
    }

    /*! Return the number of matrix rows.  Using smallcase naming following STL */
    size_type rows() const {
      return ROWS;
    }

    /*! Return the number of matrix columns.  Using smallcase naming following STL */
    size_type cols() const {
      return COLS;
    }


    /*! Returns the maximum size of the matrix or vector (STL
      compatibility).  For a fixed size matrix or vector, same as
      the size(). */
    size_type max_size() const {
        return LENGTH;
    }


    /*! Return a fixed size vector containing the row and column strides. */
    const nstride_type & strides(void) const {
        static nstride_type staticStrides(ROWSTRIDE, COLSTRIDE);
        return staticStrides;
    }

    /*! Returns the row stride. Not required by STL but provided for
      completeness. */
    difference_type row_stride() const {
        return ROWSTRIDE;
    }

    /*! Returns the column stride. Not required by STL but provided
      for completeness. */
    difference_type col_stride() const {
        return COLSTRIDE;
    }

    /*! Tell is the vector is empty (STL compatibility).  False unless
      SIZE is zero. */
    bool empty() const {
        return (LENGTH == 0);
    }

    /*! Reference a row of this matrix by index (const).
      \return a const reference to the element[index] */
    ConstRowRefType operator[](size_type index) const {
        return ConstRowRefType(Data + ROWSTRIDE * index);
    }


    /*! Returns a const pointer to an element of the container,
      specified by its index. Addition to the STL requirements.
    */
    const_pointer Pointer(size_type rowIndex, size_type colIndex) const {
        return Data + ROWSTRIDE * rowIndex + COLSTRIDE * colIndex;
    }

    /*! Returns a const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    const_pointer Pointer(void) const {
        return Data;
    }

    /*! Returns true if the index is lesser or equal to the number of
      elements of the matrix. */
    inline bool ValidIndex(size_type index) const {
        return (index < size());
    }

    /*! Returns true if both rowIndex and colIndex are valid
      indices. */
    inline bool ValidIndex(size_type rowIndex, size_type colIndex) const {
        return ((rowIndex < rows())
                && (colIndex < cols()));
    }

    /*! Returns true if rowIndex is a valid row index. */
    inline bool ValidRowIndex(size_type rowIndex) const {
        return (rowIndex < rows());
    }

    /*! Returns true if colIndex is a valid column index. */
    inline bool ValidColIndex(size_type colIndex) const {
        return (colIndex < cols());
    }


    /*! Access an element by index (const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a const reference to the index-th element (iterator order) */
    const_reference at(size_type index) const CISST_THROW(std::out_of_range) {
        ThrowUnlessValidIndex(index);
        return (begin())[index];
    }


    /*! Access an element by index (const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a const reference to the element at rowIndex, colIndex */
    const_reference at(size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        ThrowUnlessValidIndex(rowIndex, colIndex);
        return *(Pointer(rowIndex, colIndex));
    }

#ifndef SWIG
    /*! Access an element by index (const).  See method at().
      \return a const reference to element[rowIndex, colIndex] */
    const_reference operator()(size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        return at(rowIndex, colIndex);
    }
#endif


    /*! Access an element by indices (const). This method allows to
      access an element without any bounds checking.  It doesn't
      create any temporary row reference as a matrix[][] would do.

      \return a reference to the element at rowIndex, colIndex */
    const_reference Element(size_type rowIndex, size_type colIndex) const {
        return *(Pointer(rowIndex, colIndex));
    }


    ConstRowRefType Row(size_type index) const {
        return ConstRowRefType(Data + ROWSTRIDE * index);
    }

    ConstColumnRefType Column(size_type index) const {
        return ConstColumnRefType(Data + COLSTRIDE * index);
    }

    ConstDiagonalRefType Diagonal(void) const
    {
        return ConstDiagonalRefType(Data);
    }

    /*! Create a const reference to a sub matrix */
    template <vct::size_type __subRows, vct::size_type __subCols>
    vctFixedSizeConstMatrixRef<_elementType, __subRows, __subCols, _rowStride, _colStride>
    Ref(const size_type startRow = 0, const size_type startCol = 0) const CISST_THROW(std::out_of_range) {
        vctFixedSizeConstMatrixRef<_elementType, __subRows, __subCols, _rowStride, _colStride>
            result(*this, startRow, startCol);
        return result;
    }

    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the matrix. */
    //@{

    /*! Return the sum of the elements of the matrix.
      \return The sum of all the elements */
    inline value_type SumOfElements() const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the product of the elements of the matrix.
      \return The product of all the elements */
    inline value_type ProductOfElements() const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Multiplication,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the trace of the matrix, i.e. the sum of elements of
      its diagonal.
      \return The trace of the matrix.
    */
    inline value_type Trace(void) const {
        return this->Diagonal().SumOfElements();
    }

    /*! Return the square of the norm  of the matrix.
      \return The square of the norm */
    inline value_type NormSquare(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Square>::
            Run(*this);
    }

    /*! Return the norm of the matrix.
      \return The norm. */
    inline NormType Norm(void) const {
        return sqrt(NormType(NormSquare()));
    }

    /*! Return the L1 norm of the matrix, i.e. the sum of the absolute
      values of all the elements.

      \return The L1 norm. */
    inline value_type L1Norm(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the matrix, i.e. the maximum of the absolute
      values of all the elements.

      \sa MaxAbsElement

      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return this->MaxAbsElement();
    }

    /*! Return the maximum element of the matrix.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the matrix.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the maximum of the absolute
      values of all the elements.

      \sa LinfNorm.

      \return The maximum of the absolute values. */
    inline value_type MaxAbsElement(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the minimum of the absolute
      values of all the elements.

      \return The minimum of the absolute values. */
    inline value_type MinAbsElement(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Compute the minimum AND maximum elements of the matrix.
      This method is more runtime-efficient than computing them
      separately.
      \param minElement reference to store the minimum element result.
      \param maxElement reference to store the maximum element result.
      \note If this matrix is empty (null pointer) the result is undefined.
    */
    inline void MinAndMaxElement(value_type & minElement, value_type & maxElement) const
    {
        vctFixedSizeMatrixLoopEngines::MinAndMax::Run((*this), minElement, maxElement);
    }

    /*! Return true if all the elements of this vector are strictly
      positive, false otherwise */
    inline bool IsPositive(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are
      non-negative, false otherwise */
    inline bool IsNonNegative(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are
      non-positive, false otherwise */
    inline bool IsNonPositive(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are strictly
      negative, false otherwise */
    inline bool IsNegative (void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this matrix is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are finite,
      false otherwise */
    inline bool IsFinite(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsFinite>::
            Run(*this);
    }

    /*! Return true if any element of this matrix is NaN, false
      otherwise */
    inline bool HasNaN(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNaN>::
            Run(*this);
    }
    //@}


    /*! \name Storage format. */
    //@{

    /*! Test if the matrix storage order is column major.

    \return true if the row stride is lesser or equal to the column
     stride. */
    inline bool IsColMajor(void) const {
        return (row_stride() <= col_stride());
    }

    /*! Test if the matrix storage order is row major.

    \return true if the column stride is lesser or equal to the row
     stride. */
    inline bool IsRowMajor(void) const {
        return (col_stride() <= row_stride());
    }

    /*! Test if the matrix is compact, i.e. a m by n matrix actually
      uses a contiguous block of memory or size m by n. */
    inline bool IsCompact(void) const {
        return (((row_stride() == 1) && (col_stride() == static_cast<stride_type>(rows())))
                || ((col_stride() == 1) && (row_stride() == static_cast<stride_type>(cols()))));
    }

    /*! Test if the matrix is "Fortran" compatible, i.e. is compact
      and uses a column major storage order.

      \sa IsColMajor IsCompact
    */
    inline bool IsFortran(void) const {
        return (IsColMajor() && (row_stride() == 1) && (col_stride() == static_cast<stride_type>(rows())));
    }

    /*! Return the storage order, i.e. either #VCT_ROW_MAJOR or
      #VCT_COL_MAJOR. */
    inline bool StorageOrder(void) const {
        return this->IsRowMajor();
    }

    /*! Test if a matrix is square. */
    inline bool IsSquare(void) const {
        return (this->rows() == this->cols());
    }

    /*! Test if a matrix is square and of a given size. */
    inline bool IsSquare(size_type size) const {
        return ((this->rows() == size)
                && (this->cols() == size));
    }

    /*! Test if the method FastCopyOf can be used instead of Assign.
      See FastCopyOf for more details. */
    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool FastCopyCompatible(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, value_type, __dataPtrType> & source) const
    {
        return vctFastCopy::MatrixCopyCompatible(*this, source);
    }

    /*! Test if the method FastCopyOf can be used instead of Assign.
      See FastCopyOf for more details. */
    template<class __matrixOwnerType>
    inline bool FastCopyCompatible(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & source) const
    {
        return vctFastCopy::MatrixCopyCompatible(*this, source);
    }
    //@}


    /*! \name Elementwise comparisons between matrices.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two matrices of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_dataPtrType) can be different.  The comparaison
      (\f$ = \neq < \leq > \geq \f$) for Equal(), NotEqual(),
      Lesser(), LesserOrEqual(), Greater() or GreaterOrEqual() is
      performed elementwise between the two matrices.  A logical "and"
      is performed (except for NotEqual which uses a logical "or") to
      accumulate the elementwise results.  The only operators provided
      are "==" and "!=" since the semantic is not ambiguous.

      \return A boolean.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool Equal(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                      value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool operator == (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                             value_type, __dataPtrType> & otherMatrix) const {
        return Equal(otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool AlmostEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                            value_type, __dataPtrType> & otherMatrix,
                            value_type tolerance) const {
        return ((*this - otherMatrix).LinfNorm() <= tolerance);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool AlmostEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                            value_type, __dataPtrType> & otherMatrix) const {
        return ((*this - otherMatrix).LinfNorm() <= cmnTypeTraits<_elementType>::Tolerance());
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool NotEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                         value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool operator != (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                             value_type, __dataPtrType> & otherMatrix) const {
        return NotEqual(otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool Lesser(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                       value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool LesserOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                              value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool Greater(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                        value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool GreaterOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                               value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, otherMatrix);
    }

    //@}

    /*! \name Elementwise comparisons between matrices.
      Returns the matrix of comparison's results. */
    //@{
    /*! Comparison between two matrices of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_dataPtrType) can be different.  The comparaison
      (\f$ = \neq < \leq > \geq \f$) for ElementwiseEqual(),
      ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two matrices and stored in a newly created matrix.  There is no
      operator provided since the semantic would be ambiguous.

      \return A matrix of booleans.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                     value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseNotEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                        value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseLesser(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                      value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseLesserOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                             value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseGreater(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                       value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseGreaterOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                              value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherMatrix);
    }
    //@}

    /*! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar, containing the same
      type of elements.  The comparaison (\f$ = \neq < \leq > \geq
      \f$) for Equal(), NotEqual(), Lesser(), LesserOrEqual(),
      Greater() or GreaterOrEqual() is performed elementwise between
      the matrix and the scalar.  A logical "and" is performed (except
      for NotEqual which uses a logical "or") to accumulate the
      elementwise results..  The only operators provided are "==" and
      "!=" since the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator == (const value_type & scalar) const {
        return Equal(scalar);
    }

    /* documented above */
    inline bool NotEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator != (const value_type & scalar) const {
        return NotEqual(scalar);
    }

   /* documented above */
    inline bool Lesser(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, scalar);
    }

    //@}


    /*! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar, containing the same
      type of elements.  The comparaison (\f$ = \neq < \leq > \geq
      \f$) for ElementwiseEqual(), ElementwiseNotEqual(),
      ElementwiseLesser(), ElementwiseLesserOrEqual(),
      ElementwiseGreater() or ElementwiseGreaterOrEqual() is performed
      elementwise between the matrix and the scalar and stored in a
      newly created matrix.  There is no operator provided since the
      semantic would be ambiguous.

      \return A matrix of booleans.
    */
    inline BoolMatrixValueType ElementwiseEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseNotEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseLesser(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseLesserOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseGreater(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseGreaterOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, scalar);
    }

    //@}

    /*! \name Unary elementwise operations.
      Returns the result of matrix.op(). */
    //@{
    /*! Unary elementwise operations on a matrix.  For each element of
      the matrix "this", performs \f$ this[i] \leftarrow
      op(otherMatrix[i])\f$ where \f$op\f$ can calculate the absolute
      value (Abs) or the opposite (Negation).

      \return A new matrix.
    */
    inline MatrixValueType Abs(void) const;

    /* documented above */
    inline MatrixValueType Negation(void) const;

    /* documented above */
    inline MatrixValueType Floor(void) const;

    /* documented above */
    inline MatrixValueType Ceil(void) const;
    //@}


    /*! Return a transposed reference to this matrix.  The actual
      definition of this method follows the declaration of
      class vctFixedSizeConstMatrixRef.
    */
    ConstRefTransposeType TransposeRef(void) const;

    /*!  Return a transposed copy of this matrix.
    */
    TransposeValueType Transpose() const
    {
        return TransposeValueType(TransposeRef());
    }

    /*! Easy definition of a submatrix type

      This class declares a const submatrix type.
      To declare a submatrix object, here's an example.

      typedef vctFixedSizeMatrix<double, 4, 4> double4x4;
      double4x4 m;
      double4x4::ConstSubmatrix<3, 3>::Type subMatrix( m, 0, 0 );

      The submatrix strides with respect to the parent container are always 1.  The is, the
      memory strides between the elements of the parent matrix and submatrix are equal.
      For more sophisticated submatrices, the user has to write customized code.
    */
    template <size_type _subRows, size_type _subCols>
    class ConstSubmatrix
    {
    public:
        typedef vctFixedSizeConstMatrixRef<value_type, _subRows, _subCols, ROWSTRIDE, COLSTRIDE>
            Type;
    };

    /*! Return the identity matrix for the size and type of
      elements of this matrix class.  For example:
      \code
      vct4x4 m = vct4x4::Eye();
      vctInt3x3 m2 = vctInt3x3::Eye();
      typedef vctFixedSizeMatrix<float, 6, 6> MatrixType;
      MatrixType m3 = MatrixType::Eye();
      \endcode

      \note Eye is the spelling of "I".
    */
    static const MatrixValueType & Eye(void);

    /*! Return a string representation of the matrix elements */
    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    /*!  Print the matrix in a human readable format */
    void ToStream(std::ostream & outputStream) const {
        const size_type myRows = rows();
        const size_type myCols = cols();
        // preserve the formatting flags as they were
        const std::streamsize width = outputStream.width(12);
        const std::streamsize precision = outputStream.precision(6);
        bool showpoint = ((outputStream.flags() & std::ios_base::showpoint) != 0);
        outputStream << std::setprecision(6) << std::showpoint;
        size_type indexRow, indexCol;
        for (indexRow = 0; indexRow < myRows; ++indexRow) {
            for (indexCol = 0; indexCol < myCols; ++indexCol) {
                outputStream << std::setw(12) << this->Element(indexRow, indexCol);
                if (indexCol < (myCols-1)) {
                    outputStream << " ";
                }
            }
            // end of line between rows, not at the end
            if (indexRow != (myRows - 1)) {
                outputStream << std::endl;
            }
        }
        // resume the formatting flags
        outputStream << std::setprecision(precision) << std::setw(width);
        if (!showpoint) {
            outputStream << std::noshowpoint;
        }
    }

    /*! Print data only with optional separator */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const
    {
        const size_type myRows = rows();
        const size_type myCols = cols();
        size_type indexRow, indexCol;

        if (headerOnly) {
            for (indexRow = 0; indexRow < myRows; ++indexRow) {
                for (indexCol = 0; indexCol < myCols; ++indexCol) {
                    outputStream << headerPrefix << "-m" << indexRow << "_" << indexCol;
                    // delimiter between elements
                    if (indexCol < (myCols - 1)) {
                        outputStream << delimiter;
                    }
                }
                // delimiter between rows, not at the end
                if (indexRow < (myRows - 1)) {
                    outputStream << delimiter;
                }
            }
        } else {
            for (indexRow = 0; indexRow < myRows; ++indexRow) {
                for (indexCol = 0; indexCol < myCols; ++indexCol) {
                    outputStream << this->Element(indexRow, indexCol);
                    // delimiter between elements
                    if (indexCol < (myCols - 1)) {
                        outputStream << delimiter;
                    }
                }
                // delimiter between rows, not at the end
                if (indexRow < (myRows - 1)) {
                    outputStream << delimiter;
                }
            }
        }
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const
    {
        const size_type myRows = rows();
        const size_type myCols = cols();
        size_type indexRow, indexCol;

        for (indexRow = 0; indexRow < myRows; ++indexRow) {
            for (indexCol = 0; indexCol < myCols; ++indexCol) {
                cmnSerializeRaw(outputStream, this->Element(indexRow, indexCol));
            }
        }
    }

};



/*! Return true if all the elements of the matrix are nonzero, false otherwise */
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
inline bool vctAll(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix) {
    return matrix.All();
}

/*! Return true if any element of the matrix is nonzero, false otherwise */
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
inline bool vctAny(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix) {
    return matrix.Any();
}

/*! Stream out operator. */
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
std::ostream & operator << (std::ostream & output,
                            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix) {
    matrix.ToStream(output);
    return output;
}


#endif // _vctFixedSizeConstMatrixBase_h
