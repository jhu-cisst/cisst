/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Daniel Li, Ofri Sadowsky
  Created on: 2006-06-23

  (C) Copyright 2006-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicConstNArrayBase_h
#define _vctDynamicConstNArrayBase_h

/*!
  \file
  \brief Declaration of vctDynamicConstNArrayBase
*/

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnAssert.h>

#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctDynamicNArrayLoopEngines.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctForwardDeclarations.h>


template <class _nArrayOwnerType, class __nArrayOwnerType, class _elementType,
          class _elementOperationType, vct::size_type _dimension>
inline vctReturnDynamicNArray<bool, _dimension>
vctDynamicNArrayElementwiseCompareNArray(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray1,
                                         const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray2);

template <class _nArrayOwnerType, class _elementType, class _elementOperationType, vct::size_type _dimension>
inline vctReturnDynamicNArray<bool, _dimension>
vctDynamicNArrayElementwiseCompareScalar(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray,
                                         const _elementType & scalar);


/* Functions used to generate slices based on dimension, i.e. if
   dimension is 1 return an element, otherwise return an NArray of
   lesser dimension.  The class below is used to specify which
   function is used. */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline vctDynamicNArrayRef<_elementType, _dimension - 1>
vctDynamicNArrayNArraySlice(vctDynamicNArrayBase<_nArrayOwnerType, _elementType, _dimension> & input,
                            vct::size_type dimension,
                            vct::index_type index);

template <class _nArrayOwnerType, class _elementType>
inline _elementType &
vctDynamicNArrayElementSlice(vctDynamicNArrayBase<_nArrayOwnerType, _elementType, 1> & input,
                             vct::index_type index);

template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline vctDynamicConstNArrayRef<_elementType, _dimension - 1>
vctDynamicNArrayConstNArraySlice(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & input,
                                 vct::size_type dimension,
                                 vct::index_type index);

template <class _nArrayOwnerType, class _elementType>
inline const _elementType &
vctDynamicNArrayConstElementSlice(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, 1> & input,
                                  vct::index_type index);

#ifndef SWIG
/* Class used to specify the type of a slice based on the dimension.
   The class also provides a static method to select the right
   function to call.  Code couldn't be inline since it requires a full
   definition of vctDynamicNArrayRef and vctDynamicConstNArrayRef.
   This can't be solved just with forward declarations. */
template <vct::size_type _dimension>
class vctDynamicNArrayTypes
{
public:
    template <class _elementType>
    class SlicesTypes
    {
    public:
        typedef vctDynamicConstNArrayRef<_elementType, _dimension - 1> ConstSliceRefType;
        typedef vctDynamicNArrayRef<_elementType, _dimension - 1> SliceRefType;

        template <class _nArrayOwnerType>
        static ConstSliceRefType ConstSliceOf(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & input,
                                              vct::size_type dimension, vct::index_type index) {
            return vctDynamicNArrayConstNArraySlice(input, dimension, index);
        }

        template <class _nArrayOwnerType>
        static SliceRefType SliceOf(vctDynamicNArrayBase<_nArrayOwnerType, _elementType, _dimension> & input,
                                    vct::size_type dimension, vct::index_type index) {
            return vctDynamicNArrayNArraySlice(input, dimension, index);
        }
    };
};

/* Specialisation of class for dimension 1.  In this case, a slice is
   a single element. */
template <>
class vctDynamicNArrayTypes<1>
{
public:
    template <class _elementType>
    class SlicesTypes
    {
    public:
        typedef const _elementType & ConstSliceRefType;
        typedef _elementType & SliceRefType;

        template <class _nArrayOwnerType>
        static ConstSliceRefType ConstSliceOf(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, 1> & input,
                                              vct::size_type CMN_UNUSED(dimension), vct::index_type index) {
            return vctDynamicNArrayConstElementSlice(input, index);
        }

        template <class _nArrayOwnerType>
        static SliceRefType SliceOf(vctDynamicNArrayBase<_nArrayOwnerType, _elementType, 1> & input,
                                    vct::size_type CMN_UNUSED(dimension), vct::index_type index) {
            return vctDynamicNArrayElementSlice(input, index);
        }
    };
};
#endif // SWIG


