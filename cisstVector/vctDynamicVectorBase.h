/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicVectorBase_h
#define _vctDynamicVectorBase_h

/*!
  \file
  \brief Declaration of vctDynamicVectorBase
*/

#include <cstdarg>

#include <cisstVector/vctDynamicConstVectorBase.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctStoreBackUnaryOperations.h>
#include <cisstVector/vctStoreBackBinaryOperations.h>


#ifndef DOXYGEN
// forward declaration of auxiliary function to multiply matrix * vector
template <class _resultVectorOwnerType, class _matrixOwnerType, class _vectorOwnerType, class _elementType>
inline
void vctMultiplyMatrixVector(vctDynamicVectorBase<_resultVectorOwnerType, _elementType> & result,
                             const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix,
                             const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector);

// forward declaration of auxiliary function to multiply matrix * vector
template <class _resultVectorOwnerType, class _vectorOwnerType, class _matrixOwnerType, class _elementType>
inline
void vctMultiplyVectorMatrix(vctDynamicVectorBase<_resultVectorOwnerType, _elementType> & result,
                             const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                             const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix);
#endif // DOXYGEN

/*!
  This class provides all the const methods inherited from
  vctConstVectorBase, and extends them with non-const methods, such
  as SumOf.

  \sa vctDynamicConstVectorBase

*/
template <class _vectorOwnerType, typename _elementType>
class vctDynamicVectorBase : public vctDynamicConstVectorBase<_vectorOwnerType, _elementType>
{
public:
    friend class vctDynamicVectorLoopEngines;

    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    typedef vctDynamicVectorBase ThisType;
    /*! Type of the base class. */
    typedef vctDynamicConstVectorBase<_vectorOwnerType, _elementType> BaseType;

    typedef _vectorOwnerType VectorOwnerType;

    typedef typename BaseType::iterator iterator;
    typedef typename BaseType::reverse_iterator reverse_iterator;
    typedef typename BaseType::const_iterator const_iterator;
    typedef typename BaseType::const_reverse_iterator const_reverse_iterator;

    typedef typename BaseType::CopyType CopyType;
    typedef typename BaseType::VectorValueType VectorValueType;


    /*! Type traits for the elements of the vector. */
    typedef class cmnTypeTraits<value_type> TypeTraits;
    typedef typename TypeTraits::VaArgPromotion ElementVaArgPromotion;


public:
    /*! Returns an iterator on the first element (STL
      compatibility). */
    iterator begin(void) {
        return this->Vector.begin();
    }

    /*! Returns an iterator on the last element (STL
      compatibility). */
    iterator end(void) {
        return this->Vector.end();
    }

    /*! Returns a reverse iterator on the last element (STL
      compatibility). */
    reverse_iterator rbegin(void) {
        return this->Vector.rbegin();
    }

    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */
    reverse_iterator rend(void) {
        return this->Vector.rend();
    }

    /* documented in base class */
    const_iterator begin(void) const {
        return BaseType::begin();
    }

    /* documented in base class */
    const_iterator end(void) const {
        return BaseType::end();
    }

    /* documented in base class */
    const_reverse_iterator rbegin(void) const {
        return BaseType::rbegin();
    }

    /* documented in base class */
    const_reverse_iterator rend(void) const {
        return BaseType::rend();
    }

    /*! Access an element by index.
      \return a reference to the element[index] */
    reference operator[](index_type index) {
        return *Pointer(index);
    }

    /* documented in base class */
    const_reference operator[](index_type index) const {
        return BaseType::operator[](index);
    }

    /* documented in base class */
    const VectorOwnerType & Owner(void) const {
        return BaseType::Owner();
    }
    VectorOwnerType & Owner(void) {
        return this->Vector;
    }

    /*! Addition to the STL requirements.  Return a pointer to an element of the
      container, specified by its index.
    */
    pointer Pointer(index_type index = 0) {
        return this->Vector.Pointer(index);
    }

    /* documented in base class */
    const_pointer Pointer(index_type index = 0) const {
        return BaseType::Pointer(index);
    }

