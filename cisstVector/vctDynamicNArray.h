/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Daniel Li, Ofri Sadowsky, Anton Deguet
  Created on: 2006-07-10

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctDynamicNArray_h
#define _vctDynamicNArray_h

/*!
  \file
  \brief Declaration of vctDynamicNArray
*/

#include <cisstVector/vctDynamicNArrayBase.h>
#include <cisstVector/vctDynamicNArrayOwner.h>
#include <cisstVector/vctDynamicNArrayRef.h>

/*!
  \ingroup cisstVector

  \brief An nArray object of dynamic size.

  This class defines a nArray object of dynamic size with memory
  allocation.

  The algebraic operations are mostly inherited from the base classes
  vctDynamicNArrayBase and vctDynamicConstNArrayBase.  Here, we will
  briefly describe the specific properties of vctDynamicNArray, with a
  few usage examples.

  <ol>
  <li> The class is templated by its element type, that is, the nArray
  element.  Normally, the element should be an arithmetic type, that
  is, support all the standard arithmetic operations: +, -, *, /, =,
  ==, <, >, <=, >=, ...
  <li> The class is templated by dimension, i.e. for a vector-like
  nArray the dimension would be 1, 2 for a matrix-like nArray, 3 for a
  volume, ...
  <li>The types <code>nsize_type</code>, <code>nindex_type</code> and
  <code>nstride_type</code> are defined within the scope of the nArray
  classes and used in their interfaces to set the sizes, access
  elements or manipulate the layout.
  <li> The class uses dynamically allocated memory, and, more
  importantly, \em owns the memory.  That is, a vctDynamicNArray
  object automatically frees the allocated memory it owns when it is
  destroyed.
  <li>To allocate the memory, use one of the following operations.
  \code
  // define a typical element type and a couple of useful other types
  typedef double ElementType;
  enum {DIMENSION = 3};
  typedef vctDynamicNArray<ElementType, DIMENSION> ArrayType;
  typedef typename ArrayType::nsize_type SizeType;

  // constructor allocation, empty array
  ArrayType a1;

  // Create an empty nArray and later allocate memory.
  ArrayType a2;
  a2.SetSize(SizeType(5, 7, 8));
  vctRandom(a2, ElementType(0), ElementType(10));

  // Create an nArray of some size and change it later.
  // This operation does not preserve any elements in the resized
  // nArray
  ArrayType a3(SizeType(7, 8, 9));
  a3.SetSize(a2.sizes());
  vctRandom(a3, ElementType(-10), ElementType(0));

  // Store an algebraic result to a new nArray.  In this case,
  // memory is allocated by the algebraic operation, and then
  // attached to the nArray object.
  ArrayType a4 = a3 - a2;
  \endcode
  <li> NArray assignment can be facilitated through the Assign method
  (defined in the base class) or as follows.
  \code
  // Initialize all elements to the same value
  ArrayType a5(a3.sizes(), 2.0);

  // Assign one matrix to another.
  vctDynamicNArray<int, DIMENSION> nArrayInt;
  nArrayInt.Assign(a5); // preferred syntax
  nArrayInt = a5; // same operation
  \endcode
  </ol>

  A few more notes.
  <ul>
  <li> The elements lie in contiguous increasing memory addresses.
  <li> There is no direct way of ``disowning'' a vctDynamicNArray,
  that is taking ownership of its memory.
  <li> When a function returns an allocating dynamic nArray object, it
  is better to return a vctReturnDynamicNArray, which is a helper
  class designed for transfering the ownership of the allocated memory
  without reallocation.
  </ul>

  \param _elementType the type of an element in the nArray.

  \param _dimension the dimension the multi-dimensional array.

  \sa vctDynamicNArrayBase vctDynamicConstNArrayBase
*/
template<class _elementType, vct::size_type _dimension>
class vctDynamicNArray :
    public vctDynamicNArrayBase<vctDynamicNArrayOwner<_elementType, _dimension>, _elementType, _dimension>
{

    friend class vctReturnDynamicNArray<_elementType, _dimension>;

public:
    /* define most types from vctContainerTraits and vctNArrayTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(_dimension);

    typedef vctDynamicNArrayBase<vctDynamicNArrayOwner<_elementType, _dimension>, _elementType, _dimension> BaseType;
    typedef vctDynamicNArray<_elementType, _dimension> ThisType;

    /*! Default constructor. Initialize an empty nArray. */
    vctDynamicNArray()
        // The default initialization of vctDynamicNArrayOwner is empty.
    {}

    /*! Constructor: Create an nArray of the specified sizes.  Elements
       initialized with default constructor. */
    vctDynamicNArray(const nsize_type & sizes)
    {
        this->NArray.SetSize(sizes);
    }

    /*! Constructor: Create an nArray of the specified size and assign all
      elements a specific value. */
    vctDynamicNArray(const nsize_type & sizes, value_type value)
    {
        this->NArray.SetSize(sizes);
        SetAll(value);
    }

    /*! Special copy constructor: Take ownership of the data of a
      temporary nArray object of type vctReturnDynamicNArray.  Disown
      the other nArray.
    */
    vctDynamicNArray(const vctReturnDynamicNArray<value_type, _dimension> & otherNArray);


    /*! Copy constructor: Allocate memory to store a copy of the other
      nArray, and copy the elements of the other nArray to this nArray. */
    //@{
    vctDynamicNArray(const ThisType & otherNArray):
        BaseType()
    {
        SetSize(otherNArray.sizes());
        this->Assign(otherNArray);
    }
    template <class _otherNArrayOwnerType>
    vctDynamicNArray(const vctDynamicConstNArrayBase<_otherNArrayOwnerType, value_type, _dimension> & otherNArray)
    {
        SetSize(otherNArray.sizes());
        this->Assign(otherNArray);
    }
    //@}

    /*! Copy constructor: Allocate memory and copy all the elements
      from the other nArray.  This constructor can also be used for
      type conversions.
    */
    template <class _otherNArrayOwnerType, typename _otherNArrayElementType>
    explicit vctDynamicNArray(const vctDynamicConstNArrayBase<_otherNArrayOwnerType, _otherNArrayElementType, _dimension> & otherNArray)
    {
        SetSize(otherNArray.sizes());
        this->Assign(otherNArray);
    }


    /*!  Assignment from an nArray to this nArray.  The operation discards
         the old memory allocated for this nArray and allocates new memory
         the size of the input nArray.  Then the elements of the input nArray
         are copied into this nArray.
     */
    template <class __nArrayOwnerType, typename __elementType>
    ThisType & operator = (const vctDynamicConstNArrayBase<__nArrayOwnerType, __elementType, _dimension> & otherNArray)
    {
        SetSize(otherNArray.sizes());
        this->Assign(otherNArray);
        return *this;
    }

    /*!  Equals operator: Assignment from a vctDynamicNArray to this nArray
         (also a vctDynamicNArray). The operation discards the old memory
         allocated for this nArray and allocates new memory the size of the
         input vctDynamicNArray. Then the elements of the input vctDynamicNArray
         are copied into this nArray.
     */
    ThisType & operator = (const ThisType & otherNArray)
    {
        SetSize(otherNArray.sizes());
        this->Assign(otherNArray);
        return *this;
    }


    /*!  Assignment from a transitional vctReturnDynamicNArray to a
      vctDynamicNArray variable.  This specialized operation does not
      perform any element copy.  Instead it transfers ownership of the
      data from the other nArray to this nArray, and disowns the other
      nArray.  The right hand side operand must be a temporary object
      returned, e.g., from a function or overloaded operator.

      \todo This operator needs some revisions.
    */
    ThisType & operator = (const vctReturnDynamicNArray<value_type, _dimension> & otherNArray);

    /*! Assignment of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value)
    {
        this->SetAll(value);
        return *this;
    }

    // documented in base class
    template <class __nArrayOwnerType, typename __elementType>
    inline ThisType & ForceAssign(const vctDynamicConstNArrayBase<__nArrayOwnerType, __elementType, _dimension> & other) {
        this->SetSize(other.size());
        return this->Assign(other);
    }

    /*! Destructive size change.  Change the size to the specified
      size. Discard all of the old values. */
    //@{
    void SetSize(const nsize_type & sizes)
    {
        this->NArray.SetSize(sizes);
    }
    //@}
};


