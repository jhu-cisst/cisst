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


# Backward compatibility
cmake_minimum_required (VERSION 2.6.2)

# cisst (Computer Integrated Surgical Systems and Technology): www.cisst.org/cisst
# saw (Surgical Assistant Workstation): www.cisst.org/saw
project (cisst_saw)

# To organize targets in folders
option (CISST_USE_FOLDERS "Use folder to organize cisst and saw in IDE (Visual Studio, ...)" ON)
mark_as_advanced (CISST_USE_FOLDERS)
if (CISST_USE_FOLDERS)
  set_property (GLOBAL PROPERTY USE_FOLDERS ON)
else (CISST_USE_FOLDERS)
  set_property (GLOBAL PROPERTY USE_FOLDERS OFF)
endif (CISST_USE_FOLDERS)

# Create some variables so that find_package (cisst) can be used in subdirectories (examples)
set (cisst_DIR "${cisst_saw_BINARY_DIR}/cisst")

# Option to make cisst CMake functions more verbose
option (CISST_HAS_CMAKE_DEBUG "Print extra messages when configuring with CMake" OFF)
mark_as_advanced (CISST_HAS_CMAKE_DEBUG)

# Options
option (CISST_BUILD_TESTS "Build cisst tests (when turned ON, all tests are to be compiled by default)." OFF)
option (CISST_BUILD_EXAMPLES "Build cisst examples (when turned ON, user will be asked to select which examples to build)." OFF)

# Applications should be compiled by default
option (CISST_BUILD_APPLICATIONS "Build cisst applications (when turned ON, all applications are to be compile by default)." ON)
mark_as_advanced (CISST_BUILD_APPLICATIONS)

option (CISST_BUILD_SAW "Build SAW components and applications." OFF)

option (CISST_BUILD_SHARE "Build share (i.e. copy data from /share directory to build tree).  This should be left ON for most users." ON)
mark_as_advanced (CISST_BUILD_SHARE)

if (UNIX)
  option (CISST_BUILD_DOC "Build the documentation (Doxygen reference manual)." OFF)
endif (UNIX)

# Activate tests before any directory is added
if (CISST_BUILD_TESTS)
  # Settings for CTest
  enable_testing ()
  mark_as_advanced (BUILD_TESTING) # CMake variable for CTest
  include (Dart)
  mark_as_advanced (DART_ROOT)
endif (CISST_BUILD_TESTS)

# Build static or dynamic libraries
option (CISST_BUILD_SHARED_LIBS "Build cisst with shared libraries." OFF)
if (CISST_BUILD_SHARED_LIBS)
  set (IS_SHARED SHARED)
  set (BUILD_SHARED_LIBS SHARED)
  if (WIN32)
    remove_definitions (-DCISST_DLL)
    add_definitions (-DCISST_DLL)
  endif (WIN32)
else (CISST_BUILD_SHARED_LIBS)
  set (IS_SHARED STATIC)
endif (CISST_BUILD_SHARED_LIBS)

# To allow mixing static and dynamic libraries
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (CMAKE_COMPILER_IS_GNUCXX)

# To have more warnings with gcc
if (CMAKE_COMPILER_IS_GNUCXX)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag ("-Wextra" CXX_SUPPORTS_WEXTRA)
  if (CXX_SUPPORTS_WEXTRA)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
  endif (CXX_SUPPORTS_WEXTRA)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCXX)

# Finally, create lib targets
add_subdirectory (cisst)

# Conditional builds
if (CISST_BUILD_SAW)
  add_subdirectory (saw)
endif (CISST_BUILD_SAW)

if (CISST_BUILD_SHARE)
  add_subdirectory (share)
endif (CISST_BUILD_SHARE)

if (CISST_BUILD_DOC)
  add_subdirectory (doc)
endif (CISST_BUILD_DOC)


