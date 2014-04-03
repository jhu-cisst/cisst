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
#ifndef _vctFixedStrideVectorIterator_h
#define _vctFixedStrideVectorIterator_h

/*!
  \file
  \brief Declaration of vctFixedStrideVectorConstIterator and vctFixedStrideVectorIterator
 */

#include <cisstCommon/cmnPortability.h>
#include <cisstVector/vctContainerTraits.h>

#include <iterator>

/*!  Define an iterator over a memory array with stride.  The iterator
  is defined according to the STL specifications of
  random-access-iterarator.  It is almost identical to
  std::vector::iterator, except that the ++, --, +=, -=, +, -,
  operations all work in _stride increments, and these are specified
  in compile time.

  We first define a const iterator, which only allows to read the
  referenced object.  The non-const iterator hass all the
  functionality of a const_iterator, plus the mutability of the
  objects.  We therefore derive vctFixedStrideVectorIterator from
  vctFixedStrideVectorConstIterator (as done with std::list
  iterators).

  In the current version, we do not define operator=, and rely on
  explicit declarations of objects and the default operator= and copy
  constructor.

  \param _stride the stride between elements of the vector being
  iterated over.

  \param _elementType the type of the element that the iterator refers
  to.

  \sa vctFixedStrideVectorIterator
*/
template <class _elementType, vct::stride_type _stride>
class vctFixedStrideVectorConstIterator:
    public std::iterator<std::random_access_iterator_tag, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! This of the iterator itself. */
    typedef vctFixedStrideVectorConstIterator<_elementType, _stride> ThisType;

    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, _elementType> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

    /* Stride between the elements of a vector. */
    enum {STRIDE = _stride};

protected:
    /*! The data member points to the object being refered by this
      iterator.  It is not declared const, so we will be able to use
      it in the non-const iterator class.  But all the methods in
      vctFixedStrideVectorConstIterator are declared const to protect the data
      from writing.
    */
    value_type * DataPtr;

public:
    /*! Default constructor: create an uninitialized object. */
    vctFixedStrideVectorConstIterator()
    {}


    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! */
    explicit vctFixedStrideVectorConstIterator(value_type * dataPtr)
        : DataPtr(dataPtr)
    {}


    /*! Constructor taking a const element pointer.  We need it in
     order to deal with const containers being refered by this
     iterator.  We have to perform const_cast to convert back to the
     stored non-const member pointer of this iterator.  However, the
     non-const iterator class completely shadows the constructors of
     the const-iterator base class, and so we don't have to worry
     about creating a non-const iterator over a const container.
    */
    explicit vctFixedStrideVectorConstIterator(const value_type * dataPtr)
        : DataPtr(const_cast<value_type *>(dataPtr))
    {}


    /*! Pre-increment. */
    ThisType & operator++() {
        DataPtr += STRIDE;
        return *this;
    }


    /*! Post increment. */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Pre-decrement. */
    ThisType & operator--() {
        DataPtr -= STRIDE;
        return *this;
    }


    /*! Post decrement. */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Increase by given difference. */
    ThisType & operator+=(difference_type difference) {
        DataPtr += difference * STRIDE;
        return *this;
    }


    /*! Decrease by given difference. */
    ThisType & operator-=(difference_type difference) {
        DataPtr -= difference * STRIDE;
        return *this;
    }


    /*! Find difference between iterators.  This method does not test
      for reachability, and may return invalid values if the two
      iterators are not reachable.  It is the user's responsibility to
      ensure the correctness.
    */
    difference_type operator-(const ThisType & other) const {
        return (DataPtr - other.DataPtr) / STRIDE;
    }


    /*! Random access (return const reference). */
    const value_type & operator[](difference_type index) const {
        return *(DataPtr + index * STRIDE);
    }


    /*! Dereference (const) */
    const value_type & operator* () const {
        return *DataPtr;
    }


    /*! Order relation between iterators, required by STL.
     \note The STL manual states that "if j is reachable from i then i<j".
     This does not imply the converse: "if i<j then j is reachable from i".
     In the case here, the converse does not hold.
    */
    bool operator< (const ThisType & other) const {
        return ((*this) - other) < 0;
    }
    bool operator<= (const ThisType & other) const {
        return (*this < other) || (*this == other);
    }


    /*! Equality of iterators, required by STL */
    bool operator== (const ThisType & other) const {
        return DataPtr == other.DataPtr;
    }


    /*! Complementary operation to operator < */
    bool operator> (const ThisType & other) const {
        return other < (*this);
    }
    bool operator>= (const ThisType & other) const {
        return (*this > other) || (*this == other);
    }


    /*! Complementary operation to operator == */
    bool operator != (const ThisType & other) const {
        return !( (*this) == other );
    }
};




