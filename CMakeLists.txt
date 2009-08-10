#
# $Id$
#
# (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
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
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(cisst)



# Function to expend a list without repetitions, forces the list to be internal
FUNCTION(CISST_ADD_TO_INTERNAL list newElements)
  SET(TEMP_${list} ${${list}} ${${newElements}})
  IF(TEMP_${list})
    LIST(REMOVE_DUPLICATES TEMP_${list})
  ENDIF(TEMP_${list})
  SET(${list} ${TEMP_${list}} CACHE INTERNAL "CISST Internal variable")
ENDFUNCTION(CISST_ADD_TO_INTERNAL)



# Output directories.
SET(LIBRARY_OUTPUT_PATH ${cisst_BINARY_DIR}/libs/lib)
SET(EXECUTABLE_OUTPUT_PATH ${cisst_BINARY_DIR}/libs/bin)

# Where to find afterwards
SET(cisst_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(cisst_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")

# Where to find cisst defined CMake Macros/Modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${cisst_SOURCE_DIR}/libs/cmake)

# Options
OPTION(BUILD_TESTS "Build tests." OFF)
OPTION(BUILD_EXAMPLES "Build examples." OFF)


IF(UNIX)
  OPTION(BUILD_DOC "Build the documentation." OFF)
ENDIF(UNIX)

# Build static or dynamic libraries
OPTION(CISST_BUILD_SHARED_LIBS "Build cisst with shared libraries." OFF)
IF(CISST_BUILD_SHARED_LIBS)
  SET(IS_SHARED SHARED)
  SET(BUILD_SHARED_LIBS SHARED)
  IF(WIN32)
    ADD_DEFINITIONS(-DCISST_DLL)
  ENDIF(WIN32)
ELSE(CISST_BUILD_SHARED_LIBS)
  SET(IS_SHARED STATIC)
ENDIF(CISST_BUILD_SHARED_LIBS)


# Wrapping with swig for Python and/or Java
OPTION(CISST_HAS_SWIG_PYTHON "Build the Python wrappers for cisst with Swig." OFF)

IF(CISST_HAS_SWIG_PYTHON)
  # Find Swig
  FIND_PACKAGE(SWIG REQUIRED)
  INCLUDE(${SWIG_USE_FILE})

  # Check that the main libraries are compiled shared
  VARIABLE_REQUIRES(CISST_HAS_SWIG_PYTHON REQUIREMENT_MET CISST_BUILD_SHARED_LIBS)

  # Define the include directory for interface files
  SET(CISST_SWIG_INCLUDE_DIR ${cisst_SOURCE_DIR}/libs/wrapping)

  # Force to use exceptions
  SET(CMN_THROW_DOES_ABORT OFF CACHE BOOL "cmnThrow uses abort() instead of throw" FORCE)
  SET(CMN_ASSERT_THROWS_EXCEPTION ON CACHE BOOL "CMN_ASSERT throws an exception instead of abort" FORCE)

ENDIF(CISST_HAS_SWIG_PYTHON)

IF(CISST_HAS_SWIG_PYTHON)
  # Find Python
  FIND_PACKAGE(PythonLibs REQUIRED)
  # Some older CMake don't define the debug library
  IF(NOT PYTHON_DEBUG_LIBRARIES)
    SET(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
  ENDIF(NOT PYTHON_DEBUG_LIBRARIES)
  FIND_PACKAGE(PythonInterp REQUIRED)
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
ENDIF(CISST_HAS_SWIG_PYTHON)


# Use XML or not
SET(CISST_HAS_XML OFF CACHE BOOL "Include cmnXMLPath in the version (requires libxml2)")
IF(CISST_HAS_XML)
  FIND_PACKAGE(LibXml2 REQUIRED)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES LIBXML2_INCLUDE_DIR)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES LIBXML2_LIBRARIES)
ENDIF(CISST_HAS_XML)


# Check for Fortran NETLIB
OPTION(CISST_HAS_CISSTNETLIB "Use the netlib.org sets of numerical methods in Fortran (provided on cisst.org)." OFF)
IF(CISST_HAS_CISSTNETLIB)
  FIND_PACKAGE(CisstNetlib)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES CISSTNETLIB_INCLUDE_DIR)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES CISSTNETLIB_LIBRARIES)
ENDIF(CISST_HAS_CISSTNETLIB)


