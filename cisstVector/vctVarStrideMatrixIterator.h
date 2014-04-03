/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-02

  (C) Copyright 2004-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctVarStrideMatrixIterator_h
#define _vctVarStrideMatrixIterator_h

/*!
  \file
  \brief Declaration of vctVarStrideMatrixConstIterator and vctVarStrideMatrixIterator
 */

#include <iterator>
#include <cisstVector/vctContainerTraits.h>


/*!
  \param _elementType the type of the element that the iterator refers to.

  \sa vctVarStrideMatrixIterator
*/
template<class _elementType>
class vctVarStrideMatrixConstIterator:
    public std::iterator<std::random_access_iterator_tag, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! The type of the iterator itself. */
    typedef vctVarStrideMatrixConstIterator<_elementType> ThisType;

    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, _elementType> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

protected:
    /*! The data member points to the object being refered by this
      iterator.  It is not declared const, so we will be able to use
      it in the non-const iterator class.  But all the methods in
      vctVarStrideMatrixConstIterator are declared const to protect
      the data from writing.
    */
    value_type * DataPtr;

    /* Strides between the elements of a matrix. */
    stride_type ColumnStride;
    difference_type NumColumns;
    stride_type RowStride;
    difference_type CurrentColumn;

    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column exceeds the number of columns in the matrix,
      more one row down and wrap to the corresponding column starting
      from the left
    */
    inline void WrapToRight() {
        if (CurrentColumn >= NumColumns) {
            DataPtr += RowStride - (ColumnStride * NumColumns);
            CurrentColumn -= NumColumns;
        }
    }


    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column is negative, move one row up, and wrap to the
      corresponding column starting from the right
    */
    inline void WrapToLeft() {
        if (CurrentColumn < 0) {
            DataPtr -= RowStride - (ColumnStride * NumColumns);
            CurrentColumn += NumColumns;
        }
    }


