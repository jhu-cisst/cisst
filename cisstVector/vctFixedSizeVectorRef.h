/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-09-30

  (C) Copyright 2003-2015 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeVectorRef_h
#define _vctFixedSizeVectorRef_h

/*!
  \file
  \brief Declaration of vctFixedSizeVectorRef
 */

#include <cisstVector/vctFixedSizeConstVectorRef.h>


/*!  \brief An implementation of the ``abstract''
  vctFixedSizeVectorBase.

  \ingroup cisstVector

  This implementations uses a pointer to the vector beginning as the
  vector defining data member.  An instantiation of this type can be
  used as a subsequence with GetSubsequence().

  See the base class (vctFixedSizeVectorBase) for template
  parameter details.

  \sa vctFixedSizeConstVectorRef
*/
template <class _elementType, vct::size_type _size, vct::stride_type _stride>
class vctFixedSizeVectorRef : public vctFixedSizeVectorBase<
    _size, _stride, _elementType,
    typename vctFixedSizeVectorTraits<_elementType, _size, _stride>::pointer >
{
 public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctFixedSizeVectorTraits<_elementType, _size, _stride> VectorTraits;
    typedef vctFixedSizeVectorRef<value_type, _size, _stride> ThisType;
    typedef vctFixedSizeVectorBase<_size, _stride, value_type, pointer> BaseType;
    typedef typename BaseType::CopyType CopyType;


    /*! Default constructor: create an uninitialized vector */
    vctFixedSizeVectorRef() {}


    /*! Initialize the vector with a (non-const) pointer */
    vctFixedSizeVectorRef(pointer p) {
        SetRef(p);
    }

    vctFixedSizeVectorRef(const ThisType & other) {
        SetRef(other);
    }

    /*! Initialize a fixed size reference to a fixed-size vector.
      \note This constructor is explicit.
      \note The stride values are taken from the fixed size vector.
    */
    template <size_type __size, class __dataPtrType>
    explicit vctFixedSizeVectorRef(vctFixedSizeVectorBase<__size, _stride, _elementType, __dataPtrType> & otherVector,
                                   size_type startPosition = 0)
    {
        SetRef(otherVector, startPosition);
    }

    /*! Initialize a fixed size reference to a dynamic vector.
      \note This constructor is declared as explicit, since it is atypical.
    */
    template<class __vectorOwnerType>
    explicit vctFixedSizeVectorRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector,
                                   size_type startPosition = 0)
    {
        SetRef(otherVector, startPosition);
    }


    /*! Assign the vector start with a (non-const) pointer */
    void SetRef(pointer p) {
        this->Data = p;
    }

    void SetRef(const ThisType & other) {
        this->SetRef(other.Data);
    }

    /*! Set a fixed size reference to a fixed-size vector.
      \note the stride of the input vector must be identical to the stride of this
      vector (this is enforced by the template parameters).
      \note this vector must be contained in the input vector, that is, startPos+_size <= __size
      (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <size_type __size, class __dataPtrType>
    void SetRef(vctFixedSizeVectorBase<__size, _stride, _elementType, __dataPtrType> & otherVector,
                size_type startPosition = 0)
    {
        if (startPosition + this->size() > otherVector.size()) {
            cmnThrow(std::out_of_range("vctFixedSizeVectorRef SetRef out of range"));
        }
        SetRef(otherVector.Pointer(startPosition));
    }

    /*! Set a fixed size reference to a dynamic vector.
      \note the stride of the input vector must be identical to the stride of this
      vector (otherwise cmnThrow is used to throw std::runtime_error).
      \note this vector must be contained in the input vector, that is, startPos+_size <= __size
      (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template<class __vectorOwnerType>
    void SetRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector,
                size_type startPosition = 0)
    {
        if (this->stride() != otherVector.stride()) {
            cmnThrow(std::runtime_error("vctFixedSizeVectorRef SetRef with incompatible stride"));
        }
        if (startPosition + this->size() > otherVector.size()) {
            cmnThrow(std::out_of_range("vctFixedSizeVectorRef SetRef out of range"));
        }
        SetRef(otherVector.Pointer(startPosition));
    }


    /*!
      Assignment operation between vectors of different types

      \param other The vector to be copied.
    */
    //@{
    inline ThisType & operator = (const ThisType & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <stride_type __stride>
    inline ThisType & operator = (const vctFixedSizeConstVectorRef<value_type, _size, __stride> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & operator = (const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <class __vectorOwnerType>
    inline ThisType & operator = (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }
    //@}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }
};

#endif  // _vctFixedSizeVectorRef_h