    /*! Access an element by index (non-const).  Compare with
      std::vector::at.  This method can be a handy substitute for the
      overloaded operator [] when operator overloading is unavailable
      or inconvenient.

      \return a non-const reference to element[index] */
    reference at(index_type index) CISST_THROW(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

    /* documented in base class */
    const_reference at(index_type index) const CISST_THROW(std::out_of_range) {
        return BaseType::at(index);
    }

    /*! Overloaded operator () for simplified (non-const) element access with bounds checking */
    inline reference operator() (index_type index) CISST_THROW(std::out_of_range) {
        return this->at(index);
    }

    /* documented in base class */
    inline const_reference operator() (index_type index) const CISST_THROW(std::out_of_range) {
        return BaseType::operator()(index);
    }


    /*! Access an element by index (non const). This method allows
      to access an element without any bounds checking.

      \return a reference to the element at index */
    reference Element(index_type index) {
        return *(Pointer(index));
    }

    /* documented in base class */
    const_reference Element(index_type index) const {
        return BaseType::Element(index);
    }


    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the vector
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type value) {
        vctDynamicVectorLoopEngines::
            VioSi<typename vctStoreBackBinaryOperations<value_type>::SecondOperand>::
            Run(*this, value);
        return value;
    }


    /*! Assign zero to all elements.  This methods assumes that the
      element type has a zero and this zero value can be set using
      memset(0).  If the vector is not compact this method will use
      SetAll(0) and memset otherwise.  This provides a slightly more
      efficent way to set all elements to zero.

      \return true if the vector is compact and memset was used, false
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
      Assignment operation between vectors containing the same data type but different owners
      \param other The vector to be copied.
    */
    //@{
    template <class __vectorOwnerType>
    inline ThisType & Assign(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & other) {
        if (this->FastCopyCompatible(other)) {
            this->FastCopyOf(other, false);
        } else {
            vctDynamicVectorLoopEngines::
                VoVi<typename vctUnaryOperations<value_type,
                typename __vectorOwnerType::value_type>::Identity>::
                Run(*this, other);
        }
        return *this;
    }
    //@}

    /*!
      Assignment operation between vectors containing the same data type but different owners and sizes.
      The number of elements to be copied must be lower or equal to the vector's size.

      \param other The vector to be copied.
    */
    //@{
    template <class __vectorOwnerType>
    inline ThisType & Assign(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & other,
                             size_type length, size_type startPositionThis = 0, size_type startPositionOther = 0) {
        vctDynamicVectorRef<value_type> thisRef(*this, startPositionThis, length);
        vctDynamicConstVectorRef<value_type> otherRef(other, startPositionOther, length);
        thisRef.Assign(otherRef);
        return *this;
    }
    //@}

