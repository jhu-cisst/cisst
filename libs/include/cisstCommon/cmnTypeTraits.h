/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnTypeTraits.h 8 2009-01-04 21:13:48Z adeguet1 $
  
  Author(s):	Anton Deguet
  Created on: 2004-01-13

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*! \file
    \brief Declaration of the class cmnTypeTraits
*/


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
    
    /*! Special Not a Number value, defined for floating point types.
      For templated code, one can use HasNaN to check if this
      method is meaningful. */
    static CISST_EXPORT Type NaN();

    /*! Check if this type has a meaningful Not A Number. */
    inline static bool HasNaN(void);

    /*! The tolerance for the specific type. */
    static CISST_EXPORT const Type DefaultTolerance;

private:
    static CISST_EXPORT Type & ToleranceValue();
};


#ifndef DOXYGEN


/* Define which types are signed and which are unsigned */
template<>
inline bool cmnTypeTraits<float>::HasSign() {
    return true;
}

template<>
inline bool cmnTypeTraits<double>::HasSign() {
    return true;
}

template<>
inline bool cmnTypeTraits<long>::HasSign() {
    return true;
}

template<>
inline bool cmnTypeTraits<int>::HasSign() {
    return true;
}

template<>
inline bool cmnTypeTraits<short>::HasSign() {
    return true;
}

template<>
inline bool cmnTypeTraits<char>::HasSign() {
    return true;
}

template<>
inline bool cmnTypeTraits<unsigned long>::HasSign() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned int>::HasSign() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned short>::HasSign() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned char>::HasSign() {
    return false;
}


/* Define HasInfinity for some basic types */
template<>
inline bool cmnTypeTraits<float>::HasInfinity() {
    return true;
}

template<>
inline bool cmnTypeTraits<double>::HasInfinity() {
    return true;
}

template<>
inline bool cmnTypeTraits<long>::HasInfinity() {
    return false;
}

template<>
inline bool cmnTypeTraits<int>::HasInfinity() {
    return false;
}

template<>
inline bool cmnTypeTraits<short>::HasInfinity() {
    return false;
}

template<>
inline bool cmnTypeTraits<char>::HasInfinity() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long>::HasInfinity() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned int>::HasInfinity() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned short>::HasInfinity() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned char>::HasInfinity() {
    return false;
}

/* Define HasNaN for some basic types */
template<>
inline bool cmnTypeTraits<float>::HasNaN() {
    return true;
}

template<>
inline bool cmnTypeTraits<double>::HasNaN() {
    return true;
}

template<>
inline bool cmnTypeTraits<long>::HasNaN() {
    return false;
}

template<>
inline bool cmnTypeTraits<int>::HasNaN() {
    return false;
}

template<>
inline bool cmnTypeTraits<short>::HasNaN() {
    return false;
}

template<>
inline bool cmnTypeTraits<char>::HasNaN() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned long>::HasNaN() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned int>::HasNaN() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned short>::HasNaN() {
    return false;
}

template<>
inline bool cmnTypeTraits<unsigned char>::HasNaN() {
    return false;
}


/* Define limits for some types as inline functions */

// ---------- float ----------
template<>
inline float cmnTypeTraits<float>::MaxPositiveValue()
{
    return FLT_MAX;
}

template<>
inline float cmnTypeTraits<float>::MinPositiveValue()
{
    return FLT_MIN;
}

template<>
inline float cmnTypeTraits<float>::MaxNegativeValue()
{
    return -FLT_MIN;
}

template<>
inline float cmnTypeTraits<float>::MinNegativeValue()
{
    return -FLT_MAX;
}


// ---------- double ----------
template<>
inline double cmnTypeTraits<double>::MaxPositiveValue()
{
    return DBL_MAX;
}

template<>
inline double cmnTypeTraits<double>::MinPositiveValue()
{
    return DBL_MIN;
}

template<>
inline double cmnTypeTraits<double>::MaxNegativeValue()
{
    return -DBL_MIN;
}

template<>
inline double cmnTypeTraits<double>::MinNegativeValue()
{
    return -DBL_MAX;
}


// ---------- int ----------
template<>
inline int cmnTypeTraits<int>::MaxPositiveValue()
{
    return INT_MAX;
}

