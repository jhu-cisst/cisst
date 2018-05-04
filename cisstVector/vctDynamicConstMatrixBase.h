/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicConstMatrixBase_h
#define _vctDynamicConstMatrixBase_h

/*!
  \file
  \brief Declaration of vctDynamicConstMatrixBase
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnSerializer.h>

#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrixLoopEngines.h>

#include <iostream>
#include <iomanip>


/* Forward declarations */
#ifndef DOXYGEN
template <class _matrixOwnerType, class __matrixOwnerType, class _elementType,
          class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareMatrix(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix1,
                                         const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix2);

template <class _matrixOwnerType, class _elementType,
          class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareScalar(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix,
                                         const _elementType & scalar);
#endif // DOXYGEN


/*!
  This class is templated with the ``matrix owner type'', which may
  be a vctDynamicMatrixOwner or a vctMatrixRefOwner.  It provides
  const operations on the dynamic matrix, such as SumOfElements etc.

  Matrix indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _matrixOwnerType the type of matrix owner.

  \param _elementType the type of elements of the matrix.
*/
template <class _matrixOwnerType, typename _elementType>
class vctDynamicConstMatrixBase
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 2};
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    /*! Type of the matrix itself. */
    typedef vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> ThisType;

    /*! Type of the data owner (dynamic array or pointer) */
    typedef _matrixOwnerType OwnerType;

    /*! Iterator on the elements of the matrix. */
    typedef typename OwnerType::iterator iterator;

    /*! Const iterator on the elements of the matrix. */
    typedef typename OwnerType::const_iterator const_iterator;

    /*! Reverse iterator on the elements of the matrix. */
    typedef typename OwnerType::reverse_iterator reverse_iterator;

    /*! Const reverse iterator on the elements of the matrix. */
    typedef typename OwnerType::const_reverse_iterator const_reverse_iterator;

    /*! The type indicating a row of this matrix accessed by (const)
      reference */
    typedef vctDynamicConstVectorRef<_elementType> ConstRowRefType;

    /*! The type indicating a row of this matrix accessed by
      (non-const) reference */
    typedef vctDynamicVectorRef<_elementType> RowRefType;

    /*! The type indicating a column of this matrix accessed by (const)
      reference */
    typedef vctDynamicConstVectorRef<_elementType> ConstColumnRefType;

    /*! The type indicating a column of this matrix accessed by
      (non-const) reference */
    typedef vctDynamicVectorRef<_elementType> ColumnRefType;

    /*! The type indicating the main diagonal of this matrix accessed
      by (const) reference */
    typedef vctDynamicConstVectorRef<_elementType> ConstDiagonalRefType;

    /*! The type indicating the main diagonal of this matrix accessed
      by (non-const) reference */
    typedef vctDynamicVectorRef<_elementType> DiagonalRefType;

    /*! The type of object representing this matrix accessed in
      transposed order.  Access is by (const) reference */
    typedef vctDynamicConstMatrixRef<_elementType> ConstRefTransposeType;

    /*! The type of object representing this matrix accessed in
      transposed order.  Access is by (non-const) reference */
    typedef vctDynamicMatrixRef<_elementType> RefTransposeType;

    /*! The type of object required to store a transposed copy of this
        matrix */
    typedef vctDynamicMatrix<_elementType> TransposeValueType;

    /*! The type of a matrix used to store by value results of the
        same type as this object. */
    typedef vctDynamicMatrix<_elementType> MatrixValueType;

    /*! The type of a matrix returned by value from operations on this
        object */
    typedef vctReturnDynamicMatrix<_elementType> MatrixReturnType;

    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Define bool based on the container type to force some
      compilers (i.e. gcc 4.0) to delay the instantiation of the
      ElementWiseCompare methods. */
    typedef typename TypeTraits::BoolType BoolType;

    /*! The type of a matrix of booleans returned from operations on this object, e.g.,
      ElementwiseEqual. */
    typedef vctReturnDynamicMatrix<BoolType> BoolMatrixReturnType;

    /*! The type of vector used to store const pointers on the rows or columns */
    typedef vctDynamicVector<const_pointer> ConstVectorPointerType;

    /*! The type of vector used to store pointers on the rows or columns */
    typedef vctDynamicVector<pointer> VectorPointerType;


