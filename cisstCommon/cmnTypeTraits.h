/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-01-13

  (C) Copyright 2004-2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*! \file
    \brief Declaration of the class cmnTypeTraits
*/
#pragma once

#ifndef _cmnTypeTraits_h
#define _cmnTypeTraits_h

#include <cisstCommon/cmnPortability.h>

#include <string>
// these header file define numeric limits for data types
#include <limits.h>
#include <float.h>

#include <cisstCommon/cmnExport.h>

/*!  The va_arg type promotions have to be uniquely defined per type.
  See explanation for cmnTypeTraits.  This class should not be used by
  itself.  This class is then specialized for the native types.
*/
template<class _type>
class cmnVaArgPromotion {
public:
    /*! Type used for va_arg */
    typedef _type Type;
};

/*! The va_arg type promotions for float leads to double. */
template<>
class cmnVaArgPromotion<float> {
public:
    /*! Type used for va_arg */
    typedef double Type;
};

/*! The va_arg type promotions for short leads to int. */
template<>
class cmnVaArgPromotion<short> {
public:
    /*! Type used for va_arg */
    typedef int Type;
};

/*! The va_arg type promotions for char leads to int. */
template<>
class cmnVaArgPromotion<char> {
public:
    /*! Type used for va_arg */
    typedef int Type;
};

/*! The va_arg type promotions for unsigned short leads to int. */
template<>
class cmnVaArgPromotion<unsigned short> {
public:
    /*! Type used for va_arg */
    typedef unsigned int Type;
};

/*! The va_arg type promotions for unsigned char leads to int. */
template<>
class cmnVaArgPromotion<unsigned char> {
public:
    /*! Type used for va_arg */
    typedef unsigned int Type;
};

/*! The va_arg type promotions for bool leads to int. */
template<>
class cmnVaArgPromotion<bool> {
public:
    /*! Type used for va_arg */
    typedef int Type;
};




/*! \brief A collection of useful information about the C++ basic types,
  represented in a generic programming way.

  \ingroup cisstCommon

  When using generic programming, there is a lot of type information to consider.
  For example, numeric tolerance values, type promotions, the name of the type,
  numerical limits, and so on.  The standard C++ headers contain this information,
  but it is not organized in an accessible manner for generic programming.  The purpose
  of this class is to provide a common representation for type-dependent information.

  For example, to check that the result of a computation is correct within the accepted
  numerical tolerance for the type of operands, write something like
  \code
  _elementType myVariable;
  if (myVariable < cmnTypeTraits<_elementType>::Tolerance()) {
      doSomething();
  };
  \endcode

  The default tolerance for a float is set to 1.0e-5f and for a double
  it is set to 1.0e-9.

  The macros va_list, va_arg, etc. can be used with an ellipsis "..."
  to declare a method or function with an undetermined number of
  arguments.  To retrieve an argument from the stack, it is then
  required to provided the type of the data used, e.g. myVariable =
  va_arg(myArgs, double).  To use va_arg in a templated class such as:

  \code
  template <class _elementType>
  myClass {
  }
  \endcode

  one could imagine a call like va_arg(myArgs, _elementType).
  Unfortunatly, some types are automatically promoted (see ANSI C 89).
  For example, to get a char, one has to use int and to get a float,
  use a double.

  By default, the internal type VaArgPromotion is the same as the template
  parameter.  The class is specialized for the types which require a
  promotion.

  The class can then be used in the following way:

  \code
  _elementType myVariable;
  myVariable = static_cast<_elementType>(va_arg(nextArg, typename cmnTypeTraits<_elementType>::VaArgPromotion));
  \endcode

  \param _elementType The template parameter which defined the type.
*/

template<class _elementType>
class cmnTypeTraits {
public:
    /*! The type in question */
    typedef _elementType Type;

    /*! Boolean type. This has been added to delay the instantiation
      of some templated code by gcc 4.0.  This type is used to define
      vctDynamicConstVectorBase::BoolVectorValueType and
      vctDynamicConstMatrixBase::BoolMatrixValueType. */
    typedef bool BoolType;

    /*! The promotion of the type when passed in a va_arg list */
    typedef typename cmnVaArgPromotion<_elementType>::Type VaArgPromotion;

    /*! Return the numerical tolerance value for computation results. */
    inline static Type Tolerance(void) {
        return ToleranceValue();
    }

    /*! Set the numerical tolerance for this type. */
    inline static void SetTolerance(Type tolerance) {
        ToleranceValue() = tolerance;
    }

    /*! The name of the type. */
    static CISST_EXPORT std::string TypeName(void);

    /*! Return a positive number which is infinity, if available, or the
      greatest possible positive value otherwise.  This function is useful
      for ``idempotent min'' operation.
    */
    inline static Type PlusInfinityOrMax(void)
    {
        return (HasInfinity()) ? PlusInfinity() : MaxPositiveValue();
    }

