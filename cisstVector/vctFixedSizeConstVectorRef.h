/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on:	2003-09-30

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeConstVectorRef_h
#define _vctFixedSizeConstVectorRef_h

/*!
  \file
  \brief Declaration of vctFixedSizeConstVectorRef
 */

#include <cisstVector/vctFixedSizeVectorBase.h>


/*!  \brief An implementation of the ``abstract''
  vctFixedSizeConstVectorBase.

  \ingroup cisstVector

  This implementations uses a pointer to the vector beginning as the
  vector defining data member.  An instantiation of this type can be
  used as a subsequence with GetConstSubsequence().

  Note that the class provides only const operations, except for
  assigning the vector start, which does not affect the vector.
  This is despite the fact that the stored pointer is non-const.

  See the base class (vctFixedSizeConstVectorBase) for template
  parameter details.
*/
template <class _elementType, vct::size_type _size, vct::stride_type _stride>
class vctFixedSizeConstVectorRef : public vctFixedSizeConstVectorBase<
    _size, _stride, _elementType,
    typename vctFixedSizeVectorTraits<_elementType, _size, _stride>::pointer >
{
 public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctFixedSizeVectorTraits<_elementType, _size, _stride> VectorTraits;
    typedef vctFixedSizeConstVectorRef<value_type, _size, _stride> ThisType;
    typedef vctFixedSizeConstVectorBase<_size, _stride, value_type, pointer> BaseType;
    typedef typename BaseType::CopyType CopyType;


    /*! Default constructor: create an uninitialized vector */
    vctFixedSizeConstVectorRef() {}

    /*! Initialize the vector with a (non-const) pointer */
    vctFixedSizeConstVectorRef(pointer p) {
        SetRef(p);
    }

    /*! Initialize the vector with a const pointer.  This requires
      const_cast. */
    vctFixedSizeConstVectorRef(const_pointer p) {
        SetRef(p);
    }


    /*! Initialize a fixed size reference to a fixed-size vector.
      \note There is no need to declare this constructor as explicit, because a vctFixedSizeConstVectorRef
      protects the content.
      \note The stride values are taken from the fixed size vector.
    */
    template <size_type __size, class __dataPtrType>
    vctFixedSizeConstVectorRef(const vctFixedSizeConstVectorBase<__size, _stride, _elementType, __dataPtrType> & otherVector,
                               size_type startPosition = 0)
    {
        SetRef(otherVector, startPosition);
    }

    /*! Initialize a fixed size reference to a dynamic vector.
      \note This constructor is declared as explicit, since it is atypical.
    */
    template<class __vectorOwnerType>
    explicit vctFixedSizeConstVectorRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
                                        size_type startPosition = 0)
    {
        SetRef(otherVector, startPosition);
    }


    /*! Assign the vector start with a (non-const) pointer */
    void SetRef(pointer p) {
        this->Data = p;
    }


    /*! Assign the vector start with a const pointer.  This requires const_cast. */
    void SetRef(const_pointer p) {
        this->Data = const_cast<pointer>(p);
    }

    /*! Set a fixed size reference to a fixed-size vector.
      \note the stride of the input vector must be identical to the stride of this
      vector (this is enforced by the template parameters).
      \note this vector must be contained in the input vector, that is, startPos+_size <= __size
      (otherwise cmnThrow is used to throw std::out_of_range).
    */
    template <size_type __size, class __dataPtrType>
    void SetRef(const vctFixedSizeConstVectorBase<__size, _stride, _elementType, __dataPtrType> & otherVector,
                size_type startPosition = 0)
    {
        if (startPosition + this->size() > otherVector.size()) {
            cmnThrow(std::out_of_range("vctFixedSizeConstVectorRef SetRef out of range"));
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
    void SetRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
                size_type startPosition = 0)
    {
        if (this->stride() != otherVector.stride()) {
            cmnThrow(std::runtime_error("vctFixedSizeConstVectorRef SetRef with incompatible stride"));
        }
        if (startPosition + this->size() > otherVector.size()) {
            cmnThrow(std::out_of_range("vctFixedSizeConstVectorRef SetRef out of range"));
        }
        SetRef(otherVector.Pointer(startPosition));
    }

};


#endif  // _vctFixedSizeConstVectorRef_h

