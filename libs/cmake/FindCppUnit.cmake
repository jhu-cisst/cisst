#
# $Id: FindCppUnit.cmake 8 2009-01-04 21:13:48Z adeguet1 $
#
# Author(s):  Anton Deguet
# Created on: 2003-07-31
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---
#
# Find the CppUnit includes and library
#
# This module defines
# CPPUNIT_INCLUDE_DIR, where to find tiff.h, etc.
# CPPUNIT_LIBRARIES, the libraries to link against to use CppUnit.
# CPPUNIT_FOUND, If false, do not try to use CppUnit.

# also defined, but not for general use are
# CPPUNIT_LIBRARY, where to find the CppUnit library.
# CPPUNIT_DEBUG_LIBRARY, where to find the CppUnit library in debug mode.

FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
  /home/erc/include
  /usr/local/include
  /usr/include
)

# With Win32, important to have both
IF(WIN32)
  FIND_LIBRARY(CPPUNIT_LIBRARY cppunit
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /home/erc/lib
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(CPPUNIT_DEBUG_LIBRARY cppunitd
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /home/erc/lib
               /usr/local/lib
               /usr/lib)
ELSE(WIN32)
  # On unix system, debug and release have the same name
  FIND_LIBRARY(CPPUNIT_LIBRARY cppunit
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /home/erc/lib
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(CPPUNIT_DEBUG_LIBRARY cppunit
               ${CPPUNIT_INCLUDE_DIR}/../lib
               /home/erc/lib
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)

IF(CPPUNIT_INCLUDE_DIR)
  IF(CPPUNIT_LIBRARY)
    SET(CPPUNIT_FOUND "YES")
    SET(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
    SET(CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(CPPUNIT_LIBRARY)
ENDIF(CPPUNIT_INCLUDE_DIR)


#
# $Log: FindCppUnit.cmake,v $
# Revision 1.5  2007/04/26 19:33:56  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.4  2006/11/20 20:33:19  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.3  2006/09/16 02:07:12  anton
# FindCppUnit: Added debug library.  VC Express 2005 requires both versions of
# CppUnit, i.e. code generation option doesn't allow to interchange debug/release
# versions of CppUnit.
#
# Revision 1.2  2005/09/06 13:58:53  anton
# cmake files: Added license.
#
# Revision 1.1  2005/09/06 02:08:14  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.4  2003/11/17 22:03:28  anton
# Use CMAKE_DL_LIBS instead of -ldl with IF( UNIX)
#
# Revision 1.3  2003/08/14 14:49:07  anton
# -ldl if for unix only
#
# Revision 1.2  2003/08/14 14:11:13  anton
# Added -ldl for cppunit 1.9.11
#
# Revision 1.1.1.1  2003/07/31 18:20:33  anton
# Creation
#
#
