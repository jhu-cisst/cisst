/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Ofri Sadowsky, Anton Deguet
  Created on:	2003-08-18

  (C) Copyright 2003-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeVector_h
#define _vctFixedSizeVector_h

/*!
  \file
  \brief Declaration of vctFixedSizeVector
 */

#include <cisstVector/vctFixedSizeVectorRef.h>

/*! \brief Implementation of a fixed-size vector using template
  metaprogramming.

  \ingroup cisstVector

  The vector type is stored as a contiguous array of a fixed size
  (stack allocation).  It provides methods for operations which are
  implemented using template metaprogramming.  See
  vctFixedSizeConstVectorBase and vctFixedSizeVectorBase for
  more implementation details.

  \param _elementType the type of an element in the vector
  \param _size the size of the vector

  \note We do not provide a constructor from one <code>value_type *</code>.
  You can use vctFixedSizeVectorBase::Assign(const value_type * ) for
  that purpose.  The reason is we did not want to have too many functions
  in the first place.
*/
template <class _elementType, vct::size_type _size>
class vctFixedSizeVector : public vctFixedSizeVectorBase<
    _size, 1, _elementType,
    typename vctFixedSizeVectorTraits<_elementType, _size, 1>::array
    >
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type traits for the vector. Based on type of elements, size
       and stride, it defines array, pointer, etc. (see
       vctFixedSizeVectorTraits). */
    typedef vctFixedSizeVectorTraits<_elementType, _size, 1> VectorTraits;
    /* no need to document, inherit doxygen documentation from
       vctFixedSizeVectorBase */
    typedef vctFixedSizeVector<value_type, _size> ThisType;
    typedef vctFixedSizeVectorBase<_size, 1, _elementType, typename VectorTraits::array> BaseType;
    typedef typename BaseType::CopyType CopyType;
    /*! Type traits for the elements of the vector. */
    typedef class cmnTypeTraits<value_type> TypeTraits;
    typedef typename TypeTraits::VaArgPromotion ElementVaArgPromotion;

    enum {SIZEMINUSONE = _size - 1};

 public:
    /*! Default constructor.  Do nothing. */
    inline vctFixedSizeVector() {}
 
    /*! Copy constructor */
    inline vctFixedSizeVector(const ThisType & other) {
        this->Assign(other);
    }

    /*! Initialize all the elements to the given value.
      \param value the value used to set all the elements of the vector
    */
    explicit inline vctFixedSizeVector(const value_type & value) {
        this->SetAll(value);
    }

    /*! Initialize the elements of this vector with values from another
      vector.  The other vector can include elements of any type,
      which will be converted using standard conversion to elements of this
      vector.
    */
    template <class __elementType, stride_type __stride, class __dataPtrType>
    explicit inline
    vctFixedSizeVector(const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & vector) {
        this->Assign(vector);
    }

    //@{
    /*! initializing a vector from a vector-ref of the same element type does
      not need to be declared "explicit".
    */
    template <stride_type __stride>
    inline
    vctFixedSizeVector(const vctFixedSizeConstVectorRef<_elementType, _size, __stride> & vector) {
        this->Assign(vector);
    }

    template <stride_type __stride>
    inline
    vctFixedSizeVector(const vctFixedSizeVectorRef<_elementType, _size, __stride> & vector) {
        this->Assign(vector);
    }
    //@}

    /*! Initialize the elements of this vector by appending one value
      after the elements from another vector.  The other vector has
      to have the same element type as this one.
    */
    template <stride_type __stride, class __dataPtrType>
    inline
    vctFixedSizeVector(const vctFixedSizeConstVectorBase<SIZEMINUSONE, __stride, _elementType, __dataPtrType> & vector, value_type lastElement) {
        // vctFixedSizeVector(const vctFixedSizeConstVectorBase<_size-1, __stride, _elementType, __dataPtrType> & vector, value_type lastElement) {
        this->ConcatenationOf(vector, lastElement);
    }

    /*! Initialize this vector from a conventional C array by copying
      the elements (assuming input stride == 1).
    */
    template<class __elementType>
    explicit inline
    vctFixedSizeVector(const __elementType * elements)
    {
        this->Assign(elements);
    }

    /*! Initialize the vector from a dynamic vector with elements of
      the same type.

      \note Since the size of the dynamic vector might not match the
      size of the fixed size one, this constructor can throw an
      exception.

      \sa Corresponding Assign()
    */
    template <class __vectorOwnerType>
    inline vctFixedSizeVector(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & vector)
    {
        this->Assign(vector);
    }

    /*! \name Size dependant constructors.

      The following methods are size dependant, i.e. don't necessarily
      mean anything for all sizes of vector.  For example, using the
      constructor vctFixedSizeVector(value, value, value) on a vector
      of size 2 shouldn't be allowed.  It would have been possible to
      check these errors at compilation time, but this would require
      an overhead of code and therefore execution time which is not
      worth it.  Therefore, we are using #CMN_ASSERT to check that
      the template parameter _size is valid

      \note Using #CMN_ASSERT on a template parameter still allows the
      compiler to perform some optimization, which would be harder if
      assert was testing a method paramater.

      \note The constructor with one argument is reserved for a
      SetAll semantics.
      */

    //@{

    /*! Constructor for a vector of size 2. */
    inline vctFixedSizeVector(const value_type & element0, const value_type & element1) {
        CMN_ASSERT(_size == 2);
        (*this)[0] = element0;
        (*this)[1] = element1;
    }

    /*! Constructor for a vector of size 3. */
    inline vctFixedSizeVector(const value_type & element0, const value_type & element1,
                              const value_type & element2) {
        CMN_ASSERT(_size == 3);
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
    }

    /*! Constructor for a vector of size 4. */
    inline vctFixedSizeVector(const value_type & element0, const value_type & element1,
                              const value_type & element2, const value_type & element3) {
        CMN_ASSERT(_size == 4);
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
    }

    /*! Constructor for any size greater than 4, using stdarg macros
      and variable number of arguments.  This operation assumes that
      all the arguments are of type value_type, and that their number
      is equal to the size of the vector.  The user may need to
      explicitly cast the parameters to value_type to avoid runtime
      bugs and errors.  We have not checked if stdarg macros can use
      reference types (probably not), so unlike the other
      constructors, this constructor takes all arguments by value.

      \note This constructor does not assert that the size is correct,
      as there is no way to know how many arguments were passed.
    */
    inline vctFixedSizeVector(const value_type element0, const value_type element1,
        const value_type element2, const value_type element3, const value_type element4, ...)
    {
        CMN_ASSERT(_size > 4);
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        (*this)[4] = element4;
        va_list nextArg;
        va_start(nextArg, element4);
        for (index_type i = 5; i < _size; ++i) {
            (*this).at(i) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
    }

    //@}


    /*!
      Assignment operation between vectors of different types.

      \param other The vector to be copied.
    */
    //@{
	inline ThisType & operator = (const ThisType & other) {
		return reinterpret_cast<ThisType &>(this->Assign(other));
	}

	template <stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & operator = (const vctFixedSizeConstVectorBase<_size, __stride, __elementType,__dataPtrType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
	}

	template <stride_type __stride>
    inline ThisType & operator = (const vctFixedSizeConstVectorRef<value_type, _size, __stride> & other) {
		return reinterpret_cast<ThisType &>(this->Assign(other));
	}

	template <stride_type __stride, class __elementType>
    inline ThisType & operator = (const vctFixedSizeConstVectorRef<__elementType, _size, __stride> & other) {
		this->Assign(other);
		return *this;
	}
    //@}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. */
    bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ')
    {
        _elementType temp[_size];
        size_type index;
        bool valid = true;
        for (index = 0; index < _size; index++) {
            inputStream >> temp[index];  // assumes that operator >> is defined for _elementType
            if (inputStream.fail()) {
                valid = false;
                inputStream.clear();
                break;
            }
            // Now, look for the delimiter
            if (!isspace(delimiter)) {
                if (index < _size-1) {
                    char c;
                    inputStream >> c;
                    if (c != delimiter) {
                        valid = false;
                        break;
                    }
                }
            }
        }
        if (valid) {
            // Only update the object if the parsing was successful for all elements.
            for (index = 0; index < _size; index++)
                (*this)[index] = temp[index];
        }
        return valid;
    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream)
    {
        size_type index;
        const size_type mySize = this->size();
        for (index = 0; index < mySize; ++index) {
            cmnDeSerializeRaw(inputStream, this->Element(index));
        }
    }

};


