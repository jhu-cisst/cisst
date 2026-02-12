#
# $Id$
#
# Author(s):  Anton Deguet Created on: 2007-02-23
#
# (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with no warranty.  The complete license can be found
# in license.txt and http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

#
# This file contains code found on the Wiki page of CMake
#

# * Test for DirectShow on Windows. Once loaded this will define DIRECTSHOW_FOUND        - system has DirectShow
#   DIRECTSHOW_INCLUDE_DIR  - include directory for DirectShow DIRECTSHOW_LIBRARIES    - libraries you need to link to

set(DIRECTSHOW_FOUND "NO")

# DirectShow is only available on Windows platforms
if(WIN32)
    set(SVL_WIN64 FALSE)
    message(STATUS "Searching for DirectShow on Windows")

    # One more check for bitness In CMake 3.1 and up we can use CMAKE_GENERATOR_PLATFORM. For now assume that systems
    # that are still building with older versions of CMake are not using the latest build tools.
    if(NOT SVL_WIN64)
        if("${CMAKE_GENERATOR_PLATFORM}" MATCHES "(Win64|IA64|x64)")
            set(SVL_WIN64 TRUE)
        endif("${CMAKE_GENERATOR_PLATFORM}" MATCHES "(Win64|IA64|x64)")
    endif(NOT SVL_WIN64)

    set(PROGRAMFILES_DIR "$ENV{SystemDrive}/Program Files")
    set(PROGRAMFILES_X86_DIR "$ENV{SystemDrive}/Program Files (x86)")

    set(DIRECTSHOW_INCLUDE_DIRS
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

    if(SVL_WIN64)
        message(STATUS "DirectShow using 64-bit static libraries")
        set(DIRECTSHOW_LIBRARY_DIRS
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
    else(SVL_WIN64)
        set(DIRECTSHOW_LIBRARY_DIRS
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
    endif(SVL_WIN64)

    # Need CMake version 3.4 to use CMAKE_VS_WINDOWS_TARGET_PLATFORM
    message(STATUS "Appending paths")
    list(APPEND DIRECTSHOW_INCLUDE_DIRS
         "${PROGRAMFILES_X86_DIR}/Windows Kits/10/Include/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/um"
    )

    # Figure out which lib folder to use.
    set(LIB_APPEND_PATH
        "${PROGRAMFILES_X86_DIR}/Windows Kits/10/Lib/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/um"
    )
    if(SVL_WIN64)
        set(LIB_APPEND_PATH "${LIB_APPEND_PATH}/x64")
    else()
        set(LIB_APPEND_PATH "${LIB_APPEND_PATH}/x86")
    endif(SVL_WIN64)
    list(APPEND DIRECTSHOW_LIBRARY_DIRS ${LIB_APPEND_PATH})

    # Find DirectX Include Directory
    find_path(
        DIRECTX_INCLUDE_DIR
        NAMES ddraw.h
        PATHS ${DIRECTSHOW_INCLUDE_DIRS}
    )

    # if DirectX found, then find DirectShow include directory
    if(DIRECTX_INCLUDE_DIR)
        find_path(
            DIRECTSHOW_INCLUDE_DIR
            NAMES dshow.h
            PATHS ${DIRECTSHOW_INCLUDE_DIRS}
        )

        # if DirectShow include dir found, then find DirectShow libraries
        if(DIRECTSHOW_INCLUDE_DIR)
            find_library(
                DIRECTSHOW_strmiids_LIBRARY
                NAMES strmiids
                PATHS ${DIRECTSHOW_LIBRARY_DIRS}
            )
            find_library(
                DIRECTSHOW_WinMM_LIBRARY
                NAMES WinMM
                PATHS ${DIRECTSHOW_LIBRARY_DIRS}
            )

            # if DirectShow libraries found, then we're ok
            if(DIRECTSHOW_strmiids_LIBRARY AND DIRECTSHOW_WinMM_LIBRARY)
                # everything found
                set(DIRECTSHOW_FOUND "YES")
            endif(DIRECTSHOW_strmiids_LIBRARY AND DIRECTSHOW_WinMM_LIBRARY)
        endif(DIRECTSHOW_INCLUDE_DIR)
    endif(DIRECTX_INCLUDE_DIR)
endif(WIN32)

if(DIRECTSHOW_FOUND)
    set(DIRECTSHOW_INCLUDE_DIR ${DIRECTSHOW_INCLUDE_DIR} ${DIRECTX_INCLUDE_DIR})
    set(DIRECTSHOW_LIBRARIES ${DIRECTSHOW_strmiids_LIBRARY})
else(DIRECTSHOW_FOUND)
    # make FIND_PACKAGE friendly
    if(NOT DIRECTSHOW_FIND_QUIETLY)
        if(DIRECTSHOW_FIND_REQUIRED)
            message(FATAL_ERROR "DirectShow required, please specify it's location.")
        else(DIRECTSHOW_FIND_REQUIRED)
            message(STATUS "DirectShow was not found.")
        endif(DIRECTSHOW_FIND_REQUIRED)
    endif(NOT DIRECTSHOW_FIND_QUIETLY)
endif(DIRECTSHOW_FOUND)
