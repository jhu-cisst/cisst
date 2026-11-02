#
# Author(s): Anton Deguet
# Created on: 2003-07-31
#
# (C) Copyright 2003-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---
#
# Prefer a package config, then fall back to finding headers and libraries.
# Provides CppUnit::CppUnit, CppUnit_FOUND and the legacy CPPUNIT_FOUND,
# CPPUNIT_INCLUDE_DIR, CPPUNIT_LIBRARIES and CPPUNIT_DEBUG_LIBRARIES variables.
# CPPUNIT_LIBRARY and CPPUNIT_DEBUG_LIBRARY can override the fallback search.

include (FindPackageHandleStandardArgs)

# Do not reload configs that unconditionally create their imported target.
if (NOT TARGET CppUnit::CppUnit AND NOT TARGET CppUnit)
  find_package (CppUnit QUIET CONFIG)
endif ()
if (TARGET CppUnit AND NOT TARGET CppUnit::CppUnit)
  # An interface wrapper also supports non-global imports with CMake 3.16.
  add_library (CppUnit::CppUnit INTERFACE IMPORTED)
  set_target_properties (CppUnit::CppUnit PROPERTIES INTERFACE_LINK_LIBRARIES CppUnit)
endif ()
if (TARGET CppUnit::CppUnit)
  set (CPPUNIT_LIBRARIES CppUnit::CppUnit)
  set (CPPUNIT_DEBUG_LIBRARIES CppUnit::CppUnit)
  get_target_property (CPPUNIT_INCLUDE_DIR CppUnit::CppUnit INTERFACE_INCLUDE_DIRECTORIES)
  if (TARGET CppUnit AND NOT CPPUNIT_INCLUDE_DIR)
    get_target_property (CPPUNIT_INCLUDE_DIR CppUnit INTERFACE_INCLUDE_DIRECTORIES)
  endif ()
  find_package_handle_standard_args (CppUnit REQUIRED_VARS CPPUNIT_LIBRARIES)
  return ()
endif ()

find_path (CPPUNIT_INCLUDE_DIR NAMES cppunit/TestCase.h
           PATHS /opt/local/include /opt/homebrew/include)
find_library (CPPUNIT_LIBRARY NAMES cppunit
              HINTS "${CPPUNIT_INCLUDE_DIR}/../lib"
              PATHS /opt/local/lib /opt/homebrew/lib)
if (WIN32)
  find_library (CPPUNIT_DEBUG_LIBRARY NAMES cppunitd
                HINTS "${CPPUNIT_INCLUDE_DIR}/../debug/lib" "${CPPUNIT_INCLUDE_DIR}/../lib")
endif ()
if (NOT CPPUNIT_DEBUG_LIBRARY)
  set (CPPUNIT_DEBUG_LIBRARY "${CPPUNIT_LIBRARY}")
endif ()

# Sets both result spellings and honors REQUIRED and QUIET.
find_package_handle_standard_args (CppUnit REQUIRED_VARS CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARY)
if (CppUnit_FOUND)
  set (CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
  set (CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
  add_library (CppUnit::CppUnit UNKNOWN IMPORTED)
  set_target_properties (CppUnit::CppUnit PROPERTIES
                         INTERFACE_INCLUDE_DIRECTORIES "${CPPUNIT_INCLUDE_DIR}"
                         IMPORTED_LOCATION "${CPPUNIT_LIBRARY}"
                         IMPORTED_LOCATION_RELEASE "${CPPUNIT_LIBRARY}"
                         IMPORTED_LOCATION_DEBUG "${CPPUNIT_DEBUG_LIBRARY}"
                         INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}")
endif ()
mark_as_advanced (CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARY CPPUNIT_DEBUG_LIBRARY)
