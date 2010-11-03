#
# $Id$
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
# Try to find Sensable HD library (aka 3DTouch)
#
# The following values are defined
#
# SENSABLE_INCLUDE_DIR          - include directories to use 
# SENSABLE_LIBRARIES            - libraries needed
# SENSABLE_LIBRARIES_RELEASE    - libraries needed in release mode (deprecated)
# SENSABLE_LIBRARIES_DEBUG      - libraries needed in debug mode (deprecated)
#
# Important notes:
# - Implemented for Windows and Mac OS X only.
# - Only uses the environment variable 3DTOUCH_BASE, could search in
#   c:\program files in future releases.
# - Should use SENSABLE_LIBRARIES, which already includes the debug
#   and optimized CMake keywords, e.g.,:
#      TARGET_LINK_LIBRARIES(myTarget ${SENSABLE_LIBRARIES})
#   instead of:
#      TARGET_LINK_LIBRARIES(myTarget debug ${SENSABLE_LIBRARIES_DEBUG} optimized ${SENSABLE_LIBRARIES_RELEASE})
#   The latter syntax does not work correctly because the debug and optimized keywords only apply to the
#   next library in the list.

IF(WIN32)
  SET(SENSABLE_BASE_ENV $ENV{3DTOUCH_BASE})

  IF(DEFINED SENSABLE_BASE_ENV)
    SET(SENSABLE_INCLUDE_DIR
        ${SENSABLE_BASE_ENV}/include
        ${SENSABLE_BASE_ENV}/utilities/include)
    FIND_LIBRARY(SENSABLE_LIB_HD hd ${SENSABLE_BASE_ENV}/lib
                                    ${SENSABLE_BASE_ENV}/lib/win32)
    FIND_LIBRARY(SENSABLE_LIB_HL hl ${SENSABLE_BASE_ENV}/lib
                                    ${SENSABLE_BASE_ENV}/lib/win32)
    FIND_LIBRARY(SENSABLE_LIB_HDU hdu ${SENSABLE_BASE_ENV}/utilities/lib
                                      ${SENSABLE_BASE_ENV}/utilities/lib/win32/Release)
    # try to find hdud (OpenHaptics 2.x)
    FIND_LIBRARY(SENSABLE_LIB_HDUD hdud ${SENSABLE_BASE_ENV}/utilities/lib)
    IF(NOT ${SENSABLE_LIB_HDUD})
      # try to find hdu in Debug directory (OpenHaptics 3.x)
      FIND_LIBRARY(SENSABLE_LIB_HDUD hdu ${SENSABLE_BASE_ENV}/utilities/lib/win32/Debug)
    ENDIF(NOT ${SENSABLE_LIB_HDUD})
    FIND_LIBRARY(SENSABLE_LIB_HLU hlu ${SENSABLE_BASE_ENV}/utilities/lib
                                      ${SENSABLE_BASE_ENV}/utilities/lib/win32/Release)
    # try to find hlud (OpenHaptics 2.x)
    FIND_LIBRARY(SENSABLE_LIB_HLUD hlud ${SENSABLE_BASE_ENV}/utilities/lib)
    IF(NOT ${SENSABLE_LIB_HLUD})
      # try to find hlu in Debug directory (OpenHaptics 3.x)
      FIND_LIBRARY(SENSABLE_LIB_HLUD hlu ${SENSABLE_BASE_ENV}/utilities/lib/win32/Debug)
    ENDIF(NOT ${SENSABLE_LIB_HLUD})
    SET(SENSABLE_LIBRARIES_RELEASE
        ${SENSABLE_LIB_HD} ${SENSABLE_LIB_HDU}
        ${SENSABLE_LIB_HL} ${SENSABLE_LIB_HLU})
    SET(SENSABLE_LIBRARIES_DEBUG
        ${SENSABLE_LIB_HD} ${SENSABLE_LIB_HDUD}
        ${SENSABLE_LIB_HL} ${SENSABLE_LIB_HLUD})
    SET(SENSABLE_LIBRARIES
        ${SENSABLE_LIB_HD}
        ${SENSABLE_LIB_HL}
        optimized ${SENSABLE_LIB_HDU} debug ${SENSABLE_LIB_HDUD}
        optimized ${SENSABLE_LIB_HLU} debug ${SENSABLE_LIB_HLUD})

    FILE(TO_CMAKE_PATH "${SENSABLE_INCLUDE_DIR}" SENSABLE_INCLUDE_DIR)
    FILE(TO_CMAKE_PATH "${SENSABLE_LIBRARIES}" SENSABLE_LIBRARIES)

    MARK_AS_ADVANCED(SENSABLE_LIB_HD
                     SENSABLE_LIB_HL
                     SENSABLE_LIB_HDU
                     SENSABLE_LIB_HDUD
                     SENSABLE_LIB_HLU
                     SENSABLE_LIB_HLUD
                     SENSABLE_BASE_ENV
                     SENSABLE_INCLUDE_DIR
                     SENSABLE_LIBRARIES_RELEASE
                     SENSABLE_LIBRARIES_DEBUG
                     SENSABLE_LIBRARIES)

  ELSE(DEFINED SENSABLE_BASE_ENV) 
    MESSAGE("Make sure the environment variable 3DTOUCH_BASE is defined")
  ENDIF(DEFINED SENSABLE_BASE_ENV) 