    /*! Return a negative number which is infinity, if available, or the
      smallest possible negative value otherwise.  This function is useful
      for ``idempotent max'' operation.
    */
    inline static Type MinusInfinityOrMin(void)
    {
        return (HasInfinity()) ? MinusInfinity() : MinNegativeValue();
    }

    /*! The greatest positive value possible for the type. */
    static CISST_EXPORT Type MaxPositiveValue(void);

    /*! The smallest positive value possible for the type (one for
      integral types, a small positive fraction for floating point
      types). */
    static CISST_EXPORT Type MinPositiveValue(void);

    /*! The greatest negative value possible for the type (minus one
      for integral types, a small negative fraction for floating point
      types). */
    static CISST_EXPORT Type MaxNegativeValue(void);

    /*! The smallest negative value possible for the type, that is,
      the negative value of greatest magnitude. */
    static CISST_EXPORT Type MinNegativeValue(void);

    /*! Positive infinity special value, defined for floating point
      types.  For templated code, one can use HasInfinity to check if
      this method is meaningful. */
    static CISST_EXPORT Type PlusInfinity(void);

    /*! Negative infinity special value, defined for floating point
      types For templated code, one can use HasInfinity to check if
      this method is meaningful. */
    static CISST_EXPORT Type MinusInfinity(void);

    /*! Return true if this type has signed, e.g., for signed int, double, etc.
      Return false if this type is unsigned. */
    static CISST_EXPORT bool HasSign(void);

    /*! Check if this type has a meaningful positive infinity and
      negative infinity. */
    inline static bool HasInfinity(void);

    /*! Test if the value is finite. */
    static bool IsFinite(const Type & value);

    /*! Special Not a Number value, defined for floating point types.
      For templated code, one can use HasNaN to check if this
      method is meaningful. */
    static CISST_EXPORT Type NaN(void);

    /*! Test if the value is nan. */
    static CISST_EXPORT bool IsNaN(const Type & value);

    /*! Check if this type has a meaningful Not A Number. */
    inline static bool HasNaN(void);

    /*! The tolerance for the specific type. */
    static CISST_EXPORT const Type DefaultTolerance;

private:
    static CISST_EXPORT Type & ToleranceValue(void);
};


#ifndef DOXYGEN

