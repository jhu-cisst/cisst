#
# $Id: CMakeLists.txt 1018 2010-01-05 16:51:35Z adeguet1 $
#
# CMakeLists for cisstDevices wrapping
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

PROJECT(cisstDevicesWrapping)

# See if there is an interface file for this library 
SET(cisstDevices_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cisstDevices.i)
  
IF(EXISTS ${cisstDevices_INTERFACE})
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${cisstDevices_INTERFACE} PROPERTIES CPLUSPLUS ON)
  # Make sure the runtime code is not included
  SET_SOURCE_FILES_PROPERTIES(${cisstDevices_INTERFACE}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(cisstDevicesPython python ${cisstDevices_INTERFACE})
  IF(WIN32)
    SET_TARGET_PROPERTIES(_cisstDevicesPython PROPERTIES SUFFIX .pyd)
    SET_TARGET_PROPERTIES(_cisstDevicesPython PROPERTIES DEBUG_POSTFIX "_d")
  ENDIF(WIN32)
  SWIG_LINK_LIBRARIES(cisstDevicesPython cisstDevices
                      debug ${PYTHON_DEBUG_LIBRARIES}
                      optimized ${PYTHON_LIBRARIES})

  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _cisstDevicesPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstDevicesWrapping_BINARY_DIR}/cisstDevicesPython.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstDevicesPython.py)
  # Create a cisstDevices.py as CMake assumes one should be created
  # This is a bug that should be fixed in future releases of CMake.
  ADD_CUSTOM_COMMAND(TARGET _cisstDevicesPython
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${cisstDevicesWrapping_BINARY_DIR}/cisstDevicesPython.py
                             ${cisstDevicesWrapping_BINARY_DIR}/cisstDevices.py)
  # Install the interface files so that one can %import them
  INSTALL_FILES(/include
                ".i"
                ${cisstDevices_INTERFACE})
ENDIF(EXISTS ${cisstDevices_INTERFACE})
