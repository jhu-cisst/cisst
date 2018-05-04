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
#ifndef _vctDynamicVector_h
#define _vctDynamicVector_h

/*!
  \file
  \brief Declaration of vctDynamicVector
*/

#include <cisstCommon/cmnDeSerializer.h>

#include <cisstVector/vctDynamicVectorBase.h>
#include <cisstVector/vctDynamicVectorOwner.h>
#include <cisstVector/vctDynamicVectorRef.h>

/*!
  \ingroup cisstVector

  \brief A vector object of dynamic size.

  This class defines a vector object of dynamic size with memory
  allocation.  Note that unlike the STL vector class (std::vector),
  this class is not meant to be used as a dynamic container, but in
  vector algebra.

  The algebraic operations are mostly inherited from the base classes
  vctDynamicVectorBase and vctDynamicConstVectorBase.  Here, we will
  briefly describe the specific properties of vctDynamicVector, with a
  few usage examples.

  <ol>
  <li> The class is templated by its element type, that is, the vector
  element.  Normally, the element should be an arithmetic type, that
  is, support all the standard arithmetic operations: +, -, *, /, =,
  ==, <, >, <=, >=, ...
  <li> The class uses dynamically allocated memory, and, more
  importantly, \em owns the memory.  That is, a vctDynamicVector object
  automatically frees the allocated memory it owns when it is
  destroyed.
  <li>To allocate the memory, use one of the following operations.
  \code
  // define a typical element type
  typedef double ElementType;

  // the vectorSize variable can be set to any value at any time
  // before creating the vector.
  size_t vectorSize = 12;

  // constructor allocation
  vctDynamicVector<ElementType> v1(vectorSize);

  // Create an empty vector and later allocate memory.
  vctDynamicVector<ElementType> v2;
  v2.SetSize(vectorSize);

  // Create a dynamic vector of some size and then change it.
  // This operation does not preserve any elements in the resized
  // vector
  vctDynamicVector<Elements> v3(3 * vectorSize);
  v3.SetSize(2 * vectorSize);

  // resize a vector and keep as many elements as possible.
  v3.resize(vectorSize);

  // Store an algebraic result to a new vector.  In this case,
  // memory is allocated by the algebraic operation, and then
  // attached to the vector object.
  vctDynamicVector<double> v4 = v3 - v2;
  \endcode
  <li> Vector assignment can be facilitated through the Assign method
  (defined in the base class) or as follows.
  \code
  // Initialize all elements to the same value
  vctDynamicVector<ElementType> v5(vectorSize, 2.0);

  // Initialize the elements by specific value.  NOTE: All the
  // arguments MUST be of type ElementType
  vctDynamicVector<ElementType> sequence(7,
                                         1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);

  // Assign one vector to another.  This operation reallocates
  // space for the target vector.  Note that the right-hand-side
  // object can be any ``dynamic'' vector of any element type,
  // not just a vctDynamicVector<ElementType>
  vctDynamicMatrix<int> someNumbers(numRows, numCols);
  v1 = someNumbers.Column(2);
  \endcode
  </ol>

  A few more notes.
  <ul>
  <li> The stride is always 1, that is, the elements lie in contiguous
  increasing memory addresses.
  <li> There is no direct way of ``disowning'' a vctDynamicVector,
  that is taking ownership of its memory.
  <li> When a function returns an allocating dynamic vector object, it
  is better to return a vctReturnDynamicVector, which is a helper
  class designed for transfering the ownership of the allocated memory
  without reallocation.
  </ul>

  \param _elementType the type of an element in the vector

  \sa vctDynamicVectorBase vctDynamicConstVectorBase
*/
template <class _elementType>
class vctDynamicVector : public vctDynamicVectorBase<vctDynamicVectorOwner<_elementType>, _elementType>
{

    friend class vctReturnDynamicVector<_elementType>;

public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctDynamicVector<_elementType> ThisType;
    typedef vctDynamicVectorBase<vctDynamicVectorOwner<_elementType>, _elementType> BaseType;
    typedef typename BaseType::CopyType CopyType;
    typedef typename BaseType::TypeTraits TypeTraits;
    typedef typename BaseType::ElementVaArgPromotion ElementVaArgPromotion;

