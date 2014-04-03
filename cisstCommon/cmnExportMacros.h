/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2001-04-11

  (C) Copyright 2001-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Macros for the creation of Visual C++ Dlls
 */

// clean
#undef CISST_EXPORT

// do it only for windows 32 OSs (95/98/me or NT/2000) and Microsoft compilers.
// Note: we cannot rely on cmnPortability here because this file is, in fact,
// included into cmnPortability.
#if defined(WIN32) && defined(_MSC_VER)

#ifdef CISST_THIS_LIBRARY_AS_DLL
// warning regarding base class not exporting, i.e. all std::
#pragma warning (disable: 4275)
#define CISST_EXPORT _declspec(dllexport)
#elif CISST_DLL         // local and import headers
// warning regarding base class not exporting, i.e. all std::
#pragma warning (disable: 4275)
#define CISST_EXPORT _declspec(dllimport)
#endif

#endif // end windows dll stuff

// for all other operating systems and windows static libraries
#ifndef CISST_EXPORT
#define CISST_EXPORT
#endif


// Doxygen fake code, special values
#ifdef DOXYGEN

#undef CISST_EXPORT
/*!
  \brief Export a symbol for a Dll

   This macro is used only for the creation of Windows Dll with Visual
   C++.  On all other operating systems or compilers, this macro does
   nothing.

   With Windows and Microsoft VC++, whenever someone needs to create a
   Dll, the macro expands as _declspec(dllexport).  To specify that
   the file is used to create a Dll, the preprocessor must have
   \<library\>_EXPORTS defined.  This value is set automatically for
   each library by CMake if a DLL is to be compiled.

   Usage:
   Append macros to declare exported variables, functions and classes.

   - A global variable in a header file:
     \code
     extern CISST_EXPORT myType myVariable;
     \endcode

   - A global function in a header file:
     \code
     CISST_EXPORT myType functionName(..);
     \endcode

   - A class in a header file:
     \code
     class CISST_EXPORT className { ... };
     \endcode

   - For a templated class or method with inlined code only, do not use
     CISST_EXPORT

   - For a template class with no inlined code at all, use CISST_EXPORT
     for the class declaration

   - For a class mixing inlined and non inlined code, use CISST_EXPORT
     for the declarations of the non inlined code only

   When one needs to use an existing Dll, the macro must expand as
   _declspec(dllimport).  To do so, the preprocessor must have
   CISST_DLL defined.  Again, using CMake, this is automatically set.


*/
#define CISST_EXPORT "depends on the OS and compiler"

#endif // Doxygen special values