template<>
inline int cmnTypeTraits<int>::MinPositiveValue()
{
    return 1;
}

template<>
inline int cmnTypeTraits<int>::MaxNegativeValue()
{
    return -1;
}

template<>
inline int cmnTypeTraits<int>::MinNegativeValue()
{
    return INT_MIN;
}

// ---------- char ----------
template<>
inline char cmnTypeTraits<char>::MaxPositiveValue()
{
    return CHAR_MAX;
}

template<>
inline char cmnTypeTraits<char>::MinPositiveValue()
{
    return 1;
}

template<>
inline char cmnTypeTraits<char>::MaxNegativeValue()
{
    return -1;
}

template<>
inline char cmnTypeTraits<char>::MinNegativeValue()
{
    return CHAR_MIN;
}


// ---------- short ----------
template<>
inline short cmnTypeTraits<short>::MaxPositiveValue()
{
    return SHRT_MAX;
}

template<>
inline short cmnTypeTraits<short>::MinPositiveValue()
{
    return 1;
}

template<>
inline short cmnTypeTraits<short>::MaxNegativeValue()
{
    return -1;
}

template<>
inline short cmnTypeTraits<short>::MinNegativeValue()
{
    return SHRT_MIN;
}


// ---------- long ----------
template<>
inline long cmnTypeTraits<long>::MaxPositiveValue()
{
    return LONG_MAX;
}

template<>
inline long cmnTypeTraits<long>::MinPositiveValue()
{
    return 1;
}

template<>
inline long cmnTypeTraits<long>::MaxNegativeValue()
{
    return -1;
}

template<>
inline long cmnTypeTraits<long>::MinNegativeValue()
{
    return LONG_MIN;
}

// ---------- unsigned long ----------
template<>
inline unsigned long cmnTypeTraits<unsigned long>::MaxPositiveValue()
{
    return ULONG_MAX;
}

template<>
inline unsigned long cmnTypeTraits<unsigned long>::MinPositiveValue()
{
    return 1;
}

template<>
inline unsigned long cmnTypeTraits<unsigned long>::MaxNegativeValue()
{
    return 0;
}

template<>
inline unsigned long cmnTypeTraits<unsigned long>::MinNegativeValue()
{
    return 0;
}

// ---------- unsigned int ----------
template<>
inline unsigned int cmnTypeTraits<unsigned int>::MaxPositiveValue()
{
    return UINT_MAX;
}

template<>
inline unsigned int cmnTypeTraits<unsigned int>::MinPositiveValue()
{
    return 1;
}

template<>
inline unsigned int cmnTypeTraits<unsigned int>::MaxNegativeValue()
{
    return 0;
}

template<>
inline unsigned int cmnTypeTraits<unsigned int>::MinNegativeValue()
{
    return 0;
}

// ---------- unsigned short ----------
template<>
inline unsigned short cmnTypeTraits<unsigned short>::MaxPositiveValue()
{
    return USHRT_MAX;
}

template<>
inline unsigned short cmnTypeTraits<unsigned short>::MinPositiveValue()
{
    return 1;
}

template<>
inline unsigned short cmnTypeTraits<unsigned short>::MaxNegativeValue()
{
    return 0;
}

template<>
inline unsigned short cmnTypeTraits<unsigned short>::MinNegativeValue()
{
    return 0;
}

// ---------- unsigned char ----------
template<>
inline unsigned char cmnTypeTraits<unsigned char>::MaxPositiveValue()
{
    return UCHAR_MAX;
}

template<>
inline unsigned char cmnTypeTraits<unsigned char>::MinPositiveValue()
{
    return 1;
}

template<>
inline unsigned char cmnTypeTraits<unsigned char>::MaxNegativeValue()
{
    return 0;
}

template<>
inline unsigned char cmnTypeTraits<unsigned char>::MinNegativeValue()
{
    return 0;
}

// ---------- bool ----------
template<>
inline bool cmnTypeTraits<bool>::MaxPositiveValue()
{
    return true;
}

template<>
inline bool cmnTypeTraits<bool>::MinPositiveValue()
{
    return true;
}

template<>
inline bool cmnTypeTraits<bool>::MaxNegativeValue()
{
    return false;
}

