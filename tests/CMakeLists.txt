#
# $Id: CMakeLists.txt 20 2009-01-08 16:09:57Z adeguet1 $
#

# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# backward compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(cisstTests)


# Set the ouptut path for the libraries and executables
SET(LIBRARY_OUTPUT_PATH ${cisstTests_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${cisstTests_BINARY_DIR}/bin)


# Load CppUnit stuff
INCLUDE(${cisst_SOURCE_DIR}/libs/cmake/FindCppUnit.cmake)
# TO DO, check if CPPUNIT Found
INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})
LINK_LIBRARIES(optimized ${CPPUNIT_LIBRARIES}
               debug ${CPPUNIT_DEBUG_LIBRARIES})


# Which tests to build based on which libraries have been compiled
FOREACH(lib ${CISST_LIBRARIES})
  # Test if the directory exists
  IF(EXISTS ${cisstTests_SOURCE_DIR}/${lib}) 
    # Build by default
    OPTION(BUILD_TESTS_${lib} "Build ${lib} tests." ON)
  ENDIF(EXISTS ${cisstTests_SOURCE_DIR}/${lib}) 
ENDFOREACH(lib)

# Generate tests for wrapped libraries
IF(CISST_HAS_SWIG_PYTHON)
  OPTION(BUILD_TESTS_wrapping "Build wrapping tests." ON)
ENDIF(CISST_HAS_SWIG_PYTHON)


# Create the main library
SET(CISST_TESTS_MAIN_SRC
    ${cisstTests_SOURCE_DIR}/code/cisstTestMain.cpp
    ${cisstTests_SOURCE_DIR}/code/cisstTestParameters.cpp
    ${cisstTests_SOURCE_DIR}/code/cisstTestParameters.h)
ADD_LIBRARY(cisstTestsMain STATIC ${CISST_TESTS_MAIN_SRC})


# Configure the OS test program
GET_FILENAME_COMPONENT(CMAKE_C_COMPILER_NAME ${CMAKE_C_COMPILER} NAME)
GET_FILENAME_COMPONENT(CMAKE_CXX_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME)
CONFIGURE_FILE(${cisstTests_SOURCE_DIR}/cisstCommon/cmnPortabilityTestCMake.h.in
               ${cisstTests_BINARY_DIR}/include/cmnPortabilityTestCMake.h
               @ONLY@)
# Add the path for the includes
INCLUDE_DIRECTORIES(${cisstTests_BINARY_DIR}/include)

# Create targets for each test
# Build in each subdir
FOREACH(lib ${CISST_LIBS})
  IF(BUILD_LIBS_${lib})
    IF(BUILD_TESTS_${lib})
      SUBDIRS(${lib})
    ENDIF(BUILD_TESTS_${lib})
  ELSE(BUILD_LIBS_${lib})
    IF(BUILD_TESTS_${lib})
      MESSAGE("Info: BUILD_TESTS_${lib} turned off since it requires BUILD_LIBS_${lib}")
      SET(BUILD_TESTS_${lib} OFF CACHE BOOL "Build ${lib} tests." FORCE)
    ENDIF(BUILD_TESTS_${lib})
  ENDIF(BUILD_LIBS_${lib})
ENDFOREACH(lib)

# Generate tests for wrapped libraries
IF(BUILD_TESTS_wrapping)
  IF(CISST_HAS_SWIG_PYTHON)
    SUBDIRS(wrapping)
  ELSE(CISST_HAS_SWIG_PYTHON)
    MESSAGE("Info: BUILD_TESTS_wrapping turned off since it requires CISST_HAS_SWIG_PYTHON")
    SET(BUILD_TESTS_wrapping OFF CACHE BOOL "Build wrapping tests." FORCE)
  ENDIF(CISST_HAS_SWIG_PYTHON)
ENDIF(BUILD_TESTS_wrapping)

# For other libraries using the same testing framework
CONFIGURE_FILE(${cisstTests_SOURCE_DIR}/cmake/cisstTestsBuild.cmake.in
               ${cisstTests_BINARY_DIR}/cisstTestsBuild.cmake
               @ONLY)