/*!
  This class is templated with the ``nArray owner type'', which may
  be a vctDynamicNArrayOwner or a vctDynamicNArrayRefOwner.  It provides
  const operations on the nArray, such as SumOfElements etc.

  nArray indexing is zero-based.

  nArray dimensions are zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _nArrayOwnerType the type of nArray owner
  \param _elementType the type of elements of the nArray
 */
template <class _nArrayOwnerType, typename _elementType, vct::size_type _dimension>
class vctDynamicConstNArrayBase
{
public:
    /* define most types from vctContainerTraits and vctNArrayTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(_dimension);

    /*! Type of the nArray itself. */
    typedef vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> ThisType;

    /*! Type of the data owner (dynamic array or pointer) */
    typedef _nArrayOwnerType OwnerType;

    enum {DIMENSION = OwnerType::DIMENSION};

    /*! \name Iterators on the elements of the nArray */
    //@{
    typedef typename OwnerType::iterator iterator;
    typedef typename OwnerType::const_iterator const_iterator;
    typedef typename OwnerType::reverse_iterator reverse_iterator;
    typedef typename OwnerType::const_reverse_iterator const_reverse_iterator;
    //@}

    /*! \name The type of the object representing a subarray of
      this nArray. Access is by (const and non const) reference. */
    //@{
    typedef vctDynamicConstNArrayRef<value_type, DIMENSION> ConstSubarrayRefType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION> SubarrayRefType;
    //@}

    /*! \name The type of the object representing a permutation of
      this nArray. Access is by (const and non const) reference. */
    //@{
    typedef vctDynamicConstNArrayRef<value_type, DIMENSION> ConstPermutationRefType;
    typedef vctDynamicNArrayRef<value_type, DIMENSION> PermutationRefType;
    //@}

    /*! \name The type of the object representing a slice of this
      nArray. Access is by (const and non const) reference. */
    //@{
#ifndef SWIG
    typedef typename vctDynamicNArrayTypes<DIMENSION>::template SlicesTypes<_elementType> SlicesTypes;
#endif // SWIG
    typedef typename SlicesTypes::ConstSliceRefType ConstSliceRefType;
    typedef typename SlicesTypes::SliceRefType SliceRefType;
    //@}


    /*! The type of nArray used to store by value results of the same
        type as this object. */
    typedef vctReturnDynamicNArray<value_type, DIMENSION> NArrayValueType;

    /*! The type of nArray returned by value from operations on this
        object */
    typedef vctDynamicNArray<value_type, DIMENSION> NArrayReturnType;

    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Define bool based on the container type to force some
      compilers (i.e. gcc 4.0) to delay the instantiation of the
      ElementWiseCompare methods. */
    typedef typename TypeTraits::BoolType BoolType;

    /*! The type of an nArray of booleans returned from operations on
      this object, e.g., ElementwiseEqual. */
    typedef vctReturnDynamicNArray<BoolType, DIMENSION> BoolNArrayReturnType;

    //@}


protected:
    /*! Declaration of the nArray-defining member object */
    OwnerType NArray;

    /*! Check the validity of the given dimension index */
    inline void ThrowUnlessValidDimensionIndex(dimension_type dimensionIndex) const
        CISST_THROW(std::out_of_range)
    {
        if (! ValidDimensionIndex(dimensionIndex))
        {
            cmnThrow(std::out_of_range("vctDynamicNArray: Invalid index"));
        }
    }

    /*! Check the validity of an index */
    inline void ThrowUnlessValidIndex(size_type index) const
        CISST_THROW(std::out_of_range)
    {
        if (! ValidIndex(index))
        {
            cmnThrow(std::out_of_range("vctDynamicNArray: Invalid index"));
        }
    }

    /*! Check the validity of the nArray indices */
    inline void ThrowUnlessValidIndex(const nsize_type & indices) const
        CISST_THROW(std::out_of_range)
    {
        if (! ValidIndex(indices))
        {
            cmnThrow(std::out_of_range("vctDynamicNArray: Invalid indices"));
        }
    }