public:
    /*! Default constructor: create an uninitialized object. */
    vctVarStrideMatrixConstIterator():
        DataPtr(0),
        ColumnStride(1),
        NumColumns(0),
        RowStride(1),
        CurrentColumn(0)
        {}


    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! */
    vctVarStrideMatrixConstIterator(value_type * dataPtr, stride_type columnStride,
                                    size_type numColumns, stride_type rowStride,
                                    size_type initialColumn = 0):
        DataPtr(dataPtr),
        ColumnStride(columnStride),
        NumColumns(numColumns),
        RowStride(rowStride),
        CurrentColumn(initialColumn)
        {}


    /*! Constructor taking a const element pointer.  We need it in
      order to deal with const containers being refered by this
      iterator.  We have to perform const_cast to convert back to the
      stored non-const member pointer of this iterator.  However, the
      non-const iterator class completely shadows the constructors of
      the const-iterator base class, and so we don't have to worry
      about creating a non-const iterator over a const container.
    */
    vctVarStrideMatrixConstIterator(const value_type * dataPtr, difference_type columnStride,
                                    size_type numColumns, stride_type rowStride,
                                    size_type initialColumn = 0):
        DataPtr(const_cast<value_type *>(dataPtr)),
        ColumnStride(columnStride),
        NumColumns(numColumns),
        RowStride(rowStride),
        CurrentColumn(initialColumn)
    {}


    /*! Pre-increment. */
    ThisType & operator++() {
        DataPtr += ColumnStride;
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
        DataPtr -= ColumnStride;
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
        DataPtr += (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride;
        CurrentColumn += difference % NumColumns;
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
        DataPtr -= (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride;
        CurrentColumn -= difference % NumColumns;
        if (difference >= 0)
            WrapToLeft();
        else
            WrapToRight();
        return *this;
    }


    /*! Subtraction between iterators returns the number of increments needed
      for the second operand to reach the first operand, if it is reachable.

      The number of increments is found by the following equations:
      DataPtr - (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride == other.DataPtr
      DataPtr - other.DataPtr == (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride

      if (RowStride >= NumColumns * ColumnStride) {
      (DataPtr - other.DataPtr) / RowStride == row_diff == (difference / NumColumns)
      DataPtr - other.DataPtr - row_diff * RowStride == (difference % NumColumns) * ColumnStride
      (DataPtr - other.DataPtr - row_diff * RowStride) / ColumnStride == col_diff == (difference % NumColumns)
      difference == row_diff * NumColumns + col_diff
      }
      otherwise switch the roles of rows and columns.

      \note this operation assumes reachability and does not test for it.
    */
    difference_type operator-(const ThisType & other) const {
        const value_type * beginThisRow = DataPtr - CurrentColumn * ColumnStride;
        const value_type * beginThatRow = other.DataPtr - other.CurrentColumn * ColumnStride;
        const difference_type rowDiff = (beginThisRow - beginThatRow) / RowStride;
        const difference_type colDiff = CurrentColumn - other.CurrentColumn;
        const difference_type result = rowDiff * NumColumns + colDiff;
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
        return ((*this) < other) || ((*this) == other);;
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
        return ((*this) > other) || ((*this) == other);;
    }


    /*! Complementary operation to operator ==. */
    bool operator != (const ThisType & other) const {
        return !( (*this) == other );
    }

};




/*!  The non-const iterator with a fixed stride.
 \sa vctVarStrideMatrixConstIterator
*/
template<class _elementType>
class vctVarStrideMatrixIterator : public vctVarStrideMatrixConstIterator<_elementType>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctVarStrideMatrixIterator<_elementType> ThisType;
    typedef vctVarStrideMatrixConstIterator<_elementType> BaseType;
    typedef typename BaseType::iterator_category iterator_category;

    /*! Default constructor: create an uninitialized object */
    vctVarStrideMatrixIterator():
        BaseType()
        {}

    /*! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    */
    vctVarStrideMatrixIterator(value_type * dataPtr, stride_type columnStride,
                               size_type numColumns, stride_type rowStride,
                               size_type initialColumn = 0):
        BaseType(dataPtr, columnStride, numColumns, rowStride, initialColumn)
        {}


    /*! Redefine operator++ to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType & operator++() {
        this->DataPtr += this->ColumnStride;
        ++(this->CurrentColumn);
        this->WrapToRight();
        return *this;
    }


    /*! Redefine operator++ to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Redefine operator-- to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType & operator--() {
        this->DataPtr -= this->ColumnStride;
        --(this->CurrentColumn);
        this->WrapToLeft();
        return *this;
    }


    /*! Redefine operator-- to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Redefine operator+= to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator.
      \note If the number of columns of the matrix is zero then the iterator
      is returned unchanged.
    */
    ThisType & operator+=(difference_type difference) {
        if (this->NumColumns == 0)
            return *this;
        this->DataPtr += (difference / this->NumColumns) * this->RowStride + (difference % this->NumColumns) * this->ColumnStride;
        this->CurrentColumn += difference % this->NumColumns;
        if (difference >= 0)
            this->WrapToRight();
        else
            this->WrapToLeft();
        return *this;
    }


    /*! Redefine operator-= to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator
      \note If the number of columns of the matrix is zero then the iterator
      is returned unchanged.
    */
    ThisType & operator-=(difference_type difference) {
        if (this->NumColumns == 0)
            return *this;
        this->DataPtr -= (difference / this->NumColumns) * this->RowStride + (difference % this->NumColumns) * this->ColumnStride;
        this->CurrentColumn -= difference % this->NumColumns;
        if (difference >= 0)
            this->WrapToLeft();
        else
            this->WrapToRight();
        return *this;
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



/*! const_iterator + difference_type required by STL */
template<class _elementType>
vctVarStrideMatrixConstIterator<_elementType>
operator+(const vctVarStrideMatrixConstIterator<_elementType> & iterator,
          typename vctVarStrideMatrixConstIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixConstIterator<_elementType> result(iterator);
    return result += difference;
}


/*! difference_type + const_iterator required by STL */
template<class _elementType>
vctVarStrideMatrixConstIterator<_elementType>
operator+(typename vctVarStrideMatrixConstIterator<_elementType>::difference_type difference,
          const vctVarStrideMatrixConstIterator<_elementType> & iterator)
{
    vctVarStrideMatrixConstIterator<_elementType> result(iterator);
    return result += difference;
}


/*! const_iterator - difference_type required by STL */
template<class _elementType>
vctVarStrideMatrixConstIterator<_elementType>
operator-(const vctVarStrideMatrixConstIterator<_elementType> & iterator,
          typename vctVarStrideMatrixConstIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixConstIterator<_elementType> result(iterator);
    return result -= difference;
}


/*! iterator + difference_type required by STL.  Note that this method
  returns a non-const iterator, which is why it is declared separately
  from the other operator +.
*/
template<class _elementType>
vctVarStrideMatrixIterator<_elementType>
operator+(const vctVarStrideMatrixIterator<_elementType> & iterator,
          typename vctVarStrideMatrixIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixIterator<_elementType> result(iterator);
    return result += difference;
}


/*! difference_type + iterator required by STL */
template<class _elementType>
vctVarStrideMatrixIterator<_elementType>
operator+(typename vctVarStrideMatrixIterator<_elementType>::difference_type difference,
          const vctVarStrideMatrixIterator<_elementType> & iterator)
{
    vctVarStrideMatrixIterator<_elementType> result(iterator);
    return result += difference;
}


/*! iterator - difference_type required by STL */
template<class _elementType>
vctVarStrideMatrixIterator<_elementType>
operator-(const vctVarStrideMatrixIterator<_elementType> & iterator,
          typename vctVarStrideMatrixIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixIterator<_elementType> result(iterator);
    return result -= difference;
}


#endif  // _vctVarStrideMatrixIterator_h

