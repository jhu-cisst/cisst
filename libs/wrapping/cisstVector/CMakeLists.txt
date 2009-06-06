#
# $Id$
#
# CMakeLists for cisstVector wrapping
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
#

PROJECT(cisstVectorWrapping)

# See if there is an interface file for this library 
SET(cisstVector_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cisstVector.i)
  
IF(EXISTS ${cisstVector_INTERFACE})
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${cisstVector_INTERFACE} PROPERTIES CPLUSPLUS ON)
  # Make sure the runtime code is not included
  SET_SOURCE_FILES_PROPERTIES(${cisstVector_INTERFACE}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(cisstVectorPython python ${cisstVector_INTERFACE})
  SWIG_LINK_LIBRARIES(cisstVectorPython cisstVector cisstCommon
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})
  # uses .pyd instead of dll on Windows
  IF(WIN32)
    SET_TARGET_PROPERTIES(_cisstVectorPython PROPERTIES SUFFIX .pyd)
    SET_TARGET_PROPERTIES(_cisstVectorPython PROPERTIES DEBUG_POSTFIX "_d")
  ENDIF(WIN32)
  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _cisstVectorPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstVectorWrapping_BINARY_DIR}/cisstVectorPython.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstVectorPython.py)
  # Create a cisstVector.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _cisstVectorPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstVectorWrapping_BINARY_DIR}/cisstVectorPython.py
                             ${cisstVectorWrapping_BINARY_DIR}/cisstVector.py)
  # Install the interface files so that one can %import them
  INSTALL_FILES(/include
                ".i"
                ${cisstVector_INTERFACE})
ENDIF(EXISTS ${cisstVector_INTERFACE})
