#
# $Id$
#
# (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
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
PROJECT(pythonEmbedded)

# Swig requirements
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# Threads for Python
FIND_PACKAGE(Threads REQUIRED)

# Create myDerivedClass as a shared library
ADD_LIBRARY(myDerivedClass SHARED myDerivedClass.cpp myDerivedClass.h)
CISST_REQUIRES("myDerivedClass" "cisstCommon;cisstVector")

# Create myReDerivedClass as a shared library
ADD_LIBRARY(myReDerivedClass SHARED myReDerivedClass.cpp myReDerivedClass.h)
CISST_REQUIRES("myReDerivedClass" "cisstCommon;cisstVector")
TARGET_LINK_LIBRARIES(myReDerivedClass myDerivedClass)

# Allow generated c++ code to find header files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CISST_SWIG_INCLUDE_DIR})

# To link with build libraries
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


# Run swig for each interface file
SET(MODULES myDerivedClass myReDerivedClass)

FOREACH(module ${MODULES})
  SET(interface ${CMAKE_CURRENT_SOURCE_DIR}/${module}Python.i)
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${interface} PROPERTIES CPLUSPLUS ON)
  # Set the verbose flag
  SET_SOURCE_FILES_PROPERTIES(${interface}
                              PROPERTIES SWIG_FLAGS "-v;-modern")
  SWIG_ADD_MODULE(${module}Python python ${interface})
  SWIG_LINK_LIBRARIES(${module}Python ${module}
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})
  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _${module}Python
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${CMAKE_CURRENT_BINARY_DIR}/${module}Python.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.py)
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
ADD_EXECUTABLE(pythonEmbedded pythonEmbedded.cpp)
CISST_REQUIRES("pythonEmbedded" "cisstCommon;cisstVector")
TARGET_LINK_LIBRARIES(pythonEmbedded myDerivedClass myReDerivedClass ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(pythonEmbedded
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})
IF(UNIX)
  IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    TARGET_LINK_LIBRARIES(pythonEmbedded util)
  ENDIF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
ENDIF(UNIX)

# Copy the Python example to the binary tree
ADD_CUSTOM_COMMAND(TARGET pythonEmbedded
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND}
                   ARGS -E copy
                        ${CMAKE_CURRENT_SOURCE_DIR}/pythonEmbedded.py
                        ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/pythonEmbedded.py
                   COMMENT "Copy of pythonEmbedded.py to the executable directory")
