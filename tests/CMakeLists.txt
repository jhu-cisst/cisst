#
# $Id: CMakeLists.txt,v 1.27 2007/04/26 20:12:05 anton Exp $
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


#
# $Log: CMakeLists.txt,v $
# Revision 1.27  2007/04/26 20:12:05  anton
# All files in tests: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.26  2006/11/20 20:33:52  anton
# Licensing: Applied new license to tests.
#
# Revision 1.25  2006/09/16 02:07:12  anton
# FindCppUnit: Added debug library.  VC Express 2005 requires both versions of
# CppUnit, i.e. code generation option doesn't allow to interchange debug/release
# versions of CppUnit.
#
# Revision 1.24  2006/05/02 19:28:04  anton
# Build tools: Major update in output directories.  Libraries and executables
# are now organized within "libs", "examples" and "tests" directories.  Paths
# can be set using the cisstvars scripts generated during the configuration
# (i.e. by CMake).  See ticket #215.
#
# Revision 1.23  2005/09/06 16:45:17  anton
# tests CMakeLists: Turn off tests if the corresponding library is turn off.
#
# Revision 1.22  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.21  2005/07/14 04:00:55  anton
# cisst wrapping tests: Added code to handle release/debug configurations
# with MSVC.
#
# Revision 1.20  2005/06/21 17:32:09  anton
# Tests and Dart: Created the recommended DartConfig.cmake and moved settings
# from main CMakeLists.txt
#
# Revision 1.19  2005/06/17 23:06:32  anton
# Tests: : Now inherit Python settings from libs.
#
# Revision 1.18  2005/06/17 21:32:01  anton
# cisst CMake for tests: Major update to ease configuration.
#
# Revision 1.17  2005/06/14 17:41:02  anton
# Tests main CMakeLists.txt: Added code to include wrapping tests.
#
# Revision 1.16  2005/06/03 18:34:01  anton
# Tests for cisstCommon and cisstVector: Added license.
#
# Revision 1.15  2005/06/03 01:22:03  anton
# Tests: Preliminary support for Dart2.
#
# Revision 1.14  2005/04/08 16:38:13  anton
# Tests main CMakeLists.txt:  Added tests of wrapped libraries.
#
# Revision 1.13  2004/11/02 17:02:04  anton
# cisstNumerical Tests: Added a sanity check based on data from the Lawson
# and Hanson book and result from Matlab.
#
# Revision 1.12  2004/05/04 14:22:54  anton
# Removed link to all cisst libraries
#
# Revision 1.11  2004/01/22 20:49:06  anton
# Removed the INCLUDE(cisstTests.cmake) which is now obsolete
#
# Revision 1.10  2004/01/22 20:39:44  anton
# Added LINK_LIBRARIES(cisstTestsMain) so that all projects created with SUBDIRS could link correctly
#
# Revision 1.9  2003/12/12 20:43:22  ofri
# Export some test settings through cisstTestsBuild.cmake so that external
# tests may be developed using the cisst convention.
#
# Revision 1.8  2003/11/25 22:33:31  anton
# Extract the name of the compiler (remove directory) to make the test more robust
#
# Revision 1.7  2003/11/11 22:32:07  anton
# Created separate source and header files for the class cisstTestParameters
#
# Revision 1.6  2003/11/11 21:59:13  anton
# Added more warnings for gcc
#
# Revision 1.5  2003/09/17 14:23:33  anton
# added -DCISST_DLL if the libraries are shared
#
# Revision 1.4  2003/09/12 17:52:34  anton
# added cmnPortabilityTestCMake.h.in
#
# Revision 1.3  2003/09/12 14:33:36  anton
# create a library for the shared code (i.e. main())
#
# Revision 1.2  2003/08/06 14:02:42  anton
# Updated variable names from cisstXXX.cmake
#
# Revision 1.1.1.1  2003/07/31 18:20:33  anton
# Creation
#
#
