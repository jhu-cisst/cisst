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


# Backward compatibility
cmake_minimum_required (VERSION 2.6.2)

message ("todo: need to save SWIG_EXECUTABLE for external projects")
message ("todo: provide useful message if using deprecated cisst macros")
message ("todo: keep track of components required in find_package and provide warning message when using cisst_target_link_library")
message ("todo: check generated cisst-config.cmake")
message ("todo: should RTAI and QNX flags be moved to cisstOSAbstraction")
message ("todo: consolidate numericalTutorial and registrationTest in examples/cisstNumerical")
message ("todo: rename example osAbstractionTutorial")
message ("todo: remove dependency on cisstNumerical/cisstNetlib from vector tutorial")

# cisst (Computer Integrated Surgical Systems and Technology): www.cisst.org/cisst
project (cisst)

# Create some variables so that find_package (cisst) can be used in subdirectories (examples)
set (CURRENT_PROJECT_IS_CISST ON)
set (cisst_DIR "${cisst_BINARY_DIR}/libs")

# Option to make cisst CMake functions more verbose
option (CISST_HAS_CMAKE_DEBUG "Print extra messages when configuring with CMake" OFF)
mark_as_advanced (CISST_HAS_CMAKE_DEBUG)

# Options
option (CISST_BUILD_UTILITIES "Build utility programs built using cisst." ON)
option (CISST_BUILD_TESTS "Build tests." OFF)
option (CISST_BUILD_EXAMPLES "Build examples." OFF)
option (CISST_BUILD_COMPONENTS "Build components." OFF)

if (UNIX)
  option (CISST_BUILD_DOC "Build the documentation." OFF)
endif (UNIX)

# Add include directories
set (CISST_INCLUDE_DIR
     ${cisst_BINARY_DIR}/libs/include
     ${cisst_SOURCE_DIR}/libs)
if (CISST_BUILD_COMPONENTS)
  set (CISST_INCLUDE_DIR ${CISST_INCLUDE_DIR}
                         ${cisst_BINARY_DIR}/components
                         ${cisst_SOURCE_DIR}/components)
endif (CISST_BUILD_COMPONENTS)

include_directories (${CISST_INCLUDE_DIR})

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
add_subdirectory (libs)
cisst_cmake_debug ("Libraries to be built: ${CISST_LIBRARIES}")

# Conditional builds
if (CISST_BUILD_UTILITIES)
  add_subdirectory (utilities)
endif (CISST_BUILD_UTILITIES)

if (CISST_BUILD_TESTS)
  # Settings for CTest
  enable_testing ()
  mark_as_advanced (BUILD_TESTING) # CMake variable for CTest
  include (Dart)
  mark_as_advanced (DART_ROOT)
  # cisst Tests
  add_subdirectory (tests)
endif (CISST_BUILD_TESTS)

if (CISST_BUILD_EXAMPLES)
  add_subdirectory (examples)
endif (CISST_BUILD_EXAMPLES)

if (CISST_BUILD_COMPONENTS)
  add_subdirectory (components)
endif (CISST_BUILD_COMPONENTS)

if (CISST_BUILD_DOC)
  add_subdirectory (doc)
endif (CISST_BUILD_DOC)


