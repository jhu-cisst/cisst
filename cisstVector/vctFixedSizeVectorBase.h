/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2003-09-30

  (C) Copyright 2003-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeVectorBase_h
#define _vctFixedSizeVectorBase_h

/*!
  \file
  \brief Declaration of vctFixedSizeVectorBase
 */

#include <cisstCommon/cmnDeSerializer.h>
#include <cisstVector/vctFixedSizeConstVectorBase.h>

#include <cstdarg>



#ifndef DOXYGEN
// forward declaration of auxiliary function to multiply matrix*vector
template <vct::size_type _resultSize, vct::stride_type _resultStride, class _resultElementType, class _resultDataPtrType,
          vct::size_type _matrixCols, vct::stride_type _matrixRowStride, vct::stride_type _matrixColStride, class _matrixDataPtrType,
          vct::stride_type _vectorStride, class _vectorDataPtrType>
inline void MultiplyMatrixVector(
    vctFixedSizeVectorBase<_resultSize, _resultStride, _resultElementType, _resultDataPtrType> & result,
    const vctFixedSizeConstMatrixBase<_resultSize, _matrixCols, _matrixRowStride, _matrixColStride,
    _resultElementType, _matrixDataPtrType> & matrix,
    const vctFixedSizeConstVectorBase<_matrixCols, _vectorStride, _resultElementType, _vectorDataPtrType> & vector);

// forward declaration of auxiliary function to multiply vector*matrix
template <vct::size_type _resultSize, vct::stride_type _resultStride, class _resultElementType, class _resultDataPtrType,
          vct::size_type _vectorSize, vct::stride_type _vectorStride, class _vectorDataPtrType,
          vct::stride_type _matrixRowStride, vct::stride_type _matrixColStride, class _matrixDataPtrType >
inline void MultiplyVectorMatrix(
    vctFixedSizeVectorBase<_resultSize, _resultStride, _resultElementType, _resultDataPtrType> & result,
    const vctFixedSizeConstVectorBase<_vectorSize, _vectorStride, _resultElementType, _vectorDataPtrType> & vector,
    const vctFixedSizeConstMatrixBase<_vectorSize, _resultSize, _matrixRowStride, _matrixColStride,
    _resultElementType, _matrixDataPtrType> & matrix);

// forward declaration of Assign method from dynamic vector to fixed size
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType, class _vectorOwnerType>
inline void vctFixedSizeVectorBaseAssignDynamicConstVectorBase(
    vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> & fixedSizeVector,
    const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & dynamicVector);

#endif // DOXYGEN


