/*
This patch is based a CMake patch available for cppunit, which can be
obtained via the Tracker/Patches link in the SourceForge cppunit page
(http://sourceforge.net/projects/cppunit/).
*/

/* Inspired by config/config.h.in, config.h.cmake is used by CMake. */

/* define if the library defines strstream */
#cmakedefine01 CPPUNIT_HAVE_CLASS_STRSTREAM

/* Define to 1 if you have the <cmath> header file. */
#cmakedefine CPPUNIT_HAVE_CMATH

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine CPPUNIT_HAVE_DLFCN_H

/* Define to 1 if you have the `finite' function. */
#cmakedefine CPPUNIT_HAVE_FINITE

/* Define to 1 if you have the `_finite' function. */
#cmakedefine CPPUNIT_HAVE__FINITE

/* define if the compiler supports GCC C++ ABI name demangling */
#cmakedefine01 CPPUNIT_HAVE_GCC_ABI_DEMANGLE

/* define if compiler has isfinite */
#cmakedefine CPPUNIT_HAVE_ISFINITE

/* Define if you have the libdl library or equivalent. */
#cmakedefine CPPUNIT_HAVE_LIBDL

/* define to 1 if the compiler implements namespaces */
#cmakedefine CPPUNIT_HAVE_NAMESPACES

/* define if the compiler supports Run-Time Type Identification */
#cmakedefine01 CPPUNIT_HAVE_RTTI

/* Define if you have the shl_load function. */
#cmakedefine CPPUNIT_HAVE_SHL_LOAD

/* define if the compiler has stringstream */
#cmakedefine01 CPPUNIT_HAVE_SSTREAM

/* Define to 1 if you have the <strstream> header file. */
#cmakedefine CPPUNIT_HAVE_STRSTREAM

/* Define to 1 to use type_info::name() for class names */
#cmakedefine01 CPPUNIT_USE_TYPEINFO_NAME

/* Version number of package */
#cmakedefine CPPUNIT_VERSION
