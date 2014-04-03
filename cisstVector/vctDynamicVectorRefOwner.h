/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicVectorRefOwner_h
#define _vctDynamicVectorRefOwner_h

/*!
  \file
  \brief Declaration of vctDynamicVectorRefOwner
*/

#include <cisstVector/vctVarStrideVectorIterator.h>

/*!
  \ingroup cisstVector

  This templated class stores a pointer, a size, and a stride, and
  allows element access, but does not provide any other operations,
  and does not own the data */
template<class _elementType>
class vctDynamicVectorRefOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! The type of this owner. */
    typedef vctDynamicVectorRefOwner<_elementType> ThisType;

    /* iterators are container specific */
    typedef vctVarStrideVectorConstIterator<value_type> const_iterator;
    typedef vctVarStrideVectorIterator<value_type> iterator;
    typedef vctVarStrideVectorConstIterator<value_type> const_reverse_iterator;
    typedef vctVarStrideVectorIterator<value_type> reverse_iterator;


    vctDynamicVectorRefOwner()
        : Size(0)
        , Stride(1)
        , Data(0)
    {}

    vctDynamicVectorRefOwner(size_type size, value_type * data, stride_type stride = 1)
        : Size(size)
        , Stride(stride)
        , Data(data)
    {}

    void SetRef(size_type size, value_type * data, stride_type stride = 1)
    {
        Size = size;
        Stride = stride;
        Data = data;
    }

    size_type size(void) const {
        return Size;
    }

    stride_type stride(void) const {
        return Stride;
    }

    pointer Pointer(index_type index = 0) {
        return Data + Stride * index;
    }

    const_pointer Pointer(index_type index = 0) const {
        return Data + Stride * index;
    }

    const_iterator begin(void) const {
        return const_iterator(Data, Stride);
    }

    const_iterator end(void) const {
        return const_iterator(Data + Size * Stride, Stride);
    }

    iterator begin(void) {
        return iterator(Data, Stride);
    }

    iterator end(void) {
        return iterator(Data + Size * Stride, Stride);
    }

    const_reverse_iterator rbegin(void) const {
      return const_reverse_iterator(Data + (Size-1) * Stride, -Stride);
    }

    const_reverse_iterator rend(void) const {
      return const_reverse_iterator(Data - Stride, -Stride);
    }

    reverse_iterator rbegin(void) {
      return reverse_iterator(Data + (Size-1) * Stride, -Stride);
    }

    reverse_iterator rend(void) {
      return reverse_iterator(Data - Stride, -Stride);
    }

protected:
    size_type Size;
    stride_type Stride;
    value_type* Data;

private:
    // copy constructor private to prevent any call
    vctDynamicVectorRefOwner(const ThisType & CMN_UNUSED(other)) {};

};


#endif // _vctDynamicVectorRefOwner_h

