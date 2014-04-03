/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Li, Ofri Sadowsky, Anton Deguet
  Created on: 2006-07-11

  (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctVarStrideNArrayIterator_h
#define _vctVarStrideNArrayIterator_h

/*!
  \file
  \brief Declaration of vctNArrayConstIterator and vctVarStrideNArrayIterator
 */

#include <iterator>
#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctDynamicNArrayOwner.h>
#include <cisstVector/vctVarStrideVectorIterator.h>
#include <iostream>


/*!
  \param _ownerType the type of the owner of the container to
      which this iterator points
  \param _forward set to true for forward iterators, false for
      reverse iterators

  \sa vctVarStrideNArrayIterator
*/
template<class _ownerType, bool _forward>
class vctVarStrideNArrayConstIterator:
    public std::iterator<std::random_access_iterator_tag,
                         typename _ownerType::value_type>
{
public:
    /* define most types from vctContainerOwnerTraits and vctNArrayTraits */
    enum {DIMENSION = _ownerType::DIMENSION};
    typedef typename _ownerType::value_type _elementType;
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    /*! Define DIRECTION */
    enum {DIRECTION = _forward ? +1 : -1};

    /*! The type of the iterator itself. */
    typedef vctVarStrideNArrayConstIterator<_ownerType, _forward> ThisType;

    /*! The type of the owner of the container to which this
      iterator points. */
    typedef _ownerType OwnerType;

    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, value_type> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

protected:
    /*! Pointer to the address of the container being referred to
      by this iterator. */
    const OwnerType * ContainerOwner;

    /*! Pseudo-index for the iterator. See complete documentation
      for a thorough discussion of this "meta index". */
    difference_type MetaIndex;

    /*! Pointer to the object being referred to by this iterator.
      It is not declared const, so we will be able to use it in the
      non const iterator class.  But all the methods in
      vctVarStrideNArrayConstIterator are declared const to protect
      the data from writing. */
    value_type * ElementPointer;


    /*! Auxiliary method to calculate the correct ElementPointer
      corresponding to the current MetaIndex. See the complete
      documentation for a detailed description of this algorithm. */
    void UpdateElementPointer(void)
    {
        difference_type metaindex = MetaIndex;
        difference_type offset = 0;
        difference_type modulus;

        typename nstride_type::const_reverse_iterator stridesBegin = ContainerOwner->strides().rbegin();
        typename nsize_type::const_reverse_iterator sizesBegin = ContainerOwner->sizes().rbegin();
        typename nsize_type::const_reverse_iterator sizesEnd = ContainerOwner->sizes().rend();
        size_type sizes_value;
        for (; sizesBegin != sizesEnd;
             ++sizesBegin, ++stridesBegin) {
            sizes_value = (*sizesBegin == 0) ? 1 : *sizesBegin;
            modulus = metaindex % static_cast<difference_type>(sizes_value);
            offset += modulus * (*stridesBegin);
            metaindex /= static_cast<difference_type>(sizes_value);
        }

        offset += metaindex * ContainerOwner->sizes()[0] * ContainerOwner->strides()[0];

        value_type * pointer = const_cast<value_type *>( ContainerOwner->Pointer() );
        ElementPointer = pointer + offset;
    }


public:
    /*! Default constructor: create an uninitialized object. */
    vctVarStrideNArrayConstIterator(void):
        ContainerOwner(0),
        MetaIndex(0),
        ElementPointer(0)
    {}


    /*! Constructor taking a non const element pointer; the
      starting position will be the first element of the nArray.
      Note that only read operations will be performed! */
    vctVarStrideNArrayConstIterator(const OwnerType * container, difference_type index = 0):
        ContainerOwner(container),
        MetaIndex(index)
    {
        UpdateElementPointer();
    }


    /*! Copy constructor */
    vctVarStrideNArrayConstIterator(const ThisType & other):
        ContainerOwner(other.ContainerOwner),
        MetaIndex(other.MetaIndex),
        ElementPointer(other.ElementPointer)
    {}


    /*! Assignment */
    ThisType & operator = (const ThisType & other)
    {
        this->ContainerOwner = other.ContainerOwner;
        this->MetaIndex = other.MetaIndex;
        this->ElementPointer = other.ElementPointer;
        return *this;
    }


    /*! Pre-increment. */
    ThisType & operator ++ (void)
    {
        MetaIndex += DIRECTION;
        UpdateElementPointer();
        return *this;
    }


    /*! Post-increment. */
    ThisType operator ++ (int)
    {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Pre-decrement. */
    ThisType & operator -- (void)
    {
        MetaIndex -= DIRECTION;
        UpdateElementPointer();
        return *this;
    }


    /*! Post-decrement. */
    ThisType operator -- (int)
    {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Increment by offset.
      \param difference offset to increment by
    */
    ThisType & operator += (difference_type difference)
    {
        MetaIndex += difference * DIRECTION;
        UpdateElementPointer();
        return *this;
    }


    /*! Decrement by offset.
      \param difference offset to decrement by
    */
    ThisType & operator -= (difference_type difference)
    {
        MetaIndex -= difference * DIRECTION;
        UpdateElementPointer();
        return *this;
    }


    /*! Subtraction between iterators returns the number of increments needed
      for the second operand to reach the first operand, if it is reachable.
      \note this operation assumes reachability and does not test for it.
    */
    difference_type operator - (const ThisType & other) const
    {
        return ( this->MetaIndex - other.MetaIndex );
    }


    /*! Random access (return const reference). */
    const value_type & operator [] (difference_type index) const
    {
        difference_type metaindex = MetaIndex + index;
        difference_type offset = 0;
        difference_type modulus;

        typename nstride_type::const_reverse_iterator stridesBegin = ContainerOwner->strides().rbegin();
        typename nsize_type::const_reverse_iterator sizesBegin = ContainerOwner->sizes().rbegin();
        typename nsize_type::const_reverse_iterator sizesEnd = ContainerOwner->sizes().rend();
        size_type sizes_value;
        for (; sizesBegin != sizesEnd;
             ++sizesBegin, ++stridesBegin) {
            sizes_value = (*sizesBegin == 0) ? 1 : *sizesBegin;
            modulus = metaindex % static_cast<difference_type>(sizes_value);
            offset += modulus * (*stridesBegin);
            metaindex /= static_cast<difference_type>(sizes_value);
        }

        value_type * pointer = const_cast<value_type *>( ContainerOwner->Pointer() );
        value_type * elementPointer = pointer + offset;

        return *elementPointer;
    }


    /*! Dereference (const). */
    const value_type & operator * (void) const
    {
        return *ElementPointer;
    }


    /*! Equality of iterators, required by STL. */
    bool operator == (const ThisType & other) const
    {
        return
            ( this->ContainerOwner == other.ContainerOwner) &&
            ( this->MetaIndex == other.MetaIndex ) &&
            ( this->ElementPointer == other.ElementPointer );
    }


    /*! Complementary operation to operator==. */
    bool operator != (const ThisType & other) const
    {
        return !( *this == other );
    }


    /*! Order relation between iterators, required by STL.

      \note The STL manual states that "if j is reachable from i then
      i<j".  This does not imply the converse: "if i<j then j is
      reachable from i".  In the case here, the converse does not
      hold.
    */
    bool operator < (const ThisType & other) const
    {
        return ((*this) - other) < 0;
    }
    bool operator <= (const ThisType & other) const
    {
        return ((*this) < other) || ((*this) == other);
    }


    /*! Complementary operation to operator <. */
    bool operator > (const ThisType & other) const
    {
        return other < (*this);
    }
    bool operator >= (const ThisType & other) const
    {
        return ((*this) > other) || ((*this) == other);
    }
};


/*! The non-const iterator of nArrays.
  \sa vctVarStrideNArrayConstIterator
*/
template<class _ownerType, bool _forward>
class vctVarStrideNArrayIterator :
    public vctVarStrideNArrayConstIterator<_ownerType, _forward>
{
public:
    /* define most types from vctContainerOwnerTraits and vctNArrayTraits */
    enum {DIMENSION = _ownerType::DIMENSION};
    typedef typename _ownerType::value_type _elementType;
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    enum {DIRECTION = _forward ? +1 : -1};
    typedef vctVarStrideNArrayIterator<_ownerType, _forward> ThisType;
    typedef _ownerType OwnerType;
    typedef vctVarStrideNArrayConstIterator<_ownerType, _forward> BaseType;
    typedef typename BaseType::iterator_category iterator_category;


    /*! Default constructor: create an uninitialized object. */
    vctVarStrideNArrayIterator():
        BaseType()
    {}


    /*! Constructor taking a non const element pointer; the
      starting position will be the first element of the nArray.
      Note that only read operations will be performed! */
    vctVarStrideNArrayIterator(const OwnerType * container, difference_type index = 0):
        BaseType(container, index)
    {}


    /*! Copy constructor */
    vctVarStrideNArrayIterator(const ThisType & other):
        BaseType(other)
    {}


    /*! Assignment */
    ThisType & operator = (const ThisType & other)
    {
        this->ContainerOwner = other.ContainerOwner;
        this->MetaIndex = other.MetaIndex;
        this->ElementPointer = other.ElementPointer;
        return *this;
    }


    /*! Redefine operator++ to return vctVarStrideNArrayIterator
      instead of vctVarStrideNArrayConstIterator */
    ThisType & operator ++ (void)
    {
        this->MetaIndex += DIRECTION;
        this->UpdateElementPointer();
        return *this;
    }


    /*! Redefine operator++ to return vctVarStrideNArrayIterator
      instead of vctVarStrideNArrayConstIterator */
    ThisType operator ++ (int)
    {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Redefine operator-- to return vctVarStrideNArrayIterator
      instead of vctVarStrideNArrayConstIterator */
    ThisType & operator -- (void)
    {
        this->MetaIndex -= DIRECTION;
        this->UpdateElementPointer();
        return *this;
    }


    /*! Redefine operator-- to return vctVarStrideNArrayIterator
        instead of vctVarStrideNArrayConstIterator */
    ThisType operator -- (int)
    {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }

    /*! Redefine operator+= to return vctVarStrideNArrayIterator
        instead of vctVarStrideNArrayConstIterator */
    ThisType & operator += (difference_type difference)
    {
        this->MetaIndex += difference * DIRECTION;
        this->UpdateElementPointer();
        return *this;
    }


    /*! Redefine operator-= to return vctVarStrideNArrayIterator
        instead of vctVarStrideNArrayConstIterator */
    ThisType & operator -= (difference_type difference)
    {
        this->MetaIndex -= difference * DIRECTION;
        this->UpdateElementPointer();
        return *this;
    }


    /*! Add non const version of operator[] */
    value_type & operator[](difference_type index)
    {
        difference_type metaindex = this->MetaIndex + index;
        difference_type offset = 0;
        difference_type modulus;

        typename nstride_type::const_reverse_iterator stridesBegin = this->ContainerOwner->strides().rbegin();
        typename nsize_type::const_reverse_iterator sizesBegin = this->ContainerOwner->sizes().rbegin();
        typename nsize_type::const_reverse_iterator sizesEnd = this->ContainerOwner->sizes().rend();
        size_type sizes_value;
        for (;
             sizesBegin != sizesEnd;
             ++sizesBegin, ++stridesBegin) {
            sizes_value = (*sizesBegin == 0) ? 1 : *sizesBegin;
            modulus = metaindex % static_cast<difference_type>(sizes_value);
            offset += modulus * (*stridesBegin);
            metaindex /= static_cast<difference_type>(sizes_value);
        }

        value_type * pointer = const_cast<value_type *>(this->ContainerOwner->Pointer());
        value_type * elementPointer = pointer + offset;

        return *elementPointer;
    }


    /*! Add non const version of unary operator * */
    value_type & operator * (void)
    {
        return *(this->ElementPointer);
    }
};


/*! const_iterator + difference_type required by STL */
template<class _ownerType, bool _forward>
vctVarStrideNArrayConstIterator<_ownerType, _forward>
operator + (const vctVarStrideNArrayConstIterator<_ownerType, _forward> & iterator,
            typename vctVarStrideNArrayConstIterator<_ownerType, _forward>::difference_type difference)
{
    vctVarStrideNArrayConstIterator<_ownerType, _forward> result(iterator);
    return result += difference;
}


/*! difference_type + const_iterator required by STL */
template<class _ownerType, bool _forward>
vctVarStrideNArrayConstIterator<_ownerType, _forward>
operator + (typename vctVarStrideNArrayConstIterator<_ownerType, _forward>::difference_type difference,
            const vctVarStrideNArrayConstIterator<_ownerType, _forward> & iterator)
{
    vctVarStrideNArrayConstIterator<_ownerType, _forward> result(iterator);
    return result += difference;
}


/*! const_iterator - difference_type required by STL */
template<class _ownerType, bool _forward>
vctVarStrideNArrayConstIterator<_ownerType, _forward>
operator - (const vctVarStrideNArrayConstIterator<_ownerType, _forward> & iterator,
            typename vctVarStrideNArrayConstIterator<_ownerType, _forward>::difference_type difference)
{
    vctVarStrideNArrayConstIterator<_ownerType, _forward> result(iterator);
    return result -= difference;
}


/*! iterator + difference_type required by STL. Note that this method
    returns a non const iterator, which is why it is declared separately
    from the other operator+. */
template<class _ownerType, bool _forward>
vctVarStrideNArrayIterator<_ownerType, _forward>
operator + (const vctVarStrideNArrayIterator<_ownerType, _forward> & iterator,
            typename vctVarStrideNArrayIterator<_ownerType, _forward>::difference_type difference)
{
    vctVarStrideNArrayIterator<_ownerType, _forward> result(iterator);
    return result += difference;

}


/*! difference_type + iterator required by STL */
template<class _ownerType, bool _forward>
vctVarStrideNArrayIterator<_ownerType, _forward>
operator + (typename vctVarStrideNArrayIterator<_ownerType, _forward>::difference_type difference,
            const vctVarStrideNArrayIterator<_ownerType, _forward> & iterator)
{
    vctVarStrideNArrayIterator<_ownerType, _forward> result(iterator);
    return result += difference;
}


/*! iterator - difference_type required by STL */
template<class _ownerType, bool _forward>
vctVarStrideNArrayIterator<_ownerType, _forward>
operator - (const vctVarStrideNArrayIterator<_ownerType, _forward> & iterator,
            typename vctVarStrideNArrayIterator<_ownerType, _forward>::difference_type difference)
{
    vctVarStrideNArrayIterator<_ownerType, _forward> result(iterator);
    return result -= difference;
}


#endif  // _vctVarStrideNArrayIterator_h

