#
# $Id$
#
# CMakeLists for cisstMultiTask wrapping
#
# (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
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

PROJECT(cisstMultiTaskWrapping)

# See if there is an interface file for this library 
SET(cisstMultiTask_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cisstMultiTask.i)
  
IF(EXISTS ${cisstMultiTask_INTERFACE})
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${cisstMultiTask_INTERFACE} PROPERTIES CPLUSPLUS ON)
  # Make sure the runtime code is not included
  SET_SOURCE_FILES_PROPERTIES(${cisstMultiTask_INTERFACE}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(cisstMultiTaskPython python ${cisstMultiTask_INTERFACE})
  IF(WIN32)
    SET_TARGET_PROPERTIES(_cisstMultiTaskPython PROPERTIES SUFFIX .pyd)
  ENDIF(WIN32)
  SWIG_LINK_LIBRARIES(cisstMultiTaskPython cisstMultiTask
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})

  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _cisstMultiTaskPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstMultiTaskWrapping_BINARY_DIR}/cisstMultiTaskPython.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstMultiTaskPython.py)
  # Create a cisstMultiTask.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _cisstMultiTaskPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstMultiTaskWrapping_BINARY_DIR}/cisstMultiTaskPython.py
                             ${cisstMultiTaskWrapping_BINARY_DIR}/cisstMultiTask.py)
  # Install the interface files so that one can %import them
  INSTALL_FILES(/include
                ".i"
                ${cisstMultiTask_INTERFACE})
ENDIF(EXISTS ${cisstMultiTask_INTERFACE})