/*!  The non-const iterator with a fixed stride.
 \sa vctFixedStrideVectorConstIterator
*/
template<class _elementType, vct::stride_type _stride>
class vctFixedStrideVectorIterator : public vctFixedStrideVectorConstIterator<_elementType, _stride>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctFixedStrideVectorIterator<_elementType, _stride> ThisType;
    typedef vctFixedStrideVectorConstIterator<_elementType, _stride> BaseType;
    typedef typename BaseType::iterator_category iterator_category;

    /*! Default constructor: create an uninitialized object */
    vctFixedStrideVectorIterator()
        : BaseType()
        {}


    /*! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    */
    explicit vctFixedStrideVectorIterator(value_type * dataPtr)
        : BaseType(dataPtr)
        {}


    /*! Redefine operator++ to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator++() {
        this->DataPtr += this->STRIDE;
        return *this;
    }


    /*! Redefine operator++ to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType operator++(int) {
        ThisType tmp(*this);
        this->DataPtr += this->STRIDE;
        return tmp;
    }


    /*! Redefine operator-- to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator--() {
        this->DataPtr -= this->STRIDE;
        return *this;
    }


    /*! Redefine operator-- to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType operator--(int) {
        ThisType tmp(*this);
        this->DataPtr -= this->STRIDE;
        return tmp;
    }


    /*! Redefine operator+= to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator+=(difference_type difference) {
        this->DataPtr += difference * this->STRIDE;
        return *this;
    }


    /*! Redefine operator-= to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator-=(difference_type difference) {
        this->DataPtr -= difference * this->STRIDE;
        return *this;
    }


    /*! Add non-const version of operator [] */
    value_type & operator[](difference_type index) const {
        return *(this->DataPtr + index * this->STRIDE);
    }


    /*! Add non-const version of unary operator * */
    value_type & operator* () {
        return *(this->DataPtr);
    }
};



/*! const_iterator + difference_type required by STL */
template<class _elementType, vct::stride_type _stride>
vctFixedStrideVectorConstIterator<_elementType, _stride>
operator+(const vctFixedStrideVectorConstIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorConstIterator<_elementType, _stride>::difference_type difference)
{
    vctFixedStrideVectorConstIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! difference_type + const_iterator required by STL */
template<class _elementType, vct::stride_type _stride>
vctFixedStrideVectorConstIterator<_elementType, _stride>
operator+(typename vctFixedStrideVectorConstIterator<_elementType, _stride>::difference_type difference,
          const vctFixedStrideVectorConstIterator<_elementType, _stride> & iterator)
{
    vctFixedStrideVectorConstIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! const_iterator - difference_type required by STL */
template<class _elementType, vct::stride_type _stride>
vctFixedStrideVectorConstIterator<_elementType, _stride>
operator-(const vctFixedStrideVectorConstIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorConstIterator<_elementType,_stride>::difference_type difference)
{
    vctFixedStrideVectorConstIterator<_elementType, _stride> result(iterator);
    return result -= difference;
}


/*! iterator + difference_type required by STL.  Note that this method
  returns a non-const iterator, which is why it is declared separately
  from the other operator +.
*/
template<class _elementType, vct::stride_type _stride>
vctFixedStrideVectorIterator<_elementType, _stride>
operator+(const vctFixedStrideVectorIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorIterator<_elementType, _stride>::difference_type difference)
{
    vctFixedStrideVectorIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! difference_type + iterator required by STL */
template<class _elementType, vct::stride_type _stride>
vctFixedStrideVectorIterator<_elementType, _stride>
operator+(typename vctFixedStrideVectorIterator<_elementType, _stride>::difference_type difference,
          const vctFixedStrideVectorIterator<_elementType, _stride> & iterator)
{
    vctFixedStrideVectorIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! iterator - difference_type required by STL */
template<class _elementType, vct::stride_type _stride>
vctFixedStrideVectorIterator<_elementType, _stride>
operator-(const vctFixedStrideVectorIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorIterator<_elementType, _stride>::difference_type difference)
{
    vctFixedStrideVectorIterator<_elementType, _stride> result(iterator);
    return result -= difference;
}


#endif  // _vctFixedStrideVectorIterator_h
