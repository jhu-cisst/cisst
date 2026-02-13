/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

  (C) Copyright 2004-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicConstVectorBase_h
#define _vctDynamicConstVectorBase_h

/*!
  \file
  \brief Declaration of vctDynamicConstVectorBase
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnSerializer.h>

#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctDynamicVectorLoopEngines.h>
#include <cisstVector/vctUnaryOperations.h>
#include <cisstVector/vctBinaryOperations.h>
#include <cisstVector/vctFastCopy.h>
#include <cisstVector/vctForwardDeclarations.h>

#include <iostream>
#include <iomanip>
#include <sstream>

/* Forward declarations */
#ifndef DOXYGEN
template <class _vectorOwnerType, class __vectorOwnerType, class _elementType,
          class _elementOperationType>
vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareVector(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector1,
                                         const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector2);

template <class _vectorOwnerType, class _elementType,
          class _elementOperationType>
vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareScalar(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                                         const _elementType & scalar);
#endif // DOXYGEN


/*!
  This class is templated with the ``vector owner type'', which may
  be a vctDynamicVectorOwner or a vctVectorRefOwner.  It provides
  const operations on the dynamic vector, such as SumOfElements etc.

  Vector indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _vectorOwnerType the type of vector owner.

  \param _elementType the type of elements of the vector.
*/
template <class _vectorOwnerType, typename _elementType>
class vctDynamicConstVectorBase
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type of the vector itself. */
    typedef vctDynamicConstVectorBase<_vectorOwnerType, _elementType> ThisType;

    /*! Type of the data owner (dynamic array or pointer) */
    typedef _vectorOwnerType OwnerType;

    /*! Iterator on the elements of the vector. */
    typedef typename OwnerType::iterator iterator;

    /*! Const iterator on the elements of the vector. */
    typedef typename OwnerType::const_iterator const_iterator;

    /*! Reverse iterator on the elements of the vector. */
    typedef typename OwnerType::reverse_iterator reverse_iterator;

    /*! Const reverse iterator on the elements of the vector. */
    typedef typename OwnerType::const_reverse_iterator const_reverse_iterator;

    /*! The type used to create a copy. */
    typedef vctDynamicVector<_elementType> CopyType;

    /*! The type of a vector used to store by value results of the
        same type as this object. */
    typedef vctDynamicVector<_elementType> VectorValueType;

    /*! The type of a vector returned by value from operations on this
        object */
    typedef vctDynamicVector<_elementType> VectorReturnType;

    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Define bool based on the container type to force some
      compilers (i.e. gcc 4.0) to delay the instantiation of the
      ElementWiseCompare methods. */
    typedef typename TypeTraits::BoolType BoolType;

    /*! The type of a vector of booleans returned from operations on this object, e.g.,
      ElementwiseEqual. */
    typedef vctReturnDynamicVector<BoolType> BoolVectorReturnType;


protected:
    /*! Declaration of the vector-defining member object */
    OwnerType Vector;


    /*! Check the validity of an index. */
    inline void ThrowUnlessValidIndex(size_type index) const CISST_THROW(std::out_of_range) {
        if (! ValidIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicVector: Invalid index"));
        }
    }