/*!
  Class vctReturnDynamicNArray is specialized to store a temporary
  nArray object by transferring ownership.  An object of this class has
  all the methods inherited from vctDynamicNArray, but can only be
  constructed in one way -- taking the ownership from another
  vctDynamicNArray (or vctReturnDynamicNArray) object.  In turn, when
  an assignment occurs from a vctReturnDynamicNArray to a
  vctDynamicNArray (or likewise construction), the LHS
  vctDynamicNArray strips this object from the ownership of the data.

  Use this class only when you want to return a newly created dynamic
  nArray from a function, without going through memory allocation and
  deallocation.  Never use it on an object that is going to remain in
  scope after constructing the vctReturnDynamicNArray.
*/
template <class _elementType, vct::size_type _dimension>
class vctReturnDynamicNArray : public vctDynamicNArray<_elementType, _dimension>
{
public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    VCT_NARRAY_TRAITS_TYPEDEFS(_dimension);

    /*! Base type of vctReturnDynamicNArray. */
    typedef vctDynamicNArray<_elementType, _dimension> BaseType;

    explicit vctReturnDynamicNArray(const BaseType & other)
    {
        BaseType & nonConstOther = const_cast<BaseType &>(other);
        // if we don't save it in a variable, it will be destroyed in the Release operation

        const nsize_type sizes = other.sizes();
        this->NArray.Own(sizes, nonConstOther.NArray.Release());
    }
};


