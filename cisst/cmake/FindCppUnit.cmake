#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2003-07-31
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
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

find_path (CPPUNIT_INCLUDE_DIR
           NAMES cppunit/TestCase.h
           PATHS
               # standard unix
               /usr/local/include
               /usr/include
               # MacPorts
               /opt/local/include
               # Windows, many options
               C:/cppunit-1.12.1/include
               C:/cppunit-1.12.0/include
)

# With Win32, important to have both
if (WIN32)
  find_library (CPPUNIT_LIBRARY cppunit
                ${CPPUNIT_INCLUDE_DIR}/../lib
                /usr/local/lib
                /usr/lib)
  find_library (CPPUNIT_DEBUG_LIBRARY cppunitd
                ${CPPUNIT_INCLUDE_DIR}/../lib
                /usr/local/lib
                /usr/lib)
else (WIN32)
  # On unix system, debug and release have the same name
  find_library (CPPUNIT_LIBRARY cppunit
                ${CPPUNIT_INCLUDE_DIR}/../lib
                /usr/local/lib
                /usr/lib)
  find_library (CPPUNIT_DEBUG_LIBRARY cppunit
                ${CPPUNIT_INCLUDE_DIR}/../lib
                /usr/local/lib
                /usr/lib)
endif (WIN32)

set (CPPUNIT_FOUND OFF)
if (CPPUNIT_INCLUDE_DIR)
  if (CPPUNIT_LIBRARY)
    set (CPPUNIT_FOUND ON)
    set (CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
    set (CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
    mark_as_advanced (CPPUNIT_LIBRARIES CPPUNIT_DEBUG_LIBRARIES
                      CPPUNIT_LIBRARY   CPPUNIT_DEBUG_LIBRARY
                      CPPUNIT_FOUND CPPUNIT_INCLUDE_DIR)
  endif (CPPUNIT_LIBRARY)
endif (CPPUNIT_INCLUDE_DIR)

