/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Daniel Li, Ofri Sadowsky, Anton Deguet
  Created on: 2006-07-10

  (C) Copyright 2006-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicNArrayBase_h
#define _vctDynamicNArrayBase_h

/*!
  \file
  \brief Declaration of vctDynamicNArrayBase
*/

#include <cisstVector/vctDynamicConstNArrayBase.h>
#include <cisstVector/vctStoreBackUnaryOperations.h>
#include <cisstVector/vctStoreBackBinaryOperations.h>

/*!
  This class provides all the const methods inherited from
  vctDynamicConstNArrayBase, and extends them with non const methods, such
  as SumOf.

  \sa vctDynamicConstNArrayBase
*/
template<class _nArrayOwnerType, typename _elementType, vct::size_type _dimension>
class vctDynamicNArrayBase: public vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>
{
public:
    /* define most types from vctContainerTraits and vctNArrayTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(_dimension);

    /*! Type of the nArray itself */
    typedef vctDynamicNArrayBase ThisType;

    /*! Type of the base class */
    typedef vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> BaseType;

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
    typedef typename BaseType::ConstSubarrayRefType ConstSubarrayRefType;
    typedef typename BaseType::SubarrayRefType SubarrayRefType;
    //@}

    /*! \name The type of the object representing a permutation of
      this nArray. Access is by (const and non const) reference. */
    //@{
    typedef typename BaseType::ConstPermutationRefType ConstPermutationRefType;
    typedef typename BaseType::PermutationRefType PermutationRefType;
    //@}

    /*! \name The type of the object representing a slice of this
      nArray. Access is by (const and non const) reference. */
    //@{
    typedef typename BaseType::SlicesTypes SlicesTypes;
    typedef typename BaseType::ConstSliceRefType ConstSliceRefType;
    typedef typename BaseType::SliceRefType SliceRefType;
    //@}


    /*! Returns an iterator on the first element (STL
      compatibility). */
    iterator begin(void)
    {
        return this->NArray.begin();
    }

    /* documented in base class */
    const_iterator begin(void) const
    {
        return BaseType::begin();
    }

    /*! Returns an iterator on the last element (STL
      compatibility). */
    iterator end(void)
    {
        return this->NArray.end();
    }

    /* documented in base class */
    const_iterator end(void) const
    {
        return BaseType::end();
    }

    /*! Returns a reverse iterator on the last element (STL
      compatibility). */
    reverse_iterator rbegin(void)
    {
        return this->NArray.rbegin();
    }

    /* documented in base class */
    const_reverse_iterator rbegin(void) const
    {
        return BaseType::rbegin();
    }

    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */
    reverse_iterator rend(void)
    {
        return this->NArray.rend();
    }

    /* documented in base class */
    const_reverse_iterator rend(void) const
    {
        return BaseType::rend();
    }


    /* documented in base class */
    const OwnerType & Owner(void) const {
        return BaseType::Owner();
    }
    OwnerType & Owner(void) {
        return this->NArray;
    }


    /*! Return a non const pointer to an element of the container,
      specified by its indices. Addition to the STL requirements.
    */
    pointer Pointer(const nsize_type & indices)
    {
        return this->NArray.Pointer(indices);
    }

    /* documented in base class */
    const_pointer Pointer(const nsize_type & indices) const
    {
        return BaseType::Pointer(indices);
    }

    /*! Returns a non const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    pointer Pointer(void)
    {
        return this->NArray.Pointer();
    }

    /* documented in base class */
    const_pointer Pointer(void) const
    {
        return BaseType::Pointer();
    }


    /*! Access an element by index (non const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a const reference to the element at indices */
    reference at(size_type metaIndex)
        CISST_THROW(std::out_of_range)
    {
        this->ThrowUnlessValidIndex(metaIndex);
        return (begin())[metaIndex];
    }


    /* documented in base class */
    const_reference at(size_type metaIndex) const
        CISST_THROW(std::out_of_range)
    {
        return BaseType::at(metaIndex);
    }


    /*! Access an element by index.  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a reference to the element at indices */
    reference at(const nsize_type & coordinates)
        CISST_THROW(std::out_of_range)
    {
        this->ThrowUnlessValidIndex(coordinates);
        return *(Pointer(coordinates));
    }

