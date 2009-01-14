#
# $Id: CMakeLists.txt,v 1.7 2008/04/24 21:26:28 pkaz Exp $
#
# CMakeLists for cisstNumerical wrapping
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
#

PROJECT(cisstNumericalWrapping)

# See if there is an interface file for this library 
SET(cisstNumerical_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cisstNumerical.i)
  
IF(EXISTS ${cisstNumerical_INTERFACE})
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${cisstNumerical_INTERFACE} PROPERTIES CPLUSPLUS ON)
  # Make sure the runtime code is not included
  SET_SOURCE_FILES_PROPERTIES(${cisstNumerical_INTERFACE}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(cisstNumericalPython python ${cisstNumerical_INTERFACE})
  IF(WIN32)
    SET_TARGET_PROPERTIES(_cisstNumericalPython PROPERTIES SUFFIX .pyd)
    SWIG_LINK_LIBRARIES(cisstNumericalPython cisstNumerical
                        debug ${PYTHON_DEBUG_LIBRARY}
                        optimized ${PYTHON_LIBRARY})
  ELSE(WIN32)
    SWIG_LINK_LIBRARIES(cisstNumericalPython cisstNumerical ${PYTHON_LIBRARIES})
  ENDIF(WIN32)
  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _cisstNumericalPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstNumericalWrapping_BINARY_DIR}/cisstNumericalPython.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstNumericalPython.py)
  # Create a cisstNumerical.py as CMake assumes one should be created.
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _cisstNumericalPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstNumericalWrapping_BINARY_DIR}/cisstNumericalPython.py
                             ${cisstNumericalWrapping_BINARY_DIR}/cisstNumerical.py)
  # Install the interface files so that one can %import them
  INSTALL_FILES(/include
                ".i"
                ${cisstNumerical_INTERFACE})
ENDIF(EXISTS ${cisstNumerical_INTERFACE})

