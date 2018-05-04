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
#ifndef _vctDynamicMatrixBase_h
#define _vctDynamicMatrixBase_h

/*!
  \file
  \brief Declaration of vctDynamicMatrixBase
*/

#include <cstdarg>
#include <cisstVector/vctDynamicConstMatrixBase.h>
#include <cisstVector/vctStoreBackUnaryOperations.h>
#include <cisstVector/vctStoreBackBinaryOperations.h>

/*!
  This class provides all the const methods inherited from
  vctConstMatrixBase, and extends them with non-const methods, such
  as SumOf.

  \sa vctDynamicConstMatrixBase

*/
template <class _matrixOwnerType, typename _elementType>
class vctDynamicMatrixBase : public vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    typedef vctDynamicMatrixBase ThisType;
    /*! Type of the base class. */
    typedef vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> BaseType;

    typedef _matrixOwnerType OwnerType;

    typedef typename BaseType::iterator iterator;
    typedef typename BaseType::reverse_iterator reverse_iterator;
    typedef typename BaseType::const_iterator const_iterator;
    typedef typename BaseType::const_reverse_iterator const_reverse_iterator;

    typedef typename BaseType::ConstRowRefType ConstRowRefType;
    typedef typename BaseType::RowRefType RowRefType;

    typedef typename BaseType::ConstColumnRefType ConstColumnRefType;
    typedef typename BaseType::ColumnRefType ColumnRefType;

    typedef typename BaseType::ConstDiagonalRefType ConstDiagonalRefType;
    typedef typename BaseType::DiagonalRefType DiagonalRefType;

    typedef typename BaseType::ConstRefTransposeType ConstRefTransposeType;
    typedef typename BaseType::RefTransposeType RefTransposeType;

    typedef typename BaseType::ConstVectorPointerType ConstVectorPointerType;
    typedef typename BaseType::VectorPointerType VectorPointerType;


    /*! Returns an iterator on the first element (STL
      compatibility). */
    iterator begin(void) {
        return this->Matrix.begin();
    }

    /*! Returns an iterator on the last element (STL
      compatibility). */
    iterator end(void) {
        return this->Matrix.end();
    }

    /*! Returns a reverse iterator on the last element (STL
      compatibility). */
    reverse_iterator rbegin(void) {
        return this->Matrix.rbegin();
    }

    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */
    reverse_iterator rend(void) {
        return this->Matrix.rend();
    }

    /* documented in base class */
    const_iterator begin(void) const {
        return BaseType::begin();
    }

    /* documented in base class */
    const_iterator end(void) const {
        return BaseType::end();
    }

    /* documented in base class */
    const_reverse_iterator rbegin(void) const {
        return BaseType::rbegin();
    }

    /* documented in base class */
    const_reverse_iterator rend(void) const {
        return BaseType::rend();
    }

    /*! Reference a row of this matrix by index.
      \return a reference to the element[index] */
    RowRefType operator[](size_type index) {
        return RowRefType(this->cols(), Pointer(index, 0), this->col_stride());
    }

    /* documented in base class */
    ConstRowRefType operator[](size_type index) const {
        return BaseType::operator[](index);
    }


    /* documented in base class */
    const OwnerType & Owner(void) const {
        return BaseType::Owner();
    }
    OwnerType & Owner(void) {
        return this->Matrix;
    }


    /*! Return a non const pointer to an element of the container,
      specified by its indices. Addition to the STL requirements.
    */
    pointer Pointer(size_type rowIndex, size_type colIndex) {
        return this->Matrix.Pointer(rowIndex, colIndex);
    }

    /*! Returns a non const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    pointer Pointer(void) {
        return this->Matrix.Pointer();
    }

    /* documented in base class */
    const_pointer Pointer(size_type rowIndex, size_type colIndex) const {
        return BaseType::Pointer(rowIndex, colIndex);
    }

    /* documented in base class */
    const_pointer Pointer(void) const {
        return BaseType::Pointer();
    }


    /*! Access an element by index (const).  Compare with
      std::vector::at.  This method can be a handy substitute for the
      overloaded operator [] when operator overloading is unavailable
      or inconvenient.

      \return a non-const reference to element[index] */
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


    /*! Overloaded operator () for simplified (non const) element access with bounds checking */
    reference operator () (size_type rowIndex, size_type colIndex) CISST_THROW(std::out_of_range) {
        return this->at(rowIndex, colIndex);
    }

    /* documented in base class */
    const_reference operator () (size_type rowIndex, size_type colIndex) const CISST_THROW(std::out_of_range) {
        return BaseType::operator()(rowIndex, colIndex);
    }


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


    /*! \name Row and column references. */
    //@{
    /*! Create a row reference. */
    RowRefType Row(size_type index) CISST_THROW(std::out_of_range) {
        this->ThrowUnlessValidRowIndex(index);
        return RowRefType(this->cols(), Pointer(index, 0), this->col_stride());
    }

    /*! Create a column reference. */
    ColumnRefType Column(size_type index) CISST_THROW(std::out_of_range) {
        this->ThrowUnlessValidColIndex(index);
        return ColumnRefType(this->rows(), Pointer(0, index), this->row_stride());
    }

    /*! Create a non-const reference to the main diagonal of this matrix */
    DiagonalRefType Diagonal(void) {
        return DiagonalRefType( std::min(this->rows(), this->cols()), Pointer(0, 0), this->row_stride() + this->col_stride() );
    }

    /*! Resize and fill a vector of pointers on the rows of the
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
    VectorPointerType & RowPointers(VectorPointerType & rowPointers) CISST_THROW(std::runtime_error) {
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


    /* documented in base class */
    ConstRowRefType Row(size_type index) const CISST_THROW(std::out_of_range) {
        return BaseType::Row(index);
    }

    /* documented in base class */
    ConstColumnRefType Column(size_type index) const CISST_THROW(std::out_of_range) {
        return BaseType::Column(index);
    }

    /* documented in base class */
    ConstDiagonalRefType Diagonal(void) const {
        return BaseType::Diagonal();
    }

    /* documented in base class */
    ConstVectorPointerType RowPointers(ConstVectorPointerType & rowPointers) const CISST_THROW(std::runtime_error) {
        return BaseType::RowPointers(rowPointers);
    }

    //@}

    /*! Create a reference to a sub matrix */
    //@{
    vctDynamicMatrixRef<_elementType>
    Ref(const size_type rows, const size_type cols,
        const size_type startRow = 0, const size_type startCol = 0) CISST_THROW(std::out_of_range) {
        if (((startRow + rows) > this->rows())
            || ((startCol + cols) > this->cols())) {
            cmnThrow(std::out_of_range("vctDynamicMatrixBase::Ref: reference is out of range"));
        }
        return vctDynamicMatrixRef<_elementType>(rows, cols,
                                                 this->row_stride(), this->col_stride(),
                                                 Pointer(startRow, startCol));
    }

    vctDynamicConstMatrixRef<_elementType>
    Ref(const size_type rows, const size_type cols,
        const size_type startRow = 0, const size_type startCol = 0) const CISST_THROW(std::out_of_range) {
        return BaseType::Ref(rows, cols, startRow, startCol);
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
    template <class __inputMatrixOwnerType, class __indexVectorOwnerType>
    void SelectRowsFrom(const vctDynamicConstMatrixBase<__inputMatrixOwnerType, _elementType> & inputMatrix,
                        const vctDynamicConstVectorBase<__indexVectorOwnerType, index_type> & rowIndexVector)
    {
        vctDynamicMatrixLoopEngines::SelectRowsByIndex::
            Run(*this, inputMatrix, rowIndexVector);
    }

    /*! Select a subset of columns from another matrix */
    template <class __inputMatrixOwnerType, class __indexVectorOwnerType>
    void SelectColsFrom(const vctDynamicConstMatrixBase<__inputMatrixOwnerType, _elementType> & inputMatrix,
                        const vctDynamicConstVectorBase<__indexVectorOwnerType, index_type> & colIndexVector)
    {
        this->TransposeRef().SelectRowsFrom(inputMatrix.TransposeRef(), colIndexVector);
    }
    //@}

    /*! Assign a permutation of the rows of the input matrix to the rows of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedRowIndexes an array of row indices.  The assignment performed is:
      this->Row(i) <-- inputMatrix.Row( permutedRowIndexes[i] ).

      \note The current implementation does not validate that the input permuted
      indexes is an actual permutation of the numbers 0..(ROWS-1).  Nor does it
      assure that the input permutation array has the right size.  Both are the
      caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template <class __matrixOwnerType>
    void RowPermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
                          const index_type permutedRowIndexes[])
    {
        const size_type numRows = this->rows();
        size_type thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < numRows; ++thisRowIndex) {
            Row(thisRowIndex).Assign( inputMatrix.Row(permutedRowIndexes[thisRowIndex]) );
        }
    }

    /*! Assign a permutation of the rows of the input matrix to the rows of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedRowIndexes an array of row indices.  The assignment performed is:
      this->Row( permutedRowIndexes[i] ) <-- inputMatrix.Row(i).

      \note The current implementation does not validate that the input permuted
      indexes is an actual permutation of the numbers 0..(ROWS-1).  Nor does it
      assure that the input permutation array has the right size.  Both are the
      caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template <class __matrixOwnerType>
    void RowInversePermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
                                 const index_type permutedRowIndexes[])
    {
        const size_type numRows = this->rows();
        size_type thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < numRows; ++thisRowIndex) {
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
    template <class __matrixOwnerType>
    void ColumnPermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
                             const index_type permutedColumnIndexes[])
    {
        const size_type numCols = this->cols();
        size_type thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < numCols; ++thisColumnIndex) {
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
    template <class __matrixOwnerType>
    void ColumnInversePermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
                                    const index_type permutedColumnIndexes[])
    {
        const size_type numCols = this->cols();
        size_type thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < numCols; ++thisColumnIndex) {
            Column(permutedColumnIndexes[thisColumnIndex]).Assign( inputMatrix.Column(thisColumnIndex) );
        }
    }
    //@}

    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the matrix
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type value) {
        vctDynamicMatrixLoopEngines::
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


    /*!
      \name Assignment operation between matrices containing the same data type but different owners
      \param other The matrix to be copied.
    */
    //@{
    template <class __matrixOwnerType>
    inline ThisType & Assign(const vctDynamicConstMatrixBase<__matrixOwnerType, value_type> & other) {
        if (this->FastCopyCompatible(other)) {
            this->FastCopyOf(other, false);
        } else {
            vctDynamicMatrixLoopEngines::
                MoMi<typename vctUnaryOperations<value_type,
                typename __matrixOwnerType::value_type>::Identity>::
                Run(*this, other);
        }
        return *this;
    }
    //@}


    /*!
      \name Assignment operation between matrices of different types.

      \param other The matrix to be copied.
    */
    //@{
    template <class __matrixOwnerType, typename __elementType>
    inline ThisType & Assign(const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & other) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type,
            typename __matrixOwnerType::value_type>::Identity>::
            Run(*this, other);
        return *this;
    }

    template <class __matrixOwnerType, typename __elementType>
    inline ThisType & operator = (const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & other) {
        return this->Assign(other);
    }

    template <size_type __rows, size_type __cols,
              stride_type __rowStride, stride_type __colStride,
              class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, __elementType, __dataPtrType>
                             & other) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(*this, vctDynamicConstMatrixRef<__elementType>(other));
        return *this;
    }
    //@}


    /*!  \name Forced assignment operation between matrices of
      different types.  This method will use SetSize on the
      destination matrix (this matrix) to make sure the assignment
      will work.  It is important to note that if the sizes don't
      match, memory for this matrix will be re-allocated.  If the
      program has previously created some references (as in
      vctDynamic{Const}MatrixRef) on this matrix, they will become
      invalid.

      \note For a non-reallocating Assign, it is recommended to use
      the Assign() methods.

      \note If the destination matrix doesn't have the same size as
      the source and can not be resized, an exception will be thrown
      by the Assign method called internally.

      \note This method is provided for both fixed size and dynamic
      matrices for API consistency (usable in templated code).  There
      is obviously not resize involved on fixed size matrices.

      \param other The matrix to be copied.
    */
    //@{
    template <class __matrixOwnerType, typename __elementType>
    inline ThisType & ForceAssign(const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & other) {
        return this->Assign(other);
    }

    template <size_type __rows, size_type __cols,
              stride_type __rowStride, stride_type __colStride,
              class __elementType, class __dataPtrType>
    inline ThisType & ForceAssign(const vctFixedSizeConstMatrixBase<__rows, __cols,
                                                                    __rowStride, __colStride,
                                                                    __elementType, __dataPtrType>  & other) {
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
          use the same storage order, a single <code>memcpy</code> is
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

        Finally, this method will work on either matrices which have
        allocated their memory of matrix references (such as
        vctDynamicMatrixRef).  So, to copy a part of a matrix, the
        programmer can do:
        \code
        typedef unsigned short PixelType;
        vctDynamicMatrix<PixelType> image(640, 480, VCT_ROW_MAJOR);
        vctDynamicMatrixRef<PixelType> upperLeftRef(image, 0, 0, 320, 240);
        vctDynamicMatrix<PixelType> working(320, 240, VCT_ROW_MAJOR);
        // use FastCopyCompatible to make sure the sizes and strides match
        // and then use FastCopyOf without the safety checks
        working.FastCopyOf(upperLeft, vctFastCopy::SkipChecks);
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

    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, class __dataPtrType>
    inline bool FastCopyOf(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, value_type, __dataPtrType> & source,
                           bool performSafetyChecks = vctFastCopy::PerformChecks)
        CISST_THROW(std::runtime_error)
    {
        return vctFastCopy::MatrixCopy(*this, source, performSafetyChecks);
    }
    //@}


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
        // The row stride is the difference between two rows.  That is, in row-major
        // storage it is equal to the number of columns, and in column-major it is
        // equal to 1.
        const difference_type inputRowStride = (inputIsRowMajor) ? this->cols() : 1;
        const difference_type inputColStride = (inputIsRowMajor) ? 1 : this->rows();
        const vctDynamicConstMatrixRef<value_type> tmpRef( this->rows(), this->cols(),
            inputRowStride, inputColStride, elements );
        this->Assign(tmpRef);
        return *this;
    }


    /*! Assign to this matrix a set of values provided as independent
      arguments, by using cstdarg macros, that is, an unspecified
      number of arguments.  This operation assumes
      that all the arguments are of type value_type, and that their
      number is equal to the size of the matrix.  The arguments are
      passed <em>by value</em>.  The user may need to explicitly cast
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



    /*! Return a transposed reference to this matrix.
    */
    RefTransposeType TransposeRef(void)
    {
        return RefTransposeType(this->cols(), this->rows(), this->col_stride(), this->row_stride(), Pointer());
    }

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

      \param matrix1 The first operand of the binary operation

      \param matrix2 The second operand of the binary operation

      \return The matrix "this" modified.
    */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline ThisType & SumOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                            const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Addition >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline ThisType & DifferenceOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                   const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Subtraction >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline ThisType & ElementwiseProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                           const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Multiplication >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline ThisType & ElementwiseRatioOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                         const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Division >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline ThisType & ElementwiseMinOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                       const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Minimum >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline ThisType & ElementwiseMaxOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                       const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Maximum >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between two matrices.
      Store the result of op(this, otherMatrix) back to this matrix. */
    //@{
    /*! Store back binary elementwise operations between two
      matrices.  For each element of the matrices, performs \f$
      this[i] \leftarrow op(this[i], otherMatrix[i])\f$ where
      \f$op\f$ is either an addition (Add), a subtraction
      (Subtraction), a multiplication (ElementwiseMultiply) a division
      (ElementwiseDivide), a minimization (ElementwiseMin) or a
      maximisation (ElementwiseMax).

      \param otherMatrix The second operand of the binary operation
      (this[i] is the first operand)

      \return The matrix "this" modified.
    */
    template <class __matrixOwnerType>
    inline ThisType & Add(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & Subtract(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ElementwiseMultiply(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ElementwiseDivide(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ElementwiseMin(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ElementwiseMax(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & operator += (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        return this->Add(otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & operator -= (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
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
    template <class __matrixOwnerType>
    inline ThisType & SumOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                            const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & DifferenceOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                   const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & RatioOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                              const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ClippedAboveOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                     const value_type lowerBound) {
        vctDynamicMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, matrix, lowerBound);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ClippedBelowOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                     const value_type upperBound) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, matrix, upperBound);
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
    template <class __matrixOwnerType>
    inline ThisType & SumOf(const value_type scalar,
                            const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & DifferenceOf(const value_type scalar,
                                   const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ProductOf(const value_type scalar,
                                const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & RatioOf(const value_type scalar,
                              const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ClippedAboveOf(const value_type upperBound,
                                     const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Minimum >::
            Run(*this, upperBound, matrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & ClippedBelowOf(const value_type lowerBound,
                                     const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Maximum >::
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
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipAbove(const value_type upperBound) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, upperBound);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipBelow(const value_type lowerBound) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
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


    template <class __matrixOwnerType>
    inline ThisType & AddProductOf(const value_type scalar,
                                   const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix)
    {
        vctDynamicMatrixLoopEngines::
            MioSiMi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, otherMatrix);
        return *this;
    }


    template <class __matrixOwnerType1, class __matrixOwnerType2>
    inline ThisType & AddElementwiseProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                              const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2)
    {
        vctDynamicMatrixLoopEngines::
            MioMiMi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, matrix1, matrix2);
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
    template <class __matrixOwnerType>
    inline ThisType & AbsOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & NegationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Negation>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & FloorOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Floor>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline ThisType & CeilOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Ceil>::
            Run(*this, otherMatrix);
        return *this;
    }

    template <class __matrixOwnerType>
    inline ThisType & TransposeOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        Assign(otherMatrix.TransposeRef());
        return *this;
    }

    //@}

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
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeAbs>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & NegationSelf(void) {
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeNegation>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & FloorSelf(void) {
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeFloor>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & CeilSelf(void) {
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeCeil>::
            Run(*this);
        return *this;
    }
    //@}


    /*! Product of two matrices.  If the sizes of the matrices don't
      match, an exception is thrown.

    \param matrix1 The left operand of the binary operation.

    \param matrix2 The right operand of the binary operation.

    \return The matrix "this" modified. */
    template <class __matrixOwnerType1, class __matrixOwnerType2>
    void ProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                   const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        typedef vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> Input1MatrixType;
        typedef vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> Input2MatrixType;
        typedef typename Input1MatrixType::ConstRowRefType Input1RowRefType;
        typedef typename Input2MatrixType::ConstColumnRefType Input2ColumnRefType;
        vctDynamicMatrixLoopEngines::
            Product<typename vctBinaryOperations<value_type, Input1RowRefType, Input2ColumnRefType>::DotProduct>::
            Run((*this), matrix1, matrix2);
    }


    /*! Compute the outer product of two vectors and store the result to
      this matrix.  The outer product (v1*v2)[i,j] = v1[i] * v2[j] */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    void OuterProductOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & colVector,
                        const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & rowVector)
    {
        vctDynamicConstMatrixRef<_elementType> colMatrix, rowMatrix;
        colMatrix.SetRef(colVector.size(), 1, colVector.stride(), 1, colVector.Pointer(0));
        rowMatrix.SetRef(1, rowVector.size(), 1, rowVector.stride(), rowVector.Pointer(0));
        this->ProductOf(colMatrix, rowMatrix);
    }



    /*! Define a Submatrix class for compatibility with the fixed size matrices.
      A submatrix has the same stride as the parent container.

      Example:

      typedef vctDynamicMatrix<double> MatrixType;
      MatrixType M(6,6);
      MatrixType::Submatrix::Type topLeft(M, 0, 0, 3, 3);
      MatrixType::Submatrix::Type bottomRight(M, 3, 3); // implicitely everything left

      \note There is no straightforward way to define a fixed-size
      submatrix of a dynamic matrix, because the stride of the dynamic
      matrix is not known in compilation time.  A way to do it is:

      vctFixedSizeMatrixRef<double, 3, 3, 1, 6> topRight(M, 0, 3);
      vctFixedSizeMatrixRef<double, 3, 3, 1, 6> bottomLeft(M, 3, 0);
    */
#ifndef SWIG
    class Submatrix
    {
    public:
        typedef vctDynamicMatrixRef<value_type> Type;
    };
#endif // SWIG

};


#endif // _vctDynamicMatrixBase_h