/* Define which types are signed and which are unsigned */
template<>
inline bool cmnTypeTraits<float>::HasSign(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<double>::HasSign(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<long long int>::HasSign(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<long int>::HasSign(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<int>::HasSign(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<short>::HasSign(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<char>::HasSign(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<unsigned long long int>::HasSign(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long int>::HasSign(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned int>::HasSign(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned short>::HasSign(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned char>::HasSign(void) {
    return false;
}


/* Define HasInfinity for some basic types */
template<>
inline bool cmnTypeTraits<float>::HasInfinity(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<double>::HasInfinity(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<long long int>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<long int>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<int>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<short>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<char>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long long int>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long int>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned int>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned short>::HasInfinity(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned char>::HasInfinity(void) {
    return false;
}


/* Define IsFinite for some basic types */
template<>
inline bool cmnTypeTraits<float>::IsFinite(const float & value) {
    return cmnIsFinite(value);
}

template<>
inline bool cmnTypeTraits<double>::IsFinite(const double & value) {
    return cmnIsFinite(value);
}

template<>
inline bool cmnTypeTraits<long long int>::IsFinite(const long long int & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<long int>::IsFinite(const long int & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<int>::IsFinite(const int & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<short>::IsFinite(const short & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<char>::IsFinite(const char & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<unsigned long long int>::IsFinite(const unsigned long long int & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<unsigned long int>::IsFinite(const unsigned long int & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<unsigned int>::IsFinite(const unsigned int & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<unsigned short>::IsFinite(const unsigned short & CMN_UNUSED(value)) {
    return true;
}

template<>
inline bool cmnTypeTraits<unsigned char>::IsFinite(const unsigned char & CMN_UNUSED(value)) {
    return true;
}


/* Define HasNaN for some basic types */
template<>
inline bool cmnTypeTraits<float>::HasNaN(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<double>::HasNaN(void) {
    return true;
}

template<>
inline bool cmnTypeTraits<long long int>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<long int>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<int>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<short>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<char>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long long int>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long int>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned int>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned short>::HasNaN(void) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned char>::HasNaN(void) {
    return false;
}

/* Define IsNaN for some basic types */
template<>
inline bool cmnTypeTraits<float>::IsNaN(const float & value) {
    return CMN_ISNAN(value);
}

template<>
inline bool cmnTypeTraits<double>::IsNaN(const double & value) {
    return CMN_ISNAN(value);
}

template<>
inline bool cmnTypeTraits<long long int>::IsNaN(const long long int & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<long int>::IsNaN(const long int & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<int>::IsNaN(const int & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<short>::IsNaN(const short & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<char>::IsNaN(const char & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long long int>::IsNaN(const unsigned long long int & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long int>::IsNaN(const unsigned long int & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned int>::IsNaN(const unsigned int & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned short>::IsNaN(const unsigned short & CMN_UNUSED(value)) {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned char>::IsNaN(const unsigned char & CMN_UNUSED(value)) {
    return false;
}


/* Define limits for some types as inline functions */
#define CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(type, maxPositiveValue, minPositiveValue, maxNegativeValue, minNegativeValue) \
template<> inline type cmnTypeTraits<type>::MaxPositiveValue(void) { \
    return maxPositiveValue; \
} \
template<> inline type cmnTypeTraits<type>::MinPositiveValue(void) { \
    return minPositiveValue; \
} \
template<> inline type cmnTypeTraits<type>::MaxNegativeValue(void) { \
    return maxNegativeValue; \
} \
template<> inline type cmnTypeTraits<type>::MinNegativeValue(void) { \
    return minNegativeValue; \
}

CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(float, FLT_MAX, FLT_MIN, -FLT_MIN, -FLT_MAX);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(double, DBL_MAX, DBL_MIN, -DBL_MIN, -DBL_MAX);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(int, INT_MAX, 1, -1, INT_MIN);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(unsigned int, UINT_MAX, 1, 0, 0);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(char, CHAR_MAX, 1, -1, CHAR_MIN);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(unsigned char, CHAR_MAX, 1, 0, 0);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(short, SHRT_MAX, 1, -1, SHRT_MIN);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(unsigned short, SHRT_MAX, 1, 0, 0);
#if (CISST_DATA_MODEL == CISST_LP64)
  CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(long int, LLONG_MAX, 1, -1, LLONG_MIN);
#else
  CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(long int, LONG_MAX, 1, -1, LONG_MIN);
#endif
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(long long int, LLONG_MAX, 1, -1, LLONG_MIN);
#if (CISST_DATA_MODEL == CISST_LP64)
  CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(unsigned long int, LLONG_MAX, 1, 0, 0);
#else
  CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(unsigned long int, LONG_MAX, 1, 0, 0);
#endif
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(unsigned long long int, LLONG_MAX, 1, 0, 0);
CMN_TYPE_TRAITS_SPECIALIZE_LIMITS(bool, true, true, false, false);

#endif // DOXYGEN

#ifndef SWIG
/*! Check whether class T is derived from class Base
    Examples:
       cmnIsDerivedFrom<double, cmnGenericObject>::IS_DERIVED is false
       cmnIsDerivedFrom<cmnDouble, cmnGenericObject>::IS_DERIVED is true
*/
template <typename T, typename Base>
class cmnIsDerivedFrom {
private:
    typedef char One;
    typedef struct { char a[2]; } Two;
    static One Test(Base *obj);
    static One Test(const Base *obj);
    static Two Test(...);
public:
    enum { IS_DERIVED = sizeof(Test(static_cast<T*>(0))) == sizeof(One)};
    enum { IS_NOT_DERIVED = !IS_DERIVED};
};

/*! Check whether class T is derived from class templated class Base.
    This is especially convenient when the template argument to the Base class
    is not known.

    Examples:
       cmnIsDerivedFromTemplated<double, cmnGenericObjectProxy>::IS_DERIVED is false
       cmnIsDerivedFromTemplated<cmnDouble, cmnGenericObjectProxy>::IS_DERIVED is true

    Note that if you know the template argument to the Base class, you can
    instead use:
       cmnIsDerivedFrom<cmnDouble, cmnGenericObjectProxy<double> >::IS_DERIVED

*/
template <typename T, template <typename> class Base>
class cmnIsDerivedFromTemplated {
private:
    typedef char One;
    typedef struct { char a[2]; } Two;
    template <typename C> static One Test(Base<C> *obj);
    template <typename C> static One Test(const Base<C> *obj);
    static Two Test(...);
public:
    enum { IS_DERIVED = sizeof(Test(static_cast<T*>(0))) == sizeof(One)};
    enum { IS_NOT_DERIVED = !IS_DERIVED};
};
#endif // !SWIG

// A static (compile-time) assertion that the Derived class is, in fact, derived from the Base class.
// This has been written to try to generate a comprehensible error message.
template <typename Derived, typename Base, bool b> struct cmnIsDerivedFromAssert { };
template <typename Derived, typename Base> struct cmnIsDerivedFromAssert<Derived, Base, true> { typedef bool ASSERT; };
#define CMN_IS_DERIVED_FROM_ASSERT(Derived, Base) \
    typedef cmnIsDerivedFromAssert<Derived, Base, cmnIsDerivedFrom<Derived, Base>::IS_DERIVED>::ASSERT Derived##_##Base;

#endif // _cmnTypeTraits_h