template<>
inline bool cmnTypeTraits<bool>::MinNegativeValue()
{
    return false;
}

#endif // DOXYGEN

#endif // _cmnTypeTraits_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnTypeTraits.h,v $
//  Revision 1.24  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.23  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.22  2006/11/03 04:25:37  anton
//  cmnTypeTraits.h: Doxygen update and renamed VaArgPromotion to cmnVaArgPromotion
//  since the class is declared in the global scope.
//
//  Revision 1.21  2006/10/06 14:29:33  anton
//  All libraries: Re-ordered #include to include cmnPortability.h before any
//  other file, including system header files.
//
//  Revision 1.20  2006/06/23 18:01:53  ofri
//  cmnTypeTraits : added specializations for unsigned int, unsigned short,
//  unsigned char
//
//  Revision 1.19  2006/05/04 23:32:33  kapoor
//  cisstCommon: Added specialized templated methods for bool type. might not
//  make too much logical sense to have a Min & Max for bool, but needed for
//  python wrapping.
//
//  Revision 1.18  2006/01/03 03:33:04  anton
//  cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
//  cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
//  Revision 1.17  2005/12/14 20:21:31  ofri
//  cmnTypeTraits: Added type traits for unsigned long
//
//  Revision 1.16  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.15  2005/09/07 03:38:14  anton
//  cisstVector: Added BoolType to cmnTypeTraits in order to delay the
//  instantiation of the ElementWiseCompare methods for
//  vctDynamic{Vector,Matrix}ConstBase.  Credit goes to Ofri for this solution.
//  See #162.
//
//  Revision 1.14  2005/08/14 00:51:26  anton
//  cisstCommon: Port to gcc-4.0 (use template<> for template specialization).
//
//  Revision 1.13  2005/05/19 19:29:00  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.12  2005/02/03 22:26:38  anton
//  cmnTypeTraits: Added support for native types short and long.
//
//  Revision 1.11  2004/10/25 15:33:51  ofri
//  cmnTypeTraits: Moved definitions of limit functions from cpp files to inline functions
//  in h file.
//
//  Revision 1.10  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.9  2004/08/12 20:11:00  anton
//  cmnTypeTraits: Added template<> for templated class VaArgPromotion
//  specialization
//
//  Revision 1.8  2004/07/13 13:46:22  ofri
//  cmnTypeTraits: Added methods PlusInfinityOrMax() MinusInfinityOrMin() to
//  facilitate ``idempotent'' min and max operations.  Moved the definitions of
//  HasInfinity() methods to inline functions in the header files, so that they can
//  be optimized to skip the function call.
//
//  Revision 1.7  2004/07/12 20:36:36  anton
//  cmnTypeTraits.h:  Typo in documentation (ngative)
//
//  Revision 1.6  2004/04/06 15:23:15  anton
//  Doxygen clean-up
//
//  Revision 1.5  2004/04/02 21:23:23  anton
//  - Added CISST_EXPORT for method TypeName()
//  - Instantiated Infinity and NaN methods for int and char
//  - Added methods HasInfinity and HasNaN to make sure one can test that these
//    methods are meaningful
//
//  Revision 1.4  2004/04/02 16:18:50  anton
//  Removed old code commented with #if 0
//  CISST_EXPORT is not used for the class itself but for the methods that are
//  actually instantiated in cmnTypeTraits.cpp (not inlined)
//
//  Revision 1.3  2004/03/16 21:12:38  ofri
//  Fixing a compilation error
//
//  Revision 1.2  2004/03/16 20:59:50  ofri
//  All the type dependent constants are now accessed through static methods
//  rather than static members, to prevent initialization problems.  I added a
//  number of such constants.  cmnTypeTraits is no longer specialized for
//  char, float, etc., but uses externalized type-promotion class.  This simplifies
//  adding new members to cmnTypeTraits, which do not have to be replicated
//  for the specialized versions.
//
//  Revision 1.1  2004/02/06 15:38:04  anton
//  Renamed cmnVaTypes to cmnTypeTraits
//  Added Tolerance
//
//
//  Revision 1.1  2004/01/15 15:18:45  anton
//  Added cmnVaTypes to solve a type promotion issue with va_arg when used in template classes.
//
//
// ****************************************************************************