    /*! Check the validity of the given index in the (dimension)th-dimension */
    inline void ThrowUnlessValidIndex(dimension_type dimension, size_type index) const
        CISST_THROW(std::out_of_range)
    {
        if (! ValidIndex(dimension, index))
        {
            cmnThrow(std::out_of_range("vctDynamicNArray: Invalid index"));
        }
    }


public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    const_iterator begin(void) const
    {
        return NArray.begin();
    }

    /*! Returns a const iterator on the last element (STL
      compatibility). */
    const_iterator end(void) const
    {
        return NArray.end();
    }

    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */
    const_reverse_iterator rbegin(void) const
    {
        return NArray.rbegin();
    }

    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */
    const_reverse_iterator rend(void) const
    {
        return NArray.rend();
    }

    /*! Return the number of elements in the nArray.  This is not
      equivalent to the difference between the end and the beginning.
    */
    size_type size(void) const
    {
        return NArray.size();
    }

    /*! Not required by STL but provided for completeness */
    const nsize_type & sizes(void) const
    {
        return NArray.sizes();
    }

    /*! Not required by STL but provided for completeness */
    size_type size(dimension_type dimension) const
    {
        return NArray.size(dimension);
    }

    /*! Not required by STL but provided for completeness */
    const nstride_type & strides(void) const
    {
        return NArray.strides();
    }

    /*! Not required by STL but provided for completeness */
    difference_type stride(dimension_type dimension) const
    {
        return NArray.stride(dimension);
    }

    /*! Not required by STL but provided for completeness */
    dimension_type dimension(void) const
    {
        return NArray.dimension();
    }

    /*! Tell if the narray is empty (STL compatibility).  False unless
      size is zero. */
    bool empty(void) const
    {
        return (size() == 0);
    }


    /*! Returns true if the given dimension value is equal to this
      nArray's dimension. */
    inline bool ValidDimension(dimension_type dimension) const
    {
        return (dimension == this->dimension());
    }

    /*! Returns true if the given dimension index is valid. */
    inline bool ValidDimensionIndex(dimension_type dimensionIndex) const
    {
        return (dimensionIndex < this->dimension());
    }

    /*! Returns true if the index is less than the number of
      elements of the nArray. */
    inline bool ValidIndex(size_type index) const
    {
        return (index < size());
    }

    /*! Returns true if all indices are valid indices. */
    inline bool ValidIndex(const nsize_type & indices) const
    {
        nsize_type sizes = this->NArray.sizes();
        typename nsize_type::const_iterator sizesIter;
        typename nsize_type::const_iterator indicesIter;

        if (indices.size() != sizes.size())
            return false;

        for (sizesIter = sizes.begin(), indicesIter = indices.begin();
             sizesIter != sizes.end();
             sizesIter++, indicesIter++)
        {
            if (*indicesIter >= *sizesIter)
                return false;
        }

        return true;
    }

    /*! Returns true if the given index is a valid index. */
    inline bool ValidIndex(dimension_type dimension, size_type index) const
    {
        return ( index < this->size(dimension) );
    }


    /*! Access an element by index (const).  Compare with
      std::matrix::at().  This method can be a handy substitute for
      the overloaded operator [] when operator overloading is
      unavailable or inconvenient.

      \return a const reference to element[index] */
    const_reference at(size_type metaIndex) const
        CISST_THROW(std::out_of_range)
    {
        ThrowUnlessValidIndex(metaIndex);
        return (begin())[metaIndex];
    }

    /*! Access an element by indices (const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a const reference to the element at indices */
    const_reference at(const nsize_type & coordinates) const
        CISST_THROW(std::out_of_range)
    {
        ThrowUnlessValidIndex(coordinates);
        return *(Pointer(coordinates));
    }

    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    const_reference operator () (const nsize_type & coordinates) const
        CISST_THROW(std::out_of_range)
    {
        return this->at(coordinates);
    }


    /*! Access the NArray owner.  This method should be used only to
        access some extra information related to the memory layout.
        It is used by the engines (vctDynamicNArrayLoopEngines). */
    const OwnerType & Owner(void) const {
        return this->NArray;
    }


    /*! Returns a const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    const_pointer Pointer(void) const
    {
        return NArray.Pointer();
    }

    /*! Returns a const pointer to an element of the container,
      specified by its indices. Addition to the STL requirements.
    */
    const_pointer Pointer(const nsize_type & indices) const
    {
        return NArray.Pointer(indices);
    }


