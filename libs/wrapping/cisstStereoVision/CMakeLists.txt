#
# $Id: CMakeLists.txt 416 2009-06-06 03:45:00Z adeguet1 $
#
# CMakeLists for cisstStereoVision wrapping
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

PROJECT(cisstStereoVisionWrapping)

# See if there is an interface file for this library 
SET(cisstStereoVision_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cisstStereoVision.i)
  
IF(EXISTS ${cisstStereoVision_INTERFACE})
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${cisstStereoVision_INTERFACE} PROPERTIES CPLUSPLUS ON)
  # Make sure the runtime code is not included
  SET_SOURCE_FILES_PROPERTIES(${cisstStereoVision_INTERFACE}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(cisstStereoVisionPython python ${cisstStereoVision_INTERFACE})
  IF(WIN32)
    SET_TARGET_PROPERTIES(_cisstStereoVisionPython PROPERTIES SUFFIX .pyd)
    SET_TARGET_PROPERTIES(_cisstStereoVisionPython PROPERTIES DEBUG_POSTFIX "_d")
  ENDIF(WIN32)
  SWIG_LINK_LIBRARIES(cisstStereoVisionPython cisstStereoVision
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})

  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _cisstStereoVisionPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstStereoVisionWrapping_BINARY_DIR}/cisstStereoVisionPython.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstStereoVisionPython.py)
  # Create a cisstStereoVision.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _cisstStereoVisionPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstStereoVisionWrapping_BINARY_DIR}/cisstStereoVisionPython.py
                             ${cisstStereoVisionWrapping_BINARY_DIR}/cisstStereoVision.py)
  # Install the interface files so that one can %import them
  INSTALL_FILES(/include
                ".i"
                ${cisstStereoVision_INTERFACE})
ENDIF(EXISTS ${cisstStereoVision_INTERFACE})

