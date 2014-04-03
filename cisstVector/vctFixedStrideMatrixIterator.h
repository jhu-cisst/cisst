/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-09-30

  (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedStrideMatrixIterator_h
#define _vctFixedStrideMatrixIterator_h

/*!
  \file
  \brief Declaration of vctFixedStrideMatrixConstIterator and vctFixedStrideMatrixIterator
*/

#include <iterator>

#include <cisstCommon/cmnAssert.h>
#include <cisstVector/vctContainerTraits.h>


/*!
  \brief Matrix iterator.

  The matrix iterators are parametrized by the type of matrix element,
  the stride (pointer increment) in row direction and column
  direction, and the number of matrix columns spanned by the iterator.
  The iterator traverses the matrix elements in row-first order, with
  respect to its matrix, and typically possesses the matrix' strides.
  Note that the iterator does not use the number of rows, and does not
  keep track of the current row index in the matrix.  This was done in
  order to keep the iterator's memory size as compact as possible.  It
  does, however, keep track of the current column in the matrix, which
  is required in order to be able to wrap to the next row when the end
  of the column is reached.

  The matrix iterators conform to the STL specification for a Random
  Access Iterator.  This means that one can access all the elements of
  a matrix sequentially by subscripting or incrementing the iterator.
  However, in many cases it is faster to access the elements by
  row-column indices than by the iterator, since the iterator needs to
  do the bookkeeping of column indices.

  The matrix iterators are defined hierarchically, with the
  vctFixedStrideMatrixConstIterator being immutable, and
  vctFixedStrideMatrixIterator derived from it and mutable.

  \note The comparison operator == compares both the pointer and the
  column index maintained by the iterator.  This was the only way we
  could ensure that the end() method on the matrix classes
  (vctConstFixedSizeMatrixBase etc.)  will evaluate correctly and be
  comparable with an iterator that's incremented.  However, the
  ordering of iterators relies only on the values of the pointers.
  Recall that iterator ordering has reachability issues, and see the
  next note.

  \note There are reachability issues with our iterators system,
  though they conform with the STL specifications.  Generally, STL
  does not require reachability between any pair of iterators, except
  iterators that refer to the same container object.  In our case, the
  iterator may span a vector of memory locations in an arbitrary
  order, depending on the strides of both rows and columns.  In other
  words, if we trace the memory locations pointed by an iterator which
  is incremented, they may be incremented or decremented depending on
  the current column index and the strides. This means that we cannot
  rely on memory order to order the iterators, and furthermore, we
  cannot assume reachability in the general case.  Reachability is
  guaranteed only for iterators that were obtained from the same
  matrix object and have the same directionality (forward or
  reverse). The correctness of operations like ordering iterators
  depends on reachability.  Therefore, use iterator ordering with
  care, and do not try to mix iterators from different containers into
  the same expression.
*/
template <class _elementType, vct::stride_type _columnStride, vct::size_type _numColumns, vct::stride_type _rowStride>
class vctFixedStrideMatrixConstIterator:
    public std::iterator<std::random_access_iterator_tag, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! The type of the iterator itself. */
    typedef vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> ThisType;

    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, _elementType> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

    enum {COL_STRIDE = _columnStride, ROW_STRIDE = _rowStride};
    enum {NUM_COLUMNS = _numColumns};

protected:
    value_type * DataPtr;
    difference_type CurrentColumn;

    enum {ROW_FIRST_INDEX = (ROW_STRIDE >= NUM_COLUMNS * COL_STRIDE)};

    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column exceeds the number of columns in the matrix,
      more one row down and wrap to the corresponding column starting
      from the left
    */
    inline void WrapToRight() {
        if (CurrentColumn >= NUM_COLUMNS) {
            DataPtr += ROW_STRIDE - (COL_STRIDE * NUM_COLUMNS);
            CurrentColumn -= NUM_COLUMNS;
        }
    }


    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column is negative, move one row up, and wrap to the
      corresponding column starting from the right
    */
    inline void WrapToLeft() {
        if (CurrentColumn < 0) {
            DataPtr -= ROW_STRIDE - (COL_STRIDE * NUM_COLUMNS);
            CurrentColumn += NUM_COLUMNS;
        }
    }


public:
    /*! Default constructor: create an uninitialized object */
    vctFixedStrideMatrixConstIterator() {
        CMN_ASSERT(ROW_STRIDE * COL_STRIDE > 0);
    }


    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! The initial column
      position is assumed to be zero by default, but may be chosen
      otherwise (within the proper range).

      \param dataPtr data pointer
      \param initialColumn column index where the iterator is initialized
    */
    explicit vctFixedStrideMatrixConstIterator(value_type * dataPtr,
                                               index_type initialColumn = 0)
        : DataPtr(dataPtr)
        , CurrentColumn(initialColumn) {
        CMN_ASSERT(ROW_STRIDE * COL_STRIDE > 0);
        CMN_ASSERT( initialColumn < NUM_COLUMNS );
    }


    /*! Constructor taking a const element pointer.  We need it in
      order to deal with const containers being refered by this
      iterator.  We have to perform const_cast to convert back to the
      stored non-const member pointer of this iterator.  However, the
      non-const iterator class completely shadows the constructors of
      the const-iterator base class, and so we don't have to worry
      about creating a non-const iterator over a const container.

      \param dataPtr const data pointer
      \param initialColumn column index where the iterator is initialized
    */
    explicit vctFixedStrideMatrixConstIterator(const value_type * dataPtr,
                                               index_type initialColumn = 0)
        : DataPtr(const_cast<value_type *>(dataPtr))
        , CurrentColumn(initialColumn) {
        CMN_ASSERT(ROW_STRIDE * COL_STRIDE > 0);
        CMN_ASSERT( initialColumn < NUM_COLUMNS );
    }


    /*! Pre-increment. */
    ThisType & operator++() {
        DataPtr += COL_STRIDE;
        ++CurrentColumn;
        WrapToRight();
        return *this;
    }


    /*! Post-increment. */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Pre-decrement. */
    ThisType & operator--() {
        DataPtr -= COL_STRIDE;
        --CurrentColumn;
        WrapToLeft();
        return *this;
    }


    /*! Post-decrement. */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Increment by offset.
      \param difference offset to increment by
    */
    ThisType & operator +=(difference_type difference) {
        DataPtr += (NUM_COLUMNS == 0) ? 0
            : (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE;
        CurrentColumn += (NUM_COLUMNS == 0) ? 0 : difference % NUM_COLUMNS;
        if (difference >= 0)
            WrapToRight();
        else
            WrapToLeft();
        return *this;
    }


    /*! Decrement by offset.
     \param difference offset to decrement by
    */
    ThisType & operator -=(difference_type difference) {
        DataPtr -= (NUM_COLUMNS == 0) ? 0
            : (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE;
        CurrentColumn -= (NUM_COLUMNS == 0) ? 0 : difference % NUM_COLUMNS;
        if (difference >= 0)
            WrapToLeft();
        else
            WrapToRight();
        return *this;
    }


    /*! Subtraction between iterators returns the number of increments needed
      for the second operand to reach the first operand, if it is reachable.

      The number of increments is found by the following equations:
      DataPtr - (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE == other.DataPtr
      DataPtr - other.DataPtr == (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE

      if (ROW_STRIDE >= NUM_COLUMNS * COL_STRIDE) {
      (DataPtr - other.DataPtr) / ROW_STRIDE == row_diff == (difference / NUM_COLUMNS)
      DataPtr - other.DataPtr - row_diff * ROW_STRIDE == (difference % NUM_COLUMNS) * COL_STRIDE
      (DataPtr - other.DataPtr - row_diff * ROW_STRIDE) / COL_STRIDE == col_diff == (difference % NUM_COLUMNS)
      difference == row_diff * NUM_COLUMNS + col_diff
      }
      otherwise switch the roles of rows and columns.

      \note this operation assumes reachability and does not test for it.
    */
    difference_type operator-(const ThisType & other) const {
        const value_type * beginThisRow = DataPtr - CurrentColumn * COL_STRIDE;
        const value_type * beginThatRow = other.DataPtr - other.CurrentColumn * COL_STRIDE;
        const difference_type rowDiff = (beginThisRow - beginThatRow) / ROW_STRIDE;
        const difference_type colDiff = CurrentColumn - other.CurrentColumn;
        const difference_type result = rowDiff * NUM_COLUMNS + colDiff;
        return result;
    }


    /*! Random access (return const reference). */
    const value_type & operator[](difference_type index) const {
        ThisType ptrCalc(*this);
        ptrCalc += index;
        return *ptrCalc;
    }


    /*! Dereference (const). */
    const value_type & operator* () const {
        return *DataPtr;
    }


    /*! Order relation between iterators, required by STL.

      \note The STL manual states that "if j is reachable from i then
      i<j".  This does not imply the converse: "if i<j then j is
      reachable from i".  In the case here, the converse does not
      hold.
    */
    bool operator< (const ThisType & other) const {
        return ((*this) - other) < 0;
    }
    bool operator<= (const ThisType & other) const {
        return ((*this) < other) || (*this) == other;
    }


    /*! Equality of iterators, required by STL. */
    bool operator== (const ThisType & other) const {
        return (DataPtr == other.DataPtr) && (CurrentColumn == other.CurrentColumn);
    }


    /*! Complementary operation to operator <. */
    bool operator> (const ThisType & other) const {
        return other < (*this);
    }
    bool operator>= (const ThisType & other) const {
        return ((*this) > other) || (*this) == other;
    }


    /*! Complementary operation to operator ==. */
    bool operator != (const ThisType & other) const {
        return !( (*this) == other );
    }
};



/*! const_iterator + difference_type required by STL */
template <class _elementType, vct::stride_type _columnStride, vct::index_type _numColumns, vct::stride_type _rowStride>
vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns,_rowStride>
operator+(const vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>::difference_type difference)
{
    vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> result(iterator);
    return result += difference;
}

/*! difference_type + const_iterator required by STL */
template <class _elementType, vct::stride_type _columnStride, vct::index_type _numColumns, vct::stride_type _rowStride>
vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>
operator+(typename vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>::difference_type difference,
          const vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator)
{
    vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> result(iterator);
    return result += difference;
}

/*! const_iterator - difference_type required by STL */
template <class _elementType, vct::stride_type _columnStride, vct::index_type _numColumns, vct::stride_type _rowStride>
vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>
operator-(const vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns,_rowStride>::difference_type difference)
{
    vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns,_rowStride> result(iterator);
    return result -= difference;
}


/*! \sa vctFixedStrideMatrixConstIterator */
template <class _elementType, vct::stride_type _columnStride, vct::index_type _numColumns, vct::stride_type _rowStride>
class vctFixedStrideMatrixIterator:
    public vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> ThisType;
    typedef vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> BaseType;
    typedef typename BaseType::iterator_category iterator_category;

    /*! Default constructor: create an uninitialized object */
    vctFixedStrideMatrixIterator()
        : BaseType()
    {}


    /*! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    */
    explicit vctFixedStrideMatrixIterator(value_type * dataPtr,
                                          index_type initialColumn = 0)
        : BaseType(dataPtr, initialColumn)
    {}


    /*! Redefine operator++ to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType & operator++() {
        return reinterpret_cast<ThisType &>(BaseType::operator++());
    }


    /*! Redefine operator++ to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Redefine operator-- to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType & operator--() {
        return reinterpret_cast<ThisType &>(BaseType::operator--());
    }


    /*! Redefine operator-- to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Redefine operator+= to return vctArrayIterator instead of
    vctArrayConstIterator */
    ThisType & operator+=(difference_type difference) {
        return reinterpret_cast<ThisType &>(BaseType::operator+=(difference));
    }


    /*! Redefine operator-= to return vctArrayIterator instead of
    vctArrayConstIterator */
    ThisType & operator-=(difference_type difference) {
        return reinterpret_cast<ThisType &>(BaseType::operator-=(difference));
    }


    /*! Add non-const version of operator [] */
    value_type & operator[](difference_type index) const {
        ThisType ptrCalc(*this);
        ptrCalc += index;
        return *ptrCalc;
    }


    /*! Add non-const version of unary operator * */
    value_type & operator* () {
        return *(this->DataPtr);
    }

};


/*! iterator + difference_type required by STL */
template <class _elementType, vct::stride_type _columnStride, vct::index_type _numColumns, vct::stride_type _rowStride>
vctFixedStrideMatrixIterator<_elementType, _columnStride,_numColumns, _rowStride>
operator+(const vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixIterator<_elementType,_columnStride, _numColumns, _rowStride>::difference_type difference)
{
    vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns,_rowStride> result(iterator);
    return result += difference;
}

/*! difference_type + iterator required by STL */
template <class _elementType, vct::stride_type _columnStride, vct::index_type _numColumns, vct::stride_type _rowStride>
vctFixedStrideMatrixIterator< _elementType, _columnStride, _numColumns, _rowStride>
operator+(typename vctFixedStrideMatrixIterator< _elementType, _columnStride, _numColumns, _rowStride>::difference_type difference,
          const vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator)
{
    vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> result(iterator);
    return result += difference;
}

/*! iterator - difference_type required by STL */
template <class _elementType, vct::stride_type _columnStride, vct::index_type _numColumns, vct::stride_type _rowStride>
vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride>
operator-(const vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride>::difference_type difference)
{
    vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns,_rowStride> result(iterator);
    return result -= difference;
}


#endif // _vctFixedStrideMatrixIterator_h

