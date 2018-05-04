/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicConstVectorRef_h
#define _vctDynamicConstVectorRef_h

/*!
  \file
  \brief Declaration of vctDynamicConstVectorRef
*/

#include <cisstVector/vctDynamicConstVectorBase.h>
#include <cisstVector/vctDynamicVectorRefOwner.h>

/*!
  \ingroup cisstVector
  \brief Dynamic vector referencing existing memory (const)

  The class vctDynamicConstVectorRef allows to create a vector
  overlaying an existing block of memory.  It can be used to
  manipulate a container created by another toolkit (i.e. OpenGL, vtk,
  ...) or a cisstVector container using different parameters
  (different size, storage order or stride).

  To setup the overlay, one can use either the constructors or the
  SetRef methods.  When used with a cisst container, the SetRef
  methods can infer some of the memory layout information (pointer on
  first element, size, stride).  When used to overlay existing memory
  referenced by a pointer, the user has to provide all the required
  information.  In any case, the Ref object doesn't allocate nor free
  memory.

  Finally, as this overlay is const, it can be used to make sure the
  referenced content is not modified.

  For example to manipulate the first five elements of a vector, one
  can do:
  \code
  vctDynamicVector<double> wholeVector(10);
  vctRandom(wholeVector, -10.0, 10.0);
  vctDynamicConstVectorRef<double> first5;
  first5.SetRef(wholeVector, 0, 5);
  std::cout << first5 << " " << first5.SumOfElements() << std::endl;
  \endcode

  \note Make sure the underlying memory is not freed after the Ref
  object is created and before it is used.  This would lead to faulty
  memory access and potential bugs.

  \note vctDynamicConstVectorRef only performs const operations even
  if it stores a non-const <code>value_type *</code>.  It can be
  initialized with either <code>value_type *</code> or <code>const
  value_type *</code>.  For a non-const Ref, see vctDynamicVectorRef.

  \sa vctDynamicVector, vctDynamicVectorRef

  \param _elementType Type of elements referenced.  Also defined as
  <code>value_type</code>.
*/
template <class _elementType>
class vctDynamicConstVectorRef :
    public vctDynamicConstVectorBase<vctDynamicVectorRefOwner<_elementType>, _elementType>
{
public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctDynamicConstVectorRef<value_type> ThisType;
    typedef vctDynamicVectorRefOwner<value_type> VectorOwnerType;
    typedef vctDynamicConstVectorBase<vctDynamicVectorRefOwner<value_type>, _elementType> BaseType;
    typedef typename BaseType::CopyType CopyType;
    typedef typename VectorOwnerType::iterator iterator;
    typedef typename VectorOwnerType::const_iterator const_iterator;
    typedef typename VectorOwnerType::reverse_iterator reverse_iterator;
    typedef typename VectorOwnerType::const_reverse_iterator const_reverse_iterator;

    /*! Default constructor initializes the vector reference to NULL */
    vctDynamicConstVectorRef()
    {}

    /*! Copy constructor */
    vctDynamicConstVectorRef(const ThisType & other):
        BaseType()
    {
        SetRef(other.size(), other.Pointer(), other.stride());
    }

    vctDynamicConstVectorRef(size_type size, pointer data, stride_type stride = 1) {
        SetRef(size, data, stride);
    }

    vctDynamicConstVectorRef(size_type size, const_pointer data, stride_type stride = 1) {
        SetRef(size, data, stride);
    }

    /*! Initialize a dynamic reference to a fixed-size vector.
      \note This constructor is non-explicit, and allows automatic conversion
      from a fixed-size vector to a dynamic vector representation.
      \note The size and stride values are taken from the fixed size vector.
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    vctDynamicConstVectorRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector)
    {
        SetRef(otherVector);
    }

    /*! Initialize a dynamic reference to specified subvector of a fixed-size vector.
      \note This constructor allows automatic conversion
      from a fixed-size vector to a dynamic vector representation.
      \note The stride values are taken from the fixed size vector, but the starting point and
      length must be specified.
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    vctDynamicConstVectorRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                             size_type startPosition, size_type length)
    {
        SetRef(otherVector, startPosition, length);
    }

    /*! Initialize a dynamic reference to a dynamic vector.
      \note the starting point, size, and stride, are taken from the other vector.
    */
    template <class __vectorOwnerType>
    vctDynamicConstVectorRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        SetRef(otherVector);
    }

    /*! Initialize a dynamic reference to a specified subvector of a dynamic vector.
      \note the stride is taken from the other vector, but the starting point and the
      length must be specified.
    */
    template <class __vectorOwnerType>
    vctDynamicConstVectorRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
                             size_type startPosition, size_type length)
    {
        SetRef(otherVector, startPosition, length);
    }

    void SetRef(size_type size, const_pointer p, stride_type stride = 1) {
        this->Vector.SetRef(size, const_cast<pointer>(p), stride);
    }

    /*! Set a dynamic reference to a fixed-size vector
      \note the size and memory stride of this reference will be equal to the
      size and memory stride of the input vector.
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    void SetRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector)
    {
        SetRef(otherVector.size(), otherVector.Pointer(), otherVector.stride());
    }

    /*! Set a dynamic reference to a specified subvector of a fixed-size vector
      \note the memory stride of this reference will be equal to the
      memory stride of the input vector.
      \note This method verifies that the size of this vector does not exceed the
      size of the input vector (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    void SetRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                size_type startPosition, size_type length) CISST_THROW(std::out_of_range)
    {
        if (startPosition + length > otherVector.size()) {
            cmnThrow(std::out_of_range("vctDynamicConstVectorRef SetRef out of range"));
        }
        SetRef(length, otherVector.Pointer(startPosition), otherVector.stride());
    }

    /*! Set a dynamic reference to a dynamic vector
      \note the size and memory stride of this reference will be equal to the
      size memory stride of the input vector.
    */
    template <class __vectorOwnerType>
    void SetRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        SetRef(otherVector.size(), otherVector.Pointer(), otherVector.stride());
    }

    /*! Set a dynamic reference to a specified subvector of dynamic vector
      \note the memory stride of this reference will be equal to the
      memory stride of the input vector.
      \note This method verifies that the size of this vector does not exceed the
      size of the input vector (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <class __vectorOwnerType>
    void SetRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
                size_type startPosition, size_type length) CISST_THROW(std::out_of_range)
    {
        if (startPosition + length > otherVector.size()) {
            cmnThrow(std::out_of_range("vctDynamicConstVectorRef SetRef out of range"));
        }
        SetRef(length, otherVector.Pointer(startPosition), otherVector.stride());
    }

};


#endif // _vctDynamicConstVectorRef_h