protected:
    /*! Declaration of the matrix-defining member object */
    OwnerType Matrix;


    /*! Check the validity of an index. */
    inline void ThrowUnlessValidIndex(size_type index) const CISST_THROW(std::out_of_range) {
        if (! ValidIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid index"));
        }
    }


    /*! Check the validity of the row and column indices. */
    inline void ThrowUnlessValidIndex(size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        if (! ValidIndex(rowIndex, colIndex)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid indices"));
        }
    }


    /*! Throw an exception unless the row index is valid */
    inline void ThrowUnlessValidRowIndex(size_type index) const CISST_THROW(std::out_of_range) {
        if (! ValidRowIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid row index"));
        }
    }

    /*! Throw an exception unless the column index is valid */
    inline void ThrowUnlessValidColIndex(size_type index) const CISST_THROW(std::out_of_range) {
        if (! ValidColIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid column index"));
        }
    }

public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    const_iterator begin(void) const {
        return Matrix.begin();
    }

    /*! Returns a const iterator on the last element (STL
      compatibility). */
    const_iterator end(void) const {
        return Matrix.end();
    }

    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */
    const_reverse_iterator rbegin(void) const {
        return Matrix.rbegin();
    }

    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */
    const_reverse_iterator rend(void) const {
        return Matrix.rend();
    }

    /*! Return the number of elements in the matrix.  This is not
      equivalent to the difference between the end and the beginning.
    */
    size_type size(void) const {
        return Matrix.size();
    }

    /*! Return a fixed size vector containing the number of rows and columns. */
    const nsize_type & sizes(void) const {
        return Matrix.sizes();
    }

    /*! Not required by STL but provided for completeness */
    size_type rows() const {
        return Matrix.rows();
    }

    /*! Not required by STL but provided for completeness */
    size_type cols() const {
        return Matrix.cols();
    }

    /*! Not required by STL but provided for completeness */
    size_type height() const {
        return Matrix.rows();
    }

    /*! Not required by STL but provided for completeness */
    size_type width() const {
        return Matrix.cols();
    }

    /*! Return a fixed size vector containing the row and column strides. */
    const nstride_type & strides(void) const {
        return Matrix.strides();
    }

    /*! Not required by STL but provided for completeness */
    difference_type row_stride() const {
        return Matrix.row_stride();
    }

    /*! Not required by STL but provided for completeness */
    difference_type col_stride() const {
        return Matrix.col_stride();
    }

    /*! Tell is the matrix is empty (STL compatibility).  False unless
      size is zero. */
    bool empty() const {
        return (size() == 0);
    }

    /*! Reference a row of this matrix by index (const).
      \return a const reference to the element[index] */
    ConstRowRefType operator[](size_type index) const {
        return ConstRowRefType(cols(), Pointer(index, 0), col_stride());
    }

    /*! Access an element by index (const).  Compare with
      std::matrix::at().  This method can be a handy substitute for
      the overloaded operator [] when operator overloading is
      unavailable or inconvenient.  \return a const reference to
      element[index] */
    const_reference at(size_type index) const CISST_THROW(std::out_of_range) {
        ThrowUnlessValidIndex(index);
        return (begin())[index];
    }


    /*! Access the matrix owner.  This method should be used only to
        access some extra information related to the memory layout.
        It is used by the engines (vctDynamicMatrixLoopEngines). */
    const OwnerType & Owner(void) const {
        return this->Matrix;
    }


    /*! Returns a const pointer to an element of the container,
      specified by its indices. Addition to the STL requirements.
    */
    const_pointer Pointer(index_type rowIndex, index_type colIndex) const {
        return Matrix.Pointer(rowIndex, colIndex);
    }

    /*! Returns a const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    const_pointer Pointer(void) const {
        return Matrix.Pointer();
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
      \return a const reference to the element at rowIndex, colIndex */
    const_reference at(size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        ThrowUnlessValidIndex(rowIndex, colIndex);
        return *(Pointer(rowIndex, colIndex));
    }

    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    const_reference operator () (size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        return this->at(rowIndex, colIndex);
    }


    /*! Access an element by indices (const). This method allows to
      access an element without any bounds checking.  It doesn't
      create any temporary row reference as a matrix[][] would do.

      \return a reference to the element at rowIndex, colIndex */
    const_reference Element(size_type rowIndex, size_type colIndex) const {
        return *(Pointer(rowIndex, colIndex));
    }


    ConstRowRefType Row(size_type index) const CISST_THROW(std::out_of_range) {
        ThrowUnlessValidRowIndex(index);
        return ConstRowRefType(cols(), Pointer(index, 0), col_stride());
    }

    ConstColumnRefType Column(size_type index) const CISST_THROW(std::out_of_range) {
        ThrowUnlessValidColIndex(index);
        return ConstColumnRefType(rows(), Pointer(0, index), row_stride());
    }

    ConstDiagonalRefType Diagonal() const {
        return ConstDiagonalRefType( std::min(rows(), cols()), Pointer(0, 0), row_stride() + col_stride() );
    }

    /*! Resize and fill a vector of const pointers on the rows of the
      matrix.  This method is provided to ease the interfacing with C
      libraries using matrices stored as value_type**.

      To use this method, one must first create a dynamic vector of
      pointers, update it with the RowPointers method and then call
      the C function:

      \code
      vctDynamicMatrix<double> myMatrix = ...;
      vctDynamicVector<double *> rowPointers;
      myMatrix.RowPointers(rowPointers);
      c_function(rowPointers.Pointer());
      \endcode

      \note This method will throw an exception if the rows are not
      compact, i.e. if the column stride is not equal to 1.
    */
    ConstVectorPointerType & RowPointers(ConstVectorPointerType & rowPointers) const CISST_THROW(std::runtime_error) {
        if (! (this->col_stride() == 1)) {
            cmnThrow(std::runtime_error("vctDynamicMatrix: RowPointers requires compact rows"));
        }
        const size_type rows = this->rows();
        // resize the vector
        rowPointers.SetSize(rows);
        index_type index;
        for (index = 0; index < rows; ++index) {
            rowPointers[index] = this->Row(index).Pointer();
        }
        return rowPointers;
    }

    /*! Create a const reference to a sub matrix */
    vctDynamicConstMatrixRef<_elementType>
    Ref(const size_type rows, const size_type cols,
        const size_type startRow = 0, const size_type startCol = 0) const CISST_THROW(std::out_of_range) {
        if (((startRow + rows) > this->rows())
            || ((startCol + cols) > this->cols())) {
            cmnThrow(std::out_of_range("vctDynamicConstMatrixBase::Ref: reference is out of range"));
        }
        return vctDynamicConstMatrixRef<_elementType>(rows, cols,
                                                      this->row_stride(), this->col_stride(),
                                                      Pointer(startRow, startCol));
    }

    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the matrix. */
    //@{

    /*! Return the sum of the elements of the matrix.
      \return The sum of all the elements */
    inline value_type SumOfElements(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the product of the elements of the matrix.
      \return The product of all the elements */
    inline value_type ProductOfElements(void) const {
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the matrix, i.e. the maximum of the
      absolute values of all the elements.

      \sa MaxAbsElement

      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return this->MaxAbsElement();
    }

    /*! Return the maximum element of the matrix.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the matrix.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the maximum of the absolute
      values of all the elements.

      \sa LinfNorm.

      \return The maximum of the absolute values. */
    inline value_type MaxAbsElement(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the minimum of the absolute
      values of all the elements.

      \return The minimum of the absolute values. */
    inline value_type MinAbsElement(void) const {
        return vctDynamicMatrixLoopEngines::
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
        vctDynamicMatrixLoopEngines::MinAndMax::Run((*this), minElement, maxElement);
    }

    /*! Return true if all the elements of this matrix are strictly positive,
      false otherwise */
    inline bool IsPositive(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are non-negative,
      false otherwise */
    inline bool IsNonNegative(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are non-positive,
      false otherwise */
    inline bool IsNonPositive(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are strictly negative,
      false otherwise */
    inline bool IsNegative (void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this matrix is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are finite,
      false otherwise */
    inline bool IsFinite(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsFinite>::
            Run(*this);
    }

    /*! Return true if any element of this matrix is NaN, false
      otherwise */
    inline bool HasNaN(void) const {
        return vctDynamicMatrixLoopEngines::
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
        return Matrix.IsColMajor();
    }

    /*! Test if the matrix storage order is row major.

    \return true if the column stride is lesser or equal to the row
     stride. */
    inline bool IsRowMajor(void) const {
        return Matrix.IsRowMajor();
    }

    /*! Test if the matrix is compact, i.e. a m by n matrix actually
      uses a contiguous block of memory or size m by n. */
    inline bool IsCompact(void) const {
        return Matrix.IsCompact();
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
        return Matrix.StorageOrder();
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
    //@}


    /*! Test if the method FastCopyOf can be used instead of Assign.
      See FastCopyOf for more details. */
    //@{
    template <class __matrixOwnerType>
    inline bool FastCopyCompatible(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & source) const
    {
        return vctFastCopy::MatrixCopyCompatible(*this, source);
    }

    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool FastCopyCompatible(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, value_type, __dataPtrType> & source) const
    {
        return vctFastCopy::MatrixCopyCompatible(*this, source);
    }
    //@}


    /*! \name Elementwise comparisons between matrices.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two matrices of the same size, containing
      the same type of elements.  The strides can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the two
      matrices.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results.  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    template <class __matrixOwnerType>
    inline bool Equal(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool operator == (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return Equal(otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool AlmostEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix,
                            value_type tolerance) const {
        return ((*this - otherMatrix).LinfNorm() <= tolerance);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool AlmostEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return ((*this - otherMatrix).LinfNorm() <= TypeTraits::Tolerance());
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool NotEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool operator != (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return NotEqual(otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool Lesser(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool LesserOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool Greater(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool GreaterOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
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
      representation (_matrixOwnerType) can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for
      ElementwiseEqual(), ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two matrices and stored in a newly created matrix.  There is no
      operator provided since the semantic would be ambiguous.

      \return A matrix of booleans.
    */
    template <class __matrixOwnerType>
    inline BoolMatrixReturnType
    ElementwiseEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline BoolMatrixReturnType
    ElementwiseNotEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline BoolMatrixReturnType
    ElementwiseLesser(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline BoolMatrixReturnType
    ElementwiseLesserOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline BoolMatrixReturnType
    ElementwiseGreater(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline BoolMatrixReturnType
    ElementwiseGreaterOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherMatrix);
    }

    //@}


    /*! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar.  The type of the
      elements of the matrix and the scalar must be the same.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the matrix and
      the scalar.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results..  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
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
    BoolMatrixReturnType ElementwiseEqual(const value_type & scalar) const;

    /* documented above */
    BoolMatrixReturnType ElementwiseNotEqual(const value_type & scalar) const;

    /* documented above */
    BoolMatrixReturnType ElementwiseLesser(const value_type & scalar) const;

    /* documented above */
    BoolMatrixReturnType ElementwiseLesserOrEqual(const value_type & scalar) const;

    /* documented above */
    BoolMatrixReturnType ElementwiseGreater(const value_type & scalar) const;

    /* documented above */
    BoolMatrixReturnType ElementwiseGreaterOrEqual(const value_type & scalar) const;

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
    inline MatrixReturnType Abs(void) const;

    /* documented above */
    inline MatrixReturnType Negation(void) const;

    /* documented above */
    inline MatrixReturnType Floor(void) const;

    /* documented above */
    inline MatrixReturnType Ceil(void) const;
    //@}

    /*! Return a transposed reference to this matrix.  This allows access to elements
      of this matrix in transposed order.
    */
    ConstRefTransposeType TransposeRef(void) const
    {
        return ConstRefTransposeType(cols(), rows(), col_stride(), row_stride(), Pointer());
    }

    /*! Return a transposed copy of this matrix */
    inline MatrixReturnType Transpose() const
    {
        vctDynamicMatrix<value_type> result( this->TransposeRef() );
        return vctReturnDynamicMatrix<value_type>(result);
    }


    /*! Return the identity matrix for the type of elements of this
      matrix class.  The size has to be specified by the caller.  For
      example:
      \code
      vctDoubleMat m = vctDoubleMat::Eye(12);
      typedef vctDynamicMatrix<float> MatrixType;
      MatrixType m2 = MatrixType::Eye(9);
      \endcode

      \note Eye is the spelling of "I".
    */
    static MatrixReturnType Eye(size_type size);

    /*! Return a string representation of the matrix elements */
    std::string ToString(void) {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    /*!  Print the matrix to a text stream */
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

    /*! Define a ConstSubmatrix class for compatibility with the fixed size matrices.
      A const submatrix has the same stride as the parent container.

      Example:

      typedef vctDynamicMatrix<double> MatrixType;
      MatrixType M(6,6);
      MatrixType::ConstSubmatrix::Type topLeft(M, 0, 0);
      MatrixType::ConstSubmatrix::Type bottomRight(M, 3, 3);

      \note There is no straightforward way to define a fixed-size
      submatrix of a dynamic matrix, because the stride of the dynamic
      matrix is not known in compilation time.  A way to do it is:

      vctFixedSizeConstMatrixRef<double, 3, 3, 1, 6> topRight(M, 0, 3);
      vctFixedSizeConstMatrixRef<double, 3, 3, 1, 6> bottomLeft(M, 3, 0);
    */
#ifndef SWIG
    class ConstSubmatrix
    {
    public:
        typedef vctDynamicConstMatrixRef<value_type> Type;
    };
#endif // SWIG

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const
    {
        const size_type myRows = rows();
        const size_type myCols = cols();
        size_type indexRow, indexCol;

        cmnSerializeSizeRaw(outputStream, myRows);
        cmnSerializeSizeRaw(outputStream, myCols);
        for (indexRow = 0; indexRow < myRows; ++indexRow) {
            for (indexCol = 0; indexCol < myCols; ++indexCol) {
                cmnSerializeRaw(outputStream, this->Element(indexRow, indexCol));
            }
        }
    }

};

#ifndef DOXYGEN
/* documented in class.  Implementation moved here for .Net 2003 */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Equal>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseNotEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::NotEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseLesser(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Lesser>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseLesserOrEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::LesserOrEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseGreater(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Greater>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixReturnType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseGreaterOrEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::GreaterOrEqual>(*this, scalar);
}
#endif // DOXYGEN

/*! Return true if all the elements of the matrix are nonzero, false otherwise */
template <class _matrixOwnerType, typename _elementType>
inline bool vctAll(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix) {
    return matrix.All();
}

/*! Return true if any element of the matrix is nonzero, false otherwise */
template <class _matrixOwnerType, typename _elementType>
inline bool vctAny(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix) {
    return matrix.Any();
}

/*! Stream out operator. */
template <class _matrixOwnerType, typename _elementType>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix) {
    matrix.ToStream(output);
    return output;
}


// helper function declared and used in vctFixedSizeMatrixBase.h
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType, class _matrixOwnerType>
inline void vctFixedSizeMatrixBaseAssignDynamicConstMatrixBase(
    vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & fixedSizeMatrix,
    const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & dynamicMatrix)
{
    vctDynamicMatrixRef<_elementType> tempRef(fixedSizeMatrix);
    tempRef.Assign(dynamicMatrix);
}


#endif // _vctDynamicConstMatrixBase_h
