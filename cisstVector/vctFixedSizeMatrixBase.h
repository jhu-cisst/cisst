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
#ifndef _vctFixedSizeMatrixBase_h
#define _vctFixedSizeMatrixBase_h

/*!
  \file
  \brief Declaration of vctFixedSizeMatrixBase
 */

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnTypeTraits.h>
#include <cisstVector/vctFixedSizeConstMatrixBase.h>

#include <cstdarg>


// forward declaration of Assign method from dynamic matrix to fixed size
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType, class _matrixOwnerType>
inline void vctFixedSizeMatrixBaseAssignDynamicConstMatrixBase(
    vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & fixedSizeMatrix,
    const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & dynamicMatrix);



/*!
  \brief A template for a fixed size matrix with fixed spacings in
  memory.

  This class defines a matrix with read/write operations.  It
  extends vctFixedSizeConstMatrixBase with non-const methods.  See
  the base class for more documentation.

  \sa vctFixedSizeConstMatrixBase
*/
template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
class vctFixedSizeMatrixBase : public vctFixedSizeConstMatrixBase
<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>
{
 public:
    /* Declare the container-defined typed required by STL, plus the
       types completed by our traits class */

    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* documented in base class */
    typedef vctFixedSizeMatrixBase<_rows, _cols,
        _rowStride, _colStride, _elementType, _dataPtrType> ThisType;

    /*! Type of the base class. */
    typedef vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                        _elementType, _dataPtrType> BaseType;

    typedef vctFixedSizeMatrixTraits<_elementType, _rows, _cols,
                                     _rowStride, _colStride> MatrixTraits;

    typedef typename MatrixTraits::iterator iterator;
    typedef typename MatrixTraits::const_iterator const_iterator;
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;

    typedef typename BaseType::RowRefType RowRefType;
    typedef typename BaseType::ColumnRefType ColumnRefType;
    typedef typename BaseType::ConstRowRefType ConstRowRefType;
    typedef typename BaseType::ConstColumnRefType ConstColumnRefType;
    typedef typename BaseType::DiagonalRefType DiagonalRefType;
    typedef typename BaseType::ConstDiagonalRefType ConstDiagonalRefType;
    typedef typename BaseType::RefTransposeType RefTransposeType;
    typedef typename BaseType::ConstRefTransposeType ConstRefTransposeType;
    typedef typename BaseType::RowValueType RowValueType;
    typedef typename BaseType::ColumnValueType ColumnValueType;

    /*! Define the dimensions of the matrix */
    enum {ROWS = MatrixTraits::ROWS, COLS = MatrixTraits::COLS,
          LENGTH = MatrixTraits::LENGTH};
    /*! Define the strides between rows and columns of the matrix */
    enum {ROWSTRIDE = MatrixTraits::ROWSTRIDE,
          COLSTRIDE = MatrixTraits::COLSTRIDE};

 public:
    /*! Returns an iterator on the first element (STL compatibility). */
    inline iterator begin() {
        return iterator(this->Data, 0);
    }

    /* documented in base class */
    inline const_iterator begin() const {
        return BaseType::begin();
    }

    /*! Returns an iterator on the last element (STL compatibility). */
    inline iterator end() {
        return iterator(this->Data) + LENGTH;
    }

    /* documented in base class */
    inline const_iterator end() const {
        return BaseType::end();
    }

    /*! Returns a reverse iterator on the last element (STL compatibility). */
    reverse_iterator rbegin() {
        return reverse_iterator(Pointer(ROWS - 1, COLS - 1), 0);
    }

    /* documented in base class */
    const_reverse_iterator rbegin() const {
        return BaseType::rbegin();
    }

    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */
    reverse_iterator rend() {
        return reverse_iterator(this->Data - ROWSTRIDE +
                                COLSTRIDE * (COLS - 1), 0);
    }

    /* documented in base class */
    const_reverse_iterator rend() const {
        return BaseType::rend();
    }


    /*! Reference a row of this matrix by index (non-const).
      \return a const reference to the element[index] */
    RowRefType operator[](size_type index) {
        return RowRefType(this->Data + ROWSTRIDE * index);
    }

    /* documented in base class */
    ConstRowRefType operator[](size_type index) const {
        return BaseType::operator[](index);
    }


    /*! Returns a non const pointer to an element of the container,
      specified by its index. Addition to the STL requirements.
    */
    pointer Pointer(size_type rowIndex, size_type colIndex) {
        return this->Data + ROWSTRIDE * rowIndex + COLSTRIDE * colIndex;
    }

    /*! Returns a non const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    pointer Pointer(void) {
        return this->Data;
    }

    /* documented in base class */
    const_pointer Pointer(size_type rowIndex, size_type colIndex) const {
        return BaseType::Pointer(rowIndex, colIndex);
    }

    /* documented in base class */
    const_pointer Pointer(void) const {
        return BaseType::Pointer();
    }


    /*! Access an element by index (non-const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a reference to the index-th element (iterator order) */
    reference at(size_type index) CISST_THROW(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return (begin())[index];
    }

    /* documented in base class */
    const_reference at(size_type index) const CISST_THROW(std::out_of_range) {
        return BaseType::at(index);
    }


    /*! Access an element by index.  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a reference to the element at rowIndex, colIndex */
    reference at(size_type rowIndex, size_type colIndex) CISST_THROW(std::out_of_range) {
        this->ThrowUnlessValidIndex(rowIndex, colIndex);
        return *(Pointer(rowIndex, colIndex));
    }

    /* documented in base class */
    const_reference at(size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        return BaseType::at(rowIndex, colIndex);
    }

#ifndef SWIG
    /*! Access an element by index.  See method at().
      \return a const reference to element[rowIndex, colIndex] */
    reference operator()(size_type rowIndex, size_type colIndex) CISST_THROW(std::out_of_range) {
        return at(rowIndex, colIndex);
    }

    /* documented in base class */
    const_reference operator()(size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        return BaseType::operator()(rowIndex, colIndex);
    }
#endif


    /*! Access an element by indices (non const). This method allows
      to access an element without any bounds checking.  It doesn't
      create any temporary row reference as a matrix[][] would do.

      \return a reference to the element at rowIndex, colIndex */
    reference Element(size_type rowIndex, size_type colIndex) {
        return *(Pointer(rowIndex, colIndex));
    }

    /* documented in base class */
    const_reference Element(size_type rowIndex, size_type colIndex) const {
        return BaseType::Element(rowIndex, colIndex);
    }


    /*! \name Row, column, and main diagonal references. */
    //@{
    /*! Create a row reference. */
    RowRefType Row(size_type index) {
        return RowRefType(this->Data + ROWSTRIDE * index);
    }

    /*! Create a column reference. */
    ColumnRefType Column(size_type index) {
        return ColumnRefType(this->Data + COLSTRIDE * index);
    }

    /*! Create a reference to the main diagonal */
    DiagonalRefType Diagonal(void) {
        return DiagonalRefType(this->Data);
    }

    /* documented in base class */
    ConstRowRefType Row(size_type index) const {
        return BaseType::Row(index);
    }

    /* documented in base class */
    ConstColumnRefType Column(size_type index) const {
        return BaseType::Column(index);
    }

    /* documented in base class */
    ConstDiagonalRefType Diagonal(void) const {
        return BaseType::Diagonal();
    }
    //@}

    /*! Create a reference to a sub matrix */
    //@{
    template <vct::size_type __subRows, vct::size_type __subCols>
    vctFixedSizeMatrixRef<_elementType, __subRows, __subCols, _rowStride, _colStride>
    Ref(const size_type startRow = 0, const size_type startCol = 0) CISST_THROW(std::out_of_range) {
        vctFixedSizeMatrixRef<_elementType, __subRows, __subCols, _rowStride, _colStride>
            result(*this, startRow, startCol);
        return result;
    }

    template <vct::size_type __subRows, vct::size_type __subCols>
    vctFixedSizeConstMatrixRef<_elementType, __subRows, __subCols, _rowStride, _colStride>
    Ref(const size_type startRow = 0, const size_type startCol = 0) const CISST_THROW(std::out_of_range) {
        return BaseType::Ref(startRow, startCol);
    }
    //@}

    //@{ Methods to change the order of rows and columns of a matrix
    /*! Exchange two rows of the matrix */
    void ExchangeRows(const size_type row1Index, const size_type row2Index) {
        RowRefType row1( Row(row1Index) );
        RowRefType row2( Row(row2Index) );
        row1.SwapElementsWith(row2);
    }

    /*! Exchange two colums of the matrix */
    void ExchangeColumns(const size_type col1Index, const size_type col2Index) {
        ColumnRefType col1( Column(col1Index) );
        ColumnRefType col2( Column(col2Index) );
        col1.SwapElementsWith(col2);
    }

    //@{ Methods to select a subset of rows or columns from another matrix
    /*! Select a subset of rows from another matrix */
    template <size_type __rows, stride_type __rowStride, stride_type __colStride, class __dataPtrType,
              stride_type __indexStride, class __indexDataPtrType>
    void SelectRowsFrom(const vctFixedSizeConstMatrixBase<__rows, _cols, __rowStride, __colStride, _elementType, __dataPtrType> & inputMatrix,
                        const vctFixedSizeConstVectorBase<_rows, __indexStride, index_type, __indexDataPtrType> & rowIndexVector)
    {
        vctFixedSizeMatrixLoopEngines::SelectRowsByIndex::
            Run(*this, inputMatrix, rowIndexVector);
    }

    /*! Select a subset of columns from another matrix */
    template <size_type __cols, stride_type __rowStride, stride_type __colStride, class __dataPtrType,
              stride_type __indexStride, class __indexDataPtrType>
    void SelectColsFrom(const vctFixedSizeConstMatrixBase<_rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & inputMatrix,
                        const vctFixedSizeConstVectorBase<_cols, __indexStride, index_type, __indexDataPtrType> & colIndexVector)
    {
        this->TransposeRef().SelectRowsFrom(inputMatrix.TransposeRef(), colIndexVector);
    }
    //@}

    /*! Assign a permutation of the rows of the input matrix to the rows of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedRowIndexes an array of row indices.  The assignment performed is:
      this->Row(i) <-- inputMatrix.Row( permutedRowIndexes[i] ).

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(ROWS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    void RowPermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                          _elementType, __dataPtrType> & inputMatrix, const index_type permutedRowIndexes[])
    {
        index_type thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < ROWS; ++thisRowIndex) {
            Row(thisRowIndex).Assign( inputMatrix.Row(permutedRowIndexes[thisRowIndex]) );
        }
    }

    /*! Assign a permutation of the rows of the input matrix to the rows of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedRowIndexes an array of row indices.  The assignment performed is:
      this->Row( permutedRowIndexes[i] ) <-- inputMatrix.Row(i).

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(ROWS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    void RowInversePermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                                 _elementType, __dataPtrType> & inputMatrix, const index_type permutedRowIndexes[])
    {
        index_type thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < ROWS; ++thisRowIndex) {
            Row(permutedRowIndexes[thisRowIndex]).Assign( inputMatrix.Row(thisRowIndex) );
        }
    }

    /*! Assign a permutation of the columns of the input matrix to the column of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedColumnIndexes an array of column indices.  The assignment performed is:
      this->Column(i) <-- inputMatrix.Column( permutedColumnIndexes[i] ).

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(COLS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    void ColumnPermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                             _elementType, __dataPtrType> & inputMatrix, const index_type permutedColumnIndexes[])
    {
        index_type thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < COLS; ++thisColumnIndex) {
            Column(thisColumnIndex).Assign( inputMatrix.Column(permutedColumnIndexes[thisColumnIndex]) );
        }
    }

    /*! Assign a permutation of the columns of the input matrix to the column of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedColumnIndexes an array of column indices.  The assignment performed is:
      this->Column( permutedColumnIndexes[i] ) <-- inputMatrix.Column(i).

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(COLS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    void ColumnInversePermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                                    _elementType, __dataPtrType> & inputMatrix, const index_type permutedColumnIndexes[])
    {
        index_type thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < COLS; ++thisColumnIndex) {
            Column(permutedColumnIndexes[thisColumnIndex]).Assign( inputMatrix.Column(thisColumnIndex) );
        }
    }
    //@}


    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the matrix
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type value) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::SecondOperand>::
            Run(*this, value);
        return value;
    }


    /*! Assign zero to all elements.  This methods assumes that the
      element type has a zero and this zero value can be set using
      memset(0).  If the matrix is not compact this method will use
      SetAll(0) and memset otherwise.  This provides a slightly more
      efficent way to set all elements to zero.

      \return true if the matrix is compact and memset was used, false
      otherwise. */
    inline bool Zeros(void) {
        if (this->IsCompact()) {
            memset(this->Pointer(), 0, this->size() * sizeof(value_type));
            return true;
        } else if (this->col_stride() == 1) {
            /* memset row by row */
            const size_type sizeOfRow = this->cols() * sizeof(value_type);
            const stride_type rowStride = this->row_stride();
            pointer currentPointer = this->Pointer();
            const pointer endPointer = currentPointer + this->rows() * rowStride;
            for (;
                 currentPointer != endPointer;
                 currentPointer += rowStride) {
                memset(currentPointer, 0, sizeOfRow);
            }
            return true;
        } else if (this->row_stride() == 1) {
            /* memset col by col */
            const size_type sizeOfCol = this->rows() * sizeof(value_type);
            const stride_type colStride = this->col_stride();
            pointer currentPointer = this->Pointer();
            const pointer endPointer = currentPointer + this->cols() * colStride;
            for (;
                 currentPointer != endPointer;
                 currentPointer += colStride) {
                memset(currentPointer, 0, sizeOfCol);
            }
            return true;
        } else {
            this->SetAll(static_cast<value_type>(0));
            return false;
        }
    }


    /*! Assignment operation between matrices of different types
    */
    template <stride_type __rowStride, stride_type __colStride, class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, __elementType, __dataPtrType> & other) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(*this, other);
        return *this;
    }


    /*! Assign to this matrix a set of values provided as independent
      arguments, by using cstdarg macros, that is, an unspecified
      number of arguments.  This function is not using a recursive
      engine, as it may be hard and not worthwhile to use a recursive
      engine with the va_arg ``iterator''.  This operation assumes
      that all the arguments are of type value_type, and that their
      number is equal to the size of the matrix.  The arguments are
      passed by \em value.  The user may need to explicitly cast
      the parameters to value_type to avoid runtime bugs and errors.
      The order of the paramaters is row first which allows to keep
      the code pretty intuitive:
      \code
      matrix.Assign( 0.0, 1.0,
                    -1.0, 0.0);
      \endcode

      \return a reference to this matrix.
    */
    inline ThisType & Assign(const value_type element0, ...)
    {
        iterator iter = begin();
        (*iter) = element0;
        ++iter;
        va_list nextArg;
        va_start(nextArg, element0);
        for (; iter != end(); ++iter) {
            (*iter) = static_cast<value_type>(va_arg(nextArg, typename cmnTypeTraits<value_type>::VaArgPromotion));
        }
        va_end(nextArg);
        return *this;
    }


    /*! Assign to this matrix values from a C array given as a
      pointer to value_type.  The purpose of this method is to simplify
      the syntax by not necessitating the creation of an explicit matrix
      for the given array.  However, we only provide this method for
      an array of value_type.  For arrays of other types a matrix
      still needs to be declared.

      \param elements a pointer to a C array of elements to be assigned to
      this matrix.
      \param inputIsRowMajor a flag indicating the storage order of the elements
      in the input array.

      \note For lack of better knowledge, this method assumes that the input
      array is _packed_, that is, that all the elements are stored in a
      contiguous memory block with no gaps.  The only existing options now relate
      to the storage order of the input elements.  If the input is not packed,
      you should create a MatrixRef explicitly, with stride specifications,
      or use other tricks.

      \return a reference to this object.
    */
    inline ThisType & Assign(const value_type * elements, bool inputIsRowMajor = true)
    {
        if (inputIsRowMajor)
        {
            // row-major input means that the row-stride is equal to the number of elements
            // in a row, i.e, COLS, and the column-stride is 1.
            const vctFixedSizeConstMatrixRef<value_type, ROWS, COLS, COLS, 1> tmpRef(elements);
            this->Assign(tmpRef);
        }
        else
        {
            // column-major input means that the column-stride is equal to the number of
            // elements in a column, i.e., ROWS, and the row-stride is 1.
            const vctFixedSizeConstMatrixRef<value_type, ROWS, COLS, 1, ROWS> tmpRef(elements);
            this->Assign(tmpRef);
        }
        return *this;
    }


    /*! Assign to this matrix values from a dynamic matrix.  This
      method doesn't allow type conversion.

      \return a reference to this object.
    */
    template <class __matrixOwnerType>
    inline ThisType & Assign(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & other) {
        vctFixedSizeMatrixBaseAssignDynamicConstMatrixBase(*this, other);
        return *this;
    }


    /*!  \name Forced assignment operation between matrices of
      different types.  On fixed size matrices this method is
      equivalent to Assign.  See notes below!

      \note For a non-reallocating Assign, it is recommended to use
      the Assign() methods.

      \note This method is provided for both fixed size and dynamic
      matrices for API consistency (usable in templated code).  There
      is obviously not resize involved on fixed size matrices.

      \note If the destination matrix doesn't have the same size as
      the source and can not be resized, an exception will be thrown
      by the Assign method called internally.

      \param other The matrix to be copied.
    */
    //@{
    template <stride_type __rowStride, stride_type __colStride, class __elementType, class __dataPtrType>
    inline ThisType & ForceAssign(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                                                                    __elementType, __dataPtrType> & other) {
        return this->Assign(other);
    }

    template <class __matrixOwnerType>
    inline ThisType & ForceAssign(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & other) {
        return this->Assign(other);
   }
    //@}


    /*! Fast copy.  This method uses <code>memcpy</code> whenever it
        is possible to perform a fast copy from another matrix to this
        matrix.

        - The method will first verify that the source and destination
          have the same size (rows and columns) and throws an
          exception otherwise (<code>std::runtime_error</code>).  See
          ::cmnThrow for details.

        - If any of the two matrices is not compact or if the two
          matrices use a different storage order, this method will
          return <code>false</code>.  If both matrices are compact and
          use the same storage order, a <code>memcpy</code> is
          performed and the method returns <code>true</code>.

        - If the two matrices are compact by row or column, this
          method will use multiple calls to <code>memcpy</code>,
          i.e. one per row or column and the method returns
          <code>true</code>

        - To avoid the tests above, one can set the parameter
          <code>performSafetyChecks</code> to
          <code>vctFastCopy::SkipChecks</code> (boolean defined
          <code>false</code>).  This should be used only when the
          programmer knows for sure that the source and destination
          are compatible (size, storage order and compactness).

        - As opposed to Assign, this method doesn't perform any type
          conversion.

        - Since no constructor is called for the contained elements,
          this function performs a "shallow copy".  If the contained
          objects have a pointer as data member, the copied object
          will carry on the same pointer (hence pointing at the same
          memory block which could easily lead to bugs).

        The basic and safe use of this method for a matrix would be:
        \code
        if (!destination.FastCopyOf(source)) {
            destination.Assign(source);
        }
        \endcode

        If the method is to be called many times (in a loop for
        example), it is recommended to check that the source and
        destination are compatible once and then use the option
        to turn off the different safety checks for each FastCopyOf.
        \code
        bool canUseFastCopy = destination.FastCopyCompatible(source);
        vct::index_type index;
        for (index = 0; index < 1000; index++) {
            DoSomethingUseful(source);
            if (canUseFastCopy) {
                destination.FastCopyOf(source, vctFastCopy::SkipChecks); // Do not check again
            } else {
                destination.Assign(source);
            }
        }
        \endcode

        \param source Matrix used to set the content of this matrix.

        \param performSafetyChecks Flag set to <code>false</code> to
        avoid safety checks, use with extreme caution.  To make your
        code more readable use <code>vctFastCopy::SkipChecks</code> or
        <code>vctFastCopy::PerformChecks</code>.
     */
    //@{
    template <class __matrixOwnerType>
    inline bool FastCopyOf(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & source,
                           bool performSafetyChecks = vctFastCopy::PerformChecks)
        CISST_THROW(std::runtime_error)
    {
        return vctFastCopy::MatrixCopy(*this, source, performSafetyChecks);
    }

    template <class __dataPtrType>
    inline bool FastCopyOf(const vctFixedSizeConstMatrixBase<ROWS, COLS, ROWSTRIDE, COLSTRIDE, value_type, __dataPtrType> & source,
                           bool performSafetyChecks = vctFastCopy::PerformChecks)
        CISST_THROW(std::runtime_error)
    {
        return vctFastCopy::MatrixCopy(*this, source, performSafetyChecks);
    }
    //@}



    /*! Return a transposed reference to this matrix.  The actual
      definition of this method follows the declaration of
      class vctFixedSizeMatrixRef.
    */
    RefTransposeType TransposeRef(void);

    /* documented in base class */
    ConstRefTransposeType TransposeRef(void) const {
        return BaseType::TransposeRef();
    }


    /*! \name Binary elementwise operations between two matrices.
      Store the result of op(matrix1, matrix2) to a third matrix. */
    //@{
    /*! Binary elementwise operations between two matrices.  For each
      element of the matrices, performs \f$ this[i] \leftarrow
      op(matrix1[i], matrix2[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ElementwiseProductOf), a division (ElementwiseRatioOf), a
      minimum (ElementwiseMinOf) or a maximum (ElementwiseMaxOf).

      \param input1Matrix The first operand of the binary operation

      \param input2Matrix The second operand of the binary operation

      \return The vector "this" modified.
    */
    template <stride_type __input1RowStride, stride_type __input1ColStride, class __input1DataPtrType,
              stride_type __input2RowStride, stride_type __input2ColStride, class __input2DataPtrType>
    inline ThisType & SumOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                            value_type, __input1DataPtrType> & input1Matrix,
                            const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                            value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Addition>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __input1RowStride, stride_type __input1ColStride, class __input1DataPtrType,
              stride_type __input2RowStride, stride_type __input2ColStride, class __input2DataPtrType>
    inline ThisType & DifferenceOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                   value_type, __input1DataPtrType> & input1Matrix,
                                   const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                   value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Subtraction>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __input1RowStride, stride_type __input1ColStride, class __input1DataPtrType,
              stride_type __input2RowStride, stride_type __input2ColStride, class __input2DataPtrType>
    inline ThisType & ElementwiseProductOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                           value_type, __input1DataPtrType> & input1Matrix,
                                           const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                           value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Multiplication>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __input1RowStride, stride_type __input1ColStride, class __input1DataPtrType,
              stride_type __input2RowStride, stride_type __input2ColStride, class __input2DataPtrType>
    inline ThisType & ElementwiseRatioOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                         value_type, __input1DataPtrType> & input1Matrix,
                                         const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                         value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Division>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __input1RowStride, stride_type __input1ColStride, class __input1DataPtrType,
              stride_type __input2RowStride, stride_type __input2ColStride, class __input2DataPtrType>
    inline ThisType & ElementwiseMinOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                       value_type, __input1DataPtrType> & input1Matrix,
                                       const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                       value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Minimum>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __input1RowStride, stride_type __input1ColStride, class __input1DataPtrType,
              stride_type __input2RowStride, stride_type __input2ColStride, class __input2DataPtrType>
    inline ThisType & ElementwiseMaxOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                       value_type, __input1DataPtrType> & input1Matrix,
                                       const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                       value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Maximum>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between two matrices.
      Store the result of op(this, otherMatrix) back to this matrix. */
    //@{
    /*! Store back binary elementwise operations between two matrices.
      For each element of the matrices, performs \f$ this[i]
      \leftarrow op(this[i], otherMatrix[i])\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtraction), a
      multiplication (ElementwiseMultiply) or a division
      (ElementwiseDivide), a minimization (ElementwiseMin) or a
      maximisation (ElementwiseMax).

      \param otherMatrix The second operand of the binary operation
      (this[i] is the first operand)

      \return The vector "this" modified.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & Add(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Addition>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & Subtract(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Subtraction>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ElementwiseMultiply(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Multiplication>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ElementwiseDivide(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Division>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ElementwiseMin(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Minimum>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ElementwiseMax(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Maximum>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & operator += (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        return this->Add(otherMatrix);
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & operator -= (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        return this->Subtract(otherMatrix);
    }
    //@}



    /*! \name Binary elementwise operations a matrix and a scalar.
      Store the result of op(matrix, scalar) to a third matrix. */
    //@{
    /*! Binary elementwise operations between a matrix and a scalar.
      For each element of the matrix "this", performs \f$ this[i]
      \leftarrow op(matrix[i], scalar)\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf), a division (RatioOf), a minimum (ClippedAboveOf) or
      a maximum (ClippedBelowOf).

      \param matrix The first operand of the binary operation.
      \param scalar The second operand of the binary operation.

      \return The matrix "this" modified.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & SumOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix,
                            const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Addition>::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & DifferenceOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix,
                                   const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Subtraction>::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ProductOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix,
                                const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Multiplication>::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & RatioOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix,
                              const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Division>::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ClippedAboveOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix,
                                     const value_type upperBound) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, matrix, upperBound);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ClippedBelowOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix,
                                     const value_type lowerBound) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Maximum>::
            Run(*this, matrix, lowerBound);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations a scalar and a matrix.
      Store the result of op(scalar, matrix) to a third matrix. */
    //@{
    /*! Binary elementwise operations between a scalar and a matrix.
      For each element of the matrix "this", performs \f$ this[i]
      \leftarrow op(scalar, matrix[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf), a division (RatioOf), a minimum (ClippedAboveOf) or
      a maximum (ClippedBelowOf).

      \param scalar The first operand of the binary operation.
      \param matrix The second operand of the binary operation.

      \return The matrix "this" modified.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & SumOf(const value_type scalar,
                            const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Addition>::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & DifferenceOf(const value_type scalar,
                                   const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Subtraction>::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ProductOf(const value_type scalar,
                                const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Multiplication>::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & RatioOf(const value_type scalar,
                              const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Division>::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ClippedAboveOf(const value_type upperBound,
                                     const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, upperBound, matrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & ClippedBelowOf(const value_type lowerBound,
                                     const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Maximum>::
            Run(*this, lowerBound, matrix);
        return *this;
    }
    //@}

    /*! \name Binary elementwise operations between a matrix and a scalar.
      Store the result of op(this, scalar) back to this matrix. */
    //@{
    /*! Store back binary elementwise operations between a matrix and
      a scalar.  For each element of the matrix "this", performs \f$
      this[i] \leftarrow op(this[i], scalar)\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtract), a
      multiplication (Multiply), a division (Divide), a minimum
      (ClipAbove) or a maximum (ClipBelow).

      \param scalar The second operand of the binary operation
      (this[i] is the first operand.

      \return The matrix "this" modified.
    */
    inline ThisType & Add(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Addition>::
            Run(*this, scalar);
      return *this;
    }

    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Subtraction>::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Multiplication>::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Division>::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipAbove(const value_type upperBound) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Minimum>::
            Run(*this, upperBound);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipBelow(const value_type lowerBound) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Maximum>::
            Run(*this, lowerBound);
        return *this;
    }

    /* documented above */
    inline ThisType & operator += (const value_type scalar) {
        return this->Add(scalar);
    }

    /* documented above */
    inline ThisType & operator -= (const value_type scalar) {
        return this->Subtract(scalar);
    }

    /* documented above */
    inline ThisType & operator *= (const value_type scalar) {
        return this->Multiply(scalar);
    }

    /* documented above */
    inline ThisType & operator /= (const value_type scalar) {
        return this->Divide(scalar);
    }
    //@}


    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & AddProductOf(const value_type scalar,
                                   const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix)
    {
        vctFixedSizeMatrixLoopEngines::
            MioSiMi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar,  otherMatrix);
        return *this;
    }


    template <stride_type __rowStride1, stride_type __colStride1, class __dataPtrType1,
              stride_type __rowStride2, stride_type __colStride2, class __dataPtrType2>
    inline ThisType & AddElementwiseProductOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride1, __colStride1, value_type, __dataPtrType1> & matrix1,
                                              const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride2, __colStride2, value_type, __dataPtrType2> & matrix2)
    {
        vctFixedSizeMatrixLoopEngines::
            MioMiMi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, matrix1,  matrix2);
        return *this;
    }


    /*! \name Unary elementwise operations.
      Store the result of op(matrix) to another matrix. */
    //@{
    /*! Unary elementwise operations on a matrix.  For each element of
      the matrix "this", performs \f$ this[i] \leftarrow
      op(otherMatrix[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsOf), the opposite (NegationOf) or the transpose
      (TransposeOf).

      \param otherMatrix The operand of the unary operation.

      \return The matrix "this" modified.
    */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & AbsOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & NegationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Negation>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & FloorOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Floor>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & CeilOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Ceil>::
            Run(*this, otherMatrix);
        return *this;
    }
    //@}



    template <stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline ThisType & TransposeOf(const vctFixedSizeConstMatrixBase<_cols, _rows, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        Assign(otherMatrix.TransposeRef());
        return *this;
    }



    /*! \name Store back unary elementwise operations.
      Store the result of op(this) to this matrix. */
    //@{
    /*! Unary elementwise operations on a matrix.  For each element of
      the matrix "this", performs \f$ this[i] \leftarrow
      op(this[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsSelf) or the opposite (NegationSelf).

      \return The matrix "this" modified.
    */
    inline ThisType & AbsSelf(void) {
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeAbs>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & NegationSelf(void) {
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeNegation>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & FloorSelf(void) {
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeFloor>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & CeilSelf(void) {
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeCeil>::
            Run(*this);
        return *this;
    }
    //@}


    /*! Product of two matrices.  The template parameters insure that
      the size of the matrices match.

    \param input1Matrix The left operand of the binary operation.

    \param input2Matrix The right operand of the binary operation.

    \return The matrix "this" modified. */
    template <size_type __input1Cols, stride_type __input1RowStride, stride_type __input1ColStride, class __input1DataPtrType,
              stride_type __input2RowStride, stride_type __input2ColStride, class __input2DataPtrType>
        void ProductOf(const vctFixedSizeConstMatrixBase<_rows, __input1Cols, __input1RowStride, __input1ColStride,
                       _elementType, __input1DataPtrType> & input1Matrix,
                       const vctFixedSizeConstMatrixBase<__input1Cols, _cols, __input2RowStride, __input2ColStride,
                       _elementType, __input2DataPtrType> & input2Matrix) {
        typedef vctFixedSizeConstMatrixBase<_rows, __input1Cols, __input1RowStride, __input1ColStride,
            _elementType, __input1DataPtrType> Input1MatrixType;
        typedef vctFixedSizeConstMatrixBase<__input1Cols, _cols, __input2RowStride, __input2ColStride,
            _elementType, __input2DataPtrType> Input2MatrixType;
        typedef typename Input1MatrixType::ConstRowRefType Input1RowRefType;
        typedef typename Input2MatrixType::ConstColumnRefType Input2ColumnRefType;
        vctFixedSizeMatrixLoopEngines::
            Product<typename vctBinaryOperations<value_type, Input1RowRefType, Input2ColumnRefType>::DotProduct>::
            Run((*this), input1Matrix, input2Matrix);
    }

    /*! Compute the outer product of two vectors and store the result to
      this matrix.  The outer product (v1*v2)[i,j] = v1[i] * v2[j] */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
        void OuterProductOf(const vctFixedSizeConstVectorBase<_rows, __stride1, _elementType, __dataPtrType1> & columnVector,
        const vctFixedSizeConstVectorBase<_cols, __stride2, _elementType, __dataPtrType2> & rowVector)
    {
        typedef vctFixedSizeConstVectorBase<_rows, __stride1, _elementType, __dataPtrType1> InputColumnType;
        typedef vctFixedSizeConstVectorBase<_cols, __stride2, _elementType, __dataPtrType2> InputRowType;
        typedef typename InputColumnType::ColConstMatrixRefType ColMatrixType;
        typedef typename InputRowType::RowConstMatrixRefType RowMatrixType;
        const ColMatrixType colMatrix = columnVector.AsColMatrix();
        const RowMatrixType rowMatrix = rowVector.AsRowMatrix();
        this->ProductOf(colMatrix, rowMatrix);
    }


    /*! Easy definition of a submatrix type

      This class declares a non-const submatrix type.
      To declare a submatrix object, here's an example.

      typedef vctFixedSizeMatrix<double, 4, 4> double4x4;
      double4x4 m;
      double4x4::ConstSubmatrix<3, 3>::Type subMatrix( m, 0, 0 );

      The submatrix strides with respect to the parent container are always 1.  The is, the
      memory strides between the elements of the parent matrix and submatrix are equal.
      For more sophisticated submatrices, the user has to write customized code.
    */
    template <size_type _subRows, size_type _subCols>
    class Submatrix
    {
    public:
        typedef vctFixedSizeMatrixRef<value_type, _subRows, _subCols, ROWSTRIDE, COLSTRIDE>
            Type;
    };

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. */
    bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ')
    {
        const size_type myRows = this->rows();
        const size_type myCols = this->cols();
        size_type indexRow, indexCol;
        char c;
        bool valid = true;
        ThisType temp;
        for (indexRow = 0; (indexRow < myRows) && valid; ++indexRow) {
            for (indexCol = 0; (indexCol < myCols) && valid; ++indexCol) {
                inputStream >> temp.Element(indexRow, indexCol);
                if (inputStream.fail()) {
                    valid = false;
                    inputStream.clear();
                }
                // Look for the delimiter
                if (valid && !isspace(delimiter) && (indexCol < myCols-1)) {
                    inputStream >> c;
                    if (c != delimiter)
                        valid = false;
                }
            } // end for cols
            // Look for the delimiter
            if (valid && !isspace(delimiter) && (indexRow < myRows-1)) {
                inputStream >> c;
                if (c != delimiter)
                    valid = false;
            }
        } // end for rows
        if (valid) {
            // Only update the object if the parsing was successful for all elements.
            Assign(temp);
        }
        return valid;
    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream)
    {
        const size_type myRows = this->rows();
        const size_type myCols = this->cols();
        size_type indexRow, indexCol;

        for (indexRow = 0; indexRow < myRows; ++indexRow) {
            for (indexCol = 0; indexCol < myCols; ++indexCol) {
                cmnDeSerializeRaw(inputStream, this->Element(indexRow, indexCol));
            }
        }
    }

};


#endif // _vctFixedSizeMatrixBase_h