    /*!
      \name Assignment operation between vectors of different types.

      \param other The vector to be copied.
    */
    //@{
    template <class __vectorOwnerType, typename __elementType>
    inline ThisType & Assign(const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & other) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type,
            typename __vectorOwnerType::value_type>::Identity>::
            Run(*this, other);
        return *this;
    }

    template <class __vectorOwnerType, typename __elementType>
    inline ThisType & operator = (const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & other) {
        return this->Assign(other);
    }

    template <size_type __size, stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstVectorBase<__size, __stride, __elementType, __dataPtrType>
                             & other) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(*this, vctDynamicConstVectorRef<__elementType>(other));
        return *this;
    }
    //@}


    /*!  \name Forced assignment operation between vectors of
      different types.  This method will use SetSize on the
      destination vector (this vector) to make sure the assignment
      will work.  It is important to note that if the sizes don't
      match, memory for this vector will be re-allocated.  If the
      program has previously created some references (as in
      vctDynamic{Const}VectorRef) on this vector, they will become
      invalid.

      \note For a non-reallocating Assign, it is recommended to use
      the Assign() methods.

      \note This method is provided for both fixed size and dynamic
      vectors for API consistency (usable in templated code).  There
      is obviously not resize involved on fixed size vectors.

      \note If the destination vector doesn't have the same size as
      the source and can not be resized, an exception will be thrown
      by the Assign method called internally.

      \param other The vector to be copied.
    */
    //@{
    template <class __vectorOwnerType, typename __elementType>
    inline ThisType & ForceAssign(const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & other) {
        return this->Assign(other);
    }

    template <size_type __size, stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & ForceAssign(const vctFixedSizeConstVectorBase<__size, __stride, __elementType, __dataPtrType>
                                  & other) {
        return this->Assign(other);
    }
    //@}


    /*! Fast copy.  This method uses <code>memcpy</code> whenever it
        is possible to perform a fast copy from another vector to this
        vector.

        - The method will first verify that the source and destination
          have the same size and throws an exception otherwise
          (<code>std::runtime_error</code>).  See ::cmnThrow for
          details.

        - If any of the two vectors is not compact, this method will
          return <code>false</code>.  If both vectors are compact, a
          <code>memcpy</code> is performed and the method returns
          <code>true</code>.

        - To avoid the tests above, one can set the parameter
          <code>performSafetyChecks</code> to <code>false</code>.
          This should be used only when the programmer knows for sure
          that the source and destination are compatible (size and
          compactness).

        - As opposed to Assign, this method doesn't perform any type
          conversion.

        - Since no constructor is called for the contained elements,
          this function performs a "shallow copy".  If the contained
          objects have a pointer as data member, the copied object
          will carry on the same pointer (hence pointing at the same
          memory block which could easily lead to bugs).

        The basic and safe use of this method for a vector would be:
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
        bool canUseFastCopy = destination.FastCopyCompatible(source);
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

        \param source Vector used to set the content of this vector.

        \param performSafetyChecks Flag set to <code>false</code> to
        avoid safety checks, use with extreme caution.
     */
    //@{
    template <class __vectorOwnerType>
    inline bool FastCopyOf(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & source,
                           bool performSafetyChecks = true)
        CISST_THROW(std::runtime_error)
    {
        return vctFastCopy::VectorCopy(*this, source, performSafetyChecks);
    }

    template <size_type __size, stride_type __stride, class __dataPtrType>
    inline bool FastCopyOf(const vctFixedSizeConstVectorBase<__size, __stride, value_type, __dataPtrType> & source,
                           bool performSafetyChecks = true)
        CISST_THROW(std::runtime_error)
    {
        return vctFastCopy::VectorCopy(*this, source, performSafetyChecks);
    }
    //@}



    /*! Assign to this vector values from a C array given as a
      pointer to value_type.  The purpose of this method is to simplify
      the syntax by not necessitating the creation of an explicit vector
      for the given array.  However, we only provide this method for
      an array of value_type.  For arrays of other types a vector
      still needs to be declared.

      This method assumes that the input array has the necessary
      number of elements.

      \return a reference to this object.
    */
    inline ThisType & Assign(const value_type * elements)
    {
        vctDynamicVectorLoopEngines::
            VoVi< typename vctUnaryOperations<value_type>::Identity >::
            Run(*this, vctDynamicConstVectorRef<value_type>(this->size(), elements));
        return *this;
    }

    /*! Assign to this vector using stdarg macros and variable number
      of arguments.  This operation assumes that all the arguments are
      of type value_type, and that their number is equal to the size
      of the vector.  The user may need to explicitly cast the
      parameters to value_type to avoid runtime bugs and errors.  We
      have not checked if stdarg macros can use reference types
      (probably not), so unlike the other constructors, this
      constructor takes all arguments by value.

      \note This method does not assert that the size is correct,
      as there is no way to know how many arguments were passed.
    */

    inline ThisType & Assign(value_type element0, value_type element1, ...) CISST_THROW(std::runtime_error) {
        const size_type size = this->size();
        if (size < 2) {
            cmnThrow(std::runtime_error("vctDynamicVector: Assign from va_list requires size >= 2"));
        }
        this->at(0) = element0;
        this->at(1) = element1;
        va_list nextArg;
        va_start(nextArg, element1);
        for (index_type i = 2; i < size; ++i) {
            this->at(i) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
        return *this;
    }


    /*! Concatenate a single element at the end of a shorter-by-1
      vector to obtain a vector of my size.  The concatenation result
      is stored in this vector.  The function is useful, e.g., when
      embedding vectors from R^n into R^{n+1} or into homogeneous
      space.
    */
    template <class __vectorOwnerType, class __elementType>
    inline ThisType & ConcatenationOf(const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & otherVector,
                                      __elementType lastElement) {
    vctDynamicVectorRef<value_type> firstElements(*this, 0, otherVector.size() - 1);
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(firstElements, otherVector);
        (*this)[otherVector.size()] = value_type(lastElement);
        return *this;
    }


    /*! \name Size dependant methods.

      The following methods are size dependant, i.e. don't necessarily
      mean anything for all sizes of vector.  For example, using the
      Z() method on a vector of size 2 shouldn't be allowed.
      Therefore, we are using #CMN_ASSERT to check that the size is
      valid */

    //@{

    /*! Returns the first element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 1. */
    value_type & X(void) {
        CMN_ASSERT(this->size() > 0);
        return *(Pointer(0));
    }

    // we have to redeclare it here, shadowed
    const value_type & X(void) const {
        return BaseType::X();
    }


    /*! Returns the second element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 2. */
    value_type & Y(void) {
        CMN_ASSERT(this->size() > 1);
        return *(Pointer(1));
    }

    // we have to redeclare it here, shadowed
    const value_type & Y(void) const {
        return BaseType::Y();
    }


    /*! Returns the third element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 3. */
    value_type & Z(void) {
        CMN_ASSERT(this->size() > 2);
        return *(Pointer(2));
    }

    // we have to redeclare it here, shadowed
    const value_type & Z(void) const {
        return BaseType::Z();
    }

    /*! Returns the fourth element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 4. */
    value_type & W(void) {
        CMN_ASSERT(this->size() > 3);
        return *(Pointer(3));
    }

    // we have to redeclare it here, shadowed
    const value_type & W(void) const {
        return BaseType::W();
    }

    /*! Return a (non-const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal.
      (shadowed from base class).
    */
    vctDynamicVectorRef<_elementType> XY(void) {
        CMN_ASSERT(this->size() > 1);
        return vctDynamicVectorRef<_elementType>(2, Pointer(0), this->stride());
    }

    /*! Return a (non-const) vector reference for the first and third elements of this
      vector. */
    vctDynamicVectorRef<_elementType> XZ(void) {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicVectorRef<_elementType>(2, Pointer(0), 2 * this->stride());
    }

    /*! Return a (non-const) vector reference for the first and fourth elements of this
      vector. */
    vctDynamicVectorRef<_elementType> XW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(2, Pointer(0), 3 * this->stride());
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicVectorRef<_elementType> YZ(void) {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicVectorRef<_elementType>(2, Pointer(1), this->stride());
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicVectorRef<_elementType> YW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(2, Pointer(1), 2 * this->stride());
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the third. */
    vctDynamicVectorRef<_elementType> ZW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(2, Pointer(2), this->stride());
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> XY(void) const {
        return BaseType::XY();
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> XZ(void) const {
        return BaseType::XZ();
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> XW(void) const {
        return BaseType::XW();
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> YZ(void) const {
        return BaseType::YZ();
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> YW(void) const {
        return BaseType::YW();
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> ZW(void) const {
        return BaseType::ZW();
    }

    /*! Return a (non-const) vector reference for the first three
      elements of this vector.  May be used when switching from
      homogeneous coordinates to normal, or when refering to a
      subvector of a row or a column inside a small matrix.
    */
    vctDynamicVectorRef<_elementType> XYZ(void) {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicVectorRef<_elementType>(3, Pointer(0), this->stride());
    }

    /*! Return a (non-const) vector reference for the second, third and fourth elements
      of this vector. */
    vctDynamicVectorRef<_elementType> YZW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(3, Pointer(1),  this->stride());
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> XYZ(void) const {
        return BaseType::XYZ();
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> YZW(void) const {
        return BaseType::YZW();
    }

    /*! Return a (non-const) vector reference for the first four
      elements of this vector.  May be used when switching from
      homogeneous coordinates to normal, or when refering to a
      subvector of a row or a column inside a small matrix.
    */
    vctDynamicVectorRef<_elementType> XYZW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(4, Pointer(0), this->stride());
    }

    /* documented in base class */
    vctDynamicConstVectorRef<_elementType> XYZW(void) const {
        return BaseType::XYZW();
    }

    /*! Create a reference to a sub vector */
    //@{
    vctDynamicVectorRef<_elementType>
    Ref(const size_type size, const size_type startPosition = 0) CISST_THROW(std::out_of_range) {
        if ((startPosition + size) > this->size()) {
            cmnThrow(std::out_of_range("vctDynamicConstVectorBase::Ref: reference is out of range"));
        }
        return vctDynamicVectorRef<_elementType>(size, Pointer(startPosition), this->stride());
    }

    vctDynamicConstVectorRef<_elementType>
    Ref(const size_type size, const size_type startPosition = 0) const CISST_THROW(std::out_of_range) {
        return BaseType::Ref(size, startPosition);
    }
    //@}

    /*! Select a subset of elements by a given sequence of indexes.  The selected
      elements from the input vector are stored in this vector.  There is no
      requirement of order or uniqueness in the indexes sequence, and no verification
      that the indexes are valid.
    */
    template <class __inputVectorOwner, class __indexVectorOwner>
    inline void SelectFrom(
        const vctDynamicConstVectorBase<__inputVectorOwner, _elementType> & input,
        const vctDynamicConstVectorBase<__indexVectorOwner, index_type> & index)
    {
        vctDynamicVectorLoopEngines::SelectByIndex::Run(*this, input, index);
    }

    /*!  Cross Product of two vectors.  This method uses #CMN_ASSERT
      to check that the size of the vector is 3, and can only be
      performed on arguments vectors of size 3.
    */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline void CrossProductOf(
            const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & inputVector2)
    {
        CMN_ASSERT(this->size() == 3);
        CMN_ASSERT(inputVector1.size() == 3);
        CMN_ASSERT(inputVector2.size() == 3);
        (*this)[0] = inputVector1[1] *  inputVector2[2] - inputVector1[2] * inputVector2[1];
        (*this)[1] = inputVector1[2] *  inputVector2[0] - inputVector1[0] * inputVector2[2];
        (*this)[2] = inputVector1[0] *  inputVector2[1] - inputVector1[1] * inputVector2[0];
    }

    //@}



    /*! \name Binary elementwise operations between two vectors.
      Store the result of op(vector1, vector2) to a third vector. */
    //@{
    /*! Binary elementwise operations between two vectors.  For each
      element of the vectors, performs \f$ this[i] \leftarrow
      op(vector1[i], vector2[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ElementwiseProductOf), a division (ElementwiseRatioOf), a
      minimum (ElementwiseMinOf) or a maximum (ElementwiseMaxOf).

      \param vector1 The first operand of the binary operation

      \param vector2 The second operand of the binary operation

      \return The vector "this" modified.
    */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & SumOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                            const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Addition >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & DifferenceOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                   const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Subtraction >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & ElementwiseProductOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                           const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Multiplication >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & ElementwiseRatioOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                         const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Division >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & ElementwiseMinOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                       const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Minimum >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & ElementwiseMaxOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                       const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Maximum >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & ElementwiseClippedInOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & boundVector,
                                             const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::ClipIn >
            ::Run(*this, boundVector, vector);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between two vectors.
      Store the result of op(this, otherVector) back to this vector. */
    //@{
    /*! Store back binary elementwise operations between two
      vectors.  For each element of the vectors, performs \f$
      this[i] \leftarrow op(this[i], otherVector[i])\f$ where
      \f$op\f$ is either an addition (Add), a subtraction
      (Subtraction), a multiplication (ElementwiseMultiply) a division
      (ElementwiseDivide), a minimization (ElementwiseMin) or a
      maximisation (ElementwiseMax).

      \param otherVector The second operand of the binary operation
      (this[i] is the first operand)

      \return The vector "this" modified.
    */
    template <class __vectorOwnerType>
    inline ThisType & Add(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi<typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & Subtract(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ElementwiseMultiply(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ElementwiseDivide(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ElementwiseMin(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ElementwiseMax(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ElementwiseClipAbove(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & upperBoundVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, upperBoundVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ElementwiseClipBelow(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & lowerBoundVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, lowerBoundVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ElementwiseClipIn(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & boundVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::ClipIn >::
            Run(*this, boundVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & operator += (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        return this->Add(otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & operator -= (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        return this->Subtract(otherVector);
    }
    //@}


    /*! \name Binary elementwise operations between two vectors.
      Operate on both elements and store values in both. */
    //@{
    /*! Swap the elements of both vectors with each other.
    */
    template <class __vectorOwnerType>
    inline ThisType & SwapElementsWith(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVio<typename vctStoreBackBinaryOperations<value_type>::Swap >::
            Run(*this, otherVector);
        return *this;
    }
    //@}

    /*! \name Binary elementwise operations a vector and a scalar.
      Store the result of op(vector, scalar) to a third vector. */
    //@{
    /*! Binary elementwise operations between a vector and a scalar.
      For each element of the vector "this", performs \f$ this[i]
      \leftarrow op(vector[i], scalar)\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf), a division (RatioOf), a minimum (ClippedAboveOf) or
      a maximum (ClippedBelowOf).

      \param vector The first operand of the binary operation.
      \param scalar The second operand of the binary operation.

      \return The vector "this" modified.
    */
    template <class __vectorOwnerType>
    inline ThisType & SumOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                            const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & DifferenceOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                   const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ProductOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & RatioOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                              const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ClippedAboveOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                     const value_type upperBound) {
        vctDynamicVectorLoopEngines::
            VoViSi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, vector, upperBound);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ClippedBelowOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                     const value_type lowerBound) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, vector, lowerBound);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ClippedInOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                  const value_type bound) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::ClipIn >::
            Run(*this, vector, bound);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations a scalar and a vector.
      Store the result of op(scalar, vector) to a third vector. */
    //@{
    /*! Binary elementwise operations between a scalar and a vector.
      For each element of the vector "this", performs \f$ this[i]
      \leftarrow op(scalar, vector[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf), a division (RatioOf), a minimum (ClippedAboveOf) or
      a maximum (ClippedBelowOf).

      \param scalar The first operand of the binary operation.
      \param vector The second operand of the binary operation.

      \return The vector "this" modified.
    */
    template <class __vectorOwnerType>
    inline ThisType & SumOf(const value_type scalar,
                            const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & DifferenceOf(const value_type scalar,
                                   const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ProductOf(const value_type scalar,
                                const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & RatioOf(const value_type scalar,
                              const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ClippedAboveOf(const value_type upperBound,
                                     const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Minimum >::
            Run(*this, upperBound, vector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ClippedBelowOf(const value_type lowerBound,
                                     const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, lowerBound, vector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & ClippedInOf(const value_type bound,
                                  const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::ClipIn >::
            Run(*this, bound, vector);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between a vector and a scalar.
      Store the result of op(this, scalar) back to this vector. */
    //@{
    /*! Store back binary elementwise operations between a vector and
      a scalar.  For each element of the vector "this", performs \f$
      this[i] \leftarrow op(this[i], scalar)\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtract), a
      multiplication (Multiply), a division (Divide), a minimum
      (ClipAbove) or a maximum (ClipBelow).

      \param scalar The second operand of the binary operation
      (this[i] is the first operand.

      \return The vector "this" modified.
    */
    inline ThisType & Add(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipAbove(const value_type upperBound) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, upperBound);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipBelow(const value_type lowerBound) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, lowerBound);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipIn(const value_type bound) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::ClipIn >::
            Run(*this, bound);
        return *this;
    }

    /* documented above */
    inline ThisType & operator += (const value_type scalar) {
        return this->Add(scalar);
    }

    /* documented above */
    inline ThisType & operator -= (const value_type scalar) {
        return this->Subtract(scalar);
    }

    /* documented above */
    inline ThisType & operator *= (const value_type scalar) {
        return this->Multiply(scalar);
    }

    /* documented above */
    inline ThisType & operator /= (const value_type scalar) {
        return this->Divide(scalar);
    }
    //@}

    template <class __vectorOwnerType>
    inline ThisType & AddProductOf(const value_type scalar,
                                   const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        vctDynamicVectorLoopEngines::
            VioSiVi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, otherVector);
        return *this;
    }

    template <class __vectorOwnerType1, class __vectorOwnerType2>
    inline ThisType & AddElementwiseProductOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                              const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2)
    {
        vctDynamicVectorLoopEngines::
            VioViVi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, vector1, vector2);
        return *this;
    }



    /*! \name Binary elementwise operations between a vector and a
      matrix.  Store the result of op() to a third vector (this
      vector). */
    //@{

    /*!
      Product of a matrix and a vector.

      \param inputMatrix The first operand of the binary operation

      \param inputVector The second operand of the binary operation

      \return The vector "this" modified.
    */
    template <class __matrixOwnerType, class __vectorOwnerType>
    inline ThisType & ProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
                                const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & inputVector) {
        vctMultiplyMatrixVector(*this, inputMatrix, inputVector);
        return *this;
    }


    /*!
      Product of a vector and a matrix.

      \param inputVector The first operand of the binary operation

      \param inputMatrix The second operand of the binary operation

      \return The vector "this" modified.
    */
    template <class __vectorOwnerType, class __matrixOwnerType>
    inline ThisType & ProductOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & inputVector,
                                const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix) {
        vctMultiplyVectorMatrix(*this, inputVector, inputMatrix);
        return *this;
    }

    //@}


    /*! Define a Subvector class for compatibility with the fixed size vectors.
      A subvector has the same stride as the parent container.

      Example:

      typedef vctDynamicVector<double> VectorType;
      VectorType v(9);
      VectorType::Subvector::Type first4(v, 0, 4);
      VectorType::Subvector::Type last5(v, 4, 5);

      \note There is no straightforward way to define a fixed-size
      subvector of a dynamic vector, because the stride of the dynamic
      vector is not known in compilation time.  A way to do it is:

      vctFixedSizeVectorRef<double, 3 /( add stride here if necessary )/> firstThree(v, 0);
      vctFixedSizeVectorRef<double, 3 /( add stride here if necessary )/> lastThree(v, 6);
    */
#ifndef SWIG
    class Subvector
    {
    public:
        typedef vctDynamicVectorRef<value_type> Type;
    };
#endif

    /*! \name Unary elementwise operations.
      Store the result of op(vector) to another vector. */
    //@{
    /*! Unary elementwise operations on a vector.  For each element of
      the vector "this", performs \f$ this[i] \leftarrow
      op(otherVector[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsOf), the opposite (NegationOf) or the normalized
      version (NormalizedOf).

      \param otherVector The operand of the unary operation.

      \return The vector "this" modified.
    */
    template <class __vectorOwnerType>
    inline ThisType & AbsOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & NegationOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::Negation>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & FloorOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::Floor>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & CeilOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::Ceil>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline ThisType & NormalizedOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) CISST_THROW(std::runtime_error) {
        *this = otherVector;
        this->NormalizedSelf();
        return *this;
    }
    //@}

    /*! \name Store back unary elementwise operations.
      Store the result of op(this) to this vector. */
    //@{
    /*! Unary elementwise operations on a vector.  For each element of
      the vector "this", performs \f$ this[i] \leftarrow
      op(this[i])\f$ where \f$op\f$ can calculate the absolute value
      (AbsSelf), the opposite (NegationSelf) or the normalized version
      (NormalizedSelf).

      \return The vector "this" modified.
    */
    inline ThisType & AbsSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeAbs>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & NegationSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeNegation>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & FloorSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeFloor>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & CeilSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeCeil>::
            Run(*this);
        return *this;
    }

    inline ThisType & NormalizedSelf(void) CISST_THROW(std::runtime_error) {
        value_type norm = value_type(this->Norm());
        if (norm >= TypeTraits::Tolerance()) {
            this->Divide(norm);
        } else {
            cmnThrow(std::runtime_error("Division by quasi zero detected in vctDynamicVector NormalizedSelf()"));
        }
        return *this;
    }
    //@}


};


#endif // _vctDynamicVectorBase_h