    /* documented in base class */
    const_reference at(const nsize_type & coordinates) const
        CISST_THROW(std::out_of_range)
    {
        return BaseType::at(coordinates);
    }


    /*! Overloaded operator () for simplified (non const) element access with bounds checking */
    reference operator () (const nsize_type & coordinates)
        CISST_THROW(std::out_of_range)
    {
        return this->at(coordinates);
    }

    /*! documented in base class */
     const_reference operator () (const nsize_type & coordinates) const
        CISST_THROW(std::out_of_range)
    {
        return BaseType::operator()(coordinates);
    }


    /*! Access an element by indices (non const). This method allows
      to access an element without any bounds checking.  It doesn't
      create any temporary references as a nArray[][]...[] would do.

      \return a reference to the element at the specified indices */
    reference Element(const nsize_type & coordinates)
    {
        return *(Pointer(coordinates));
    }

    /* documented in base class */
    const_reference Element(const nsize_type & coordinates) const
    {
        return BaseType::Element(coordinates);
    }


    /*! Create a non-const reference to a subarray of this nArray.
        \param startPosition vector of indices of the element position where the reference will start
        \param lengths the number of elements to reference in each dimension

        \note Preserves the number of dimensions.
    */
    SubarrayRefType Subarray(const nsize_type & startPosition,
                             const nsize_type & lengths)
    {
        SubarrayRefType subarray;
        subarray.SubarrayOf(*this, startPosition, lengths);
        return subarray;
    }


    /*! documented in base class */
    ConstSubarrayRefType Subarray(const nsize_type & startPosition,
                                  const nsize_type & lengths) const
    {
        return BaseType::Subarray(startPosition, lengths);
    }


    /*! Create a non-const reference to this nArray with the strides permuted.
        \param dimensions a sequence of dimension numbers representing the
          permuted order of strides

        \note Preserves the number of dimensions.
    */
    PermutationRefType Permutation(const ndimension_type & dimensions)
    {
        PermutationRefType permutation;
        permutation.PermutationOf(*this, dimensions);
        return permutation;
    }


    /*! documented in base class */
    ConstPermutationRefType Permutation(const ndimension_type & dimensions) const
    {
        return BaseType::Permutation(dimensions);
    }


    /*! Create a non-const reference to a slice of dimension n-1, where n is the
          dimension of this nArray.
        \param dimension the dimension number of the direction by which to reduce
        \param slice the index number in the dimension specified by which to set
          the reference

        \note The number of dimensions is reduced by one.
    */
    SliceRefType Slice(dimension_type dimension, size_type index)
    {
        return SlicesTypes::SliceOf(*this, dimension, index);
    }


    /*! documented in base class */
    ConstSliceRefType Slice(dimension_type dimension, size_type index) const
    {
        return BaseType::Slice(dimension, index);
    }


    /*! Reference an (n-1)-dimension slice of this nArray by index (non-const).
        \return a non-const reference to the given index in the most
          significant dimension
    */
    SliceRefType operator[](size_type index)
    {
        return this->Slice(0, index);
    }