    /*! Access an element by indices (const). This method allows
      to access an element without any bounds checking.  It doesn't
      create any temporary references as nArray[][]...[] would do.

      \return a reference to the element at the specified indices */
    const_reference Element(const nsize_type & coordinates) const
    {
        return *(Pointer(coordinates));
    }


    /*! Create a const reference to a subarray of this nArray.
        \param startPosition vector of indices of the element position where the reference will start
        \param lengths the number of elements to reference in each dimension

        \note Preserves the number of dimensions.
    */
    ConstSubarrayRefType Subarray(const nsize_type & startPosition,
                                  const nsize_type & lengths) const
    {
        ConstSubarrayRefType subarray;
        subarray.SubarrayOf(*this, startPosition, lengths);
        return subarray;
    }


    /*! Create a const reference to this nArray with the strides permuted.
        \param dimensions a sequence of dimension numbers representing the
          permuted order of strides

        \note Preserves the number of dimensions.
    */
    ConstPermutationRefType Permutation(const ndimension_type & dimensions) const
    {
        ConstPermutationRefType permutation;
        permutation.PermutationOf(*this, dimensions);
        return permutation;
    }


    /*! Create a const reference to a slice of dimension n-1, where n is the
          dimension of this nArray.
        \param dimension the dimension number of the direction by which to reduce
        \param slice the index number in the dimension specified by which to set
          the reference

        \note The number of dimensions is reduced by one.
    */
    ConstSliceRefType Slice(dimension_type dimension, size_type index) const
    {
        return SlicesTypes::ConstSliceOf(*this, dimension, index);
    }


    /*! Reference an (n-1)-dimension slice of this nArray by index (const).
        \return a const reference to the given index in the most
          significant dimension
    */
    ConstSliceRefType operator [] (size_type index) const
    {
        return this->Slice(0, index);
    }


    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the nArray. */
    //@{