# Environment variables for Windows
IF(WIN32)
  # Script to set cisst environment variables
  FILE(TO_NATIVE_PATH ${cisst_BINARY_DIR} cisst_BINARY_DIR_NATIVE)
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.bat.in
                 ${cisst_BINARY_DIR}/cisstvars.bat)
  # Script to set Microsoft compiler environment variables
  GET_FILENAME_COMPONENT(BUILD_TOOL_DIRECTORY ${CMAKE_MAKE_PROGRAM} PATH)
  FIND_PROGRAM(VSVARS32_BAT
               NAMES vsvars32.bat
               PATHS ${BUILD_TOOL_DIRECTORY}/../../Common7/Tools)
  MARK_AS_ADVANCED(VSVARS32_BAT)
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvsvars.bat.in
                 ${cisst_BINARY_DIR}/cisstvsvars.bat)

  # If Visual Studio
  IF(CMAKE_GENERATOR MATCHES "Visual Studio*" )
    FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
      CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/visual-studio.bat.in
                     ${cisst_BINARY_DIR}/visual-studio-${config}.bat)
    ENDFOREACH(config)
  ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio*" )

  # Offer to register the cisst paths
  OPTION(CISST_PATH_REGISTER "Register the paths to cisst libraries" OFF)
  IF(CISST_PATH_REGISTER)

    # Set and test configuration (i.e. Debug, Release, ...)
    SET(CISST_PATH_CONFIGURATION CACHE STRING "Configuration used for the paths")
    SET(CISST_PATH_CONFIGURATION_VALID FALSE)

    FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
      IF("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
        SET(CISST_PATH_CONFIGURATION_VALID TRUE)
      ENDIF("${config}" STREQUAL "CISST_PATH_CONFIGURATION")
    ENDFOREACH(config)

    # Configuration is valid, build the path and compare it to the current one
    IF(CISST_PATH_CONFIGURATION_VALID)

      # Initial settings for paths
      SET(CISST_PATH_CMAKE_FORMAT
          "${EXECUTABLE_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}"
          "${LIBRARY_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}")
      SET(CISST_PYTHONPATH_CMAKE_FORMAT
          "${EXECUTABLE_OUTPUT_PATH}" "${EXECUTABLE_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}"
          "${LIBRARY_OUTPUT_PATH}" "${LIBRARY_OUTPUT_PATH}/${CISST_PATH_CONFIGURATION}")

      # To find tests programs and libraries
      IF(CISST_BUILD_TESTS)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/tests/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/tests/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/tests/lib" "${cisst_BINARY_DIR}/tests/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/tests/bin" "${cisst_BINARY_DIR}/tests/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(CISST_BUILD_TESTS)

      # To find examples programs and libraries
      IF(CISST_BUILD_EXAMPLES)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/examples/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/examples/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/examples/lib" "${cisst_BINARY_DIR}/examples/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/examples/bin" "${cisst_BINARY_DIR}/examples/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(CISST_BUILD_EXAMPLES)

      # To find utilities programs and libraries
      IF(CISST_BUILD_UTILITIES)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/utilities/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/utilities/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/utilities/lib" "${cisst_BINARY_DIR}/utilities/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/utilities/bin" "${cisst_BINARY_DIR}/utilities/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(CISST_BUILD_UTILITIES)

      # To find components (libraries) and example programs
      IF(BUILD_COMPONENTS)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/components/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/components/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/components/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/components/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(BUILD_COMPONENTS)

      # Convert to native format to be able to compare
      FILE(TO_NATIVE_PATH "${CISST_PATH_CMAKE_FORMAT}" CISST_PATH)
      FILE(TO_NATIVE_PATH "${CISST_PYTHONPATH_CMAKE_FORMAT}" CISST_PYTHONPATH)

      # Retrieve current path, little trick to read register key/value
      SET(CISST_PATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PATH")
      GET_FILENAME_COMPONENT(CURRENT_CISST_PATH_CMAKE_FORMAT
                             "[HKEY_CURRENT_USER\\Environment;CISST_PATH]\\dummy.dum"
                             PATH)
      FILE(TO_NATIVE_PATH "${CURRENT_CISST_PATH_CMAKE_FORMAT}" CURRENT_CISST_PATH)

      SET(CISST_PYTHONPATH_REG_KEY "HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH")
      GET_FILENAME_COMPONENT(CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT
                             "[HKEY_CURRENT_USER\\Environment;CISST_PYTHONPATH]\\dummy.dum"
                             PATH)
      FILE(TO_NATIVE_PATH "${CURRENT_CISST_PYTHONPATH_CMAKE_FORMAT}" CURRENT_CISST_PYTHONPATH)

      # If there is no existing path, add it
      IF("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        # not defined, add it to register using cmake -E
        EXEC_PROGRAM(${CMAKE_COMMAND}
                     ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
      ELSE("${CURRENT_CISST_PATH}" STREQUAL "\\registry")
        IF("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # MESSAGE("CISST_PATH is the same")
        ELSE("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")
          # The variable already exists and is different
          OPTION(CISST_PATH_OVERWRITE "Overwrite existing CISST_PATH" OFF)
          IF(CISST_PATH_OVERWRITE)
            # add it to register using cmake -E
            EXEC_PROGRAM(${CMAKE_COMMAND}
                         ARGS "-E write_regv \"${CISST_PATH_REG_KEY}\" \"${CISST_PATH}\"")
            # Set overwrite back to OFF by default.
            SET(CISST_PATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PATH" FORCE)
          ELSE(CISST_PATH_OVERWRITE)
            # Display a message of information
            MESSAGE("A different CISST_PATH exists:\n  ${CURRENT_CISST_PATH}\nTurn CISST_PATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
          ENDIF(CISST_PATH_OVERWRITE)
        ENDIF("${CISST_PATH}" STREQUAL "${CURRENT_CISST_PATH}")

      ENDIF("${CURRENT_CISST_PATH}" STREQUAL "\\registry")

      # Do the same for PYTHONPATH if wrappers are compiled
      IF(CISST_HAS_SWIG_PYTHON)
        # If there is no existing path, add it
        IF("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          # not defined, add it to register using cmake -E
          EXEC_PROGRAM(${CMAKE_COMMAND}
                       ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
        ELSE("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
          IF("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # MESSAGE("CISST_PATH is the same")
          ELSE("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")
            # The variable already exists and is different
            OPTION(CISST_PYTHONPATH_OVERWRITE "Overwrite existing CISST_PYTHONPATH" OFF)
            IF(CISST_PYTHONPATH_OVERWRITE)
              # Add it to register using cmake -E
              EXEC_PROGRAM(${CMAKE_COMMAND}
                           ARGS "-E write_regv \"${CISST_PYTHONPATH_REG_KEY}\" \"${CISST_PYTHONPATH}\"")
              # Set overwrite back to OFF by default.
              SET(CISST_PYTHONPATH_OVERWRITE OFF CACHE BOOL "Overwrite existing CISST_PYTHONPATH" FORCE)
            ELSE(CISST_PYTHONPATH_OVERWRITE)
              # Display a message of information
              MESSAGE("A different CISST_PYTHONPATH exists:\n  ${CURRENT_CISST_PYTHONPATH}\nTurn CISST_PYTHONPATH_OVERWRITE \"ON\" or CISST_PATH_REGISTER \"OFF\"")
            ENDIF(CISST_PYTHONPATH_OVERWRITE)
          ENDIF("${CISST_PYTHONPATH}" STREQUAL "${CURRENT_CISST_PYTHONPATH}")

        ENDIF("${CURRENT_CISST_PYTHONPATH}" STREQUAL "\\registry")
      ENDIF(CISST_HAS_SWIG_PYTHON)

    ELSE(CISST_PATH_CONFIGURATION_VALID)
      MESSAGE("CISST_PATH_CONFIGURATION must be one of ${CMAKE_CONFIGURATION_TYPES}")
    ENDIF(CISST_PATH_CONFIGURATION_VALID)

  ENDIF(CISST_PATH_REGISTER)

ENDIF(WIN32)

# Environment variables for Unix
if (UNIX)
  # Script to set cisst environment variables
  configure_file (${cisst_SOURCE_DIR}/libs/utils/cisstvars.csh.in
                  ${cisst_BINARY_DIR}/cisstvars.csh
                  @ONLY)
  # Script to set cisst environment variables
  configure_file (${cisst_SOURCE_DIR}/libs/utils/cisstvars.sh.in
                  ${cisst_BINARY_DIR}/cisstvars.sh
                  @ONLY)
endif (UNIX)