    /*! Default constructor. Initialize an empty vector. */
    vctDynamicVector()
        // The default initialization of vctDynamicVectorOwner is empty.
    {}


    /*! Constructor: Create a vector of the specified size.  Elements
       initialized with default constructor. */
    explicit vctDynamicVector(size_type size) {
        this->SetSize(size);
    }

    /*! Constructor: Create a vector of the specified size and assign all
      elements a specific value. */
    vctDynamicVector(size_type size, value_type value) {
        this->SetSize(size);
        this->SetAll(value);
    }

    /*! Constructor for any size greater than or equal to 2, using
      stdarg macros and variable number of arguments.  This operation
      assumes that all the arguments are of type value_type, and that
      their number is equal to the size of the vector.  The user may
      need to explicitly cast the parameters to value_type to avoid
      runtime bugs and errors.  We have not checked if stdarg macros
      can use reference types (probably not), so unlike the other
      constructors, this constructor takes all arguments by value.

      \note This constructor does not assert that the size is correct,
      as there is no way to know how many arguments were passed.
    */
    vctDynamicVector(size_type size, value_type element0, value_type element1, ...) CISST_THROW(std::runtime_error) {
        if (size < 2) {
            cmnThrow(std::runtime_error("vctDynamicVector: Constructor from va_list requires size >= 2"));
        }
        this->SetSize(size);
        this->at(0) = element0;
        this->at(1) = element1;
        va_list nextArg;
        va_start(nextArg, element1);
        for (index_type i = 2; i < size; ++i) {
            this->at(i) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
    }

    /*! Constructor: Create a vector of the specified size and assign the
      elements values from the memory block pointed to */
    vctDynamicVector(size_type size, const value_type * values) {
        this->SetSize(size);
        this->Assign(values);
    }

    /*! Special copy constructor: Take ownership of the data of a
      temporary vector object of type vctReturnDynamicVector.  Disown
      the other vector.
    */
    vctDynamicVector(const vctReturnDynamicVector<value_type> & otherVector);


    /*! Copy constructor: Allocate memory to store a copy of the other
      vector, and copy the elements of the other vector to this
      vector.
    */
    vctDynamicVector(const ThisType & otherVector):
        BaseType()
    {
        this->SetSize(otherVector.size());
        this->Assign(otherVector);
    }


    /*! Copy constructor: Allocate memory and copy all the elements
      from the other vector.
    */
    template <class _otherVectorOwnerType>
    vctDynamicVector(const vctDynamicConstVectorBase<_otherVectorOwnerType, value_type> & otherVector) {
        this->SetSize(otherVector.size());
        this->Assign(otherVector);
    }


    /*! Copy constructor: Allocate memory and copy all the elements
      from the other vector.  This constructor can also be used for
      type conversions.
    */
    template <class _otherVectorOwnerType, typename _otherVectorElementType>
    explicit vctDynamicVector(const vctDynamicConstVectorBase<_otherVectorOwnerType, _otherVectorElementType> & otherVector) {
        this->SetSize(otherVector.size());
        this->Assign(otherVector);
    }

    /*! Constructor from a fixed-size vector */
    template <size_type __size, stride_type __stride, class __elementType, class __dataPtrType>
    explicit vctDynamicVector(const vctFixedSizeConstVectorBase<__size, __stride, __elementType, __dataPtrType> & fixedVector) {
        this->SetSize(__size);
        this->Assign(fixedVector);
    }

    /*!  Assignment from a dynamic vector to a vector.  The
      operation discards the old memory allocated for this vector, and
      allocates new memory the size of the input vector.  Then the
      elements of the input vector are copied into this vector.
    */
    template <class __vectorOwnerType, typename __elementType>
    ThisType & operator = (const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & otherVector) {
        this->SetSize(otherVector.size());
        this->Assign(otherVector);
        return *this;
    }


    /*!  Assignment from a dynamic vector to this vector.  The
      operation discards the old memory allocated for this vector, and
      allocates new memory the size of the input vector.  Then the
      elements of the input vector are copied into this vector.
    */
    ThisType & operator = (const ThisType& other) {
        this->SetSize(other.size());
        this->Assign(other);
        return *this;
    }

    /*!  Assignement from a transitional vctReturnDynamicVector to a
      vctDynamicVector variable.  This specialized operation does not
      perform any element copy.  Instead it transfers ownership of the
      data from the other vector to this vector, and disowns the other
      vector.  The right hand side operand must be a temporary object
      returned, e.g., from a function or overloaded operator.
    */
    ThisType & operator = (const vctReturnDynamicVector<value_type> & other);

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

    // documented in base class
    template <class __vectorOwnerType, typename __elementType>
    inline ThisType & ForceAssign(const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & other) {
        this->SetSize(other.size());
        this->Assign(other);
        return *this;
    }

    // documented in base class
    template <size_type __size, stride_type __stride, class __elementType, class __dataPtrType>
    inline ThisType & ForceAssign(const vctFixedSizeConstVectorBase<__size, __stride, __elementType, __dataPtrType>
                                  & other) {
        this->SetSize(other.size());
        this->Assign(other);
        return *this;
    }

    /*! Non-destructive size change.  Change the size to the specified
      size, and copy as many elements as possible from the former
      vector.
    */
    void resize(size_type size) {
        const size_type oldSize = this->size();
        if (oldSize == size)
            return;
        ThisType newData(size);
        size_type minSizes = std::min(size, oldSize);
        const size_type corner = 0;
        vctDynamicConstVectorRef<value_type> myDataMinSpaceRef(*this, corner, minSizes);
        vctDynamicVectorRef<value_type> newDataMinSpaceRef(newData, corner, minSizes);
        newDataMinSpaceRef.Assign(myDataMinSpaceRef);
        this->Vector.Disown();
        this->Vector.Own(size, newData.Vector.Release());
    }

    /*! DESTRUCTIVE size change.  Change the size to the specified
      size.  Discard of all the old values. */
    void SetSize(size_type size) {
        this->Vector.SetSize(size);
    }

    void SetSize(size_type size, const value_type newValue) {
        this->Vector.SetSize(size);
        this->SetAll(newValue);
    }

    /*! Read from an unformatted text input (e.g., one created by ToStreamRaw).
      Returns true if successful. This particular implementation assumes that
      the correct vector size is already set. */
    bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ')
    {
        size_type size = this->size();
        _elementType *temp = new _elementType[size];
        size_type index;
        bool valid = true;
        for (index = 0; index < size; index++) {
            inputStream >> temp[index];  // assumes that operator >> is defined for _elementType
            if (inputStream.fail()) {
                valid = false;
                inputStream.clear();
                break;
            }
            // Now, look for the delimiter
            if (!isspace(delimiter)) {
                if (index < size-1) {
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
            for (index = 0; index < size; index++)
                (*this)[index] = temp[index];
        }
        delete [] temp;
        return valid;
    }

    /*! Binary deserialization */
    void DeSerializeRaw(std::istream & inputStream)
    {
        // get and set size
        size_type mySize = 0;
        cmnDeSerializeSizeRaw(inputStream, mySize);
        this->SetSize(mySize);

        // get data
        size_type index;
        for (index = 0; index < mySize; ++index) {
            cmnDeSerializeRaw(inputStream, this->Element(index));
        }
    }

};


/*!
  \ingroup cisstVector

  Class vctReturnDynamicVector is specialized to store a temporary
  vector object by transfering ownership.  An object of this class has
  all the methods inherited from vctDynamicVector, but can only be
  constructed in one way -- taking the ownership from another
  vctDynamicVector (or vctReturnDynamicVector) object.  In turn, when
  an assignment occurs from a vctReturnDynamicVector to a
  vctDynamicVector (or likewise construction), the lhs
  vctDynamicVector strips this object from the ownership of the data.

  Use this class only when you want to return a newly created dynamic
  vector from a function, without going through memory allocation and
  deallocation.  Never use it on an object that is going to remain
  in scope after constructing the vctReturnDynamicVector.
*/
template <class _elementType>
class vctReturnDynamicVector : public vctDynamicVector<_elementType> {
public:
    /*! Base type of vctReturnDynamicVector. */
    typedef vctDynamicVector<_elementType> BaseType;
    explicit vctReturnDynamicVector(const BaseType & other) {
        BaseType & nonConstOther = const_cast<BaseType &>(other);
        // if we don't save it in a variable, it will be destroyed in the Release operation
        const vct::size_type size = other.size();
        this->Vector.Own(size, nonConstOther.Vector.Release());
    }
};


// implementation of the special copy constuctor of vctDynamicVector
template <class _elementType>
vctDynamicVector<_elementType>::vctDynamicVector(const vctReturnDynamicVector<_elementType> & other) {
    vctReturnDynamicVector<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicVector<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const size_type size = other.size();
    this->Vector.Own(size, nonConstOther.Vector.Release());
}


// implementation of the special assignment operator from vctReturnDynamicVector to vctDynamicVector
template <class _elementType>
vctDynamicVector<_elementType> &
vctDynamicVector<_elementType>::operator = (const vctReturnDynamicVector<_elementType> & other) {
    vctReturnDynamicVector<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicVector<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const vct::size_type size = other.size();
    this->Vector.Disown();
    this->Vector.Own(size, nonConstOther.Vector.Release());
    return *this;
}


/*! \name Elementwise operations between dynamic vectors. */
//@{
/*! Operation between dynamic vectors (same size).
  \param inputVector1 The first operand of the binary operation.
  \param inputVector2 The second operand of the binary operation.
  \return The vector result of \f$op(vector1, vector2)\f$.
*/
template <class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
operator + (const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector1);
    resultStorage.Add(inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template <class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector1);
    resultStorage.Subtract(inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}


/*! \name Cross product between dynamic vectors. */
/*! The cross product between two dynamic vectors will assert that
  both input vectors are of size 3.

  \param inputVector1 left operand of the cross product

  \param inputVector2 right operand of the cross product

  \return The cross product as a vctReturnDynamicVector
*/
//@{
template <class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
operator % (const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(3);
    resultStorage.CrossProductOf(inputVector1, inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

template <class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
vctCrossProduct(const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
                const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(3);
    resultStorage.CrossProductOf(inputVector1, inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a dynamic vector and a
  scalar. */
//@{
/*! Operation between a dynamic vector and a scalar.
  \param inputVector The first operand of the binary operation.
  \param inputScalar The second operand of the binary operation.
  \return The vector result of \f$op(vector, scalar)\f$.
*/
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator + (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Add(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Subtract(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Multiply(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator / (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Divide(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a scalar and a dynamic
  vector. */
//@{
/*! Operation between a scalar and a dynamic vector.
  \param inputScalar The first operand of the binary operation.
  \param inputVector The second operand of the binary operation.
  \return The vector result of \f$op(scalar, vector)\f$.
*/
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator + (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.SumOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.DifferenceOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.ProductOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator / (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.RatioOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}


/*! \name Elementwise operations on a dynamic vector. */
//@{
/*! Unary operation on a dynamic vector.
  \param inputVector The operand of the unary operation
  \return The vector result of \f$op(vector)\f$.
*/
template <class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.NegationOf(inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}



/*
  Methods declared previously and implemented here because they require vctReturnDynamicVector
*/

#ifndef DOXYGEN
/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Abs(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::AbsValue>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Negation(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::Negation>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Floor(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::Floor>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Ceil(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::Ceil>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorReturnType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Normalized(void) const CISST_THROW(std::runtime_error) {
    vctDynamicVector<value_type> resultStorage(*this);
    resultStorage.NormalizedSelf();
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class __vectorOwnerType, class _elementType,
          class _elementOperationType>
inline vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareVector(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector1,
                                         const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector2) {
    vctDynamicVector<bool> result(vector1.size());
    vctDynamicVectorLoopEngines::
        VoViVi<_elementOperationType>::Run(result, vector1, vector2);
    return vctReturnDynamicVector<bool>(result);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType, class _elementOperationType>
inline vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareScalar(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                                         const _elementType & scalar) {
    vctDynamicVector<bool> result(vector.size());
    vctDynamicVectorLoopEngines::
        VoViSi<_elementOperationType>::Run(result, vector, scalar);
    return vctReturnDynamicVector<bool>(result);
}

#endif // DOXYGEN

#endif // _vctDynamicVector_h
