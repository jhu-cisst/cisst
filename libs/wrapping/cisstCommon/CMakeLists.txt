#
# $Id$
#
# CMakeLists for cisstCommon wrapping
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

PROJECT(cisstCommonWrapping)

SET(cisstCommon_PYTHON_INTERFACE ${cisstCommonWrapping_SOURCE_DIR}/cisstCommon.i)

IF(EXISTS ${cisstCommon_PYTHON_INTERFACE})
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${cisstCommon_PYTHON_INTERFACE} PROPERTIES CPLUSPLUS ON)
  # Make sure the runtime code is not included
  SET_SOURCE_FILES_PROPERTIES(${cisstCommon_PYTHON_INTERFACE}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(cisstCommonPython python ${cisstCommon_PYTHON_INTERFACE})
  IF(WIN32)
    SET_TARGET_PROPERTIES(_cisstCommonPython PROPERTIES SUFFIX .pyd)
  ENDIF(WIN32)
  SWIG_LINK_LIBRARIES(cisstCommonPython cisstCommon
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})

  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _cisstCommonPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstCommonWrapping_BINARY_DIR}/cisstCommonPython.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstCommonPython.py)
  # Create a cisstCommon.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _cisstCommonPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstCommonWrapping_BINARY_DIR}/cisstCommonPython.py
                             ${cisstCommonWrapping_BINARY_DIR}/cisstCommon.py)
  # Install the interface files so that one can %import them
  INSTALL_FILES(/include
                ".i"
                ${cisstCommon_INTERFACE})

ENDIF(EXISTS ${cisstCommon_PYTHON_INTERFACE})