    /*! documented in base class */
    ConstSliceRefType operator[](size_type index) const
    {
        return BaseType::operator[](index);
    }


    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the nArray
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type value)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSi<typename vctStoreBackBinaryOperations<value_type>::SecondOperand>::
            Run(*this, value);
        return value;
    }


    /*! Assign zero to all elements.  This methods assumes that the
      element type has a zero and this zero value can be set using
      memset(0).  If the nArray is not compact this method will use
      SetAll(0) and memset otherwise.  This provides a slightly more
      efficent way to set all elements to zero.

      \return true if the nArray is compact and memset was used, false
      otherwise. */
    inline bool Zeros(void) {
        if (this->IsCompact()) {
            memset(this->Pointer(), 0, this->size() * sizeof(value_type));
            return true;
        } else {
            this->SetAll(static_cast<value_type>(0));
            return false;
        }
    }

    /*!
      \name Assignment operation between arrays containing the same data type but different owners
      \param other The array to be copied.
    */
    //@{
    template <class __nArrayOwnerType>
    inline ThisType & Assign(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & other) {
        if (this->FastCopyCompatible(other)) {
            this->FastCopyOf(other, false);
        } else {
            vctDynamicNArrayLoopEngines<DIMENSION>::template
                NoNi<typename vctUnaryOperations<value_type,
                typename __nArrayOwnerType::value_type>::Identity>::
                Run(*this, other);
        }
        return *this;
    }
    //@}


    /*!
      \name Assignment operation between nArrays of different types.

      \param other The nArray to be copied.
    */
    //@{
    template <class __nArrayOwnerType, typename __elementType>
    inline ThisType & Assign(const vctDynamicConstNArrayBase<__nArrayOwnerType, __elementType, DIMENSION> & other)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNi<typename vctUnaryOperations<value_type,
            typename __nArrayOwnerType::value_type>::Identity>::
            Run(*this, other);
        return *this;
    }

    template <class __nArrayOwnerType, typename __elementType>
    inline ThisType & operator = (const vctDynamicConstNArrayBase<__nArrayOwnerType, __elementType, DIMENSION> & other)
    {
        return this->Assign(other);
    }
    //@}


    /*! Assign to this nArray values from a C array given as a
      pointer to value_type. The purpose of this method is to simplify
      the syntax by not necessitating the creation of an explicit nArray
      for the given array. However, we only provide this method for
      an array of value_type. For arrays of other types an nArray
      still needs to be declared.

      \param elements a pointer to a C array of elements to be assigned
        to this nArray

      \note This method assumes two things: (1) that the input array is
        _packed_, that is, that all the elements are stored in a
        contiguous memory block with no gaps; and (2) that the storage
        order of the input array matches the storage order of this
        nArray.

      \return a reference to this object.
    */
    inline ThisType & Assign(const value_type * elements)
    {
        nstride_type strides;
        typename nsize_type::const_reverse_iterator sizesIter = this->sizes().rbegin();
        typename nstride_type::reverse_iterator stridesIter = strides.rbegin();
        const typename nstride_type::const_reverse_iterator stridesEnd = strides.rend();
        size_type sizesIter_value;
        stride_type previous_stride;

        if (stridesIter != stridesEnd)
        {
            *stridesIter = 1;
            previous_stride = 1;
            ++stridesIter;
        }

        for (; stridesIter != stridesEnd;
             ++stridesIter, ++sizesIter)
        {
            sizesIter_value = (*sizesIter == 0) ? 1 : *sizesIter;
            *stridesIter = sizesIter_value * previous_stride;
            previous_stride = *stridesIter;
        }

        const vctDynamicConstNArrayRef<value_type, DIMENSION> tmpRef(elements,
                                                                      this->sizes(),
                                                                      strides);
        this->Assign(tmpRef);
        return *this;
    }


    /*!  Forced assignment operation between arrays of
      different types.  This method will use SetSize on the
      destination array (this nArray) to make sure the assignment
      will work.  It is important to note that if the sizes don't
      match, memory for this array will be re-allocated.  If the
      program has previously created some references (as in
      vctDynamic{Const}NArrayRef) on this array, they will become
      invalid.

      \note For a non-reallocating Assign, it is recommended to use
      the Assign() methods.

      \note If the destination array doesn't have the same size as
      the source and can not be resized, an exception will be thrown
      by the Assign method called internally.

      \param other The array to be copied.
    */
    template <class __nArrayOwnerType, typename __elementType>
    inline ThisType & ForceAssign(const vctDynamicConstNArrayBase<__nArrayOwnerType, __elementType, DIMENSION> & other) {
        return this->Assign(other);
    }


    /*! Fast copy.  This method uses <code>memcpy</code> whenever it
        is possible to perform a fast copy from another array to this
        array.

        - The method will first verify that the source and destination
          have the same sizes and throws an exception otherwise
          (<code>std::runtime_error</code>).  See ::cmnThrow for
          details.

        - If any of the two narray is not compact or if the two narray
          use a different layout (different strides), this method will
          return <code>false</code>.  If both narrays are compact and
          have the same strides, a <code>memcpy</code> is performed
          and the method returns <code>true</code>.

        - To avoid the tests above, one can set the parameter
          <code>performSafetyChecks</code> to <code>false</code>.
          This should be used only when the programmer knows for sure
          that the source and destination are compatible (sizes,
          strides and compactness).

        - As opposed to Assign, this method doesn't perform any type
          conversion.

        - Since no constructor is called for the contained elements,
          this function performs a "shallow copy".  If the contained
          objects have a pointer as data member, the copied object
          will carry on the same pointer (hence pointing at the same
          memory block which could easily lead to bugs).

        The basic and safe use of this method for an array would be:
        \code
        if (!destination.FastCopyOf(source)) {
            destination.Assign(source);
        }
        \endcode

        If the method is to be called many times (in a loop for
        example), it is recommended to check that the source and
        destination are compatible once and then use the option
        to turn off the different safety checks for each FastCopyOf.
        \code
        bool canUseFastCopy = destination.FastCopyPossible(source);
        vct::index_type index;
        for (index = 0; index < 1000; index++) {
            DoSomethingUseful(source);
            if (canUseFastCopy) {
                destination.FastCopyOf(source, false); // Do not check again
            } else {
                destination.Assign(source);
            }
        }
        \endcode

        \param source NArray used to set the content of this array.

        \param performSafetyChecks Flag set to <code>false</code> to
        avoid safety checks, use with extreme caution.
     */
    //@{
    template <class __nArrayOwnerType>
    inline bool FastCopyOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & source,
                           bool performSafetyChecks = true)
        CISST_THROW(std::runtime_error)
    {
        return vctFastCopy::NArrayCopy(*this, source, performSafetyChecks);
    }
    //@}


    /*! \name Binary elementwise operations between two nArrays.
      Store the result of op(nArray1, nArray2) to a third nArray. */
    //@{
    /*! Binary elementwise operations between two nArrays.  For each
      element of the nArrays, performs \f$ this[i] \leftarrow
      op(nArray1[i], nArray2[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ElementwiseProductOf), a division (ElementwiseRatioOf), a
      minimum (ElementwiseMinOf) or a maximum (ElementwiseMaxOf).

      \param nArray1 The first operand of the binary operation

      \param nArray2 The second operand of the binary operation

      \return The nArray "this" modified.
    */
    template <class __nArrayOwnerType1, class __nArrayOwnerType2>
    inline ThisType & SumOf(const vctDynamicConstNArrayBase<__nArrayOwnerType1, value_type, DIMENSION> & nArray1,
                            const vctDynamicConstNArrayBase<__nArrayOwnerType2, value_type, DIMENSION> & nArray2)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiNi< typename vctBinaryOperations<value_type>::Addition >
            ::Run(*this, nArray1, nArray2);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType1, class __nArrayOwnerType2>
    inline ThisType & DifferenceOf(const vctDynamicConstNArrayBase<__nArrayOwnerType1, value_type, DIMENSION> & nArray1,
                                   const vctDynamicConstNArrayBase<__nArrayOwnerType2, value_type, DIMENSION> & nArray2)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiNi< typename vctBinaryOperations<value_type>::Subtraction >
            ::Run(*this, nArray1, nArray2);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType1, class __nArrayOwnerType2>
    inline ThisType & ElementwiseProductOf(const vctDynamicConstNArrayBase<__nArrayOwnerType1, value_type, DIMENSION> & nArray1,
                                           const vctDynamicConstNArrayBase<__nArrayOwnerType2, value_type, DIMENSION> & nArray2)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiNi< typename vctBinaryOperations<value_type>::Multiplication >
            ::Run(*this, nArray1, nArray2);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType1, class __nArrayOwnerType2>
    inline ThisType & ElementwiseRatioOf(const vctDynamicConstNArrayBase<__nArrayOwnerType1, value_type, DIMENSION> & nArray1,
                                         const vctDynamicConstNArrayBase<__nArrayOwnerType2, value_type, DIMENSION> & nArray2)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiNi< typename vctBinaryOperations<value_type>::Division >
            ::Run(*this, nArray1, nArray2);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType1, class __nArrayOwnerType2>
    inline ThisType & ElementwiseMinOf(const vctDynamicConstNArrayBase<__nArrayOwnerType1, value_type, DIMENSION> & nArray1,
                                       const vctDynamicConstNArrayBase<__nArrayOwnerType2, value_type, DIMENSION> & nArray2)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiNi< typename vctBinaryOperations<value_type>::Minimum >
            ::Run(*this, nArray1, nArray2);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType1, class __nArrayOwnerType2>
    inline ThisType & ElementwiseMaxOf(const vctDynamicConstNArrayBase<__nArrayOwnerType1, value_type, DIMENSION> & nArray1,
                                       const vctDynamicConstNArrayBase<__nArrayOwnerType2, value_type, DIMENSION> & nArray2)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiNi< typename vctBinaryOperations<value_type>::Maximum >
            ::Run(*this, nArray1, nArray2);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between two nArrays.
      Store the result of op(this, otherNArray) back to this nArray. */
    //@{
    /*! Store back binary elementwise operations between two
      nArrays.  For each element of the nArrays, performs \f$
      this[i] \leftarrow op(this[i], otherNArray[i])\f$ where
      \f$op\f$ is either an addition (Add), a subtraction
      (Subtraction), a multiplication (ElementwiseMultiply) a division
      (ElementwiseDivide), a minimization (ElementwiseMin) or a
      maximisation (ElementwiseMax).

      \param otherNArray The second operand of the binary operation
      (this[i] is the first operand)

      \return The nArray "this" modified.
    */
    template <class __nArrayOwnerType>
    inline ThisType & Add(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioNi<typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & Subtract(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioNi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ElementwiseMultiply(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioNi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ElementwiseDivide(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioNi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ElementwiseMin(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioNi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ElementwiseMax(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioNi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & operator += (const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        return this->Add(otherNArray);
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & operator -= (const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        return this->Subtract(otherNArray);
    }
    //@}


    /*! \name Binary elementwise operations an nArray and a scalar.
      Store the result of op(nArray, scalar) to a third nArray. */
    //@{
    /*! Binary elementwise operations between an nArray and a scalar.
      For each element of the nArray "this", performs \f$ this[i]
      \leftarrow op(nArray[i], scalar)\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf), a division (RatioOf), a minimum (ClippedAboveOf) or
      a maximum (ClippedBelowOf).

      \param nArray The first operand of the binary operation.
      \param scalar The second operand of the binary operation.

      \return The nArray "this" modified.
    */
    template <class __nArrayOwnerType>
    inline ThisType & SumOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray,
                            const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiSi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, nArray, scalar);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & DifferenceOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray,
                                   const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiSi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, nArray, scalar);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ProductOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray,
                                const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiSi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, nArray, scalar);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & RatioOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray,
                              const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiSi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, nArray, scalar);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ClippedAboveOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray,
                                     const value_type lowerBound)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiSi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, nArray, lowerBound);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ClippedBelowOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray,
                                     const value_type upperBound)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNiSi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, nArray, upperBound);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations a scalar and an nArray.
      Store the result of op(scalar, nArray) to a third nArray. */
    //@{
    /*! Binary elementwise operations between a scalar and an nArray.
      For each element of the nArray "this", performs \f$ this[i]
      \leftarrow op(scalar, nArray[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf), a division (RatioOf), a minimum (ClippedAboveOf) or
      a maximum (ClippedBelowOf).

      \param scalar The first operand of the binary operation.
      \param nArray The second operand of the binary operation.

      \return The nArray "this" modified.
    */
    template <class __nArrayOwnerType>
    inline ThisType & SumOf(const value_type scalar,
                            const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoSiNi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, scalar, nArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & DifferenceOf(const value_type scalar,
                                   const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoSiNi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar, nArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ProductOf(const value_type scalar,
                                const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoSiNi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, nArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & RatioOf(const value_type scalar,
                              const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoSiNi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, scalar, nArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ClippedAboveOf(const value_type upperBound,
                                     const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoSiNi< typename vctBinaryOperations<value_type>::Minimum >::
            Run(*this, upperBound, nArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & ClippedBelowOf(const value_type lowerBound,
                                     const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & nArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoSiNi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, lowerBound, nArray);
        return *this;
    }
    //@}


    /*! \name Binary elementwise operations between an nArray and a scalar.
      Store the result of op(this, scalar) back to this nArray. */
    //@{
    /*! Store back binary elementwise operations between an nArray and
      a scalar.  For each element of the nArray "this", performs \f$
      this[i] \leftarrow op(this[i], scalar)\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtract), a
      multiplication (Multiply), a division (Divide), a minimum
      (ClipAbove) or a maximum (ClipBelow).

      \param scalar The second operand of the binary operation
        (this[i] is the first operand).

      \return The nArray "this" modified.
    */
    inline ThisType & Add(const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Subtract(const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Multiply(const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Divide(const value_type scalar)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipAbove(const value_type upperBound)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, upperBound);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipBelow(const value_type lowerBound)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, lowerBound);
        return *this;
    }

    /* documented above */
    inline ThisType & operator += (const value_type scalar)
    {
        return this->Add(scalar);
    }

    /* documented above */
    inline ThisType & operator -= (const value_type scalar)
    {
        return this->Subtract(scalar);
    }

    /* documented above */
    inline ThisType & operator *= (const value_type scalar)
    {
        return this->Multiply(scalar);
    }

    /* documented above */
    inline ThisType & operator /= (const value_type scalar)
    {
        return this->Divide(scalar);
    }
    //@}


    template <class __nArrayOwnerType>
    inline ThisType & AddProductOf(const value_type scalar,
                                   const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioSiNi<typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, otherNArray);

        return *this;
    }


    template <class __nArrayOwnerType1, class __nArrayOwnerType2>
    inline ThisType & AddElementwiseProductOf(const vctDynamicConstNArrayBase<__nArrayOwnerType1, value_type, DIMENSION> & nArray1,
                                              const vctDynamicConstNArrayBase<__nArrayOwnerType2, value_type, DIMENSION> & nArray2)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NioNiNi<typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, nArray1, nArray2);

        return *this;
    }


    /*! \name Unary elementwise operations.
      Store the result of op(nArray) to another nArray. */
    //@{
    /*! Unary elementwise operations on an nArray.  For each element of
      the nArray "this", performs \f$ this[i] \leftarrow
      op(otherNArray[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsOf) or the opposite (NegationOf).

      \param otherNArray The operand of the unary operation.

      \return The nArray "this" modified.
    */
    template <class __nArrayOwnerType>
    inline ThisType & AbsOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNi<typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & NegationOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNi<typename vctUnaryOperations<value_type>::Negation>::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & FloorOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNi<typename vctUnaryOperations<value_type>::Floor>::
            Run(*this, otherNArray);
        return *this;
    }

    /* documented above */
    template <class __nArrayOwnerType>
    inline ThisType & CeilOf(const vctDynamicConstNArrayBase<__nArrayOwnerType, value_type, DIMENSION> & otherNArray)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            NoNi<typename vctUnaryOperations<value_type>::Ceil>::
            Run(*this, otherNArray);
        return *this;
    }

    //@}

    /*! \name Store back unary elementwise operations.
      Store the result of op(this) to this nArray. */
    //@{
    /*! Unary elementwise operations on an nArray.  For each element of
      the nArray "this", performs \f$ this[i] \leftarrow
      op(this[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsSelf) or the opposite (NegationSelf).

      \return The nArray "this" modified.
    */
    inline ThisType & AbsSelf(void)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            Nio<typename vctStoreBackUnaryOperations<value_type>::MakeAbs>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & NegationSelf(void)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            Nio<typename vctStoreBackUnaryOperations<value_type>::MakeNegation>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & FloorSelf(void)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            Nio<typename vctStoreBackUnaryOperations<value_type>::MakeFloor>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & CeilSelf(void)
    {
        vctDynamicNArrayLoopEngines<DIMENSION>::template
            Nio<typename vctStoreBackUnaryOperations<value_type>::MakeCeil>::
            Run(*this);
        return *this;
    }
    //@}
};


#endif // _vctDynamicNArrayBase_h