/*! \name Elementwise operations between fixed size vectors. */
//@{
/*! Operation between fixed size vectors (same size).
  \param vector1 The first operand of the binary operation.
  \param vector2 The second operand of the binary operation.
  \return The vector result of \f$op(vector1, vector2)\f$. */
template <vct::size_type _size, vct::stride_type _vector1Stride, class _vector1Data,
          vct::stride_type _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, _size>
operator + (const vctFixedSizeConstVectorBase<_size, _vector1Stride, _elementType, _vector1Data> & vector1,
            const vctFixedSizeConstVectorBase<_size, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.SumOf(vector1, vector2);
    return result;
}

/* documented above */
template <vct::size_type _size, vct::stride_type _vector1Stride, class _vector1Data,
          vct::stride_type _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, _size>
operator - (const vctFixedSizeConstVectorBase<_size, _vector1Stride, _elementType, _vector1Data> & vector1,
            const vctFixedSizeConstVectorBase<_size, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.DifferenceOf(vector1, vector2);
    return result;
}

//@}


/*! Cross product between fixed size vectors (size 3).
  \param vector1 The first operand of the cross product.
  \param vector2 The second operand of the cross product.
  \return The cross product. */
template <vct::stride_type _vector1Stride, class _vector1Data,
          vct::stride_type _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, 3>
