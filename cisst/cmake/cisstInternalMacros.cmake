#
# $Id$
#
# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
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
    set (_csps_FILENAME "${CISST_CMAKE_BINARY_DIR}/${whoRequires}${package}.cmake")
    if (NOT "${OLD_VALUE}" STREQUAL "${${FULL_VARIABLE_NAME}}")
      file (WRITE  ${_csps_FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
      file (APPEND ${_csps_FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
      # dump all in a config file, over and over
      foreach (keyword ${CISST_PACKAGE_KEYWORDS})
        set (FULL_VARIABLE_NAME CISST_${keyword}_FOR_${whoRequires}_USING_${package})
        if (${FULL_VARIABLE_NAME})
          file (APPEND ${_csps_FILENAME} "set (${FULL_VARIABLE_NAME} \"${${FULL_VARIABLE_NAME}}\")\n")
        endif (${FULL_VARIABLE_NAME})
      endforeach (keyword ${CISST_PACKAGE_KEYWORDS})
    endif (NOT "${OLD_VALUE}" STREQUAL "${${FULL_VARIABLE_NAME}}")
    install (FILES ${_csps_FILENAME}
             DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
             COMPONENT ${whoRequires})
    # Also update the list of external packages for the given library
    set (_csps_FILENAME_2 "${CISST_CMAKE_BINARY_DIR}/${whoRequires}External.cmake")
    file (WRITE  ${_csps_FILENAME_2} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
    file (APPEND ${_csps_FILENAME_2} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
    file (APPEND ${_csps_FILENAME_2} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
    file (APPEND ${_csps_FILENAME_2} "set (CISST_GENERAL_SETTINGS_FOR_${whoRequires} \"${CISST_GENERAL_SETTINGS_FOR_${whoRequires}}\")\n")
    install (FILES ${_csps_FILENAME_2}
             DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
             COMPONENT ${whoRequires})
  else (${value})
    message (SEND_ERROR "cisst_set_package_settings: value for variable ${variable} provided for package ${package} used by library ${whoRequires} is not properly defined")
  endif (${value})
endfunction (cisst_set_package_settings)


# Remove all settings for a given external package.
function (cisst_unset_all_package_settings whoRequires package)
  cisst_load_package_setting (${whoRequires})
  if (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
    set (PACKAGE_FILE "${CISST_CMAKE_BINARY_DIR}/${whoRequires}${package}.cmake")
    file (REMOVE ${PACKAGE_FILE})
    list (REMOVE_ITEM CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} ${package})
    set (_cuaps_FILENAME "${CISST_CMAKE_BINARY_DIR}/${whoRequires}External.cmake")
    file (REMOVE ${_cuaps_FILENAME})
    if (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires})
      file (WRITE  ${_cuaps_FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
      file (APPEND ${_cuaps_FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
      file (APPEND ${_cuaps_FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
      install (FILES ${_cuaps_FILENAME}
               DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
               COMPONENT ${whoRequires})
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
  set (_clus_FILENAME "${CISST_CMAKE_BINARY_DIR}/${whoRequires}External.cmake")
  file (WRITE  ${_clus_FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
  file (APPEND ${_clus_FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
  file (APPEND ${_clus_FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
  file (APPEND ${_clus_FILENAME} "set (CISST_GENERAL_SETTINGS_FOR_${whoRequires} \"${CISST_GENERAL_SETTINGS_FOR_${whoRequires}}\")\n")
  install (FILES ${_clus_FILENAME}
           DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
           COMPONENT ${whoRequires})
endfunction (cisst_library_use_settings)


function (cisst_library_remove_settings whoRequires value)
  # load existing settings
  cisst_load_package_setting (${whoRequires})
  # Add to list of all external dependencies
  list (REMOVE_ITEM CISST_GENERAL_SETTINGS_FOR_${whoRequires} ${value})
  # Also update the list of external packages for the given library
  set (_clrs_FILENAME "${CISST_CMAKE_BINARY_DIR}/${whoRequires}External.cmake")
  file (WRITE  ${_clrs_FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
  file (APPEND ${_clrs_FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
  file (APPEND ${_clrs_FILENAME} "set (CISST_EXTERNAL_PACKAGES_FOR_${whoRequires} \"${CISST_EXTERNAL_PACKAGES_FOR_${whoRequires}}\")\n")
  file (APPEND ${_clrs_FILENAME} "set (CISST_GENERAL_SETTINGS_FOR_${whoRequires} \"${CISST_GENERAL_SETTINGS_FOR_${whoRequires}}\")\n")
  install (FILES ${_clrs_FILENAME}
           DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
           COMPONENT ${whoRequires})
endfunction (cisst_library_remove_settings)


# Function to propagate library dependencies across libraries, e.g. cisstVector requires cisstCommon
function (cisst_library_use_libraries whoRequires value)
  # load existing libraries
  cisst_load_package_setting (${whoRequires})
  # Add to list of all external dependencies
  set (CISST_LIBRARIES_FOR_${whoRequires} ${CISST_LIBRARIES_FOR_${whoRequires}} ${value})
  list (REMOVE_DUPLICATES CISST_LIBRARIES_FOR_${whoRequires})
  # Also update the list of external packages for the given library
  set (_clul_FILENAME "${CISST_CMAKE_BINARY_DIR}/${whoRequires}Internal.cmake")
  file (WRITE  ${_clul_FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
  file (APPEND ${_clul_FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
  file (APPEND ${_clul_FILENAME} "set (CISST_LIBRARIES_FOR_${whoRequires} \"${CISST_LIBRARIES_FOR_${whoRequires}}\")\n")
  install (FILES ${_clul_FILENAME}
           DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
           COMPONENT ${whoRequires})
endfunction (cisst_library_use_libraries)


function (cisst_library_remove_libraries whoRequires value)
  # load existing libraries
  cisst_load_package_setting (${whoRequires})
  # Add to list of all external dependencies
  list (REMOVE_ITEM CISST_LIBRARIES_FOR_${whoRequires} ${value})
  # Also update the list of external packages for the given library
  set (_clrl_FILENAME "${CISST_CMAKE_BINARY_DIR}/${whoRequires}Internal.cmake")
  file (WRITE  ${_clrl_FILENAME} "# This file is generated automatically by CMake for cisst, DO NOT EDIT\n")
  file (APPEND ${_clrl_FILENAME} "# Source: ${CMAKE_SOURCE_DIR}\n\n")
  file (APPEND ${_clrl_FILENAME} "set (CISST_LIBRARIES_FOR_${whoRequires} \"${CISST_LIBRARIES_FOR_${whoRequires}}\")\n")
  install (FILES ${_clrl_FILENAME}
           DESTINATION ${CISST_CMAKE_INSTALL_SUFFIX}
           COMPONENT ${whoRequires})
endfunction (cisst_library_remove_libraries)


# Offer the option to compile a given application or remove the option
# based on CISST_BUILD_APPLICATIONS
# For example: cisst_offer_application (cisstMultiTask ComponentGenerator ON)
macro (cisst_offer_application library application default)
  set (coa_OPTION_NAME CISST_${library}_${application})
  if (CISST_BUILD_APPLICATIONS)
    option (${coa_OPTION_NAME} "Build cisst${application}" ${default})
    mark_as_advanced (${coa_OPTION_NAME})
    if (${coa_OPTION_NAME})
      add_subdirectory (cisst${application})
    endif (${coa_OPTION_NAME})
  else (CISST_BUILD_APPLICATIONS)
    unset (${coa_OPTION_NAME} CACHE)
  endif (CISST_BUILD_APPLICATIONS)
endmacro (cisst_offer_application)


macro (cisst_do_not_offer_application library application)
  set (coa_OPTION_NAME CISST_${library}_${application})
  unset (${coa_OPTION_NAME} CACHE)
endmacro (cisst_do_not_offer_application)


# Offer the option to compile all examples in relative directory "examples"
# Default values are hard coded intentionally
macro (cisst_offer_examples library)
  set (coe_OPTION_NAME CISST_${library}_EXAMPLES)
  if (CISST_BUILD_EXAMPLES)
    option (${coe_OPTION_NAME} "Build ${library} examples" OFF)
    if (${coe_OPTION_NAME})
      add_subdirectory (examples)
    endif (${coe_OPTION_NAME})
  else (CISST_BUILD_EXAMPLES)
    unset (${coe_OPTION_NAME} CACHE)
  endif (CISST_BUILD_EXAMPLES)
endmacro (cisst_offer_examples)

# Offer the option to compile all tests in relative directory "tests"
# Default values are hard coded intentionally
macro (cisst_offer_tests library)
  set (cot_OPTION_NAME CISST_${library}_TESTS)
  if (CISST_BUILD_TESTS)
    option (${cot_OPTION_NAME} "Build ${library} tests" ON)
    mark_as_advanced (${cot_OPTION_NAME})
    if (${cot_OPTION_NAME})
      add_subdirectory (tests)
    endif (${cot_OPTION_NAME})
  else (CISST_BUILD_TESTS)
    unset (${cot_OPTION_NAME} CACHE)
  endif (CISST_BUILD_TESTS)
endmacro (cisst_offer_tests)

# Offer the option to compile all tests in relative directory "codePython"
# Default values are hard coded intentionally
macro (cisst_offer_python library)
  set (cop_OPTION_NAME CISST_${library}Python)
  if (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)
    option (${cop_OPTION_NAME} "Build ${library} Python" ON)
    mark_as_advanced (${cop_OPTION_NAME})
    if (${cop_OPTION_NAME})
      add_subdirectory (codePython)
    endif (${cop_OPTION_NAME})
  else (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)
    unset (${cop_OPTION_NAME} CACHE)
  endif (CISST_HAS_SWIG_PYTHON AND CISST_SWIG_FOUND)
endmacro (cisst_offer_python)

# Offer the option to compile all tests in relative directory "testsPython"
# Default values are hard coded intentionally
macro (cisst_offer_tests_python library)
  set (cotp_OPTION_NAME CISST_${library}Python_TESTS)
  if (CISST_${library}Python AND CISST_BUILD_TESTS)
    option (${cotp_OPTION_NAME} "Build ${library} Python tests" ON)
    mark_as_advanced (${cotp_OPTION_NAME})
    if (${cotp_OPTION_NAME})
      add_subdirectory (testsPython)
    endif (${cotp_OPTION_NAME})
  else (CISST_${library}Python AND CISST_BUILD_TESTS)
    unset (${cotp_OPTION_NAME} CACHE)
  endif (CISST_${library}Python AND CISST_BUILD_TESTS)
endmacro (cisst_offer_tests_python)