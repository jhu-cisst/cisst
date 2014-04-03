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
#ifndef _vctDynamicMatrixRefOwner_h
#define _vctDynamicMatrixRefOwner_h

/*!
  \file
  \brief Declaration of vctDynamicMatrixRefOwner
*/

#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctVarStrideMatrixIterator.h>
#include <cisstVector/vctDynamicMatrixOwner.h>

/*!
  This templated class stores a pointer, a size, and a stride, and
  allows element access, but does not provide any other operations,
  and does not own the data */
template<class _elementType>
class vctDynamicMatrixRefOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 2};
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    typedef vctDynamicMatrixRefOwner<value_type> ThisType;

    /* iterators are container specific */
    typedef vctVarStrideMatrixConstIterator<value_type> const_iterator;
    typedef vctVarStrideMatrixConstIterator<value_type> const_reverse_iterator;
    typedef vctVarStrideMatrixIterator<value_type> iterator;
    typedef vctVarStrideMatrixIterator<value_type> reverse_iterator;


    vctDynamicMatrixRefOwner():
        SizesMember(0, 0),
        StridesMember(1, 1),
        Data(0)
    {
        this->UpdateCachedData();
    }

    vctDynamicMatrixRefOwner(size_type rows, size_type cols,
                             stride_type rowStride, stride_type colStride,
                             pointer data):
        SizesMember(nsize_type(rows, cols)),
        StridesMember(nstride_type(rowStride, colStride)),
        Data(data)
    {
        this->UpdateCachedData();
    }

    void SetRef(size_type rows, size_type cols,
                pointer dataPointer,
                bool storageOrder) {
        this->SetRef(rows, cols,
                     storageOrder == VCT_ROW_MAJOR ? cols : 1,
                     storageOrder == VCT_ROW_MAJOR ? 1 : rows,
                     dataPointer);
    }

    void SetRef(const nsize_type & sizes,
                pointer dataPointer,
                bool storageOrder) {
        this->SetRef(sizes,
                     storageOrder == VCT_ROW_MAJOR ? sizes.Element(1) : 1,
                     storageOrder == VCT_ROW_MAJOR ? 1 : sizes.Element(0),
                     dataPointer);
    }

    void SetRef(size_type rows, size_type cols,
                stride_type rowStride, stride_type colStride,
                pointer data) {
        this->SetRef(nsize_type(rows, cols), nstride_type(rowStride, colStride), data);
    }

    void SetRef(const nsize_type & sizes,
                stride_type rowStride, stride_type colStride,
                pointer data) {
        this->SetRef(sizes, nstride_type(rowStride, colStride), data);
    }

    void SetRef(const nsize_type & sizes, const nstride_type & strides, pointer data) {
        SizesMember.Assign(sizes);
        StridesMember.Assign(strides);
        Data = data;
        this->UpdateCachedData();
    }

    size_type size(void) const {
        return SizesMember.ProductOfElements();
    }

    const nsize_type & sizes(void) const {
        return SizesMember;
    }

    size_type rows(void) const {
        return SizesMember.Element(0);
    }

    size_type cols(void) const {
        return SizesMember.Element(1);
    }

    const nstride_type & strides(void) const {
        return StridesMember;
    }

    difference_type row_stride(void) const {
        return StridesMember.Element(0);
    }

    difference_type col_stride(void) const {
        return StridesMember.Element(1);
    }

    pointer Pointer(index_type rowIndex, index_type colIndex) {
        return Data +  rowIndex * row_stride() + colIndex * col_stride();
    }

    pointer Pointer(void) {
        return Data;
    }

    const_pointer Pointer(index_type rowIndex, index_type colIndex) const {
        return Data +  rowIndex * row_stride() + colIndex * col_stride();
    }

    const_pointer Pointer(void) const {
        return Data;
    }

    const_iterator begin(void) const {
        return const_iterator(Data, col_stride(), cols(), row_stride());
    }

    const_iterator end(void) const {
        return const_iterator(Data, col_stride(), cols(), row_stride())
            + rows() * cols();
    }

    iterator begin(void) {
        return iterator(Data, col_stride(), cols(), row_stride());
    }

    iterator end(void) {
        return iterator(Data, col_stride(), cols(), row_stride())
            + rows() * cols();
    }

    const_reverse_iterator rbegin(void) const {
        return const_reverse_iterator(Data + row_stride() * (rows() - 1) + col_stride() * (cols() - 1),
                                      -col_stride(), cols(), -row_stride());
    }

    const_reverse_iterator rend(void) const {
        return const_reverse_iterator(Data - row_stride() + col_stride() * (cols() - 1),
                                      -col_stride(), cols(), -row_stride());
    }

    reverse_iterator rbegin(void) {
        return reverse_iterator(Data + row_stride() * (rows() - 1) + col_stride() * (cols() - 1),
                                -col_stride(), cols(), -row_stride());
    }

    reverse_iterator rend(void) {
        return reverse_iterator(Data - row_stride() + col_stride() * (cols() - 1),
                                -col_stride(), cols(), -row_stride());
    }

    inline bool IsColMajor(void) const {
        return (this->row_stride() <= this->col_stride());
    }

    inline bool IsRowMajor(void) const {
        return (this->col_stride() <= this->row_stride());
    }

    inline bool IsCompact(void) const {
        return IsCompactMember;
    }

    inline bool StorageOrder(void) const {
        return this->IsRowMajor();
    }

 protected:
    nsize_type SizesMember;
    nstride_type StridesMember;
    bool IsCompactMember;
    value_type* Data;

    inline void UpdateCachedData(void) {
        IsCompactMember = (((this->row_stride() == 1) && (this->col_stride() == static_cast<stride_type>(this->rows())))
                           || ((this->col_stride() == 1) && (this->row_stride() == static_cast<stride_type>(this->cols()))));
    }

private:
    // copy constructor private to prevent any call
    vctDynamicMatrixRefOwner(const ThisType & CMN_UNUSED(other)) {};

};


#endif // _vctDynamicMatrixRefOwner_h

