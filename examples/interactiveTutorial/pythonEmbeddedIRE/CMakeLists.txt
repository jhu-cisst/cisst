#
# $Id: CMakeLists.txt,v 1.9 2008/09/04 02:07:23 pkaz Exp $
#
# (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
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
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(pythonEmbeddedIRE)

# Swig requirements
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# Threads for Python
FIND_PACKAGE(Threads REQUIRED)

# Create SineGenerator as a shared library
ADD_LIBRARY(SineGenerator SHARED SineGenerator.cpp SineGenerator.h)
CISST_REQUIRES("SineGenerator" "cisstCommon")

# If OS Abstraction is compiled, offer a choice
IF(BUILD_LIBS_cisstOSAbstraction)
  OPTION(EXAMPLE_IRE_USE_OSATHREAD "Use osaThread in pythonEmbeddedIRE example." OFF)
ENDIF(BUILD_LIBS_cisstOSAbstraction)


# Allow generated c++ code to find header files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CISST_SWIG_INCLUDE_DIR})

# To link with build libraries
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


# Run swig for each interface file
SET(MODULES SineGenerator)

FOREACH(module ${MODULES})
  SET(interface ${CMAKE_CURRENT_SOURCE_DIR}/${module}Python.i)
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${interface} PROPERTIES CPLUSPLUS ON)
  # Set the verbose flag
  SET_SOURCE_FILES_PROPERTIES(${interface}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(${module}Python python ${interface})
  SWIG_LINK_LIBRARIES(${module}Python ${module}
                      debug ${PYTHON_DEBUG_LIBRARY}
                      optimized ${PYTHON_LIBRARY})
  # uses .pyd instead of dll on Windows
  IF(WIN32)
    SET_TARGET_PROPERTIES(_${module}Python PROPERTIES SUFFIX .pyd)
  ENDIF(WIN32)
  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _${module}Python
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${CMAKE_CURRENT_BINARY_DIR}/${module}Python.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.py)
  # Create a SineGenerator.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _${module}Python
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${CMAKE_CURRENT_BINARY_DIR}/${module}Python.py
                             ${CMAKE_CURRENT_BINARY_DIR}/${module}.py)
  # Build list of files to clean
  SET(CISST_CLEAN_FILES ${CISST_CLEAN_FILES}
                        ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.py
                        ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.pyc
                        ${module}Python_wrap.cxx)
ENDFOREACH(module)

# Additional files to clean
SET_DIRECTORY_PROPERTIES(PROPERTIES
                         ADDITIONAL_MAKE_CLEAN_FILES
                         "${CISST_CLEAN_FILES}")

# Built executable with embedding
IF(EXAMPLE_IRE_USE_OSATHREAD)
  ADD_EXECUTABLE(pythonEmbeddedIRE pythonEmbeddedIRE.cpp IreLaunchCpp.h)
  ADD_DEFINITIONS(-DCISST_OSATHREAD)
  CISST_REQUIRES("pythonEmbeddedIRE" "cisstCommon;cisstInteractive;cisstOSAbstraction")
ELSE(EXAMPLE_IRE_USE_OSATHREAD)
  ADD_EXECUTABLE(pythonEmbeddedIRE pythonEmbeddedIRE.cpp IreLaunchPy.h)
  CISST_REQUIRES("pythonEmbeddedIRE" "cisstCommon;cisstInteractive")
ENDIF(EXAMPLE_IRE_USE_OSATHREAD)

TARGET_LINK_LIBRARIES(pythonEmbeddedIRE SineGenerator ${CMAKE_THREAD_LIBS_INIT})
IF(UNIX)
  IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    TARGET_LINK_LIBRARIES(pythonEmbeddedIRE util)
  ENDIF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
ENDIF(UNIX)


# Add custom dependencies on dynamically loaded modules
ADD_DEPENDENCIES(pythonEmbeddedIRE
                 _cisstCommonPython
                 _SineGeneratorPython)


# Copy the Python example to the binary tree
ADD_CUSTOM_COMMAND(TARGET pythonEmbeddedIRE
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND}
                   ARGS -E copy
                        ${CMAKE_CURRENT_SOURCE_DIR}/pythonEmbeddedIRE.py
                        ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/pythonEmbeddedIRE.py
                   COMMENT "Copy of pythonEmbeddedIRE.py to the library directory")
