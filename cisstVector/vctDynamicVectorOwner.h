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
#ifndef _vctDynamicVectorOwner_h
#define _vctDynamicVectorOwner_h

/*!
  \file
  \brief Declaration of vctDynamicVectorOwner
*/

#include <cisstVector/vctFixedStrideVectorIterator.h>

/*!
  This templated class owns a dynamically allocated array, but does
  not provide any other operations */
template<class _elementType>
class vctDynamicVectorOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! The type of this owner. */
    typedef vctDynamicVectorOwner<_elementType> ThisType;

    /* iterators are container specific */
    enum { DEFAULT_STRIDE = 1 };
#ifndef SWIG
    typedef vctFixedStrideVectorConstIterator<value_type, DEFAULT_STRIDE> const_iterator;
    typedef vctFixedStrideVectorConstIterator<value_type, -DEFAULT_STRIDE> const_reverse_iterator;
    typedef vctFixedStrideVectorIterator<value_type, DEFAULT_STRIDE> iterator;
    typedef vctFixedStrideVectorIterator<value_type, -DEFAULT_STRIDE> reverse_iterator;
#endif // SWIG

    vctDynamicVectorOwner()
        : Size(0)
        , Data(0)
    {}

    vctDynamicVectorOwner(size_type size):
        Size(0)
    {
        SetSize(size);
    }

    ~vctDynamicVectorOwner() {
        Disown();
    }

    size_type size(void) const {
        return Size;
    }

    stride_type stride(void) const
    {
        return DEFAULT_STRIDE;
    }

    pointer Pointer(index_type index = 0) {
        return Data + index;
    }

    const_pointer Pointer(index_type index = 0) const {
        return Data + index;
    }

    const_iterator begin(void) const {
        return const_iterator(Data);
    }

    const_iterator end(void) const {
        return const_iterator(Data + Size);
    }

    iterator begin(void) {
        return iterator(Data);
    }

    iterator end(void) {
        return iterator(Data + Size);
    }

    const_reverse_iterator rbegin(void) const {
        return const_reverse_iterator(Data + Size - 1);
    }

    const_reverse_iterator rend(void) const {
        return const_reverse_iterator(Data - 1);
    }

    reverse_iterator rbegin(void) {
        return reverse_iterator(Data + Size-1);
    }

    reverse_iterator rend(void) {
        return reverse_iterator(Data - 1);
    }

    /*!  Non-preserving resize operation.  This method discards of all
      the current data of the dynamic array and allocates new space in
      the requested size.

      \note If the size is unchanged, this method does nothing.

      \note If the size is set to zero, the data pointer is set to
      null (0).
     */
    void SetSize(size_type size) {
        if (size == Size) return;
        Disown();
        Own(size, (size == 0) ? 0 : new value_type[size]);
    }

    /*! Release the currently owned data pointer from being owned.
      Reset this owner's data pointer and size to zero.  Return the
      old data pointer without freeing memory.
     */
    value_type * Release()
    {
        value_type * oldData = Data;
        Data = 0;
        Size = 0;
        return oldData;
    }

    /*! Have this owner take ownership of a new data pointer. Return
      the old data pointer without freeing memory.
    */
    value_type * Own(size_type size, value_type * data) {
        value_type * oldData = Data;
        Size = size;
        Data = data;
        return oldData;
    }

    /*! Free the memory allocated for the data pointer.  Reset data
      pointer and size to zero.
    */
    void Disown(void) {
        delete[] Data;
        Size = 0;
        Data = 0;
    }


protected:
    size_type Size;
    value_type* Data;

private:
    // copy constructor private to prevent any call
    vctDynamicVectorOwner(const ThisType & CMN_UNUSED(other)) {};

};


#endif // _vctDynamicVectorOwner_h

