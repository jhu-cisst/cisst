/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2003-09-08

  (C) Copyright 2003-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _cmnPortability_h
#define _cmnPortability_h


#include <cisstConfig.h>
#include <cisstCommon/cmnExport.h>

/*!
  \file
  \brief Portability across compilers and operating systems tools.
  \ingroup cisstCommon
*/

/*!
  Fallback value for both #CISST_OS and #CISST_COMPILER.
*/
#define CISST_UNDEFINED 0


/*! \name Testing the operating system */
//@{
/*!
  Value used to set the Operating System used.  To test if the
  operating system is this one, compare with CISST_OS:
  \code
  #if (CISST_OS == CISST_LINUX)
  ...
  #endif
  \endcode
*/
#define CISST_WINDOWS        1
#define CISST_LINUX          2
#define CISST_RTLINUX        3
#define CISST_IRIX           4
#define CISST_SOLARIS        5
#define CISST_LINUX_RTAI     6
#define CISST_CYGWIN         7
#define CISST_DARWIN         8
#define CISST_QNX            9
#define CISST_LINUX_XENOMAI 10
//@}

/*! \name Testing the compiler */
//@{
/*!
  Value used to set the compiler used.  To test if the compiler is
  this one, compare with CISST_COMPILER:
  \code
  #if (CISST_COMPILER == CISST_GCC)
  ...
  #endif
  \endcode
*/
#define CISST_GCC         1
#define CISST_VCPP6       2
#define CISST_DOTNET7     3
#define CISST_SGI_CC      4
#define CISST_SUN_CC      5
#define CISST_INTEL_CC    6
#define CISST_DOTNET2003  7
#define CISST_DOTNET2005  8
#define CISST_DOTNET2008  9
#define CISST_DOTNET2010 10
#define CISST_DOTNET2012 11
#define CISST_DOTNET2013 12
#define CISST_CLANG      13
#define CISST_DOTNET2015 14
#define CISST_DOTNET2017 15
//@}


/*! Data models for 32 and 64 bits architectures. */
//@{
#define CISST_ILP32 1 // Integers, Longs and Pointers are 32 bits
#define CISST_LP64  2 // Longs and Pointers are 64 bits - Linux, MacOS 64
#define CISST_LLP64 3 // Long Longs and Pointers are 64 bits, longs are still 32 bits - Windows
//@}


// skip rest of tests for doxygen
#ifndef DOXYGEN

// First, see if it's gcc and then determine OS
// (Another compiler may determine the OS differently).
#ifdef __GNUC__  // see man gcc
  #define CISST_COMPILER CISST_GCC
  #if (defined(linux) || defined(__linux__)) // see cpp -dM and ctrl+d
    #define CISST_OS CISST_LINUX
  #endif // linux
  #if CISST_HAS_LINUX_RTAI // overwrite if RTAI
    #undef CISST_OS
    #define CISST_OS CISST_LINUX_RTAI
  #endif // linux RTAI
  #if CISST_HAS_LINUX_XENOMAI // overwrite if Xenomai
    #undef CISST_OS
    #define CISST_OS CISST_LINUX_XENOMAI
  #endif // linux Xenomai
  #ifdef sun
    #define CISST_OS CISST_SOLARIS
  #endif // sun
  #ifdef sgi
    #define CISST_OS CISST_IRIX
  #endif // sgi
  #ifdef __CYGWIN__
    #define CISST_OS CISST_CYGWIN
  #endif // __CYGWIN__
  #ifdef __APPLE__
    #define CISST_OS CISST_DARWIN
  #endif // __APPLE__
  #ifdef __QNX__
    #define CISST_OS CISST_QNX
  #endif // __QNX__
#else // __GNUC__
  #ifdef sgi
    #define CISST_OS CISST_IRIX
    #ifndef __GNUC__
       #define CISST_COMPILER CISST_SGI_CC
    #endif
  #endif // sgi
  #ifdef sun
    #define CISST_OS CISST_SOLARIS
    #ifndef __GNUC__
       #define CISST_COMPILER CISST_SUN_CC
    #endif
  #endif // sun
#endif // __GNUC__


// CLang compiler
#ifdef __clang__  // clang -dM -E -x c /dev/null
  #undef CISST_COMPILER
  #define CISST_COMPILER CISST_CLANG
#endif // __clang__