/*!
  \brief A template for a fixed length vector with fixed spacing in
  memory.

  This class defines a vector with read/write operations.  It
  extends vctFixedSizeConstVectorBase with non-const methods.  See
  the base class for more documentation.

  \sa vctFixedSizeConstVectorBase
*/
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
class vctFixedSizeVectorBase : public vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type traits for the vector. Based on type of elements, size,
      stride and data representation, it defines array, pointer,
      etc. (see vctFixedSizeVectorTraits). */
    typedef vctFixedSizeVectorTraits<_elementType, _size, _stride> VectorTraits;
    /* documented in the base class */
    typedef vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> ThisType;
    /*! Type of the base class. */
    typedef vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> BaseType;
    typedef typename BaseType::CopyType CopyType;
    typedef cmnTypeTraits<value_type> TypeTraits;

    /* Declare the container-defined typed required by STL, plus the
       types completed by our traits class */
    typedef typename VectorTraits::iterator iterator;
    typedef typename VectorTraits::const_iterator const_iterator;
    typedef typename VectorTraits::reverse_iterator reverse_iterator;
    typedef typename VectorTraits::const_reverse_iterator const_reverse_iterator;

    /*! type of overlay row matrix over this sequence */
    //@{
    typedef typename BaseType::RowConstMatrixRefType RowConstMatrixRefType;
    typedef typename BaseType::RowMatrixRefType RowMatrixRefType;
    //@}
    /*! type of overlay column matrix over this sequence */
    //@{
    typedef typename BaseType::ColConstMatrixRefType ColConstMatrixRefType;
    typedef typename BaseType::ColMatrixRefType ColMatrixRefType;
    //@}

    enum {SIZE = BaseType::SIZE};
    enum {STRIDE = BaseType::STRIDE};
    enum {SIZEMINUSONE = SIZE - 1};

    /*! Returns an iterator on the first element (STL
      compatibility). */
    inline iterator begin(void) {
        return iterator(this->Data);
    }


    /* documented in base class */
    inline const_iterator begin(void) const {
        return BaseType::begin();
    }


    /*! Returns an iterator on the last element (STL
      compatibility). */
    iterator end(void) {
        return iterator(this->Data + STRIDE * SIZE);
    }


    /* documented in base class */
    inline const_iterator end(void) const {
        return BaseType::end();
    }


    /*! Returns a reverse iterator on the last element (STL
      compatibility). */
    reverse_iterator rbegin(void) {
        return reverse_iterator(this->Data + STRIDE * (SIZE - 1));
    }


    /* documented in base class */
    const_reverse_iterator rbegin(void) const {
        return BaseType::rbegin();
    }


    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */
    reverse_iterator rend(void) {
        return reverse_iterator(this->Data - STRIDE);
    }


    /* documented in base class */
    const_reverse_iterator rend(void) const {
        return BaseType::rend();
    }


    /*! Access an element by index.
      \return a reference to the element[index] */
    reference operator[](size_type index) {
        return *(Pointer(index));
    }


    /* documented in base class */
    const_reference operator[](size_type index) const {
        return BaseType::operator[](index);
    }

    /*! Access an element by index (const).  Compare with
      std::vector::at.  This method can be a handy substitute for the
      overloaded operator [] when operator overloading is unavailable
      or inconvenient.

      \return a non-const reference to element[index] */
    reference at(size_type index) CISST_THROW(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

    /* documented in base class */
    const_reference at(size_type index) const CISST_THROW(std::out_of_range) {
        return BaseType::at(index);
    }

#ifndef SWIG
    /*! Access an element by index (non-const).  See method at().
      \return a non-const reference to element[index] */
    reference operator()(size_type index) CISST_THROW(std::out_of_range) {
        return at(index);
    }

    /* documented in base class */
    const_reference operator()(size_type index) const CISST_THROW(std::out_of_range) {
        return BaseType::operator()(index);
    }
#endif


    /*! Access an element by index (non const). This method allows
      to access an element without any bounds checking.

      \return a reference to the element at index */
    reference Element(size_type index) {
        return *(Pointer(index));
    }

    /* documented in base class */
    const_reference Element(size_type index) const {
        return BaseType::Element(index);
    }


    /*! Addition to the STL requirements.  Return a pointer to an element of the
      container, specified by its index.
    */
    pointer Pointer(size_type index = 0) {
        return this->Data + STRIDE * index;
    }


    /* documented in base class */
    const_pointer Pointer(size_type index = 0) const {
        return BaseType::Pointer(index);
    }


    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the vector
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type & value) {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVoSi<typename vctBinaryOperations<value_type>::SecondOperand>::
            Unfold(*this, value);
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
      \name Assignment operation between vectors of different types.

      \param other The vector to be copied.
    */
    //@{
    template <stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & other) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type, __elementType>::Identity >::
            Unfold(*this, other);
        return *this;
    }

    template <stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & operator = (const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & other) {
        return this->Assign(other);
    }
    //@}

    /*! The following Assign() methods provide a convenient interface for assigning a
      list of values to a vector without a need for type conversion.  They precede the
      use of var-arg, which in turn is incapable of enforcing argument type on the
      arguments following the ellipsis.  This simpler interface is defined for a
      selected collection of vector sizes, namely 1 to 4.  For larger sizes, the
      va_arg interface takes over.

      Each of these implementations checks match between the number of arguments
      and the size of the target vector object.  It throws a std::runtime_error
      if there is a size mismatch.
    */
    //@{
    inline ThisType & Assign(const value_type element0) CISST_THROW(std::runtime_error)
    {
        if (this->size() != 1) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (1) and vector size"));
        }
        (*this)[0] = element0;
        return *this;
    }

    inline ThisType & Assign(const value_type element0, const value_type element1) CISST_THROW(std::runtime_error)
    {
        if (this->size() != 2) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (2) and vector size"));
        }
        (*this)[0] = element0;
        (*this)[1] = element1;
        return *this;
    }

    inline ThisType & Assign(const value_type element0, const value_type element1,
                             const value_type element2) CISST_THROW(std::runtime_error)
    {
        if (this->size() != 3) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (3) and vector size"));
        }
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        return *this;
    }

    inline ThisType & Assign(const value_type element0, const value_type element1,
                             const value_type element2, const value_type element3) CISST_THROW(std::runtime_error)
    {
        if (this->size() != 4) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (4) and vector size"));
        }
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        return *this;
    }
    //@}

    /*! Assign to this vector a set of values provided as independent
      arguments, by using cstdarg macros, that is, an unspecified
      number of arguments, greater than 4.  This function is not using
      a recursive engine, as it may be hard and not worthwhile to use
      a recursive engine with the va_arg ``iterator''.  This operation
      assumes that all the arguments are of type value_type, and that
      their number is equal to the size of the vector.  The arguments
      are passed <em>by value</em>.  The user may need to explicitly
      cast the parameters to value_type to avoid runtime bugs and
      errors.

      \return a reference to this vector.
    */
    inline ThisType & Assign(const value_type element0, const value_type element1,
                             const value_type element2, const value_type element3, const value_type element4, ...)
    {
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        (*this)[4] = element4;
        va_list nextArg;
        va_start(nextArg, element4);
        index_type i;
        for (i = 5; i < _size; ++i) {
            (*this)[i] = static_cast<value_type>(va_arg(nextArg, typename TypeTraits::VaArgPromotion));
        }
        va_end(nextArg);
        return *this;
    }


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
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Identity >::
            Unfold(*this, elements);
        return *this;
    }


    /*! Assign to this vector values from a dynamic vector.  This
      method doesn't allow type conversion.

      \note This methods creates a dynamic reference (see
      vctDynamicVectorRef) over the fixed size vector and then use the
      Assign between two dynamic vectors.  As a consequence, the copy
      uses a loop and not the recursive engines.

      \return a reference to this object.
    */
    template <class __vectorOwnerType>
    inline ThisType & Assign(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & other) {
        vctFixedSizeVectorBaseAssignDynamicConstVectorBase(*this, other);
        return *this;
    }


    /*!  \name Forced assignment operation between vectors of
      different types.  On fixed size vectors this method is
      equivalent to Assign.  See notes below!

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
    template <stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & ForceAssign(const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & other) {
        return this->Assign(other);
    }

    template <class __vectorOwnerType>
    inline ThisType & ForceAssign(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & other) {
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
        index_type index;
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

    template <class __dataPtrType>
    inline bool FastCopyOf(const vctFixedSizeConstVectorBase<SIZE, STRIDE, value_type, __dataPtrType> & source,
                           bool performSafetyChecks = true)
        CISST_THROW(std::runtime_error)
    {
        return vctFastCopy::VectorCopy(*this, source, performSafetyChecks);
    }
    //@}



    /*! Concatenate a single element at the end of a shorter-by-1 vector to obtain a vector
      of my size.  The concatenation result is stored in this vector.  The function is useful, e.g.,
      when embedding vectors from R^n into R^{n+1} or into homogeneous space.
    */
    template <stride_type __stride, class __elementTypeVector, class __dataPtrType, class __elementType>
    inline ThisType & ConcatenationOf(const vctFixedSizeConstVectorBase<SIZEMINUSONE, __stride, __elementTypeVector, __dataPtrType> & other,
                                      __elementType last) {
        // recursive copy for the first size-1 elements
        vctFixedSizeVectorRecursiveEngines<SIZEMINUSONE>::template
            VoVi<typename vctUnaryOperations<value_type, __elementTypeVector>::Identity>::
            Unfold(*this, other);
        // cast and assign last element
        (*this)[SIZEMINUSONE] = value_type(last);
        return *this;
    }



    /*! \name Size dependant methods.

      The following methods are size dependant, i.e. don't necessarily
      mean anything for all sizes of vector.  For example, using the
      Z() method on a vector of size 2 shouldn't be allowed.  It would
      have been possible to check these errors at compilation time,
      but this would require an overhead of code and therefore
      execution time which is not worth it.  Therefore, we are using
      and #CMN_ASSERT() to check that the template parameter _size is
      valid

      \note Using #CMN_ASSERT on a template parameter still allows the
      compiler to perform some optimization, which would be harder if
      #CMN_ASSERT was testing a method paramater. */

    //@{

    /*! Returns the first element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 1. */
    value_type & X(void) {
        CMN_ASSERT(_size > 0);
        return *(Pointer(0));
    }

    /* documented in base class */
    const value_type & X(void) const {
        return BaseType::X();
    }


    /*! Returns the second element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 2. */
    value_type & Y(void) {
        CMN_ASSERT(_size > 1);
        return *(Pointer(1));
    }

    /* documented in base class */
    const value_type & Y(void) const {
        return BaseType::Y();
    }


    /*! Returns the third element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 3. */
    value_type & Z(void) {
        CMN_ASSERT(_size > 2);
        return *(Pointer(2));
    }

    /* documented in base class */
    const value_type & Z(void) const {
        return BaseType::Z();
    }

    /*! Returns the fourth element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 4. */
    value_type & W(void) {
        CMN_ASSERT(_size > 3);
        return *(Pointer(3));
    }

    /* documented in base class */
    const value_type & W(void) const {
        return BaseType::W();
    }


    /*! Return a (non-const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal */
    vctFixedSizeVectorRef<_elementType, 2, _stride> XY(void) {
        CMN_ASSERT(_size > 1);
        return vctFixedSizeVectorRef<_elementType, 2, _stride>(Pointer(0));
    }

    /*! Return a (non-const) vector reference for the first and third elements of this
      vector. */
    vctFixedSizeVectorRef<_elementType, 2, 2 * _stride> XZ(void) {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeVectorRef<_elementType, 2, 2 * _stride>(Pointer(0));
    }

    /*! Return a (non-const) vector reference for the first and fourth elements of this
      vector. */
    vctFixedSizeVectorRef<_elementType, 2, 3 * _stride> XW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 2, 3 * _stride>(Pointer(0));
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the second. */
    vctFixedSizeVectorRef<_elementType, 2, _stride> YZ(void) {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeVectorRef<_elementType, 2, _stride>(Pointer(1));
    }

    /*! Return a (non-const) vector reference for the second and fourth elements
      of this vector. */
    vctFixedSizeVectorRef<_elementType, 2, 2 * _stride> YW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 2, 2 * _stride>(Pointer(1));
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the third. */
    vctFixedSizeVectorRef<_elementType, 2, _stride> ZW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 2, _stride>(Pointer(2));
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> XY(void) const {
        return BaseType::XY();
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride> XZ(void) const {
        return BaseType::XZ();
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 2, 3 * _stride> XW(void) const {
        return BaseType::XW();
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> YZ(void) const {
        return BaseType::YZ();
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride> YW(void) const {
        return BaseType::YW();
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> ZW(void) const {
        return BaseType::ZW();
    }

    /*! Return a (non-const) vector reference for the first three elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
    */
    vctFixedSizeVectorRef<_elementType, 3, _stride> XYZ(void) {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeVectorRef<_elementType, 3, _stride>(Pointer(0));
    }


    /*! Return a (non-const) vector reference for the second, third and fourth elements
      of this vector. */
    vctFixedSizeVectorRef<_elementType, 3, _stride> YZW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 3, _stride>(Pointer(1));
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 3, _stride> XYZ(void) const {
        return BaseType::XYZ();
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 3, _stride> YZW(void) const {
        return BaseType::YZW();
    }

    /*! Return a (non-const) vector reference for the first four elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
    */
    vctFixedSizeVectorRef<_elementType, 4, _stride> XYZW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 4, _stride>(Pointer(0));
    }

    /* documented in base class */
    vctFixedSizeConstVectorRef<_elementType, 4, _stride> XYZW(void) const {
        return BaseType::XYZW();
    }
    //@}

    /*! Create an overlay matrix (MatrixRef) on top of this vector and
      return it.
    */
    //@{
    RowConstMatrixRefType AsRowMatrix(void) const
    {
        return BaseType::AsRowMatrix();
    }

    RowMatrixRefType AsRowMatrix(void)
    {
        return RowMatrixRefType(Pointer());
    }

    ColConstMatrixRefType AsColMatrix(void) const
    {
        return BaseType::AsColMatrix();
    }

    ColMatrixRefType AsColMatrix(void)
    {
        return ColMatrixRefType(Pointer());
    }
    //@}

    /*! Create a reference to a sub vector */
    //@{
    template <vct::size_type __subSize>
    vctFixedSizeConstVectorRef<_elementType, __subSize, _stride>
    Ref(const size_type startPosition = 0) const CISST_THROW(std::out_of_range) {
        return BaseType::Ref(startPosition);
    }

    template <vct::size_type __subSize>
    vctFixedSizeVectorRef<_elementType, __subSize, _stride>
    Ref(const size_type startPosition = 0) CISST_THROW(std::out_of_range) {
        vctFixedSizeVectorRef<_elementType, __subSize, _stride> result(*this, startPosition);
        return result;
    }
    //@}

    /*! Select a subset of elements by a given sequence of indexes.  The selected
      elements from the input vector are stored in this vector.  There is no
      requirement of order or uniqueness in the indexes sequence, and no verification
      that the indexes are valid.
    */
    template <size_type __inputSize, stride_type __inputStride, class __inputDataPtrType,
              stride_type __indexStride, class __indexDataPtrType>
    inline void SelectFrom(const vctFixedSizeConstVectorBase<__inputSize, __inputStride, _elementType, __inputDataPtrType> & input,
                           const vctFixedSizeConstVectorBase<_size, __indexStride, index_type, __indexDataPtrType> & index)
    {
        vctFixedSizeVectorRecursiveEngines<_size>::SelectByIndex::Unfold(*this, input, index);
    }


    /*! Cross Product of two vectors.  This method uses #CMN_ASSERT to
      check that the size of the vector is 3, and can only be
      performed on arguments vectors of size 3.
    */
    template <stride_type __stride1, class __dataPtr1Type, stride_type __stride2, class __dataPtr2Type>
    inline void CrossProductOf(const vctFixedSizeConstVectorBase<3, __stride1, _elementType, __dataPtr1Type> & inputVector1,
                               const vctFixedSizeConstVectorBase<3, __stride2, _elementType, __dataPtr2Type> & inputVector2)
    {
        CMN_ASSERT(SIZE == 3);
        (*this)[0] = inputVector1[1] *  inputVector2[2] - inputVector1[2] * inputVector2[1];
        (*this)[1] = inputVector1[2] *  inputVector2[0] - inputVector1[0] * inputVector2[2];
        (*this)[2] = inputVector1[0] *  inputVector2[1] - inputVector1[1] * inputVector2[0];
    }


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
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline ThisType & SumOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1,
                            const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Addition >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }


    /* documented above */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline ThisType & DifferenceOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1,
                                   const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Subtraction >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline ThisType & ElementwiseProductOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1,
                                           const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Multiplication >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline ThisType & ElementwiseRatioOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1,
                                         const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Division >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline ThisType & ElementwiseMinOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1,
                                       const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Minimum >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template <stride_type __stride1, class __dataPtrType1, stride_type __stride2, class __dataPtrType2>
    inline ThisType & ElementwiseMaxOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1,
                                       const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Maximum >
            ::Unfold(*this, vector1, vector2);
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
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & Add(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & Subtract(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ElementwiseMultiply(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ElementwiseDivide(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ElementwiseMin(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ElementwiseMax(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & operator += (const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        return this->Add(otherVector);
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & operator -= (const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        return this->Subtract(otherVector);
    }
    //@}


    /*! \name Binary elementwise operations between two vectors.
      Operate on both elements and store values in both. */
    //@{
    /*! Swap the elements of both vectors with each other.
    */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & SwapElementsWith(vctFixedSizeVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVio< typename vctStoreBackBinaryOperations<value_type>::Swap >::
            Unfold(*this, otherVector);
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
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & SumOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                            const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Addition >::
            Unfold(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & DifferenceOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                   const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ProductOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & RatioOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                              const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Division >::
            Unfold(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ClippedAboveOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                     const value_type upperBound) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi<typename vctBinaryOperations<value_type>::Minimum>::
            Unfold(*this, vector, upperBound);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ClippedBelowOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                     const value_type lowerBound) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Maximum >::
            Unfold(*this, vector, lowerBound);
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
      (ProductOf), a division (RatioOf), a minimum (ClippedAboveOf) or a
      maximum (ClippedBelowOf).

      \param scalar The first operand of the binary operation.
      \param vector The second operand of the binary operation.

      \return The vector "this" modified.
    */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & SumOf(const value_type scalar,
                            const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Addition >::
            Unfold(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & DifferenceOf(const value_type scalar,
                                   const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ProductOf(const value_type scalar,
                                const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & RatioOf(const value_type scalar,
                              const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Division >::
            Unfold(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ClippedAboveOf(const value_type upperBound,
                                     const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Minimum >::
            Unfold(*this, upperBound, vector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & ClippedBelowOf(const value_type lowerBound,
                                     const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Maximum >::
            Unfold(*this, lowerBound, vector);
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
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Unfold(*this, scalar);
      return *this;
    }

    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Unfold(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipAbove(const value_type upperBound) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Unfold(*this, upperBound);
        return *this;
    }

    /* documented above */
    inline ThisType & ClipBelow(const value_type lowerBound) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Unfold(*this, lowerBound);
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


    template <stride_type __stride, class __dataPtrType>
    inline ThisType & AddProductOf(const value_type scalar,
                                   const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector)
    {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSiVi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, scalar, otherVector);
        return *this;
    }

    template <stride_type __stride1, class __dataPtrType1,
              stride_type __stride2, class __dataPtrType2>
    inline ThisType & AddElementwiseProductOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1,
                                              const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2)

    {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioViVi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, vector1, vector2);
        return *this;
    }

    /*! \name Binary elementwise operations between a vector and a matrix.
      Store the result of op() to a third vector. */
    //@{
    /*!
      Product of a matrix and a vector.

      \param inputMatrix The first operand of the binary operation

      \param inputVector The second operand of the binary operation

      \return The vector "this" modified.
    */
    template <size_type __matrixCols, stride_type __matrixRowStride, stride_type __matrixColStride, class __matrixDataPtrType,
              stride_type __vectorStride, class __vectorDataPtrType>
    inline ThisType & ProductOf(const vctFixedSizeConstMatrixBase<_size, __matrixCols, __matrixRowStride, __matrixColStride, _elementType, __matrixDataPtrType> & inputMatrix,
                                const vctFixedSizeConstVectorBase<__matrixCols, __vectorStride, _elementType, __vectorDataPtrType> & inputVector)
    {
        MultiplyMatrixVector(*this, inputMatrix, inputVector);
        return *this;
    }

    template <size_type __vectorSize, stride_type __vectorStride, class __vectorDataPtrType,
              stride_type __matrixRowStride, stride_type __matrixColStride, class __matrixDataPtrType>
    inline ThisType & ProductOf(const vctFixedSizeConstVectorBase<__vectorSize,  __vectorStride, _elementType, __vectorDataPtrType> & inputVector,
                                const vctFixedSizeConstMatrixBase<__vectorSize, _size, __matrixRowStride, __matrixColStride, _elementType, __matrixDataPtrType> & inputMatrix )
    {
        MultiplyVectorMatrix(*this, inputVector, inputMatrix);
        return *this;
    }
    //@}



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
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & AbsOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::AbsValue >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & NegationOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Negation >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & FloorOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Floor >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & CeilOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Ceil>::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template <stride_type __stride, class __dataPtrType>
    inline ThisType & NormalizedOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) CISST_THROW(std::runtime_error) {
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
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeAbs >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & NegationSelf(void) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeNegation >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & FloorSelf(void) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeFloor >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & CeilSelf(void) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeCeil >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline ThisType & NormalizedSelf(void) CISST_THROW(std::runtime_error) {
        value_type norm = value_type(this->Norm());
        if (norm >= TypeTraits::Tolerance()) {
            this->Divide(norm);
        } else {
            cmnThrow(std::runtime_error("Division by quasi zero detected in vctFixedSizeVector NormalizedSelf()"));
        }
        return *this;
    }
    //@}


#if 0   // eliminating definition of GetSubsequence and GetConstSubsequence methods
    /*! Initialize a subsequence of the container, starting at a
      specified position.  The size and the stride of the subsequence
      are given by the type of the result object.

      A subsequence obtained this way may be non-const.  Use
      GetConstSubsequence to obtain a const subsequence.
    */
    template <class _subsequenceType>
    void GetSubsequence(size_type position, _subsequenceType & result) {
        CMN_ASSERT( (_subsequenceType::STRIDE % ThisType::STRIDE) == 0 );
        CMN_ASSERT( position +
                    ((_subsequenceType::SIZE-1) * (_subsequenceType::STRIDE / ThisType::STRIDE))
                    <= (ThisType::SIZE-1) );
        result.SetRef( Pointer(position) );
    }

    template <class _subsequenceType>
    void GetConstSubsequence(size_type position, _subsequenceType & result) {
        BaseType::GetConstSubsequence(position, result);
    }
#endif  // eliminate definition

    /*! ``Syntactic sugar'' to simplify the creation of a reference to a subvector.
      This class declares a non-const subvector type.
      To declare a subvector object, here's an example.

      typedef vctFixedSizeVector<double, 9> Vector9;
      Vector9 v;
      Vector9::Subvector<3>::Type first3(v, 0);  // first 3 elements of v
      Vector9::Subvector<3>::Type mid3(v, 0);  // middle 3 elements of v
      Vector9::Subvector<3>::Type last3(v, 6);  // last 3 elements of v

      \param _subSize the size of the subvector

      \note the stride of the subvector with respect to its parent container
      is always 1.  That is, the memory strides of the subvector and the
      parent container are equal.  For more sophisticated subsequences,
      the user has to write cusomized code.
    */
    template <size_type _subSize>
    class Subvector {
    public:
        typedef vctFixedSizeVectorRef<value_type, _subSize, STRIDE> Type;
    };

};


#endif  // _vctFixedSizeVectorBase_h