    /*! Return the sum of the elements of the nArray.
      \return The sum of all the elements */
    inline value_type SumOfElements(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the product of the elements of the nArray.
      \return The product of all the elements */
    inline value_type ProductOfElements(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Multiplication,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the square of the norm of the nArray.
      \return The square of the norm */
    inline value_type NormSquare(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Square>::
            Run(*this);
    }

    /*! Return the norm of the nArray.
      \return The norm. */
    inline NormType Norm(void) const
    {
        return sqrt(NormType(NormSquare()));
    }

    /*! Return the L1 norm of the nArray, i.e. the sum of the absolute
      values of all the elements.

      \return The L1 norm. */
    inline value_type L1Norm(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the nArray, i.e. the maximum of the absolute
      values of all the elements.

      \return The Linf norm. */
    inline value_type LinfNorm(void) const
    {
        return this->MaxAbsElement();
    }

    /*! Return the maximum element of the nArray.
      \return The maximum element */
    inline value_type MaxElement(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the nArray.
      \return The minimum element */
    inline value_type MinElement(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the maximum of the absolute
      values of all the elements.

      \sa LinfNorm.

      \return The maximum of the absolute values. */
    inline value_type MaxAbsElement(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the minimum of the absolute
      values of all the elements.

      \return The minimum of the absolute values. */
    inline value_type MinAbsElement(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }


    /*! Compute the minimum AND maximum elements of the nArray.
      This method is more runtime-efficient than computing them
      separately.
      \param minElement reference to store the minimum element result.
      \param maxElement reference to store the maximum element result.
      \note If this nArray is empty (null pointer) the result is undefined.
    */
    inline void MinAndMaxElement(value_type & minElement, value_type & maxElement) const
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::MinAndMax::Run((*this), minElement, maxElement);
    }


    /*! Return true if all the elements of this nArray are strictly positive,
      false otherwise */
    inline bool IsPositive(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this nArray are non-negative,
      false otherwise */
    inline bool IsNonNegative(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this nArray are non-positive,
      false otherwise */
    inline bool IsNonPositive(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this nArray are strictly negative,
      false otherwise */
    inline bool IsNegative (void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this nArray are nonzero,
      false otherwise */
    inline bool All(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this nArray is nonzero, false
      otherwise */
    inline bool Any(void) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if all the elements of this nArray are finite,
      false otherwise */
    inline bool IsFinite(void) const {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsFinite>::
            Run(*this);
    }

    /*! Return true if any element of this nArray is NaN, false
      otherwise */
    inline bool HasNaN(void) const {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNaN>::
            Run(*this);
    }
    //@}


    /*! \name Storage format. */
    //@{

    /*!  Test if the nArray is compact, i.e. the nArray actually
         uses a contiguous block of memory.

         \note An empty nArray is considered compact
     */
    inline bool IsCompact(void) const
    {
        return NArray.IsCompact();
    }
    //@}


    /*! Test if the method FastCopyOf can be used instead of Assign.
      See FastCopyOf for more details. */
    template <class __nArrayOwnerType>
    inline bool FastCopyCompatible(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & source) const
    {
        return vctFastCopy::NArrayCopyCompatible(*this, source);
    }


    /*! \name Elementwise comparisons between nArray.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two nArray of the same size, containing
      the same type of elements.  The strides can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the two
      nArrays.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results.  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    template <class __nArrayOwnerType>
    inline bool Equal(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiNi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool operator == (const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return Equal(otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool AlmostEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray,
                            value_type tolerance) const
    {
        return ((*this - otherNArray).LinfNorm() <= tolerance);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool AlmostEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return ((*this - otherNArray).LinfNorm() <= TypeTraits::Tolerance());
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool NotEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiNi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool operator != (const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return NotEqual(otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool Lesser(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiNi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool LesserOrEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiNi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool Greater(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiNi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline bool GreaterOrEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiNi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, otherNArray);
    }
    //@}


    /*! \name Elementwise comparisons between nArrays.
      Returns the nArray of comparison's results. */
    //@{
    /*! Comparison between two nArrays of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_nArrayOwnerType) can be different.  The
      comparison (\f$ = \neq < \leq > \geq \f$) for
      ElementwiseEqual(), ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two nArrays and stored in a newly created nArray.  There is no
      operator provided since the semantic would be ambiguous.

      \return An nArray of booleans.
    */
    template <class __nArrayOwnerType>
    inline BoolNArrayReturnType
    ElementwiseEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayElementwiseCompareNArray<_nArrayOwnerType, __nArrayOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline BoolNArrayReturnType
    ElementwiseNotEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayElementwiseCompareNArray<_nArrayOwnerType, __nArrayOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline BoolNArrayReturnType
    ElementwiseLesser(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayElementwiseCompareNArray<_nArrayOwnerType, __nArrayOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline BoolNArrayReturnType
    ElementwiseLesserOrEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayElementwiseCompareNArray<_nArrayOwnerType, __nArrayOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline BoolNArrayReturnType
    ElementwiseGreater(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayElementwiseCompareNArray<_nArrayOwnerType, __nArrayOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline BoolNArrayReturnType
    ElementwiseGreaterOrEqual(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray) const
    {
        return vctDynamicNArrayElementwiseCompareNArray<_nArrayOwnerType, __nArrayOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherNArray);
    }

    //@}


    /*! \name Elementwise comparisons between an nArray and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between an nArray and a scalar.  The type of the
      elements of the nArray and the scalar must be the same.  The
      comparison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the nArray and
      the scalar.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results.  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator == (const value_type & scalar) const
    {
        return Equal(scalar);
    }

    /* documented above */
    inline bool NotEqual(const value_type & scalar) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiSi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator != (const value_type & scalar) const
    {
        return NotEqual(scalar);
    }

    /* documented above */
    inline bool Lesser(const value_type & scalar) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const
    {
        return vctDynamicNArrayLoopEngines<DIMENSION>::template
            SoNiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, scalar);
    }
    //@}


    /*! \name Elementwise comparisons between an nArray and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between an nArray and a scalar, containing the same
      type of elements.  The comparison (\f$ = \neq < \leq > \geq
      \f$) for ElementwiseEqual(), ElementwiseNotEqual(),
      ElementwiseLesser(), ElementwiseLesserOrEqual(),
      ElementwiseGreater() or ElementwiseGreaterOrEqual() is performed
      elementwise between the nArray and the scalar and stored in a
      newly created nArray.  There is no operator provided since the
      semantic would be ambiguous.

      \return An nArray of booleans.
    */
    BoolNArrayReturnType ElementwiseEqual(const value_type & scalar) const;

    /* documented above */
    BoolNArrayReturnType ElementwiseNotEqual(const value_type & scalar) const;

    /* documented above */
    BoolNArrayReturnType ElementwiseLesser(const value_type & scalar) const;

    /* documented above */
    BoolNArrayReturnType ElementwiseLesserOrEqual(const value_type & scalar) const;

    /* documented above */
    BoolNArrayReturnType ElementwiseGreater(const value_type & scalar) const;

    /* documented above */
    BoolNArrayReturnType ElementwiseGreaterOrEqual(const value_type & scalar) const;

    //@}

    /*! \name Unary elementwise operations.
      Returns the result of nArray.op(). */
    //@{
    /*! Unary elementwise operations on an nArray.  For each element of
      the nArray "this", performs \f$ this[i] \leftarrow
      op(otherNArray[i])\f$ where \f$op\f$ can calculate the absolute
      value (Abs) or the opposite (Negation).

      \return A new nArray.
    */
    inline NArrayReturnType Abs(void) const;

    /* documented above */
    inline NArrayReturnType Negation(void) const;

    /* documented above */
    inline NArrayReturnType Floor(void) const;

    /* documented above */
    inline NArrayReturnType Ceil(void) const;
    //@}


    /*! Return a string representation of the nArray elements */
    std::string ToString(void)
    {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }


    /*!  Print the nArray to a text stream */
    void ToStream(std::ostream & outputStream) const {
        // preserve the formatting flags as they were
        const size_t width = outputStream.width(12);
        const size_t precision = outputStream.precision(6);
        bool showpoint = ((outputStream.flags() & std::ios_base::showpoint) != 0);
        outputStream << std::setprecision(6) << std::showpoint;
#if 0
        // this needs to be implemented using template based recursion on dimension, See [] operator implementation.
        this->ToStreamInternal(outputStream);
#endif
        // resume the formatting flags
        outputStream << std::setprecision(precision) << std::setw(width);
        if (!showpoint) {
            outputStream << std::noshowpoint;
        }
    }
};

#ifndef DOXYGEN
/* documented in class.  Implementation moved here for .Net 2003 */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::BoolNArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::ElementwiseEqual(const _elementType & scalar) const
{
    return vctDynamicNArrayElementwiseCompareScalar<_nArrayOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Equal>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::BoolNArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::ElementwiseNotEqual(const _elementType & scalar) const
{
    return vctDynamicNArrayElementwiseCompareScalar<_nArrayOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::NotEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::BoolNArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::ElementwiseLesser(const _elementType & scalar) const
{
    return vctDynamicNArrayElementwiseCompareScalar<_nArrayOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Lesser>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::BoolNArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::ElementwiseLesserOrEqual(const _elementType & scalar) const
{
    return vctDynamicNArrayElementwiseCompareScalar<_nArrayOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::LesserOrEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::BoolNArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::ElementwiseGreater(const _elementType & scalar) const
{
    return vctDynamicNArrayElementwiseCompareScalar<_nArrayOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Greater>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::BoolNArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::ElementwiseGreaterOrEqual(const _elementType & scalar) const
{
    return vctDynamicNArrayElementwiseCompareScalar<_nArrayOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::GreaterOrEqual>(*this, scalar);
}
#endif // DOXYGEN

/*! Return true if all the elements of the NArray are nonzero, false otherwise */
template <class _nArrayOwnerType, typename _elementType, vct::size_type _dimension>
inline bool vctAll(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray)
{
    return nArray.All();
}

/*! Return true if any element of the NArray is nonzero, false otherwise */
template <class _nArrayOwnerType, typename _elementType, vct::size_type _dimension>
inline bool vctAny(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray) {
    return nArray.Any();
}

/*! Stream out operator. */
template <class _nArrayOwnerType, typename _elementType, vct::size_type _dimension>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray) {
    nArray.ToStream(output);
    return output;
}


#endif // _vctDynamicConstNArrayBase_h
