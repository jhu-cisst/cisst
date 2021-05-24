#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2007-02-23
#
# (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
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
# This file contains code found on the Wiki page of CMake
#

# - Test for DirectShow on Windows.
# Once loaded this will define
#   DIRECTSHOW_FOUND        - system has DirectShow
#   DIRECTSHOW_INCLUDE_DIR  - include directory for DirectShow
#   DIRECTSHOW_LIBRARIES    - libraries you need to link to

SET(DIRECTSHOW_FOUND "NO")

# DirectShow is only available on Windows platforms
IF(WIN32)
    SET(SVL_WIN64 FALSE)

    # Note that newer versions of MSVC generators as well as newer versions of CMake do not include the "bitness" of the generator.
    IF("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
        SET(SVL_WIN64 TRUE)
    ENDIF("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")

    # One more check for bitness
    # In CMake 3.1 and up we can use CMAKE_GENERATOR_PLATFORM. For now assume that systems that are still building
    # with older versions of CMake are not using the latest build tools. 
    IF(NOT SVL_WIN64 AND ${CMAKE_MAJOR_VERSION} GREATER_EQUAL 3 AND ${CMAKE_MINOR_VERSION} GREATER_EQUAL 1) 
        IF("${CMAKE_GENERATOR_PLATFORM}" MATCHES "(Win64|IA64|x64)")
            set(SVL_WIN64 TRUE)
        ENDIF("${CMAKE_GENERATOR_PLATFORM}" MATCHES "(Win64|IA64|x64)")
    ENDIF(NOT SVL_WIN64 AND ${CMAKE_MAJOR_VERSION} GREATER_EQUAL 3 AND ${CMAKE_MINOR_VERSION} GREATER_EQUAL 1)

    SET(PROGRAMFILES_DIR "$ENV{SystemDrive}/Program Files")
    SET(PROGRAMFILES_X86_DIR "$ENV{SystemDrive}/Program Files (x86)")

    SET(DIRECTSHOW_INCLUDE_DIRS
        "${PROGRAMFILES_DIR}/Microsoft SDKs/Windows/v7.0/Include"
        "${PROGRAMFILES_DIR}/Microsoft SDKs/Windows/v6.0A/Include"
        "${PROGRAMFILES_DIR}/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Include"
        "${PROGRAMFILES_DIR}/Microsoft DirectX SDK (February 2006)/Include"
        "${PROGRAMFILES_DIR}/Microsoft DirectX 9.0 SDK (June 2005)/Include"
        "${PROGRAMFILES_X86_DIR}/Microsoft SDKs/Windows/v7.0/Include"
        "${PROGRAMFILES_X86_DIR}/Microsoft SDKs/Windows/v6.0A/Include"
        "${PROGRAMFILES_X86_DIR}/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Include"
        "${PROGRAMFILES_X86_DIR}/Microsoft DirectX SDK (February 2006)/Include"
        "${PROGRAMFILES_X86_DIR}/Microsoft DirectX 9.0 SDK (June 2005)/Include"
        "$ENV{SystemDrive}/DXSDK/Include"
        )

    IF(SVL_WIN64)
       MESSAGE(STATUS "DirectShow using 64-bit static libraries")
       SET(DIRECTSHOW_LIBRARY_DIRS
           "${PROGRAMFILES_DIR}/Microsoft SDKs/Windows/v7.0/Lib/x64"
           "${PROGRAMFILES_DIR}/Microsoft SDKs/Windows/v6.0A/Lib/x64"
           "${PROGRAMFILES_DIR}/Microsoft Platform SDK for Windows Server 2003 R2/Lib/x64"
           "${PROGRAMFILES_DIR}/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib/x64"
           "${PROGRAMFILES_DIR}/Microsoft Platform SDK/Lib/x64"
           "${PROGRAMFILES_X86_DIR}/Microsoft SDKs/Windows/v7.0/Lib/x64"
           "${PROGRAMFILES_X86_DIR}/Microsoft SDKs/Windows/v6.0A/Lib/x64"
           "${PROGRAMFILES_X86_DIR}/Microsoft Platform SDK for Windows Server 2003 R2/Lib/x64"
           "${PROGRAMFILES_X86_DIR}/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib/x64"
           "${PROGRAMFILES_X86_DIR}/Microsoft Platform SDK/Lib/x64"
           "$ENV{SystemDrive}/DXSDK/Include/Lib/x64"
           )
    ELSE(SVL_WIN64)
       SET(DIRECTSHOW_LIBRARY_DIRS
           "${PROGRAMFILES_DIR}/Microsoft SDKs/Windows/v7.0/Lib"
           "${PROGRAMFILES_DIR}/Microsoft SDKs/Windows/v6.0A/Lib"
           "${PROGRAMFILES_DIR}/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
           "${PROGRAMFILES_DIR}/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
           "${PROGRAMFILES_DIR}/Microsoft Platform SDK/Lib"
           "${PROGRAMFILES_X86_DIR}/Microsoft SDKs/Windows/v7.0/Lib"
           "${PROGRAMFILES_X86_DIR}/Microsoft SDKs/Windows/v6.0A/Lib"
           "${PROGRAMFILES_X86_DIR}/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
           "${PROGRAMFILES_X86_DIR}/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
           "${PROGRAMFILES_X86_DIR}/Microsoft Platform SDK/Lib"
           "$ENV{SystemDrive}/DXSDK/Include/Lib"
           )
    ENDIF(SVL_WIN64)

    # Need CMake version 3.4 to use CMAKE_VS_WINDOWS_TARGET_PLATFORM
    IF(${CMAKE_MAJOR_VERSION} GREATER_EQUAL 3 AND ${CMAKE_MINOR_VERSION} GREATER_EQUAL 4)
        message(STATUS "Appending paths")
        LIST(APPEND DIRECTSHOW_INCLUDE_DIRS "${PROGRAMFILES_X86_DIR}/Windows Kits/10/Include/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/um")
        
        # Figure out which lib folder to use.
        SET(LIB_APPEND_PATH "${PROGRAMFILES_X86_DIR}/Windows Kits/10/Lib/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/um")
        IF(SVL_WIN64)
            SET(LIB_APPEND_PATH "${LIB_APPEND_PATH}/x64")
        ELSE()
            SET(LIB_APPEND_PATH "${LIB_APPEND_PATH}/x86")
        endif(SVL_WIN64)
        LIST(APPEND DIRECTSHOW_LIBRARY_DIRS ${LIB_APPEND_PATH})
    ENDIF(${CMAKE_MAJOR_VERSION} GREATER_EQUAL 3 AND ${CMAKE_MINOR_VERSION} GREATER_EQUAL 4)

    # Find DirectX Include Directory
    FIND_PATH(DIRECTX_INCLUDE_DIR
              NAMES ddraw.h
              PATHS ${DIRECTSHOW_INCLUDE_DIRS}
              )

    # if DirectX found, then find DirectShow include directory
    IF(DIRECTX_INCLUDE_DIR)
        FIND_PATH(DIRECTSHOW_INCLUDE_DIR
                  NAMES dshow.h
                  PATHS ${DIRECTSHOW_INCLUDE_DIRS}
                  )

        # if DirectShow include dir found, then find DirectShow libraries
        IF(DIRECTSHOW_INCLUDE_DIR)
            FIND_LIBRARY(DIRECTSHOW_strmiids_LIBRARY
                         NAMES strmiids
                         PATHS ${DIRECTSHOW_LIBRARY_DIRS}
                         )
            FIND_LIBRARY(DIRECTSHOW_WinMM_LIBRARY
                         NAMES WinMM
                         PATHS ${DIRECTSHOW_LIBRARY_DIRS}
                         )

            # if DirectShow libraries found, then we're ok
            IF(DIRECTSHOW_strmiids_LIBRARY AND DIRECTSHOW_WinMM_LIBRARY)
                # everything found
                SET(DIRECTSHOW_FOUND "YES")
            ENDIF(DIRECTSHOW_strmiids_LIBRARY AND DIRECTSHOW_WinMM_LIBRARY)
        ENDIF(DIRECTSHOW_INCLUDE_DIR)
    ENDIF(DIRECTX_INCLUDE_DIR)
ENDIF(WIN32)


IF(DIRECTSHOW_FOUND)
    SET(DIRECTSHOW_INCLUDE_DIR
        ${DIRECTSHOW_INCLUDE_DIR}
        ${DIRECTX_INCLUDE_DIR}
        )
    SET(DIRECTSHOW_LIBRARIES
        ${DIRECTSHOW_strmiids_LIBRARY}
        )
ELSE(DIRECTSHOW_FOUND)
    # make FIND_PACKAGE friendly
    IF(NOT DIRECTSHOW_FIND_QUIETLY)
        IF(DIRECTSHOW_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "DirectShow required, please specify it's location.")
        ELSE(DIRECTSHOW_FIND_REQUIRED)
            MESSAGE(STATUS "DirectShow was not found.")
        ENDIF(DIRECTSHOW_FIND_REQUIRED)
    ENDIF(NOT DIRECTSHOW_FIND_QUIETLY)
ENDIF(DIRECTSHOW_FOUND)