// implementation of the special copy constructor of vctDynamicNArray
template <class _elementType, vct::size_type _dimension>
vctDynamicNArray<_elementType, _dimension>::vctDynamicNArray(const vctReturnDynamicNArray<_elementType, _dimension> & other)
{
    vctReturnDynamicNArray<_elementType, _dimension> & nonConstOther =
        const_cast< vctReturnDynamicNArray<_elementType, _dimension> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation

    const nsize_type sizes = other.sizes();
    this->NArray.Own(sizes, nonConstOther.NArray.Release());
}


// implementation of the special assignment operator from vctReturnDynamicNArray to vctDynamicNArray
template <class _elementType, vct::size_type _dimension>
vctDynamicNArray<_elementType, _dimension> &
vctDynamicNArray<_elementType, _dimension>::operator = (const vctReturnDynamicNArray<_elementType, _dimension> & other)
{
    vctReturnDynamicNArray<_elementType, _dimension> & nonConstOther =
        const_cast< vctReturnDynamicNArray<_elementType, _dimension> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation

    const nsize_type sizes = other.sizes();
    this->NArray.clear();
    this->NArray.Own(sizes, nonConstOther.NArray.Release());
    return *this;
}


/*! \name Elementwise operations between nArrays. */
//@{
/*! Operation between nArrays (same size).
  \param inputNArray1 The first operand of the binary operation.
  \param inputNArray2 The second operand of the binary operation.
  \return The nArray result of \f$op(nArray1, nArray2)\f$.
*/
template <class _nArrayOwnerType1, class _nArrayOwnerType2, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator + (const vctDynamicConstNArrayBase<_nArrayOwnerType1, _elementType, _dimension> & inputNArray1,
            const vctDynamicConstNArrayBase<_nArrayOwnerType2, _elementType, _dimension> & inputNArray2)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray1);
    resultStorage.Add(inputNArray2);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented above */
template <class _nArrayOwnerType1, class _nArrayOwnerType2, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator - (const vctDynamicConstNArrayBase<_nArrayOwnerType1, _elementType, _dimension> & inputNArray1,
            const vctDynamicConstNArrayBase<_nArrayOwnerType2, _elementType, _dimension> & inputNArray2)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray1);
    resultStorage.Subtract(inputNArray2);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}
//@}



/*! \name Elementwise operations between an nArray and a
  scalar. */
//@{
/*! Operation between an nArray and a scalar.
  \param inputNArray The first operand of the binary operation.
  \param inputScalar The second operand of the binary operation.
  \return The nArray result of \f$op(nArray, scalar)\f$.
*/
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator + (const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray,
            const _elementType & inputScalar)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray);
    resultStorage.Add(inputScalar);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented above */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator - (const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray,
            const _elementType & inputScalar)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray);
    resultStorage.Subtract(inputScalar);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented above */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator * (const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray,
            const _elementType & inputScalar)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray);
    resultStorage.Multiply(inputScalar);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented above */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator / (const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray,
            const _elementType & inputScalar)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray);
    resultStorage.Divide(inputScalar);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}
//@}



/*! \name Elementwise operations between a scalar and an nArray. */
//@{
/*! Operation between a scalar and an nArray.
  \param inputScalar The first operand of the binary operation.
  \param inputNArray The second operand of the binary operation.
  \return The nArray result of \f$op(scalar, nArray)\f$.
*/
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator + (const _elementType & inputScalar,
            const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray.sizes());
    resultStorage.SumOf(inputScalar, inputNArray);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented above */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator - (const _elementType & inputScalar,
            const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray.sizes());
    resultStorage.DifferenceOf(inputScalar, inputNArray);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented above */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator * (const _elementType & inputScalar,
            const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray.sizes());
    resultStorage.ProductOf(inputScalar, inputNArray);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented above */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator / (const _elementType & inputScalar,
            const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray.sizes());
    resultStorage.RatioOf(inputScalar, inputNArray);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}
//@}


/*! \name Elementwise operations on an nArray. */
//@{
/*! Unary operation on an nArray.
  \param inputNArray The operand of the unary operation
  \return The nArray result of \f$op(nArray)\f$.
*/
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator - (const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & inputNArray)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray.sizes());
    resultStorage.NegationOf(inputNArray);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}