public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    //@{
    const_iterator begin(void) const {
        return Vector.begin();
    }
    const_iterator cbegin(void) const {
        return Vector.begin();
    }
    //@}

    /*! Returns a const iterator on the last element (STL
      compatibility). */
    //@{
    const_iterator end(void) const {
        return Vector.end();
    }
    const_iterator cend(void) const {
        return Vector.end();
    }
    //@}

    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */
    //@{
    const_reverse_iterator rbegin(void) const {
        return Vector.rbegin();
    }
    const_reverse_iterator crbegin(void) const {
        return Vector.rbegin();
    }
    //@}

    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */
    //@{
    const_reverse_iterator rend(void) const {
        return Vector.rend();
    }
    const_reverse_iterator crend(void) const {
        return Vector.rend();
    }
    //@}

    /*! Return the number of elements in the vector.  This is not
      equivalent to the difference between the end and the beginning.
    */
    size_type size(void) const {
        return Vector.size();
    }

    /*! Not required by STL but provided for completeness */
    difference_type stride() const {
        return Vector.stride();
    }

    /*! Tell is the vector is empty (STL compatibility).  False unless
      size is zero. */
    bool empty() const {
        return (size() == 0);
    }

    /*! Access an element by index (const).
      \return a const reference to the element[index] */
    const_reference operator[](index_type index) const {
        return *Pointer(index);
    }

    /*! Access an element by index (const).  Compare with
      std::vector::at().  This method can be a handy substitute for
      the overloaded operator [] when operator overloading is
      unavailable or inconvenient.  \return a const reference to
      element[index] */
    const_reference at(size_type index) const CISST_THROW(std::out_of_range) {
        ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    inline const_reference operator() (size_type index) const CISST_THROW(std::out_of_range) {
        return this->at(index);
    }


    /*! Access an element by index (const). This method allows
      to access an element without any bounds checking.

      \return a reference to the element at index */
    const_reference Element(size_type index) const {
        return *(Pointer(index));
    }


    /*! Access the vector owner.  This method should be used only to
        access some extra information related to the memory layout.
        It is used by the engines (vctDynamicVectorLoopEngines). */
    const OwnerType & Owner(void) const {
        return this->Vector;
    }


    /*! Returns a const pointer to an element of the container,
      specified by its index. Addition to the STL requirements.
    */
    const_pointer Pointer(index_type index = 0) const {
        return Vector.Pointer(index);
    }


    /*! Returns true if this index is valid. */
    inline bool ValidIndex(size_type index) const {
        return (index < size());
    }


    /*! \name Size dependant methods.

    The following methods are size dependant, i.e. don't necessarily
    mean anything for all sizes of vector.  For example, using the Z()
    method on a vector of size 2 shouldn't be allowed.  Therefore, we
    are using #CMN_ASSERT to check that the size is valid. */

    //@{

    /*! Returns the first element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 1. */
    const value_type & X(void) const {
        CMN_ASSERT(this->size() > 0);
        return *(Pointer(0));
    }


    /*! Returns the second element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 2. */
    const value_type & Y(void) const {
        CMN_ASSERT(this->size() > 1);
        return *(Pointer(1));
    }


    /*! Returns the third element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 3. */
    const value_type & Z(void) const {
        CMN_ASSERT(this->size() > 2);
        return *(Pointer(2));
    }

    /*! Returns the fourth element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 4. */
    const value_type & W(void) const {
        CMN_ASSERT(this->size() > 3);
        return *(Pointer(3));
    }

    /*! Return a (const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal.
      (shadowed from base class).
    */
    vctDynamicConstVectorRef<_elementType> XY(void) const {
        CMN_ASSERT(this->size() > 1);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(0), this->stride());
    }

    /*! Return a (const) vector reference for the first and third elements of this
      vector. */
    vctDynamicConstVectorRef<_elementType> XZ(void) const {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(0), 2 * this->stride());
    }

    /*! Return a (const) vector reference for the first and fourth elements of this
      vector. */
    vctDynamicConstVectorRef<_elementType> XW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(0), 3 * this->stride());
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicConstVectorRef<_elementType> YZ(void) const {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(1), this->stride());
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicConstVectorRef<_elementType> YW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(1), 2 * this->stride());
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the third. */
    vctDynamicConstVectorRef<_elementType> ZW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(2), this->stride());
    }

    /*! Return a (const) vector reference for the first three elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
      (shadowed from base class).
    */
    vctDynamicConstVectorRef<_elementType> XYZ(void) const {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicConstVectorRef<_elementType>(3, Pointer(0), this->stride());
    }

    /*! Return a (const) vector reference for the second, third and fourth elements
      of this vector. */
    vctDynamicConstVectorRef<_elementType> YZW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(3, Pointer(1),  this->stride());
    }

    /*! Return a (const) vector reference for the first four elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
      (shadowed from base class).
    */
    vctDynamicConstVectorRef<_elementType> XYZW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(4, Pointer(0), this->stride());
    }

    //@}

    /*! Create a const reference to a sub vector */
    vctDynamicConstVectorRef<_elementType>
    Ref(const size_type size, const size_type startPosition = 0) const CISST_THROW(std::out_of_range) {
        if ((startPosition + size) > this->size()) {
            cmnThrow(std::out_of_range("vctDynamicConstVectorBase::Ref: reference is out of range"));
        }
        return vctDynamicConstVectorRef<_elementType>(size, Pointer(startPosition), this->stride());
    }

    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the vector. */
    //@{

    /*! Return the sum of the elements of the vector.
      \return The sum of all the elements */
    inline value_type SumOfElements(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the product of the elements of the vector.
      \return The product of all the elements */
    inline value_type ProductOfElements(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Multiplication,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the square of the norm  of the vector.
      \return The square of the norm */
    inline value_type NormSquare(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Square>::
            Run(*this);
    }

    /*! Return the norm of the vector.
      \return The norm. */
    inline NormType Norm(void) const {
        return sqrt(NormType(NormSquare()));
    }

    /*! Test if this vector is normalized.  This method will use the default
      tolerance defined in cmnTypeTraits as the maximum difference between the
      norm and 1 to consider the vector normalized.
    */
    inline bool IsNormalized(void) const {
        if (vctUnaryOperations<_elementType>::AbsValue::Operate(_elementType(Norm() - 1)) < TypeTraits::Tolerance()) {
            return true;
        } else {
            return false;
        }
    }

    /*! Test if this vector is normalized.  This method uses the
      tolerance provided by the user.

      \param tolerance The maximum difference between the norm and 1
      to consider the vector normalized.

      \note This method has been overloaded instead of using a default
      value for the tolerance parameter to allow the use of
      vctDynamicVector with types for which don't have a tolerance
      defined via cmnTypeTraits.  Otherwise, some compilers will try
      to expand the signature and will not be able to set the default
      tolerance.
    */
    inline bool IsNormalized(_elementType tolerance) const {
        if (vctUnaryOperations<_elementType>::AbsValue::Operate(_elementType(Norm() - 1)) < tolerance) {
            return true;
        } else {
            return false;
        }
    }

    /*! Return the L1 norm of the vector, i.e. the sum of the absolute
      values of all the elements.

      \return The L1 norm. */
    inline value_type L1Norm(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the vector, i.e. the maximum of the
      absolute values of all the elements.

      \sa MaxAbsElement

      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return this->MaxAbsElement();
    }

    /*! Return the maximum element of the vector.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the vector.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the maximum of the absolute
      values of all the elements.

      \sa LinfNorm.

      \return The maximum of the absolute values. */
    inline value_type MaxAbsElement(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the minimum of the absolute
      values of all the elements.

      \return The minimum of the absolute values. */
    inline value_type MinAbsElement(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Compute the minimum AND maximum elements of the vector.
      This method is more runtime-efficient than computing them
      separately.
      \param minElement reference to store the minimum element result.
      \param maxElement reference to store the maximum element result.
      \note If this vector is empty (null pointer) the result is undefined.
    */
    inline void MinAndMaxElement(value_type & minElement, value_type & maxElement) const
    {
        vctDynamicVectorLoopEngines::MinAndMax::Run((*this), minElement, maxElement);
    }

    /*! Return true if all the elements of this vector are strictly positive,
      false otherwise */
    inline bool IsPositive(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are non-negative,
      false otherwise */
    inline bool IsNonNegative(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are non-positive,
      false otherwise */
    inline bool IsNonPositive(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are strictly negative,
      false otherwise */
    inline bool IsNegative (void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this vector is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are finite,
      false otherwise */
    inline bool IsFinite(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsFinite>::
            Run(*this);
    }

    /*! Return true if any element of this vector is NaN, false
      otherwise */
    inline bool HasNaN(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNaN>::
            Run(*this);
    }
    //@}


    /*! \name Storage format. */
    //@{

    /*! Test if the vector is compact, i.e. a vector of length l
      actually uses a contiguous block of memory or size l.  A compact
      vector has a stride equal to 1. */
    inline bool IsCompact(void) const {
        return (this->stride() == 1);
    }

    /*! Test if the vector is "Fortran" compatible, i.e. is compact.
      \sa IsCompact
    */
    inline bool IsFortran(void) const {
        return this->IsCompact();
    }

    /*! Test if the method FastCopyOf can be used instead of Assign.
      See FastCopyOf for more details. */
    template <class __vectorOwnerType>
    inline bool FastCopyCompatible(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & source) const
    {
        return vctFastCopy::VectorCopyCompatible(*this, source);
    }

    /*! Test if the method FastCopyOf can be used instead of Assign.
      See FastCopyOf for more details. */
    template <size_type __size, stride_type __stride, class __dataPtrType>
    inline bool FastCopyCompatible(const vctFixedSizeConstVectorBase<__size, __stride, value_type, __dataPtrType> & source) const
    {
        return vctFastCopy::VectorCopyCompatible(*this, source);
    }
    //@}


    /*! Dot product with another vector <em>of the same type and size</em>

    \param otherVector second operand of the dot product ("this" is the first operand)
    \return The dot product of this and otherVector.
    */
    template <class __vectorOwnerType>
    inline value_type DotProduct(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication>::
            Run(*this, otherVector);
    }


    /*! \name Elementwise comparisons between vectors.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two vectors of the same size, containing
      the same type of elements.  The stride can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the two
      vectors.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results.  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    template <class __vectorOwnerType>
    inline bool Equal(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool operator == (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return Equal(otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool AlmostEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
                            value_type tolerance) const {
        return ((*this - otherVector).LinfNorm() <= tolerance);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool AlmostEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return ((*this - otherVector).LinfNorm() <= TypeTraits::Tolerance());
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool NotEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool operator != (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return NotEqual(otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool Lesser(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool LesserOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool Greater(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool GreaterOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, otherVector);
    }
    //@}


    /*! \name Elementwise comparisons between vectors.
      Returns the vector of comparison's results. */
    //@{
    /*! Comparison between two vectors of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_vectorOwnerType) can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for
      ElementwiseEqual(), ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two vectors and stored in a newly created vector.  There is no
      operator provided since the semantic would be ambiguous.

      \return A vector of booleans.
    */
    template <class __vectorOwnerType>
    BoolVectorReturnType
    ElementwiseEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherVector);
    }


    /* documented above */
    template <class __vectorOwnerType>
    BoolVectorReturnType
    ElementwiseNotEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    BoolVectorReturnType
    ElementwiseLesser(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    BoolVectorReturnType
    ElementwiseLesserOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    BoolVectorReturnType
    ElementwiseGreater(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    BoolVectorReturnType
    ElementwiseGreaterOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherVector);
    }
    //@}


    /*! \name Elementwise comparisons between a vector and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a vector and a scalar.  The type of the
      elements of the vector and the scalar must be the same.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the vector and
      the scalar.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results..  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator == (const value_type & scalar) const {
        return Equal(scalar);
    }

    /* documented above */
    inline bool NotEqual(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator != (const value_type & scalar) const {
        return NotEqual(scalar);
    }

    /* documented above */
    inline bool Lesser(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, scalar);
    }
    //@}

    /*! \name Elementwise comparisons between a vector and a scalar.
      Returns the vector of comparison's results. */
    //@{
    /*! Comparison between a vector and a scalar, containing the same
      type of elements.  The comparaison (\f$ = \neq < \leq > \geq
      \f$) for ElementwiseEqual(), ElementwiseNotEqual(),
      ElementwiseLesser(), ElementwiseLesserOrEqual(),
      ElementwiseGreater() or ElementwiseGreaterOrEqual() is performed
      elementwise between the vector and the scalar and stored in a
      newly created vector.  There is no operator provided since the
      semantic would be ambiguous.

      \return A vector of booleans.
    */
    BoolVectorReturnType ElementwiseEqual(const value_type & scalar) const;

    /* documented above */
    BoolVectorReturnType ElementwiseNotEqual(const value_type & scalar) const;

    /* documented above */
    BoolVectorReturnType ElementwiseLesser(const value_type & scalar) const;

    /* documented above */
    BoolVectorReturnType ElementwiseLesserOrEqual(const value_type & scalar) const;

    /* documented above */
    BoolVectorReturnType ElementwiseGreater(const value_type & scalar) const;

    /* documented above */
    BoolVectorReturnType ElementwiseGreaterOrEqual(const value_type & scalar) const;
    //@}

    /*! Define a ConstSubvector class for compatibility with the fixed size vectors.
      A const subvector has the same stride as the parent container.

      Example:

      typedef vctDynamicVector<double> VectorType;
      VectorType v(9);
      VectorType::ConstSubvector::Type first4(v, 0, 4);
      VectorType::ConstSubvector::Type last5(v, 4, 5);

      \note There is no straightforward way to define a fixed-size
      subvector of a dynamic vector, because the stride of the dynamic
      vector is not known in compilation time.  A way to do it is:

      vctFixedSizeConstVectorRef<double, 3 /( add stride here if necessary )/> firstThree(v, 0);
      vctFixedSizeConstVectorRef<double, 3 /( add stride here if necessary )/> lastThree(v, 6);
    */
#ifndef SWIG
    class ConstSubvector
    {
    public:
        typedef vctDynamicConstVectorRef<value_type> Type;
    };
#endif // SWIG

    /*! \name Unary elementwise operations.
      Returns the result of vector.op(). */
    //@{
    /*! Unary elementwise operations on a vector.  For each element of
      the vector "this", performs \f$ this[i] \leftarrow
      op(otherVector[i])\f$ where \f$op\f$ can calculate the absolute
      value (Abs), the opposite (Negation) or the normalized version
      (Normalized).

      \return A new vector.
    */
    inline VectorReturnType Abs(void) const;

    /* documented above */
    inline VectorReturnType Negation(void) const;

    /* documented above */
    inline VectorReturnType Floor(void) const;

    /* documented above */
    inline VectorReturnType Ceil(void) const;

    /* documented above */
    inline VectorReturnType Normalized(void) const CISST_THROW(std::runtime_error);
    //@}


    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    void ToStream(std::ostream & outputStream) const
    {
        size_type index;
        const size_type mySize = size();
        // preserve the formatting flags as they were
        const std::streamsize width = outputStream.width(12);
        const std::streamsize precision = outputStream.precision(6);
        bool showpoint = ((outputStream.flags() & std::ios_base::showpoint) != 0);
        outputStream << std::setprecision(6) << std::showpoint;
        for (index = 0; index < mySize; ++index) {
            outputStream << std::setw(12) << (*this)[index];
            if (index < (mySize-1)) {
                outputStream << " ";
            }
        }
        // resume the formatting flags
        outputStream << std::setprecision(precision) << std::setw(width);
        if (!showpoint) {
            outputStream << std::noshowpoint;
        }
    }


    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const
    {
        size_type index;
        const size_type mySize = size();
        if (headerOnly) {
            for (index = 0; index < mySize; ++index) {
                outputStream << headerPrefix << "-v" << index;
                if (index < (mySize-1)) {
                    outputStream << delimiter;
                }
            }
        } else {
            for (index = 0; index < mySize; ++index) {
                outputStream << (*this)[index];
                if (index < (mySize-1)) {
                    outputStream << delimiter;
                }
            }
        }
    }

    /*! Binary serialization */
    void SerializeRaw(std::ostream & outputStream) const
    {
        const size_type mySize = size();
        size_type index;
        cmnSerializeSizeRaw(outputStream, mySize);
        for (index = 0; index < mySize; ++index) {
            cmnSerializeRaw(outputStream, this->Element(index));
        }
    }

};



/* documented in class.  Implementation moved here for .Net 2003 */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseNotEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseLesser(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseLesserOrEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseGreater(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseGreaterOrEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, scalar);
}


/*!
  Dot product of two vectors <em>of the same type and size</em>.

  \param vector1 first operand of the dot product.
  \param vector2 second operand of the dot product.
  \return The dot product of vector1 and vector2.
*/
template <class _vector1OwnerType, class _vector2OwnerType, class _elementType>
inline _elementType vctDotProduct(const vctDynamicConstVectorBase<_vector1OwnerType, _elementType> & vector1,
                                  const vctDynamicConstVectorBase<_vector2OwnerType, _elementType> & vector2) {
    return vector1.DotProduct(vector2);
}

/*!
  Dot product of two vectors <em>of the same type and size</em>.

  \param vector1 first operand of the dot product.
  \param vector2 second operand of the dot product.
  \return The dot product of vector1 and vector2.
*/
template <class _vector1OwnerType, class _vector2OwnerType, class _elementType>
inline _elementType operator * (const vctDynamicConstVectorBase<_vector1OwnerType, _elementType> & vector1,
                                const vctDynamicConstVectorBase<_vector2OwnerType, _elementType> & vector2) {
    return vector1.DotProduct(vector2);
}

/*! Return true if all the elements of the vector are nonzero, false otherwise */
template <class _vectorOwnerType, typename _elementType>
inline bool vctAll(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    return vector.All();
}

/*! Return true if any element of the vector is nonzero, false otherwise */
template <class _vectorOwnerType, typename _elementType>
inline bool vctAny(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    return vector.Any();
}

/*! Stream out operator. */
template <class _vectorOwnerType, typename _elementType>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    vector.ToStream(output);
    return output;
}

#ifndef DOXYGEN
template <vct::size_type __size, class _vectorOwnerType, class __elementType>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicConstVectorBase< _vectorOwnerType, vctFixedSizeVector<__elementType, __size> > & vector)
{
    vct::size_type numElements = vector.size();
    vct::size_type counter;
    for (counter = 0; counter < numElements; ++counter) {
        output << "[ ";
        vector[counter].ToStream(output);
        output << "]\n";
    }
    return output;
}
#endif // DOXYGEN


// helper function declared and used in vctFixedSizeVectorBase.h
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType, class _vectorOwnerType>
inline void vctFixedSizeVectorBaseAssignDynamicConstVectorBase(
    vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> & fixedSizeVector,
    const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & dynamicVector)
{
    vctDynamicVectorRef<_elementType> tempRef(fixedSizeVector);
    tempRef.Assign(dynamicVector);
}



#endif // _vctDynamicConstVectorBase_h