ENDIF(WIN32)

IF(APPLE)
  SET(SENSABLE_FRAMEWORKS)
  INCLUDE(CMakeFindFrameworks)
  CMAKE_FIND_FRAMEWORKS(HD)
  IF(HD_FRAMEWORKS)
    SET(SENSABLE_INCLUDE_DIR ${SENSABLE_INCLUDE_DIR} ${HD_FRAMEWORKS}/Headers)
    SET(SENSABLE_FRAMEWORKS "-framework HD" ${SENSABLE_FRAMEWORKS})
  ENDIF(HD_FRAMEWORKS)
  CMAKE_FIND_FRAMEWORKS(HL)
  IF(HL_FRAMEWORKS)
    SET(SENSABLE_INCLUDE_DIR ${SENSABLE_INCLUDE_DIR} ${HL_FRAMEWORKS}/Headers)
    SET(SENSABLE_FRAMEWORKS "-framework HL" ${SENSABLE_FRAMEWORKS})
  ENDIF(HL_FRAMEWORKS)
  CMAKE_FIND_FRAMEWORKS(HDU)
  IF(HDU_FRAMEWORKS)
    SET(SENSABLE_INCLUDE_DIR ${SENSABLE_INCLUDE_DIR} ${HDU_FRAMEWORKS}/Headers)
    SET(SENSABLE_FRAMEWORKS "-framework HDU" ${SENSABLE_FRAMEWORKS})
  ENDIF(HDU_FRAMEWORKS)
  CMAKE_FIND_FRAMEWORKS(HLU)
  IF(HLU_FRAMEWORKS)
    SET(SENSABLE_INCLUDE_DIR ${SENSABLE_INCLUDE_DIR} ${HLU_FRAMEWORKS}/Headers)
    SET(SENSABLE_FRAMEWORKS "-framework HLU" ${SENSABLE_FRAMEWORKS})
  ENDIF(HLU_FRAMEWORKS)

  IF(SENSABLE_FRAMEWORKS)
    SET(SENSABLE_LIBRARIES_RELEASE ${SENSABLE_FRAMEWORKS})
    SET(SENSABLE_LIBRARIES_DEBUG ${SENSABLE_FRAMEWORKS})
    SET(SENSABLE_LIBRARIES ${SENSABLE_FRAMEWORKS})
  ENDIF(SENSABLE_FRAMEWORKS)

  MARK_AS_ADVANCED(SENSABLE_FRAMEWORKS
                   SENSABLE_INCLUDE_DIR
                   SENSABLE_LIBRARIES_RELEASE
                   SENSABLE_LIBRARIES_DEBUG
                   SENSABLE_LIBRARIES)
ENDIF(APPLE)

if("${CMAKE_SYSTEM}" MATCHES "Linux")

 set( SENSABLE_SEARCH_PATH /usr /usr/local )
  
  find_path( SENSABLE_DIR include/HD/hd.h ${SENSABLE_SEARCH_PATH} )
  
  if( SENSABLE_DIR ) 

    set( SENSABLE_INCLUDE_DIR ${SENSABLE_DIR}/include )
    set( SENSABLE_FOUND true )
    
    find_library( SENSABLE_LIB_HD HD  ${SENSABLE_DIR}/lib)
    find_library( SENSABLE_LIB_HDU HDU  ${SENSABLE_DIR}/lib)
    find_library( SENSABLE_LIB_HL HL  ${SENSABLE_DIR}/lib)
    find_library( SENSABLE_LIB_HLU HLU  ${SENSABLE_DIR}/lib)

    set( SENSABLE_LIBRARIES 
        ${SENSABLE_LIB_HD}
        ${SENSABLE_LIB_HDU}
        ${SENSABLE_LIB_HL}
        ${SENSABLE_LIB_HLU} )    
    
  MARK_AS_ADVANCED( SENSABLE_LIB_HD SENSABLE_LIB_HDU SENSABLE_LIB_HL SENSABLE_LIB_HLU )


  endif( SENSABLE_DIR )

endif("${CMAKE_SYSTEM}" MATCHES "Linux")