// Microsoft compilers
#ifdef _WIN32  // see msdn.microsoft.com
// we require Windows 2000, XP or more
#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0500
#endif
  #define CISST_OS CISST_WINDOWS

  // check compiler versions
  #ifdef _MSC_VER
    #define CISST_COMPILER_IS_MSVC

    #ifdef _WIN64
      #define CISST_COMPILER_IS_MSVC_64
    #endif

    #if (_MSC_VER >= 1910)
      #define CISST_COMPILER CISST_DOTNET2017
    #elif (_MSC_VER >= 1900)
      #define CISST_COMPILER CISST_DOTNET2015
    #elif (_MSC_VER >= 1800)
      #define CISST_COMPILER CISST_DOTNET2013
    #elif (_MSC_VER >= 1700)
      #define CISST_COMPILER CISST_DOTNET2012
    #elif (_MSC_VER >= 1600)
      #define CISST_COMPILER CISST_DOTNET2010
    #elif (_MSC_VER >= 1500)
      #define CISST_COMPILER CISST_DOTNET2008
    #elif (_MSC_VER >= 1400)
      #define CISST_COMPILER CISST_DOTNET2005
    #elif (_MSC_VER >= 1310)
      #define CISST_COMPILER CISST_DOTNET2003
    #elif (_MSC_VER >= 1300)
      #define CISST_COMPILER CISST_DOTNET7
    #elif (_MSC_VER >= 1200)
      #define CISST_COMPILER CISST_VCPP6
    #endif
  #endif

#endif // _WIN32


// Intel compiler
#if defined(__INTEL_COMPILER)
  #define CISST_COMPILER CISST_INTEL_CC
  #if defined (__linux__) // linux
    #define CISST_OS CISST_LINUX
  #endif // linux
  #if CISST_HAS_LINUX_RTAI // overwrite if RTAI
    #undef CISST_OS
    #define CISST_OS CISST_LINUX_RTAI
  #endif // linux RTAI
  #if CISST_HAS_LINUX_XENOMAI // overwrite if Xenomai
    #undef CISST_OS
    #define CISST_OS CISST_LINUX_XENOMAI
  #endif // linux Xenomai
  #ifndef CISST_OS // windows at last
    #define CISST_OS CISST_WINDOWS
  #endif // windows
#endif // __INTEL_COMPILER


// Make sure that both OS and compilers are set
#ifndef CISST_COMPILER
  #define CISST_COMPILER CISST_UNDEFINED
#endif

#ifndef CISST_OS
  #define CISST_OS CISST_UNDEFINED
#endif

// We define unifying macros for the two Windows environments:
// Windows and Cygwin
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
// CISST_OS_IS_WINDOWS is set to 1 for Windows and Cygwin,
// and set to zero otherwise
#  define CISST_OS_IS_WINDOWS 1
// Define WIN32 as 1 for any case (Cygwin is not entirely compatible
// otherwise).
#  ifndef WIN32
#    define WIN32 1
#  else
#    undef WIN32
#    define WIN32 1
#  endif
#else
// CISST_OS_IS_WINDOWS is set to 1 for Windows and Cygwin,
// and set to zero otherwise
#  define CISST_OS_IS_WINDOWS 0
#endif


// Define CISST_DATA_MODEL, i.e. either CISST_LLP64 (Windows) or CISST_LP64 (Unix, most of them).  Also use ILP32.
#if (CISST_COMPILER == CISST_GCC)
  #ifdef _LP64
    #define CISST_DATA_MODEL CISST_LP64
  #else
    #ifdef _LLP64
      #define CISST_DATA_MODEL CISST_LLP64
    #else
      // default for now, might have to refine for cygwin?
      #define CISST_DATA_MODEL CISST_ILP32
    #endif
  #endif
#endif // CISST_COMPILER == CISST_GCC

#if (CISST_COMPILER == CISST_CLANG)
  #if defined(__LP64__) && __LP64__
    #define CISST_DATA_MODEL CISST_LP64
  #endif // __LP64__
#endif // CISST_COMPILER == CISST_CLANG

#ifdef CISST_COMPILER_IS_MSVC
  #ifdef CISST_COMPILER_IS_MSVC_64
    #define CISST_DATA_MODEL CISST_LLP64
  #else
    #define CISST_DATA_MODEL CISST_ILP32
  #endif
#endif // CISST_COMPILER_IS_MSVC

#ifndef CISST_DATA_MODEL
  #define CISST_DATA_MODEL CISST_UNDEFINED
#endif



#endif // DOXYGEN end of doxygen section to skip


/*!
  \def CISST_DEPRECATED

  Define the macro CISST_DEPRECATED.  In gcc and .NET, a method or
  other interfaces (e.g., function, and sometimes class, type or
  variable) can be declared as deprecated by adding specific
  nonstandard qualifier in the declaration of the object.

  We have unified these qualifiers into a single macro.  Here's a usage example.

  \code
  void CISST_DEPRECATED f();
  \endcode

  After this declaration, when a programmer uses a call to f() inside
  any other block, a compiler warning is generated on those compilers
  which support deprecation qualifiers.

  The macro is defined as blank if the compiler does not support deprecation.

  \note More work needs to be done on how to declare a deprecated class
*/
//@{
#if (CISST_COMPILER == CISST_GCC) || (CISST_COMPILER == CISST_CLANG)
#define CISST_DEPRECATED __attribute__ ((deprecated))
#else
  #ifdef CISST_COMPILER_IS_MSVC
    #define CISST_DEPRECATED __declspec(deprecated)
  #else
    #define CISST_DEPRECATED
  #endif
