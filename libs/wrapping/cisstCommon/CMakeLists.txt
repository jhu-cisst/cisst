#
# $Id: CMakeLists.txt 8 2009-01-04 21:13:48Z adeguet1 $
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
    SWIG_LINK_LIBRARIES(cisstCommonPython cisstCommon
                        debug ${PYTHON_DEBUG_LIBRARY}
                        optimized ${PYTHON_LIBRARY})
  ELSE(WIN32)
    SWIG_LINK_LIBRARIES(cisstCommonPython cisstCommon ${PYTHON_LIBRARIES})
  ENDIF(WIN32)
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


#
# $Log: CMakeLists.txt,v $
# Revision 1.9  2008/04/24 21:26:28  pkaz
# libs/wrapping:  Create .pyd files instead of .dll for Python wrapping on Windows.
# Also added workaround for bug in Swig 1.3.34 and 1.3.35.
#
# Revision 1.8  2008/01/16 22:49:11  anton
# libs Python wrapping: Updated CMakeLists.txt to handle CMake 2.4.7 (now required
# when wrappers on compiled) and added useless copy to avoid bug added in 2.4.7.
#
# Revision 1.7  2007/04/26 19:33:58  anton
# All files in libraries: Applied new license text, separate copyright and
# updated dates, added standard header where missing.
#
# Revision 1.6  2007/03/30 20:55:51  anton
# Wrappers: Removed code related to Java wrappers as these were never
# functional
#
# Revision 1.5  2006/11/20 20:33:20  anton
# Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
# cisstInteractive, cisstImage and cisstOSAbstraction.
#
# Revision 1.4  2006/05/24 14:23:52  anton
# cisstCommon wrappers: Attempt to use SWIG to generate Java wrappers.  Work
# in progress, currently untested and partial wrapping only.
#
# Revision 1.3  2006/05/02 19:28:04  anton
# Build tools: Major update in output directories.  Libraries and executables
# are now organized within "libs", "examples" and "tests" directories.  Paths
# can be set using the cisstvars scripts generated during the configuration
# (i.e. by CMake).  See ticket #215.
#
# Revision 1.2  2006/03/10 14:34:37  anton
# Misc CMakeLists.txt:  New flags for SWIG to reduce size of code and avoid
# Visual C++ warnings.
#
# Revision 1.1  2005/09/01 06:24:26  anton
# cisst wrapping:
# *: Reorganized files so that each library has its own CMakeLists.txt
# *: Added preliminary wrapping for some transformations.  This compiles, loads
# but is not tested.
#
#
