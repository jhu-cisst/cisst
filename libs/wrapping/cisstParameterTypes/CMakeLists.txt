#
# $Id$
#
# CMakeLists for cisstParameterTypes wrapping
#
# (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
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

PROJECT(cisstParameterTypesWrapping)

# See if there is an interface file for this library 
SET(cisstParameterTypes_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cisstParameterTypes.i)
  
IF(EXISTS ${cisstParameterTypes_INTERFACE})
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${cisstParameterTypes_INTERFACE} PROPERTIES CPLUSPLUS ON)
  # Make sure the runtime code is not included
  SET_SOURCE_FILES_PROPERTIES(${cisstParameterTypes_INTERFACE}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(cisstParameterTypesPython python ${cisstParameterTypes_INTERFACE})
  IF(WIN32)
    SET_TARGET_PROPERTIES(_cisstParameterTypesPython PROPERTIES SUFFIX .pyd)
    SET_TARGET_PROPERTIES(_cisstParameterTypesPython PROPERTIES DEBUG_POSTFIX "_d")
  ENDIF(WIN32)
  SWIG_LINK_LIBRARIES(cisstParameterTypesPython cisstParameterTypes
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})

  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _cisstParameterTypesPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstParameterTypesWrapping_BINARY_DIR}/cisstParameterTypesPython.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstParameterTypesPython.py)
  # Create a cisstParameterTypes.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _cisstParameterTypesPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstParameterTypesWrapping_BINARY_DIR}/cisstParameterTypesPython.py
                             ${cisstParameterTypesWrapping_BINARY_DIR}/cisstParameterTypes.py)
  # Install the interface files so that one can %import them
  INSTALL_FILES(/include
                ".i"
                ${cisstParameterTypes_INTERFACE})
ENDIF(EXISTS ${cisstParameterTypes_INTERFACE})

