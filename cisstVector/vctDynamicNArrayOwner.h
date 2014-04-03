/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Daniel Li
  Created on:	2006-06-27

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicNArrayOwner_h
#define _vctDynamicNArrayOwner_h

/*!
  \file
  \brief Declaration of vctDynamicNArrayOwner
*/

#include <cisstVector/vctForwardDeclarations.h>

/*!
  This templated class owns a dynamically allocated array, but does
  not provide any other operations */
template <class _elementType, vct::size_type _dimension>
class vctDynamicNArrayOwner
{
public:
    /* define most types from vctContainerTraits and vctNArrayTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(_dimension);

    enum {DIMENSION = _dimension};

    /*! The type of this owner. */
    typedef vctDynamicNArrayOwner<_elementType, _dimension> ThisType;

    /* iterators are container specific */
    typedef vctVarStrideNArrayIterator<ThisType, true> iterator;
    typedef vctVarStrideNArrayConstIterator<ThisType, true> const_iterator;
    typedef vctVarStrideNArrayIterator<ThisType, false> reverse_iterator;
    typedef vctVarStrideNArrayConstIterator<ThisType, false> const_reverse_iterator;


    vctDynamicNArrayOwner():
        SizesMember(0),
        StridesMember(0),
        Data(0)
    {}

    vctDynamicNArrayOwner(const nsize_type & sizes)
        : SizesMember(0)
        , StridesMember(0)
        , Data(0)
    {
        SetSize(sizes);
    }

    ~vctDynamicNArrayOwner()
    {
        Disown();
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
        if (DIMENSION == 0)
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
    stride_type stride(dimension_type dimension) const
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
    pointer Pointer(const nsize_type & indices)
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
        return (Data + StridesMember.DotProduct(indicesRef));
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

    bool IsCompact(void) const {
        return true;
    }

    /*!  Non-preserving resize operation.  This method discards of all
      the current data of the nArray and allocates new space in the
      requested size. Dimensionality remains unchanged.
      \param sizes Vector of sizes in each dimension. If this vector
      does not have the same number of elements as the nArray's
      dimension, this method does nothing.

      \note If the number of dimensions and the sizes of all dimensions
        are unchanged, this method does nothing.
     */
    void SetSize(const nsize_type & sizes)
    {
        if (SizesMember.Equal(sizes)) return;
        Disown();
        const size_type totalSize = sizes.ProductOfElements();
        Own(sizes, (totalSize == 0) ? 0 : new value_type[totalSize]);
    }

    /*! Release the currently owned data pointer from being owned.
        Reset this owner's data pointer and size to zero. Return the
        old data pointer without freeing memory.
     */
    value_type * Release(void)
    {
        value_type* oldData = Data;
        Data = 0;
        SizesMember.SetAll(0);
        UpdateStrides();
        return oldData;
    }

    /*! Have this owner take ownership of a new data pointer. Return the
        old data pointer without freeing memory.
        \return data pointer the nArray held before taking ownership of
           the new data pointer
        \note This method returns a pointer to the previously owned
           memory block but doesn't tell the size of the block.
     */
    value_type * Own(const nsize_type & sizes, value_type * data)
    {
        value_type* oldData = Data;
        Data = data;
        SizesMember.Assign(sizes);
        UpdateStrides();
        return oldData;
    }

    /*! Free the memory allocated for the data pointer.
        Set sizes in all dimensions to zero, but keep
        dimensionality. */
    void clear(void)
    {
        Disown();
    }

    /*! Free the memory allocated for the data pointer.
        Reset data pointer and size to zero. */
    void Disown(void)
    {
        delete[] Data;
        Data = 0;
        SizesMember.SetAll(0);
        UpdateStrides();
    }

protected:
    nsize_type SizesMember;
    nstride_type StridesMember;
    value_type* Data;

    /*! Update the StridesMember vector to reflect the elements of
        the SizesMember vector. */
    void UpdateStrides(void)
    {
        typename nsize_type::const_reverse_iterator sizesIter = SizesMember.rbegin();
        typename nstride_type::reverse_iterator stridesIter = StridesMember.rbegin();
        const typename nstride_type::const_reverse_iterator stridesEnd = StridesMember.rend();
        size_type sizesIter_value;
        stride_type previous_stride;

        if (stridesIter == stridesEnd)
            return;

        *stridesIter = 1;
        previous_stride = 1;
        ++stridesIter;

        for (; stridesIter != stridesEnd;
             ++stridesIter, ++sizesIter)
        {
            sizesIter_value = (*sizesIter == 0) ? 1 : *sizesIter;
            *stridesIter = sizesIter_value * previous_stride;
            previous_stride = *stridesIter;
        }
    }

private:
    // copy constructor private to prevent any call
    vctDynamicNArrayOwner(const ThisType & CMN_UNUSED(other)) {};

};


#endif // _vctDynamicNArrayOwner_h

