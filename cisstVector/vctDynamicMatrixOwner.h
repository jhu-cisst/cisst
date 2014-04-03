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
#ifndef _vctDynamicMatrixOwner_h
#define _vctDynamicMatrixOwner_h

/*!
  \file
  \brief Declaration of vctDynamicMatrixOwner
*/

#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctVarStrideMatrixIterator.h>
#include <cisstVector/vctDynamicMatrixRefOwner.h>

/*!
  This templated class owns a dynamically allocated array, but does
  not provide any other operations */
template<class _elementType>
class vctDynamicMatrixOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 2};
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    typedef vctDynamicMatrixOwner<value_type> ThisType;

    /* iterators are container specific */
    typedef vctVarStrideMatrixConstIterator<value_type> const_iterator;
    typedef vctVarStrideMatrixConstIterator<value_type> const_reverse_iterator;
    typedef vctVarStrideMatrixIterator<value_type> iterator;
    typedef vctVarStrideMatrixIterator<value_type> reverse_iterator;

    vctDynamicMatrixOwner():
        SizesMember(0, 0),
        StridesMember(0, 1),
        RowMajor(VCT_DEFAULT_STORAGE),
        Data(0)
    {}

    vctDynamicMatrixOwner(const nsize_type & newSizes, bool rowMajor = VCT_DEFAULT_STORAGE):
        SizesMember(0, 0),
        StridesMember(0, 1),
        RowMajor(VCT_DEFAULT_STORAGE),
        Data(0)
    {
        SetSize(newSizes, rowMajor);
    }

    vctDynamicMatrixOwner(size_type rows, size_type cols, bool rowMajor = VCT_DEFAULT_STORAGE):
        SizesMember(0, 0),
        StridesMember(0, 1),
        RowMajor(VCT_DEFAULT_STORAGE),
        Data(0)
    {
        SetSize(nsize_type(rows, cols), rowMajor);
    }

    ~vctDynamicMatrixOwner() {
        Disown();
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
        return Data + rowIndex * row_stride() + colIndex * col_stride();
    }

    pointer Pointer(void) {
        return Data;
    }

    const_pointer Pointer(index_type rowIndex, index_type colIndex) const {
        return Data + rowIndex * row_stride() + colIndex * col_stride();
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

    /*!  Non-preserving resize operation.  This method discards of all
      the current data of the dynamic array and allocates new space in
      the requested size.

      \note If the storage order and the sizes (both rows and columns)
      are unchanged, this method does nothing.

      \note If the size is set to zero, the data pointer is set to
      null (0).
     */
    //@{
    void  SetSize(size_type rows, size_type cols, bool rowMajor) {
        this->SetSize(nsize_type(rows, cols), rowMajor);
    }

    void SetSize(const nsize_type & newSizes, bool rowMajor) {
        if ((newSizes == this->sizes()) && (rowMajor == RowMajor)) return;
        Disown();
        const size_type totalSize = newSizes.ProductOfElements();
        Own(newSizes, rowMajor, (totalSize == 0) ? 0 : new value_type[totalSize]);
    }
    //@}

    /*! Release the currently owned data pointer from being owned.
      Reset this owner's data pointer and size to zero.  Return the
      old data pointer without freeing memory.
     */
    pointer Release() {
        pointer oldData = Data;
        Data = 0;
        SizesMember.SetAll(0);
        RowMajor = VCT_DEFAULT_STORAGE;
        return oldData;
    }

    /*! Have this owner take ownership of a new data pointer. Return
      the old data pointer without freeing memory.

      \note This method returns a pointer to the previously owned
      memory block but doesn't tell if the old block was row or column
      major nor the size of the block.
    */
    //@{
    pointer Own(size_type rows, size_type cols, bool rowMajor, pointer data) {
        return this->Own(nsize_type(rows, cols), rowMajor, data);
    }

    pointer Own(const nsize_type & newSizes, bool rowMajor, pointer data) {
        pointer oldData = Data;
        SizesMember.Assign(newSizes);
        StridesMember.Element(0) = rowMajor ? this->cols() : 1;
        StridesMember.Element(1) = rowMajor ? 1 : this->rows();
        RowMajor = rowMajor;
        Data = data;
        return oldData;
    }
    //@}

    /*! Free the memory allocated for the data pointer.  Reset data
      pointer and size to zero.
    */
    void Disown(void) {
        delete[] Data;
        SizesMember.SetAll(0);
        StridesMember.Element(0) = RowMajor ? 0 : 1;
        StridesMember.Element(1) = RowMajor ? 1 : 0;
        Data = 0;
    }

    inline bool IsColMajor(void) const {
        return !RowMajor;
    }

    inline bool IsRowMajor(void) const {
        return RowMajor;
    }

    inline bool IsCompact(void) const {
        return true;
    }

    inline bool StorageOrder(void) const {
        return RowMajor;
    }

protected:
    nsize_type SizesMember;
    nstride_type StridesMember;
    bool RowMajor;
    value_type* Data;

private:
    // copy constructor private to prevent any call
    vctDynamicMatrixOwner(const ThisType & CMN_UNUSED(other)) {};
};


#endif // _vctDynamicMatrixOwner_h

