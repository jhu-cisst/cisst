/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Li, Ofri Sadowsky, Anton Deguet
  Created on: 2006-07-06

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicNArrayRefOwner_h
#define _vctDynamicNArrayRefOwner_h

/*!
  \file
  \brief Declaration of vctDynamicNArrayRefOwner
*/

#include <cisstVector/vctVarStrideNArrayIterator.h>

/*!
  This templated class owns a dynamically allocated array, but does
  not provide any other operations */
template<class _elementType, vct::size_type _dimension>
class vctDynamicNArrayRefOwner
{
public:
    /* define most types from vctContainerTraits and vctNArrayTraits */
    enum {DIMENSION = _dimension};
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    /*! The type of this owner. */
    typedef vctDynamicNArrayRefOwner<_elementType, DIMENSION> ThisType;

    /* iterators are container specific */
    typedef vctVarStrideNArrayIterator<ThisType, true> iterator;
    typedef vctVarStrideNArrayConstIterator<ThisType, true> const_iterator;
    typedef vctVarStrideNArrayIterator<ThisType, false> reverse_iterator;
    typedef vctVarStrideNArrayConstIterator<ThisType, false> const_reverse_iterator;


    vctDynamicNArrayRefOwner():
        SizesMember(0),
        StridesMember(0),
        Data(0)
    {}

    vctDynamicNArrayRefOwner(pointer data,
                             const nsize_type & sizes,
                             const nstride_type & strides)
    {
        SetRef(data, sizes, strides);
    }

    void SetRef(pointer data,
                const nsize_type & sizes,
                const nstride_type & strides)
    {
        Data = data;
        SizesMember = sizes;
        StridesMember = strides;
        this->UpdateCachedData();
    }

    /*! Return the number of dimensions of the nArray.
        \return the number of dimensions of this nArray */
    dimension_type dimension(void) const
    {
        return DIMENSION;
    }

    /*! Return the size of each dimension.
        \return a vector containing the size of the nArray in each dimension */
    const nsize_type & sizes(void) const
    {
        return SizesMember;
    }

    /*! Return the size of a dimension.
        \param dimension the dimension of interest
        \return the size of the nArray in the given dimension

        \note the dimension is zero-based
     */
    size_type size(dimension_type dimension) const
    {
        if (dimension < DIMENSION)
            return SizesMember[dimension];
        else
            return 1;
    }

    /*! Return the size of the nArray.
        \return the number of elements

        \note This method returns 0 if the dimension
        of the nArray is zero.
     */
    size_type size(void) const
    {
        if (SizesMember.size() == 0)
            return 0;
        else
            return SizesMember.ProductOfElements();
    }

    /*! Return the strides of each dimension.
        \return a vector containing the strides of the nArray in each dimension */
    const nstride_type & strides(void) const
    {
        return StridesMember;
    }

    /*! Return the stride of a dimension.
        \param dimension the dimension of interest
        \return the stride of the nArray in the given dimension */
    difference_type stride(dimension_type dimension) const
    {
        if (dimension < DIMENSION)
            return StridesMember[dimension];
        else
            return 1;
    }

    /*! Return a pointer to the nArray's data.
        \return a pointer to this nArray's data */
    pointer Pointer(void)
    {
        return Data;
    }

    /*! Return a pointer to the nArray's data at the specified element.
        \return a pointer to this nArray's data at the specified element */
    pointer Pointer(const nindex_type & indices)
    {
        vctFixedSizeConstVectorRef<stride_type, DIMENSION, 1>
            indicesRef(reinterpret_cast<stride_type *>(const_cast<size_type *>(indices.Pointer())));
        return (Data + StridesMember.DotProduct(indicesRef));
    }

    /*! Return a const pointer to the nArray's data.
        \return a const pointer to this nArray's data */
    const_pointer Pointer(void) const
    {
        return Data;
    }

    /*! Return a const pointer to the nArray's data at the specified element.
        \return a const pointer to this nArray's data at the specified element */
    const_pointer Pointer(const nsize_type & indices) const
    {
        vctFixedSizeConstVectorRef<stride_type, DIMENSION, 1>
            indicesRef(reinterpret_cast<stride_type *>(const_cast<size_type *>(indices.Pointer())));
        return Data + StridesMember.DotProduct(indicesRef);
    }

    iterator begin(void)
    {
        return iterator(this, 0);
    }

    iterator end(void)
    {
        return iterator(this, 0) + size();
    }

    const_iterator begin(void) const
    {
        return const_iterator(this, 0);
    }

    const_iterator end(void) const
    {
        return const_iterator(this, 0) + size();
    }

    reverse_iterator rbegin(void)
    {
        size_type endIndex = size() - 1;
        return reverse_iterator(this, endIndex);
    }

    reverse_iterator rend(void)
    {
        return reverse_iterator(this, 0) + 1;
    }

    const_reverse_iterator rbegin(void) const
    {
        size_type endIndex = size() - 1;
        return const_reverse_iterator(this, endIndex);
    }

    const_reverse_iterator rend(void) const
    {
        return const_reverse_iterator(this, 0) + 1;
    }


    inline bool IsCompact(void) const
    {
        return IsCompactMember;
    }


protected:
    nsize_type SizesMember;
    nstride_type StridesMember;
    pointer Data;
    bool IsCompactMember;

    inline void UpdateCachedData(void) {
        /* sort strides and sizes, using Gnome sort as found on
           wikipedia and www.cs.vu.nl/~dick/gnomesort.html */
        nsize_type sorted_sizes(SizesMember);
        nstride_type sorted_strides(StridesMember);

        index_type i = 0;
        bool swap;
        while (i < this->dimension()) {
            swap = true;
            if (i == 0) {
                swap = false;
            } else {
                if (sorted_strides[i - 1] == sorted_strides[i]) {
                    if (sorted_sizes[i - 1] >= sorted_sizes[i]) {
                        swap = false;
                    }
                } else {
                    if (sorted_strides[i - 1] > sorted_strides[i]) {
                        swap = false;
                    }
                }
            }
            if (swap) {
                std::swap(sorted_strides[i - 1], sorted_strides[i]);
                std::swap(sorted_sizes[i - 1], sorted_sizes[i]);
                i--;
            } else {
                i++;
            }
        }

        /* check compactness for sorted strides and sizes */
        typename nsize_type::const_reverse_iterator sizesIter = sorted_sizes.rbegin();
        typename nstride_type::const_reverse_iterator stridesIter = sorted_strides.rbegin();
        const typename nstride_type::const_reverse_iterator stridesEnd = sorted_strides.rend();
        size_type sizesIter_value;
        stride_type previous_stride;

        if (stridesIter != stridesEnd) {
            if (*stridesIter != 1) {
                IsCompactMember = false;
                return;
            } else {
                previous_stride = 1;
                ++stridesIter;
            }
        }

        for (;
             stridesIter != stridesEnd;
             ++stridesIter, ++sizesIter) {
            sizesIter_value = (*sizesIter == 0) ? 1 : *sizesIter;
            if (*stridesIter != static_cast<stride_type>(sizesIter_value) * previous_stride) {
                IsCompactMember = false;
                return;
            } else {
                previous_stride = *stridesIter;
            }
        }
        IsCompactMember = true;
    }

private:
    // copy constructor private to prevent any call
    vctDynamicNArrayRefOwner(const ThisType & CMN_UNUSED(other)) {};

};


#endif // _vctDynamicNArrayRefOwner_h

