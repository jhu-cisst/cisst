#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---


# List all keywords allowed for cisst additional settings
set (CISST_PACKAGE_KEYWORDS INCLUDE_DIRECTORIES LIBRARIES LINK_DIRECTORIES PACKAGES PACKAGE_COMPONENTS CMAKE_FILES)


# Add dependency information for a given package used by cisst
# Usage example: cisst_set_package_settings (cisstCommon FLTK INCLUDE_DIRECTORIES FLTK_INCLUDE_DIR)
function (cisst_set_package_settings whoRequires package variable value)
  # todo, do not replicate lib debug/optimized (used for path?)
  # todo, can we check whoRequires to be a valid library?

  # make sure the variable is allowed
  list (FIND CISST_PACKAGE_KEYWORDS ${variable} VARIABLE_IS_A_KEYWORD)
  if (NOT ${VARIABLE_IS_A_KEYWORD} GREATER -1)
    message (SEND_ERROR "cisst_set_package_settings: third argument (${variable}) should be one of: ${CISST_PACKAGE_KEYWORDS}")
  endif (NOT ${VARIABLE_IS_A_KEYWORD} GREATER -1)
  # make sure a value has been provided
  if (${value})
    # Load existing settings
    cisst_load_package_setting (${whoRequires})
    # Add to list of all external dependencies
    set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} ${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}} ${package})
    list (REMOVE_DUPLICATES CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
    # Full variable name
    set (FULL_VARIABLE_NAME CISST_${variable}_FOR_${whoRequires}_USING_${package})
    # Keep previous value to compare if there is anything new
    set (OLD_VALUE ${${FULL_VARIABLE_NAME}})
    set (${FULL_VARIABLE_NAME} "${${value}}")
    # Save to file if needed
    if (NOT "${OLD_VALUE}" STREQUAL "${${FULL_VARIABLE_NAME}}")
      set (FILENAME "${cisst_BINARY_DIR}/cisst-dependencies/${whoRequires}${package}.cmake")
      file (WRITE  ${FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
      file (APPEND ${FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
      # dump all in a config file, over and over
      foreach (keyword ${CISST_PACKAGE_KEYWORDS})
        set (FULL_VARIABLE_NAME CISST_${keyword}_FOR_${whoRequires}_USING_${package})
        if (${FULL_VARIABLE_NAME})
          file (APPEND ${FILENAME} "set (${FULL_VARIABLE_NAME} \"${${FULL_VARIABLE_NAME}}\")\n")
        endif (${FULL_VARIABLE_NAME})
      endforeach (keyword ${CISST_PACKAGE_KEYWORDS})
    endif (NOT "${OLD_VALUE}" STREQUAL "${${FULL_VARIABLE_NAME}}")
    # Also update the list of external packages for the given library
    set (FILENAME "${cisst_BINARY_DIR}/cisst-dependencies/${whoRequires}External.cmake")
    file (WRITE  ${FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
    file (APPEND ${FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
    file (APPEND ${FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
    file (APPEND ${FILENAME} "set (CISST_GENERAL_SETTINGS_FOR_${whoRequires} \"${CISST_GENERAL_SETTINGS_FOR_${whoRequires}}\")\n")
  else (${value})
    message (SEND_ERROR "cisst_set_package_settings: value for variable ${variable} provided for package ${package} used by library ${whoRequires} is not properly defined")
  endif (${value})
endfunction (cisst_set_package_settings)


# Remove all settings for a given external package.
function (cisst_unset_all_package_settings whoRequires package)
  cisst_load_package_setting (${whoRequires})
  if (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
    set (PACKAGE_FILE "${CISST_BINARY_DIR}/cisst-dependencies/${whoRequires}${package}.cmake")
    file (REMOVE ${PACKAGE_FILE})
    list (REMOVE_ITEM CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} ${package})
    set (FILENAME "${cisst_BINARY_DIR}/cisst-dependencies/${whoRequires}External.cmake")
    file (REMOVE ${FILENAME})
    if (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
      file (WRITE  ${FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
      file (APPEND ${FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
      file (APPEND ${FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
    endif (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
  endif (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
endfunction (cisst_unset_all_package_settings)


# Function to propagate settings across libraries, e.g. Qt for all Qt based libraries
function (cisst_library_use_settings whoRequires value)
  # load existing settings
  cisst_load_package_setting (${whoRequires})
  # Add to list of all external dependencies
  set (CISST_GENERAL_SETTINGS_FOR_${whoRequires} ${CISST_GENERAL_SETTINGS_FOR_${whoRequires}} ${value})
  list (REMOVE_DUPLICATES CISST_GENERAL_SETTINGS_FOR_${whoRequires})
  # Also update the list of external packages for the given library
  set (FILENAME "${cisst_BINARY_DIR}/cisst-dependencies/${whoRequires}External.cmake")
  file (WRITE  ${FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
  file (APPEND ${FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
  file (APPEND ${FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
  file (APPEND ${FILENAME} "set (CISST_GENERAL_SETTINGS_FOR_${whoRequires} \"${CISST_GENERAL_SETTINGS_FOR_${whoRequires}}\")\n")
endfunction (cisst_library_use_settings)
