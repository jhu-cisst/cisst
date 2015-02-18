#
# Author(s):  Anton Deguet
# Created on: 2010-08-11
#
# (C) Copyright 2010-2015 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---
#

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

# Add link and include directories based on required libraries
cisst_set_directories (${CISST_LIBRARIES_REQUIRED_INTERNAL})
