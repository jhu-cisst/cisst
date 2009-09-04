#
# $Id: FindNOVINT.cmake 252 2009-04-15 02:47:00Z adeguet1 $
#
# Author(s):  Anton Deguet
# Created on: 2008-04-04
#
# (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
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
# Try to find NOVINT HDL library (aka 3DTouch)
#
# The following values are defined
#
# NOVINT_INCLUDE_DIR          - include directories to use 
# NOVINT_LIBRARIES            - libraries needed
#
# Important notes:
# - Implemented for Windows only.

IF(WIN32)
  SET(NOVINT_SDK_DIR "D:/Program Files/Novint/HDAL_SDK_2.1.3")

  FIND_LIBRARY(NOVINT_LIB_HDL hdl ${NOVINT_SDK_DIR}/lib)
  
  SET(NOVINT_INCLUDE_DIR ${NOVINT_SDK_DIR}/include)
  
  
  SET(NOVINT_LIBRARIES
        ${NOVINT_LIB_HDL}) 
		
  FILE(TO_CMAKE_PATH "${NOVINT_INCLUDE_DIR}" NOVINT_INCLUDE_DIR)
  FILE(TO_CMAKE_PATH "${NOVINT_LIBRARIES}" NOVINT_LIBRARIES)

  MARK_AS_ADVANCED(NOVINT_LIB_HDL
                   NOVINT_INCLUDE_DIR
				   NOVINT_SDK_DIR
                   NOVINT_LIBRARIES)

ENDIF(WIN32)
