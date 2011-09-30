#
# $Id$
#

# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstTests)

# Find cisst using all libraries
find_package (cisst REQUIRED)

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  # Set the ouptut path for the libraries and executables
  set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
  set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

  # Load CppUnit stuff
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CISST_CMAKE_DIR})
  find_package (CppUnit REQUIRED)
  # TO DO, check if CPPUNIT Found
  include_directories (${CPPUNIT_INCLUDE_DIR})
  link_libraries (optimized ${CPPUNIT_LIBRARIES}
                  debug ${CPPUNIT_DEBUG_LIBRARIES})

  # Which tests to build based on which libraries have been compiled
  foreach (lib ${CISST_LIBRARIES})
    # Test if the directory exists
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib})
      # Build by default and mark as advanced
      option (CISST_BUILD_TESTS_${lib} "Build ${lib} tests." ON)
      mark_as_advanced (CISST_BUILD_TESTS_${lib})
    endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${lib})
  endforeach (lib)

  # Generate tests for wrapped libraries
  if (CISST_HAS_SWIG_PYTHON)
    option (CISST_BUILD_TESTS_wrapping "Build wrapping tests." ON)
    mark_as_advanced (CISST_BUILD_TESTS_wrapping)
  endif (CISST_HAS_SWIG_PYTHON)

  # Create the main library
  set (CISST_TESTS_MAIN_SRC
       ${CMAKE_CURRENT_SOURCE_DIR}/code/cisstTestMain.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/code/cisstTestParameters.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/code/cisstTestParameters.h)
  add_library (cisstTestsMain STATIC ${CISST_TESTS_MAIN_SRC})

  # Configure the OS test program
  get_filename_component (CMAKE_C_COMPILER_NAME ${CMAKE_C_COMPILER} NAME)
  get_filename_component (CMAKE_CXX_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cisstCommon/cmnPortabilityTestCMake.h.in
                  ${CMAKE_CURRENT_BINARY_DIR}/include/cmnPortabilityTestCMake.h
                  @ONLY@)
  # Add the path for the includes
  include_directories (${CMAKE_CURRENT_BINARY_DIR}/include)

  # Create targets for each test
  # Build in each subdir
  foreach (lib ${CISST_LIBRARIES})
    if (CISST_BUILD_TESTS_${lib})
      add_subdirectory (${lib})
    endif (CISST_BUILD_TESTS_${lib})
  endforeach (lib)

  # Generate tests for wrapped libraries
  if (CISST_BUILD_TESTS_wrapping)
    if (CISST_HAS_SWIG_PYTHON)
      add_subdirectory (wrapping)
    else (CISST_HAS_SWIG_PYTHON)
      message ("Info: CISST_BUILD_TESTS_wrapping turned off since it requires CISST_HAS_SWIG_PYTHON")
      set (CISST_BUILD_TESTS_wrapping OFF CACHE BOOL "Build wrapping tests." FORCE)
    endif (CISST_HAS_SWIG_PYTHON)
  endif (CISST_BUILD_TESTS_wrapping)

  # For other libraries using the same testing framework
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake/cisstTestsBuild.cmake.in
                  ${CMAKE_CURRENT_BINARY_DIR}/cisstTestsBuild.cmake
                  @ONLY)

else (cisst_FOUND_AS_REQUIRED)
  message ("Code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires at least some of the cisst libraries")
endif (cisst_FOUND_AS_REQUIRED)