#endif
//@}

/*! Disable some specifically annoying warnings printed by Microsoft
  compilers:

  4290 C++ exception specification ignored ...
  4251 'identifier' : class 'type' needs to have dll-interface ...
  4786 'identifier' : identifier was truncated to 'number' characters ...
*/
#ifdef CISST_COMPILER_IS_MSVC
  #pragma warning(disable: 4290 4251 4786)
  #ifndef _CRT_NONSTDC_NO_DEPRECATE
    #define _CRT_NONSTDC_NO_DEPRECATE
  #endif
  #ifndef _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE
  #endif
  #ifndef _SCL_SECURE_NO_DEPRECATE
    #define _SCL_SECURE_NO_DEPRECATE
  #endif
#endif


// Arrays of string providing a human readable description of the OSs and
// compilers.  The system header "string" is included now, i.e. after all
// the flags have been defined (MS Compilers warnings for example)
#include <string>
#ifndef DOXYGEN
extern CISST_EXPORT const std::string cmnOperatingSystemsStrings[];
extern CISST_EXPORT const std::string cmnCompilersStrings[];
#endif // DOXYGEN


#ifndef CISST_USE_CMATH
#include <math.h>
#else
#include <cmath>
#endif
/*!  Discard of the Windows.h definition of macros min and max
*/
#if CISST_OS_IS_WINDOWS

#ifndef NOMINMAX
  #define NOMINMAX
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#endif


/*!
  \brief Detect a NaN

  This macro is a wrapper for different compilers to simplify the use
  of the NaN macro.  When possible, it uses std::isnan from cmath.
  Otherwise, with gcc and icc, it uses isnan() and for Microsoft
  compilers, _isnan().  In most cases, the test should look like:

  \code
  if (CMN_ISNAN(myValue)) {
      ... deal with the problem, i.e. not a number;
  }
  \endcode

  \param x The number to be tested.
*/
// we favor std::isnan
#if CISST_HAS_STD_ISNAN
  #include <cmath>
  #define CMN_ISNAN(x) std::isnan(x)
#else
// hopefully we can removed this when we stop supporting old compilers
  #ifdef CISST_COMPILER_IS_MSVC
    #include <float.h>
    #define CMN_ISNAN(x) (_isnan(x) != 0)
  #else
    #if (CISST_OS == CISST_DARWIN)
      #ifndef isnan
        extern "C" int isnan (double);
      #endif
    #endif
    #ifndef CISST_USE_STD_ISNAN
      #define CMN_ISNAN(x) isnan(x)
    #else
      #define CMN_ISNAN(x) std::isnan(x)
    #endif
  #endif
#endif


/*!
  \brief Detect a finite number

  This function is a wrapper for different compilers to simplify the use
  of the finite number macro.  With gcc and icc, is uses finite() and
  for Microsoft compilers, _finite().  In most cases, the test should
  look like:
  \code
  if (!cmnIsFinite(myValue)) {
      ... deal with the problem, i.e. infinite number;
  }
  \endcode

  \param x The number to be tested.
*/
bool CISST_EXPORT cmnIsFinite(const float & value);
bool CISST_EXPORT cmnIsFinite(const double & value);

// for backward compatibility
#define CMN_ISFINITE(x) cmnIsFinite(x)

/*!
  For whatever reason, there is a syntactic incompatiblity between .NET and gcc
  on the issue of member function template specialization.  .NET requires a strict
  declaration of the specialized member:

  template returnType foo::bar<templateArgs>(parameters);

  followed by the definition

  returnType foo::bar(parameters) { ... }

  Whereas gcc can deduce the template arguments by simply looking at the definition

  template<> returnType foo::bar(parameters) { ... }


  To provide specialization of template functions, we unify these two structures
  by mandating the explicit declaration preceded by the macro
  CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION , then writing the definition preceded
  by the macro CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION.

  For example:

  CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION
  void foo::bar<templateArgs>(parameters);

  CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION
  void foo::bar(parameters) { ... }
*/
#if (CISST_COMPILER == CISST_DOTNET7)
#define CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION template
#define CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION
#else
#define CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION template<>
#define CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION template<>
#endif