# Extra link flag if for Apple
IF(CISST_HAS_CISSTNETLIB AND APPLE AND CISST_BUILD_SHARED_LIBS)
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -single_module")
ENDIF(CISST_HAS_CISSTNETLIB AND APPLE AND CISST_BUILD_SHARED_LIBS)

# Check for RTAI
OPTION(CISST_HAS_LINUX_RTAI "Use the RTAI real time extensions for Linux." OFF)
IF(CISST_HAS_LINUX_RTAI)
  FIND_PACKAGE(RTAI)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_INCLUDE_DIRECTORIES RTAI_INCLUDE_DIR)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES RTAI_LIBRARIES)
ENDIF(CISST_HAS_LINUX_RTAI)

# Determine if cmnThrow uses "throw" or "abort"
OPTION(CMN_THROW_DOES_ABORT "cmnThrow uses abort() instead of throw" OFF)
MARK_AS_ADVANCED(CMN_THROW_DOES_ABORT)
IF(CMN_THROW_DOES_ABORT)
  ADD_DEFINITIONS(-DCMN_THROW_DOES_ABORT)
ENDIF(CMN_THROW_DOES_ABORT)

# Determine if CMN_ASSERT should be disabled
OPTION(CMN_ASSERT_DISABLED "CMN_ASSERT is disabled, i.e. does not assert (for release mode)" OFF)
MARK_AS_ADVANCED(CMN_ASSERT_DISABLED)
IF(CMN_ASSERT_DISABLED)
  ADD_DEFINITIONS(-DCMN_ASSERT_DISABLED)
ENDIF(CMN_ASSERT_DISABLED)

# Determine if CMN_ASSERT should throw an exception instead of abort
OPTION(CMN_ASSERT_THROWS_EXCEPTION "CMN_ASSERT throws an exception instead of abort" OFF)
MARK_AS_ADVANCED(CMN_ASSERT_THROWS_EXCEPTION)
IF(CMN_ASSERT_THROWS_EXCEPTION)
  ADD_DEFINITIONS(-DCMN_ASSERT_THROWS_EXCEPTION)
ENDIF(CMN_ASSERT_THROWS_EXCEPTION)


# Put "." in the rpath of all CISST shared libraries.
LINK_DIRECTORIES(. ${LIBRARY_OUTPUT_PATH})

# Determine if we want to use rpath, no by default
IF(UNIX)
  OPTION(CMAKE_SKIP_RPATH "Do NOT use rpath to hard code the path to dynamic libraries." ON)
  MARK_AS_ADVANCED(FORCE CMAKE_SKIP_RPATH)
ENDIF(UNIX)


# Add include directories
SET(CISST_INCLUDE_DIR
    ${cisst_BINARY_DIR}/libs/include
    ${cisst_SOURCE_DIR}/libs/include)
INCLUDE_DIRECTORIES(${CISST_INCLUDE_DIR} ${CISST_ADDITIONAL_INCLUDE_DIRECTORIES})

# Additional link directories
LINK_DIRECTORIES(${CISST_ADDITIONAL_LINK_DIRECTORIES})


# To have more warnings with gcc
IF(CMAKE_COMPILER_IS_GNUCXX)
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-Wextra" CXX_SUPPORTS_WEXTRA)
  IF(CXX_SUPPORTS_WEXTRA)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
  ENDIF(CXX_SUPPORTS_WEXTRA)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# Find native threads
FIND_PACKAGE(Threads REQUIRED)
CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES CMAKE_THREAD_LIBS_INIT)


# Which libraries to build
SET(CISST_LIBS_STABLE
    cisstCommon
    cisstVector
    )

SET(CISST_LIBS_UNSTABLE
    cisstNumerical
    cisstInteractive
    cisstOSAbstraction
    cisstMultiTask
    cisstParameterTypes
    cisstDevices
    cisstImage
    cisstStereoVision
    cisst3DUserInterface
    )

SET(CISST_LIBS
    ${CISST_LIBS_STABLE}
    ${CISST_LIBS_UNSTABLE})

# Build by default
FOREACH(lib ${CISST_LIBS_STABLE})
  OPTION(BUILD_LIBS_${lib} "Build ${lib}." ON)
ENDFOREACH(lib)

FOREACH(lib ${CISST_LIBS_UNSTABLE})
  OPTION(BUILD_LIBS_${lib} "Build ${lib}." OFF)
