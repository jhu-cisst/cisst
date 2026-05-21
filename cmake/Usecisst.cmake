#
# Author(s):  Anton Deguet
# Created on: 2010-08-11
#
# (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---
#

# DEPRECATION NOTICE:
# Usecisst.cmake is now deprecated.  Modern consumers should use the
# target-based interface instead:
#
#   find_package (cisst REQUIRED COMPONENTS cisstCommon cisstVector ...)
#   target_link_libraries (myTarget PRIVATE cisst::cisstVector)
#
# The include/link paths and transitive dependencies propagate automatically.
# Explicit calls to cisst_set_directories(), cisst_include_directories(),
# cisst_load_package_setting() and include(${CISST_USE_FILE}) are no longer
# needed.  This file is kept for backward compatibility only.
#
if (NOT _cisst_usecisst_deprecation_shown)
  message (DEPRECATION
    "include(\${CISST_USE_FILE}) / Usecisst.cmake is deprecated. "
    "Link directly against cisst:: INTERFACE targets instead: "
    "target_link_libraries(myTarget PRIVATE cisst::cisstVector ...)")
  set (_cisst_usecisst_deprecation_shown TRUE CACHE INTERNAL "")
endif ()

# Test if this file has been included within the cisst project itself
if (NOT CURRENT_PROJECT_IS_CISST)

  # Extend CMake Module Path to find cisst defined Macros
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CISST_CMAKE_DIRS})
  include (cisstMacros)

  # Add the include and lib paths for cisst
  include_directories (${CISST_INCLUDE_DIR})
  link_directories (${CISST_LIBRARY_DIR})

  # Load all settings for external dependencies
  cisst_load_package_setting (${CISST_LIBRARIES_REQUIRED_INTERNAL})

  # Include default cisst settings
  find_file (_cisstSettings_file
             cisstSettings.cmake
             PATHS ${CISST_CMAKE_DIRS})
  if (EXISTS ${_cisstSettings_file})
    include (${_cisstSettings_file})
  else ()
    message (FATAL_ERROR "Can't find cisstSettings.cmake in path: ${CISST_CMAKE_DIRS}")
  endif ()

endif (NOT CURRENT_PROJECT_IS_CISST)

# Add link and include directories based on required libraries.
# For most external deps this is handled transitively by linking to the
# modern cisst:: targets.  jsoncpp is special: its headers are included
# directly by cisst public headers (cmnGenericObject.h → json/json.h), so
# every consumer directory needs the include path even without an explicit
# target_link_libraries call.
if (CISST_HAS_JSON AND CISST_JSON_INCLUDE_DIR)
  include_directories (${CISST_JSON_INCLUDE_DIR})
endif ()
cisst_set_directories (${CISST_LIBRARIES_REQUIRED_INTERNAL})