//@}



#ifndef DOXYGEN
template <class _nArrayOwnerType1, class _nArrayOwnerType2, class _elementType, vct::size_type _dimension>
vctReturnDynamicNArray<_elementType, _dimension>
operator * (const vctDynamicConstNArrayBase<_nArrayOwnerType1, _elementType, _dimension> & inputNArray1,
            const vctDynamicConstNArrayBase<_nArrayOwnerType2, _elementType, _dimension> & inputNArray2)
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(inputNArray1.sizes());
    resultStorage.ProductOf(inputNArray1, inputNArray2);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}


/*
  Methods declared previously and implemented here because they require vctReturnDynamicNArray
*/


/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::NArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::Abs(void) const
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(this->sizes());
    vctDynamicNArrayLoopEngines<_dimension>::template
        NoNi<typename vctUnaryOperations<value_type>::AbsValue>::
        Run(resultStorage, *this);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::NArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::Negation(void) const
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(this->sizes());
    vctDynamicNArrayLoopEngines<_dimension>::template
        NoNi<typename vctUnaryOperations<value_type>::Negation>::
        Run(resultStorage, *this);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::NArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::Floor(void) const
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(this->sizes());
    vctDynamicNArrayLoopEngines<_dimension>::template
        NoNi<typename vctUnaryOperations<value_type>::Floor>::
        Run(resultStorage, *this);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline typename vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::NArrayReturnType
vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension>::Ceil(void) const
{
    typedef _elementType value_type;
    vctDynamicNArray<value_type, _dimension> resultStorage(this->sizes());
    vctDynamicNArrayLoopEngines<_dimension>::template
        NoNi<typename vctUnaryOperations<value_type>::Ceil>::
        Run(resultStorage, *this);
    return vctReturnDynamicNArray<value_type, _dimension>(resultStorage);
}

/* Documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class __nArrayOwnerType, class _elementType,
         class _elementOperationType, vct::size_type _dimension>
inline vctReturnDynamicNArray<bool, _dimension>
vctDynamicNArrayElementwiseCompareNArray(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray1,
                                         const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray2)
{
    vctDynamicNArray<bool, _dimension> result(nArray1.sizes());
    vctDynamicNArrayLoopEngines<_dimension>::template
        NoNiNi<_elementOperationType>::Run(result, nArray1, nArray2);
    return vctReturnDynamicNArray<bool, _dimension>(result);
}

/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType, class _elementOperationType, vct::size_type _dimension>
inline vctReturnDynamicNArray<bool, _dimension>
vctDynamicNArrayElementwiseCompareScalar(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & nArray,
                                         const _elementType & scalar)
{
    vctDynamicNArray<bool, _dimension> result(nArray.sizes());
    vctDynamicNArrayLoopEngines<_dimension>::template
        NoNiSi<_elementOperationType>::Run(result, nArray, scalar);
    return vctReturnDynamicNArray<bool, _dimension>(result);
}


/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline vctDynamicNArrayRef<_elementType, _dimension - 1>
vctDynamicNArrayNArraySlice(vctDynamicNArrayBase<_nArrayOwnerType, _elementType, _dimension> & input,
                            vct::size_type dimension,
                            vct::index_type index)
{
    vctDynamicNArrayRef<_elementType, _dimension - 1> slice;
    slice.SliceOf(input, dimension, index);
    return slice;
}

/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType>
inline _elementType &
vctDynamicNArrayElementSlice(vctDynamicNArrayBase<_nArrayOwnerType, _elementType, 1> & input,
                             vct::index_type index)
{
    return input.Element(vctFixedSizeVector<vct::index_type, 1>(index));
}

/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType, vct::size_type _dimension>
inline vctDynamicConstNArrayRef<_elementType, _dimension - 1>
vctDynamicNArrayConstNArraySlice(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, _dimension> & input,
                                 vct::size_type dimension,
                                 vct::index_type index)
{
    vctDynamicConstNArrayRef<_elementType, _dimension - 1> slice;
    slice.SliceOf(input, dimension, index);
    return slice;
}

/* documented in class vctDynamicConstNArrayBase */
template <class _nArrayOwnerType, class _elementType>
inline const _elementType &
vctDynamicNArrayConstElementSlice(const vctDynamicConstNArrayBase<_nArrayOwnerType, _elementType, 1> & input,
                                  vct::index_type index)
{
    return input.Element(vctFixedSizeVector<vct::index_type, 1>(index));
}

#endif // DOXYGEN


#endif // _vctDynamicNArray_h

