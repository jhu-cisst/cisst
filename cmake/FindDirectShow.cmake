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
    IF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 Win64"     OR
       ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 Win64"     OR
       ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005 Win64" OR
       ${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008 Win64")
       SET(SVL_WIN64 TRUE)
    ENDIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 Win64"     OR
          ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 Win64"     OR
          ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005 Win64" OR
          ${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008 Win64")

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