ENDFOREACH(lib)

# If OS abstraction is compiled, require dynamic loading libraries
IF(BUILD_LIBS_cisstOSAbstraction OR BUILD_LIBS_cisstInteractive)
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES CMAKE_DL_LIBS)
ENDIF(BUILD_LIBS_cisstOSAbstraction OR BUILD_LIBS_cisstInteractive)

# Once all variables have been defined, use cisstMacros
INCLUDE(cisstMacros)

# Finally, create lib targets
ADD_SUBDIRECTORY(libs)

# List of libraries to be built 
FOREACH(lib ${CISST_LIBS})
  IF(BUILD_LIBS_${lib})
    SET(CISST_LIBRARIES ${lib} ${CISST_LIBRARIES})
  ENDIF(BUILD_LIBS_${lib})
ENDFOREACH(lib)

# Contional builds
IF(BUILD_TESTS)
  # Settings for CTest
  ENABLE_TESTING()
  MARK_AS_ADVANCED(BUILD_TESTING)
  INCLUDE(Dart)
  # cisst Tests
  SUBDIRS(tests)
ENDIF(BUILD_TESTS)

IF(BUILD_EXAMPLES)
  ADD_SUBDIRECTORY(examples)
ENDIF(BUILD_EXAMPLES)

IF(BUILD_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF(BUILD_DOC)


# Create misc. config files
SET(CISST_BUILD_INCLUDE_DIRECTORIES ${CISST_INCLUDE_DIR})
SET(CISST_BUILD_DOXYGEN_DIRECTORY ${cisst_SOURCE_DIR}/libs/include)
SET(CISST_BUILD_LINK_DIRECTORIES ${cisst_LIBRARY_PATH})
SET(CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_F95_LIBRARIES})
IF(NOT UNIX)
    SET(CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISST_BUILD_CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_GCC_LIBRARIES})
ENDIF(NOT UNIX)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/cmake/cisstBuild.cmake.in
               ${cisst_BINARY_DIR}/cisstBuild.cmake
               @ONLY)

SET(CISST_INSTALLED_INCLUDE_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/include)
SET(CISST_INSTALLED_DOXYGEN_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
SET(CISST_INSTALLED_LINK_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/lib)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/cmake/cisstInstalled.cmake.in
               ${cisst_BINARY_DIR}/cisstInstalled.cmake
               @ONLY)
INSTALL_FILES(/share/cisst/cmake
              ".cmake"
              ${cisst_BINARY_DIR}/cisstInstalled.cmake)


# Configuration file
SET(CISST_BINARY_DIR ${cisst_BINARY_DIR})
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/include/cisstConfig.h.in
               ${cisst_BINARY_DIR}/libs/include/cisstConfig.h
               @ONLY)
INSTALL_FILES(/include
               ".h"
               ${cisst_BINARY_DIR}/libs/include/cisstConfig.h)


# Environment variables for Windows
IF(WIN32)
  # Script to set cisst environment variables
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
      
      # To find tests releated programs and libraries
      IF(BUILD_TESTS)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/tests/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/tests/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/tests/lib" "${cisst_BINARY_DIR}/tests/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/tests/bin" "${cisst_BINARY_DIR}/tests/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(BUILD_TESTS)

      # To find examples releated programs and libraries
      IF(BUILD_EXAMPLES)
        SET(CISST_PATH_CMAKE_FORMAT
            ${CISST_PATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/examples/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/examples/bin/${CISST_PATH_CONFIGURATION}")
        SET(CISST_PYTHONPATH_CMAKE_FORMAT
            ${CISST_PYTHONPATH_CMAKE_FORMAT}
            "${cisst_BINARY_DIR}/examples/lib" "${cisst_BINARY_DIR}/examples/lib/${CISST_PATH_CONFIGURATION}"
            "${cisst_BINARY_DIR}/examples/bin" "${cisst_BINARY_DIR}/examples/bin/${CISST_PATH_CONFIGURATION}")
      ENDIF(BUILD_EXAMPLES)

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
IF(UNIX)
  # Script to set cisst environment variables
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.csh.in
                 ${cisst_BINARY_DIR}/cisstvars.csh
                 @ONLY)
  # Script to set cisst environment variables
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.sh.in
                 ${cisst_BINARY_DIR}/cisstvars.sh
                 @ONLY)
ENDIF(UNIX)

