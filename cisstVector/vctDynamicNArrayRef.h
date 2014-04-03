/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Li, Ofri Sadowsky, Anton Deguet
  Created on: 2006-07-13

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicNArrayRef_h
#define _vctDynamicNArrayRef_h

/*!
  \file
  \brief Declaration of vctDynamicNArrayRef
*/

#include <cisstVector/vctDynamicNArrayBase.h>
#include <cisstVector/vctDynamicNArrayRefOwner.h>
#include <cisstVector/vctDynamicConstNArrayRef.h>

/*!
  \ingroup cisstVector
  \brief Dynamic nArray referencing existing memory

  The class vctDynamicNArrayRef allows to create a nArray overlaying
  an existing block of memory.  It can be used to manipulate a
  container created by another toolkit (i.e. OpenGL, vtk, ...) or a
  cisstVector container using different parameters (different size,
  storage order or stride).

  To setup the overlay, one can use either the constructors or the
  SetRef methods.  When used with a cisst container, the SetRef
  methods can infer some of the memory layout information (pointer on
  first element, sizes, strides).  When used to overlay existing
  memory referenced by a pointer, the user has to provide all the
  required information.  In any case, the Ref object doesn't allocate
  nor free memory.

  For example to manipulate the first five elements of a nArray, one
  can do:
  \code
  vctDynamicNArray<double, 2> wholeNArray(size_type(10, 10));
  vctRandom(wholeNArray, -10.0, 10.0);
  vctDynamicNArrayRef<double, 2> first5x5;
  first5x5.SetRef(wholeNArray, size_type(0, 0), size_type(5, 5));
  first5x5.SetAll(5.0);
  \endcode

  \note On top of the SetRef methods provided for the cisst vectors
  and matrices (see vctDynamicVectorRef and vctDynamicMatrixRef), the
  ref classes for nArrays provides the methods SubarrayOf (re-size,
  same dimension), PermutationOf (same dimension, same sizes but
  different ordering of dimensions), SliceOf (reduce dimension by 1).
  By combining different Refs created using a mix of these methods,
  one can create any desired overlay.

  \note Make sure the underlying memory is not freed after the Ref
  object is created and before it is used.  This would lead to faulty
  memory access and potential bugs.

  \note vctDynamicNArrayRef can perform const and non-const
  operations.  It can be initialized with a non-const object only
  (<code>value_type *</code>).  For a const Ref, see
  vctDynamicConstNArrayRef.

  \sa vctDynamicNArray, vctDynamicConstNArrayRef

  \param _elementType Type of elements referenced.  Also defined as
  <code>value_type</code>.

  \param _dimension Dimension of the nArray ref.
*/
template <class _elementType, vct::size_type _dimension>
class vctDynamicNArrayRef:
    public vctDynamicNArrayBase<vctDynamicNArrayRefOwner<_elementType, _dimension>, _elementType, _dimension>
{
public:
    /* define most types from vctContainerTraits and vctNArrayTraits */
    enum {DIMENSION = _dimension};
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(DIMENSION);

    /* documented in base class */
    typedef vctDynamicNArrayRef<_elementType, DIMENSION> ThisType;
    typedef vctDynamicNArrayRefOwner<_elementType, DIMENSION> NArrayOwnerType;
    typedef vctDynamicNArrayBase<vctDynamicNArrayRefOwner<_elementType, DIMENSION>, _elementType, DIMENSION> BaseType;
    typedef typename NArrayOwnerType::iterator iterator;
    typedef typename NArrayOwnerType::const_iterator const_iterator;
    typedef typename NArrayOwnerType::reverse_iterator reverse_iterator;
    typedef typename NArrayOwnerType::const_reverse_iterator const_reverse_iterator;


    vctDynamicNArrayRef()
    {
        this->SetRef(0, nsize_type(0), nstride_type(0));
    }

    /*! Copy constructor.  This copy constructor takes a const
      reference on the object to copy but has to remove the const-ness
      of the point to create a vctDynamicNArrayRef and allow it to
      manipulate elements.  The explict use of a const_cast is far
      from great but there is no easy solution as C++ requires the
      copy constructor to take a const reference. */
    vctDynamicNArrayRef(const ThisType & other):
        BaseType()
    {
        this->SetRef(const_cast<pointer>(other.Pointer()), other.sizes(), other.strides());
    }

    /*! Create a reference to a memory location.
        \note see corresponding SetRef method below
     */
    vctDynamicNArrayRef(pointer dataPointer, const nsize_type & sizes, const nstride_type & strides)
    {
        this->SetRef(dataPointer, sizes, strides);
    }

    /*! Create a reference to an nArray.
        \note see corresponding SetRef method below
     */
    template <class __ownerType>
    inline vctDynamicNArrayRef(vctDynamicNArrayBase<__ownerType, _elementType, DIMENSION> & otherNArray)
    {
        this->SetRef(otherNArray);
    }

    /*! Create a reference to a dynamic vector.
        \note see corresponding SetRef method below
     */
    template <class __ownerType>
    inline vctDynamicNArrayRef(vctDynamicConstVectorBase<__ownerType, _elementType> & vector)
    {
        this->SetRef(vector);
    }

    /*! Create a reference to a fixed-size vector.
        \note see corresponding SetRef method below
     */
    template <size_type __size, stride_type __stride, typename __dataPtrType>
    inline vctDynamicNArrayRef(vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & vector)
    {
        this->SetRef(vector);
    }

    /*! Create a reference to a dynamic matrix.
        \note see corresponding SetRef method below
     */
    template <class __ownerType>
    inline vctDynamicNArrayRef(vctDynamicConstMatrixBase<__ownerType, _elementType> & matrix)
    {
        this->SetRef(matrix);
    }

    /*! Create a reference to a fixed-size matrix.
        \note see corresponding SetRef method below
     */
    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, typename __dataPtrType>
    inline vctDynamicNArrayRef(vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & matrix)
    {
        this->SetRef(matrix);
    }


    /*! Set a reference to a memory location.  The user should specify all the parameters
        of the referenced memory, including a start memory address.
     */
    void SetRef(pointer dataPointer, const nsize_type & sizes, const nstride_type & strides)
    {
        this->NArray.SetRef(dataPointer, sizes, strides);
    }

    /*! Set a reference to an nArray. The reference will have identical dimensions,
        sizes and strides as the input nArray.
     */
    template <class __ownerType>
    inline void SetRef(vctDynamicNArrayBase<__ownerType, _elementType, DIMENSION> & otherNArray)
    {
        this->SetRef(otherNArray.Pointer(), otherNArray.sizes(), otherNArray.strides());
    }

    /*! Set a reference to a subarray of an nArray.
        \param otherNArray the nArray over which to set this reference
        \param startPosition vector of indices of the element position where the reference will start
        \param lengths the number of elements to reference in each dimension

        \note Preserves the number of dimensions.
     */
    template <class __ownerType>
    inline void SubarrayOf(vctDynamicNArrayBase<__ownerType, _elementType, DIMENSION> & otherNArray,
                           const nsize_type & startPosition,
                           const nsize_type & lengths)
    {
        CMN_ASSERT((startPosition + lengths).LesserOrEqual(otherNArray.sizes()));
        this->SetRef(otherNArray.Pointer(startPosition), lengths, otherNArray.strides());
    }

    /*! Set a reference to an nArray with a permutation of its strides.
        \param otherNArray the nArray over which to set this reference
        \param dimensions a sequence of dimension numbers representing the
            permuted order of strides

        \note Preserves the number of dimensions.
     */
    template <class __ownerType>
    inline void PermutationOf(vctDynamicNArrayBase<__ownerType, _elementType, DIMENSION> & otherNArray,
                              const ndimension_type & dimensions)
    {
        nsize_type sizes;
        nstride_type strides;

        const typename ndimension_type::const_iterator dimensionsBegin = dimensions.begin();
        const typename ndimension_type::const_iterator dimensionsEnd = dimensions.end();
        typename ndimension_type::const_iterator dimensionsIter;
        typename nsize_type::iterator sizesIter = sizes.begin();
        typename nstride_type::iterator stridesIter = strides.begin();
        for (dimensionsIter = dimensionsBegin;
             dimensionsIter != dimensionsEnd;
             ++dimensionsIter, ++sizesIter, ++stridesIter) {
            *sizesIter = otherNArray.size(*dimensionsIter);
            *stridesIter = otherNArray.stride(*dimensionsIter);
        }

        this->SetRef(otherNArray.Pointer(), sizes, strides);
    }

    /*! Set a reference to an (n-1)-dimension slice of an n-dimension nArray.
        \param otherNArray the nArray over which to set this reference
        \param dimension the dimension number of the direction by which to reduce
        \param slice the index number in the dimension specified by which to set
          the reference

        \note The number of dimensions is reduced by one.
     */
    template <class __ownerType>
    inline void SliceOf(vctDynamicNArrayBase<__ownerType, _elementType, DIMENSION + 1> & otherNArray,
                        dimension_type dimension,
                        size_type index)
    {
        // set up local sizes and strides
        nsize_type sizes;
        nstride_type strides;
        size_type i;

        typedef vctDynamicNArrayBase<__ownerType, value_type, DIMENSION + 1> OtherArrayType;
        typedef typename OtherArrayType::nsize_type other_nsize_type;
        typedef typename OtherArrayType::nstride_type other_nstride_type;

        // set up iterators
        const typename other_nsize_type::const_iterator otherSizesBegin = otherNArray.sizes().begin();
        typename other_nsize_type::const_iterator otherSizesIter;
        const typename other_nstride_type::const_iterator otherStridesBegin = otherNArray.strides().begin();
        typename other_nstride_type::const_iterator otherStridesIter;
        const typename nsize_type::iterator sizesBegin = sizes.begin();
        const typename nsize_type::iterator sizesEnd = sizes.end();
        typename nsize_type::iterator sizesIter;
        const typename nstride_type::iterator stridesBegin = strides.begin();
        typename nstride_type::iterator stridesIter;

        // copy sizes and strides in every dimension except the one specified
        otherSizesIter = otherSizesBegin;
        otherStridesIter = otherStridesBegin;
        sizesIter = sizesBegin;
        stridesIter = stridesBegin;
        i = 0;
        while (sizesIter != sizesEnd) {
            if (i == dimension) {
                ++otherSizesIter;
                ++otherStridesIter;
                ++i;
            } else {
                *sizesIter = *otherSizesIter;
                *stridesIter = *otherStridesIter;

                ++otherSizesIter;
                ++otherStridesIter;
                ++sizesIter;
                ++stridesIter;
                ++i;
            }
        }

        // set start position
        other_nsize_type startPosition(static_cast<size_type>(0));
        startPosition[dimension] = index;
        this->SetRef(otherNArray.Pointer(startPosition), sizes, strides);
    }

    /*! Set a reference to a dynamic vector. The reference will have
        identical size and stride as the input vector.
     */
    template <class __ownerType>
    inline void SetRef(vctDynamicConstVectorBase<__ownerType, _elementType> & vector)
    {
        this->SetRef( vector.Pointer(), nsize_type(vector.size()), nstride_type(vector.stride()) );
    }

    /*! Set a reference to a fixed-size vector. The reference will have
        identical size and stride as the input vector.
     */
    template <size_type __size, stride_type __stride, typename __dataPtrType>
    inline void SetRef(vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & vector)
    {
        this->SetRef( vector.Pointer(), nsize_type(__size), nstride_type(__stride) );
    }

    /*! Set a reference to a dynamic matrix. The reference will have
        identical sizes and strides as the input matrix.
     */
    template <class __ownerType>
    inline void SetRef(vctDynamicConstMatrixBase<__ownerType, _elementType> & matrix)
    {
        this->SetRef(matrix.Pointer(),
                     nsize_type(matrix.rows(), matrix.cols()),
                     nstride_type(matrix.row_stride(), matrix.col_stride()));
    }

    /*! Set a reference to a fixed-size matrix. The reference will have
        identical sizes and strides as the input matrix.
     */
    template <size_type __rows, size_type __cols, stride_type __rowStride, stride_type __colStride, typename __dataPtrType>
    inline void SetRef(vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & matrix)
    {
        this->SetRef(matrix.Pointer(),
                     nsize_type(__rows, __cols),
                     nstride_type(__rowStride, __colStride));
    }


    /*!
      \name Assignment operation into an nArray reference
      \param other the nArray to be copied.
    */
    //@{
    inline ThisType & operator = (const ThisType & other)
    {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    inline ThisType & operator = (const vctDynamicConstNArrayRef<_elementType, DIMENSION> & other)
    {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }

    template <class __nArrayOwnerType, typename __elementType>
    inline ThisType & operator = (const vctDynamicNArrayBase<__nArrayOwnerType, __elementType, DIMENSION> & other)
    {
        return reinterpret_cast<ThisType &>(this->Assign(other));
    }
    //@}

    /*! Assignment of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value)
    {
        this->SetAll(value);
        return *this;
    }

};


#endif // _vctDynamicNArrayRef_h