# Environment variables for Windows
if (WIN32)
  # Script to set cisst environment variables
  file (TO_NATIVE_PATH ${cisst_saw_BINARY_DIR} cisst_saw_BINARY_DIR_NATIVE)
  configure_file (${cisst_saw_SOURCE_DIR}/cisst/utils/cisstvars.bat.in
                  ${cisst_saw_BINARY_DIR}/cisstvars.bat)
  # Script to set Microsoft compiler environment variables
  get_filename_component (BUILD_TOOL_DIRECTORY ${CMAKE_MAKE_PROGRAM} PATH)
  find_program (VSVARS32_BAT
                NAMES vsvars32.bat
                PATHS ${BUILD_TOOL_DIRECTORY}/../../Common7/Tools)
  mark_as_advanced (VSVARS32_BAT)
  configure_file (${cisst_saw_SOURCE_DIR}/cisst/utils/cisstvsvars.bat.in
                  ${cisst_saw_BINARY_DIR}/cisstvsvars.bat)

  # If Visual Studio
  if (CMAKE_GENERATOR MATCHES "Visual Studio*" )
    foreach (config ${CMAKE_CONFIGURATION_TYPES})
      configure_file (${cisst_saw_SOURCE_DIR}/cisst/utils/visual-studio.bat.in
                      ${cisst_saw_BINARY_DIR}/visual-studio-${config}.bat)
    endforeach (config)
  endif (CMAKE_GENERATOR MATCHES "Visual Studio*" )

  # Offer to register the cisst paths
  option (CISST_PATH_REGISTER "Register the paths to cisst libraries" OFF)
  if (CISST_PATH_REGISTER)

    # Set and test configuration (i.e. Debug, Release, ...)
    set (CISST_PATH_CONFIGURATION CACHE STRING "Configuration used for the paths")
    set (CISST_PATH_CONFIGURATION_VALID FALSE)

    foreach (config ${CMAKE_CONFIGURATION_TYPES})
      if ("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
        set (CISST_PATH_CONFIGURATION_VALID TRUE)
      endif ("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
    endforeach (config)

    # Configuration is valid, build the path and compare it to the current one
    if (CISST_PATH_CONFIGURATION_VALID)

      # Initial settings for paths
      set (CISST_PATH_CMAKE_FORMAT
           "${cisst_saw_BINARY_DIR}/cisst/lib/${CISST_PATH_CONFIGURATION}"
           "${cisst_saw_BINARY_DIR}/cisst/bin/${CISST_PATH_CONFIGURATION}")
      set (CISST_PYTHONPATH_CMAKE_FORMAT
           "${cisst_saw_BINARY_DIR}/cisst/lib" "${cisst_saw_BINARY_DIR}/cisst/lib/${CISST_PATH_CONFIGURATION}"
           "${cisst_saw_BINARY_DIR}/cisst/bin" "${cisst_saw_BINARY_DIR}/cisst/bin/${CISST_PATH_CONFIGURATION}")
      set (CISST_ROOT_CMAKE_FORMAT
           "${cisst_saw_BINARY_DIR}/cisst")

      # Convert to native format to be able to compare
      file (TO_NATIVE_PATH "${CISST_PATH_CMAKE_FORMAT}" CISST_PATH)
      file (TO_NATIVE_PATH "${CISST_PYTHONPATH_CMAKE_FORMAT}" CISST_PYTHONPATH)
      file (TO_NATIVE_PATH "${CISST_ROOT_CMAKE_FORMAT}" CISST_ROOT)

      # Retrieve current path, little trick to read register key/value
      set (CISST_PATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PATH")
      get_filename_component (CURRENT_CISST_PATH_CMAKE_FORMAT
                              "[HKEY_CURRENT_USER\\Environment;CISST_PATH]\\dummy.dum"
                              PATH)
      file (TO_NATIVE_PATH "${CURRENT_CISST_PATH_CMAKE_FORMAT}" CURRENT_CISST_PATH)

      set (CISST_PYTHONPATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH")
      get_filename_component (CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT
                              "[HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH]\\dummy.dum"
                              PATH)
      file (TO_NATIVE_PATH "${CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT}" CURRENT_CISST_PYTHONPATH)

      set (CISST_ROOT_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_ROOT")
      get_filename_component (CURRENT_CISST_ROOT_CMAKE_FORMAT
                              "[HKEY_CURRENT_USER\\Environment;CISST_ROOT]\\dummy.dum"
                              PATH)
      file (TO_NATIVE_PATH "${CURRENT_CISST_ROOT_CMAKE_FORMAT}" CURRENT_CISST_ROOT)

      # If there is no existing path, add it
      if ("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        # not defined, add it to register using cmake -E
        exec_program (${CMAKE_COMMAND}
                      ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
      else ("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        if ("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # message ("CISST_PATH is the same")
        else ("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # The variable already exists and is different
          option (CISST_PATH_OVERWRITE "Overwrite existing CISST_PATH" OFF)
          if (CISST_PATH_OVERWRITE)
            # add it to register using cmake -E
            exec_program (${CMAKE_COMMAND}
                          ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
            # Set overwrite back to OFF by default.
            set (CISST_PATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PATH" FORCE)
          else (CISST_PATH_OVERWRITE)
            # Display a message of information
            message ("A different CISST_PATH exists:\n  ${CURRENT_CISST_PATH}\nTurn CISST_PATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
          endif (CISST_PATH_OVERWRITE)
        endif ("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")

      endif ("${CURRENT_CISST_PATH}" STREQUAL "\\registry")

      # Do the same for PYTHONPATH if wrappers are compiled
      if (CISST_HAS_SWIG_PYTHON)
        # If there is no existing path, add it
        if ("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          # not defined, add it to register using cmake -E
          exec_program (${CMAKE_COMMAND}
                        ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
        else ("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          if ("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # message ("CISST_PATH is the same")
          else ("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # The variable already exists and is different
            option (CISST_PYTHONPATH_OVERWRITE "Overwrite existing CISST_PYTHONPATH" OFF)
            if (CISST_PYTHONPATH_OVERWRITE)
              # Add it to register using cmake -E
              exec_program (${CMAKE_COMMAND}
                            ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
              # Set overwrite back to OFF by default.
              set (CISST_PYTHONPATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PYTHONPATH" FORCE)
            else (CISST_PYTHONPATH_OVERWRITE)
              # Display a message of information
              message ("A different CISST_PYTHONPATH exists:\n  ${CURRENT_CISST_PYTHONPATH}\nTurn CISST_PYTHONPATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
            endif (CISST_PYTHONPATH_OVERWRITE)
          endif ("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")

        endif ("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
      endif (CISST_HAS_SWIG_PYTHON)


      # If there is no CISST_ROOT
      if ("${CURRENT_CISST_ROOT}" STREQUAL "\\registry")
        # not defined, add it to register using cmake -E
        exec_program (${CMAKE_COMMAND}
                      ARGS "-E write_regv \"${CISST_ROOT_REG_KEY}\" \"${CISST_ROOT}\"")
      else ("${CURRENT_CISST_ROOT}" STREQUAL "\\registry")
        if ("${CISST_ROOT}" STREQUAL "${CURRENT_CISST_ROOT}")
          # message ("CISST_ROOT is the same")
        else ("${CISST_ROOT}" STREQUAL "${CURRENT_CISST_ROOT}")
          # The variable already exists and is different
          option (CISST_ROOT_OVERWRITE "Overwrite existing CISST_ROOT" OFF)
          if (CISST_ROOT_OVERWRITE)
            # add it to register using cmake -E
            exec_program (${CMAKE_COMMAND}
                          ARGS "-E write_regv \"${CISST_ROOT_REG_KEY}\" \"${CISST_ROOT}\"")
            # Set overwrite back to OFF by default.
            set (CISST_ROOT_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_ROOT" FORCE)
          else (CISST_ROOT_OVERWRITE)
            # Display a message of information
            message ("A different CISST_ROOT exists:\n  ${CURRENT_CISST_ROOT}\nTurn CISST_ROOT_OVERWRITE \"ON\" or CISST_ROOT_REGISTER \"OFF\"")
          endif (CISST_ROOT_OVERWRITE)
        endif ("${CISST_ROOT}" STREQUAL "${CURRENT_CISST_ROOT}")

      endif ("${CURRENT_CISST_ROOT}" STREQUAL "\\registry")

    else (CISST_PATH_CONFIGURATION_VALID)
      message ("CISST_PATH_CONFIGURATION must be one of ${CMAKE_CONFIGURATION_TYPES}")
    endif (CISST_PATH_CONFIGURATION_VALID)

  endif (CISST_PATH_REGISTER)

endif (WIN32)

# Environment variables for Unix
if (UNIX)
  # Script to set cisst environment variables
  configure_file (${cisst_saw_SOURCE_DIR}/cisst/utils/cisstvars.csh.in
                  ${cisst_saw_BINARY_DIR}/cisstvars.csh
                  @ONLY)
  # Script to set cisst environment variables
  configure_file (${cisst_saw_SOURCE_DIR}/cisst/utils/cisstvars.sh.in
                  ${cisst_saw_BINARY_DIR}/cisstvars.sh
                  @ONLY)
endif (UNIX)

# License file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/license.txt
                ${CMAKE_CURRENT_BINARY_DIR}/license.txt COPYONLY)