/*!  Macro to indicate that a parameter is not used.  In some cases,
  it might be useful to declare a parameter but nevertheless not use
  it.  Some compilers (such as gcc) might issue a warning when this
  happens (with options -Wall -Wextra).  To avoid these warnings, one
  should use the CMN_UNUSED macro in the method/function declaration:

  <code>
     void FunctionOrMethod(const std::string & CMN_UNUSED(filename) = "default");
  </code>

  This macro should be used only when necessary.  Known cases are
  template specialization, passing an argument to resolve template
  inference, arguments provided for API consistency with others
  classes/methods.

  \note This macro is only required where the method is defined.  If
  the declaration and the definition are in two different places
  (e.g. header file and code file), it is not technically required to
  use the macro in the header file.  Nervertheless, we recommend to
  use it in both places.
 */
#if (CISST_COMPILER == CISST_GCC) || (CISST_COMPILER == CISST_CLANG)
#define CMN_UNUSED(argument) MARKED_AS_UNUSED ## argument __attribute__((unused))
#else
#define CMN_UNUSED(argument)
#endif



/*!  \brief Define how to use the constructor of a templated type in a
  method's signature.

  This macro is required as compilers don't expand the class name
  (constructor) of a templated type used in a method's signature the
  same way.  For example:
  \code
  template <class _type>
  void myFunction(const _type & arg1, const _type & arg2 = _type()); // for gcc
  template <class _type>
  void myFunction(const _type & arg1, const _type & arg2 = _type::_type()); // for windows compilers only
  \endcode
  The macro should be used as follows:
  \code
  template <class _type>
  void myFunction(const _type & arg1,
                  const _type & arg2 = CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(_type));
  \endcode
  \param type The template parameter defining the class constructor to call
*/
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003) || (CISST_COMPILER == CISST_DOTNET2005)
#define CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(type) type::type()
#else
#define CMN_DEFAULT_TEMPLATED_CONSTRUCTOR(type) type()
#endif


/*! \brief Somewhat portable compilation warning message.  This works
  with very recent versions of gcc (4.5) and with Microsoft
  compilers.  This macro has not been ported to other compilers. */
#if (CISST_COMPILER == CISST_GCC) || (CISST_COMPILER == CISST_CLANG)
    // gcc 4.5 and above will support this, for lower versions there is a warning about the pragma itself
    #define CMN_DO_PRAGMA(x) _Pragma (#x)
    #define CMN_COMPILATION_WARNING(warningMessage) CMN_DO_PRAGMA(warning("Warning: " #warningMessage))
#else
    #ifdef CISST_COMPILER_IS_MSVC
        #define CMN_COMPILATION_WARNING(warningMessage) __pragma(message("Warning: " ## warningMessage))
    #endif
#endif


/*!  Macro to used for dynamic exception specification.  Dynamic
  exception specification is actually deprecated in C++ 11 so this
  macro expends as nothing on most recent C++ compilers.  When using
  SWIG, we still specify the exception so SWIG can add some custom
  exception handling if the user specifies them.
 */
#undef CISST_THROW
#ifndef SWIG
  #ifdef __cplusplus
    #if (__cplusplus > 199711L)
      #define CISST_THROW(exceptionParameter)
    #endif
  #endif
#endif

#ifndef CISST_THROW
  #define CISST_THROW(exceptionParameter) throw (exceptionParameter)
#endif


#ifndef DOXYGEN

// No __FUNCTION__ for g++ version < 2.6 __FUNCDNAME__ Valid only
// within a function and returns the decorated name of the enclosing
// function (as a string). __FUNCDNAME__ is not expanded if you use
// the /EP or /P compiler option.  __FUNCSIG__ Valid only within a
// function and returns the signature of the enclosing function (as a
// string). __FUNCSIG__ is not expanded if you use the /EP or /P
// compiler option.  __FUNCTION__ Valid only within a function and
// returns the undecorated name of the enclosing function (as a
// string). __FUNCTION__ is not expanded if you use the /EP or /P
// compiler option.


// Visual C++
#ifdef CISST_COMPILER_IS_MSVC
    #ifdef __FUNCSIG__
        #define CMN_PRETTY_FUNCTION __FUNCSIG__
    #else
        #warning "With Visual Studio, you need /EP or /P to have __FUNCSIG__"
    #endif

// GNU CC and Intel CC
#elif (CISST_COMPILER == CISST_GCC) || (CISST_COMPILER == CISST_INTEL_CC) || (CISST_COMPILER == CISST_CLANG)
    #define CMN_PRETTY_FUNCTION __PRETTY_FUNCTION__

// Otherwise
#else
// For SWIG pre-processor, no need to issue a warning
    #ifndef SWIG
         #warning "Visual C++, GNU C++ and Intel CC are supported so far"
    #endif
#endif

// Set a default value
#ifndef CMN_PRETTY_FUNCTION
    #define CMN_PRETTY_FUNCTION ""
#endif

#endif // DOXYGEN


#endif // _cmnPortability_h
