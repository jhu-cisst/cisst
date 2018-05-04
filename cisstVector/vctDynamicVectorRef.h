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
#ifndef _vctDynamicVectorRef_h
#define _vctDynamicVectorRef_h

/*!
  \file
  \brief Declaration of vctDynamicVectorRef
*/

#include <cisstCommon/cmnDeSerializer.h>

#include<cisstVector/vctDynamicVectorBase.h>
#include<cisstVector/vctDynamicVectorRefOwner.h>
#include<cisstVector/vctDynamicConstVectorRef.h>

/*!
  \ingroup cisstVector
  \brief Dynamic vector referencing existing memory

  The class vctDynamicVectorRef allows to create a vector overlaying
  an existing block of memory.  It can be used to manipulate a
  container created by another toolkit (i.e. OpenGL, vtk, ...) or a
  cisstVector container using different parameters (different size,
  storage order or stride).

  To setup the overlay, one can use either the constructors or the
  SetRef methods.  When used with a cisst container, the SetRef
  methods can infer some of the memory layout information (pointer on
  first element, size, stride).  When used to overlay existing memory
  referenced by a pointer, the user has to provide all the required
  information.  In any case, the Ref object doesn't allocate nor free
  memory.

  For example to manipulate the first five elements of a vector, one
  can do:
  \code
  vctDynamicVector<double> wholeVector(10);
  vctRandom(wholeVector, -10.0, 10.0);
  vctDynamicVectorRef<double> first5;
  first5.SetRef(wholeVector, 0, 5);
  first5.SetAll(5.0);
  \endcode

  \note Make sure the underlying memory is not freed after the Ref
  object is created and before it is used.  This would lead to faulty
  memory access and potential bugs.

  \note vctDynamicVectorRef can perform const and non-const
  operations.  It can be initialized with a non-const object only
  (<code>value_type *</code>).  For a const Ref, see
  vctDynamicConstVectorRef.

  \sa vctDynamicVector, vctDynamicConstVectorRef

  \param _elementType Type of elements referenced.  Also defined as
  <code>value_type</code>.
*/
template <class _elementType>
class vctDynamicVectorRef: public vctDynamicVectorBase<vctDynamicVectorRefOwner<_elementType>, _elementType>
{
public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctDynamicVectorRef<_elementType> ThisType;
    typedef vctDynamicVectorRefOwner<_elementType> VectorOwnerType;
    typedef vctDynamicVectorBase<vctDynamicVectorRefOwner<_elementType>, _elementType> BaseType;
    typedef typename BaseType::CopyType CopyType;
    typedef typename VectorOwnerType::iterator iterator;
    typedef typename VectorOwnerType::const_iterator const_iterator;
    typedef typename VectorOwnerType::reverse_iterator reverse_iterator;
    typedef typename VectorOwnerType::const_reverse_iterator const_reverse_iterator;

    /*! Default constructor initializes the vector reference to NULL */
    vctDynamicVectorRef()
    {}

    /*! Copy constructor.  This copy constructor takes a const
      reference on the object to copy but has to remove the const-ness
      of the point to create a vctDynamicVectorRef and allow it to
      manipulate elements.  The explict use of a const_cast is far
      from great but there is no easy solution as C++ requires the
      copy constructor to take a const reference. */
    vctDynamicVectorRef(const ThisType & other):
        BaseType()
    {
        this->SetRef(other.size(), const_cast<pointer>(other.Pointer()), other.stride());
    }

    vctDynamicVectorRef(size_type size, value_type* data, stride_type stride = 1)
    {
        this->SetRef(size, data, stride);
    }