vctCrossProduct(const vctFixedSizeConstVectorBase<3, _vector1Stride, _elementType, _vector1Data> & vector1,
                const vctFixedSizeConstVectorBase<3, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, 3> result;
    result.CrossProductOf(vector1, vector2);
    return result;
}


/*! Cross product between fixed size vectors (size 3).
  \param vector1 The first operand of the cross product.
  \param vector2 The second operand of the cross product.
  \return The cross product. */
template <vct::stride_type _vector1Stride, class _vector1Data,
          vct::stride_type _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, 3>
operator % (const vctFixedSizeConstVectorBase<3, _vector1Stride, _elementType, _vector1Data> & vector1,
            const vctFixedSizeConstVectorBase<3, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, 3> result;
    result.CrossProductOf(vector1, vector2);
    return result;
}


/*! \name Elementwise operations between a fixed size vector and a scalar. */
//@{
/*! Operation between a fixed size vector and a scalar.
 \param vector The first operand of the binary operation.
 \param scalar The second operand of the binary operation.
 \return The vector result of \f$op(vector, scalar)\f$. */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator + (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.SumOf(vector, scalar);
    return result;
}

/* documented above */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator - (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.DifferenceOf(vector, scalar);
    return result;
}

/* documented above */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator * (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.ProductOf(vector, scalar);
    return result;
}

/* documented above */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator / (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.RatioOf(vector, scalar);
    return result;
}
//@}


/*! \name Elementwise operations between a scalar and a fixed size vector. */
//@{
/*! Operation between a scalar and a fixed size vector.
 \param scalar The first operand of the binary operation.
 \param vector The second operand of the binary operation.
 \return The vector result of \f$op(scalar, vector)\f$. */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator + (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)

{
    vctFixedSizeVector<_elementType, _size> result;
    result.SumOf(scalar, vector);
    return result;
}

/* documented above */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator - (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.DifferenceOf(scalar, vector);
    return result;
}

/* documented above */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator * (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.ProductOf(scalar, vector);
    return result;
}

/* documented above */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator / (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.RatioOf(scalar, vector);
    return result;
}
//@}


/*! \name Elementwise operations on a fixed size vector. */
//@{
/*! Unary operation on a fixed size vector.
  \param vector The operand of the unary operation
  \return The vector result of \f$op(vector)\f$.
*/
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size>
operator - (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.NegationOf(vector);
    return result;
}
//@}


/*! Concatenation operator between vectors.  The operands may be any two fixed-size
  vectors.  The result is a vector of a size equal to the sum of sizes, that
  contains the concatenated elements.*/
#ifndef SWIG // SWIG 1.3.22 doesn't like _size1 + _size2 for the template "instantiation" of vector
template <vct::size_type _size1, vct::stride_type _stride1, class _elementType, class _dataPtr1,
          vct::size_type _size2, vct::stride_type _stride2, class _dataPtr2>
inline vctFixedSizeVector<_elementType, _size1 + _size2>
operator & (const vctFixedSizeConstVectorBase<_size1, _stride1, _elementType, _dataPtr1> & vector1,
            const vctFixedSizeConstVectorBase<_size2, _stride2, _elementType, _dataPtr2> & vector2)
{
    vctFixedSizeVector<_elementType, _size1+_size2> result;
    vctFixedSizeVectorRef<_elementType, _size1, 1> subvector1(result.Pointer(0));
    vctFixedSizeVectorRef<_elementType, _size2, 1> subvector2(result.Pointer(_size1));
    subvector1.Assign(vector1);
    subvector2.Assign(vector2);

    return result;
}
#endif


/*
  Methods declared previously and implemented here because they require vctFixedSizeVector
*/

#ifndef DOXYGEN

/* documented in class vctFixedSizeConstVectorBase */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Abs(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::AbsValue>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Negation(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::Negation>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Floor(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::Floor>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Ceil(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::Ceil>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Normalized(void) const CISST_THROW(std::runtime_error) {
    vctFixedSizeVector<_elementType, _size> result(*this);
    result.NormalizedSelf();
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <vct::size_type _size,
          vct::stride_type _stride, class _dataPtrType,
          vct::stride_type __stride, class __dataPtrType,
          class _elementType,
          class _elementOperationType>
inline vctFixedSizeVector<bool, _size>
vctFixedSizeVectorElementwiseCompareVector(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector1,
                                           const vctFixedSizeConstVectorBase<_size, __stride, _elementType, __dataPtrType> & vector2) {
    vctFixedSizeVector<bool, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoViVi<_elementOperationType>::Unfold(result, vector1, vector2);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template<vct::size_type _size,
         vct::stride_type _stride, class _dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeVector<bool, _size>
vctFixedSizeVectorElementwiseCompareScalar(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
                                           const _elementType & scalar) {
    vctFixedSizeVector<bool, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoViSi<_elementOperationType>::Unfold(result, vector, scalar);
    return result;
}

#endif // DOXYGEN

#endif // _vctFixedSizeVector_h