    /*! Initialize a dynamic reference to a fixed-size vector.
      \note This constructor is non-explicit, and allows automatic conversion
      from a fixed-size vector to a dynamic vector representation.
      \note The size and stride values are taken from the fixed size vector.
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    vctDynamicVectorRef(vctFixedSizeVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                        size_type startPosition = 0)
    {
        this->SetRef(otherVector, startPosition);
    }

    /*! Initialize a dynamic reference to specified subvector of a fixed-size vector.
      \note This constructor allows automatic conversion
      from a fixed-size vector to a dynamic vector representation.
      \note The stride values are taken from the fixed size vector, but the starting point and
      length must be specified.
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    vctDynamicVectorRef(vctFixedSizeVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                        size_type startPosition, size_type length)
    {
        this->SetRef(otherVector, startPosition, length);
    }

    /*! Initialize a dynamic reference to a dynamic vector.
      \note the starting point, size, and stride, are taken from the other vector.
    */
    template <class __vectorOwnerType>
    vctDynamicVectorRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        this->SetRef(otherVector);
    }

    /*! Initialize a dynamic reference to a dynamic vector.
      \note the stride is taken from the other vector, but the starting point and the
      length must be specified.
    */
    template <class __vectorOwnerType>
    vctDynamicVectorRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector,
                        size_type startPosition, size_type length)
    {
        this->SetRef(otherVector, startPosition, length);
    }

    void SetRef(size_type size, pointer data, stride_type stride = 1)
    {
        this->Vector.SetRef(size, data, stride);
    }

    /*! Set a dynamic reference to a fixed-size vector.
      \param otherVector the vector for which a new reference is set.
      \param startPosition the index of the first element in otherVector to be indexed
      by this reference.
      \note The size of the reference is set to the size of otherVector minus the startPosition.
      By default, startPosition=0 and the behavior is transparent.
      \note the size and memory stride of this reference will be equal to the
      size and memory stride of the input vector.
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    void SetRef(vctFixedSizeVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                size_type startPosition = 0)
    {
        SetRef(otherVector.size() - startPosition, otherVector.Pointer(startPosition),
               otherVector.stride());
    }

    /*! Set a dynamic reference to a specified subvector of a fixed-size vector.
      \note the memory stride of this reference will be equal to the
      memory stride of the input vector.
      \note This method verifies that the size of this vector does not exceed the
      size of the input vector (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    void SetRef(vctFixedSizeVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                size_type startPosition, size_type length) CISST_THROW(std::out_of_range)
    {
        if (startPosition + length > otherVector.size()) {
            cmnThrow(std::out_of_range("vctDynamicVectorRef SetRef out of range"));
        }
        SetRef(length, otherVector.Pointer(startPosition), otherVector.stride());
    }

    /*! Set a dynamic reference to a dynamic vector.
      \note the size and memory stride of this reference will be equal to the
      size memory stride of the input vector.
    */
    template <class __vectorOwnerType>
    void SetRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        SetRef(otherVector.size(), otherVector.Pointer(), otherVector.stride());
    }

    /*! Set a dynamic reference to a specified subvector of a dynamic vector.
      \note the memory stride of this reference will be equal to the
      memory stride of the input vector.
      \note This method verifies that the size of this vector does not exceed the
      size of the input vector (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <class __vectorOwnerType>
    void SetRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector,
                size_type startPosition, size_type length) CISST_THROW(std::out_of_range)
    {
        if (startPosition + length > otherVector.size()) {
            cmnThrow(std::out_of_range("vctDynamicVectorRef SetRef out of range"));
        }
        SetRef(length, otherVector.Pointer(startPosition), otherVector.stride());
    }

    /*!
      Assignment operation between vectors of different types

      \param other The vector to be copied.
      \note We overload operator = to perform elemenwise copy/assign. Although one
      may consider other meanings for the overloaded operator, we thought this was
      the most natural one.
    */
    //@{

#ifndef _cisstVectorPython_EXPORTS
    /* Equivalent to Assign.  Please note that this operator performs a data
      copy, not an object copy as understood with a C++ copy constructor.  If
      the size of operands don't match an exception will be thrown. */
    inline ThisType & operator = (const ThisType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }
#endif // _cisstVectorPython_EXPORTS

    inline ThisType & operator = (const vctDynamicConstVectorRef<value_type> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <class __vectorOwnerType, typename __elementType>
    inline ThisType & operator = (const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <size_type __size, stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & operator =
    (const vctFixedSizeConstVectorBase<__size, __stride, __elementType, __dataPtrType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }
    //@}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

    /*! Binary deserialization.  This method can not resize the
      existing block of memory and will throw an exception is the
      sizes don't match. */
    void DeSerializeRaw(std::istream & inputStream) CISST_THROW(std::runtime_error)
    {
        // get and set size
        size_type mySize;
        cmnDeSerializeRaw(inputStream, mySize);

        if (mySize != this->size()) {
            cmnThrow(std::runtime_error("vctDynamicVectorRef::DeSerializeRaw: Sizes of vectors don't match"));
        }

        // get data
        size_type index;
        for (index = 0; index < mySize; ++index) {
            cmnDeSerializeRaw(inputStream, this->Element(index));
        }
    }

};

#endif // _vctDynamicVectorRef_h
