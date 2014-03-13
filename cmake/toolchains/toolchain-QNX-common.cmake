#
# $Id: toolchain-QNX-common.cmake 3907 2012-12-10 20:04:49Z adeguet1 $
#
# Author(s):  Min Yang Jung
# Created on: 2011-09-13
#
# (C) Copyright 2011-2014 Johns Hopkins University (JHU), All Rights
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

set (CMAKE_SYSTEM_NAME      QNX)
set (CMAKE_SYSTEM_VERSION   ${QNX_VERSION_STRING})
set (CMAKE_SYSTEM_PROCESSOR x86)
set (TOOLCHAIN              QNX)

set (CMAKE_SHARED_LIBRARY_PREFIX "lib")
set (CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set (CMAKE_STATIC_LIBRARY_PREFIX "lib")
set (CMAKE_STATIC_LIBRARY_SUFFIX ".a")

# set executable extension on Windows
if (CMAKE_HOST_WIN32)
    set (HOST_EXECUTABLE_SUFFIX ".exe")
endif ()

# set QNX_HOST
if (DEFINED ENV{QNX_HOST})
    set (QNX_HOST $ENV{QNX_HOST})
else ()
    FIND_PATH (QNX_HOST
               NAME usr/bin/qcc${HOST_EXECUTABLE_SUFFIX}
               PATHS "/opt/qnx${QNX_VERSION}/host/linux/x86"
                     "C:/QNX${QNX_VERSION}/host/win32/x86"
               NO_CMAKE_PATH
               NO_CMAKE_ENVIRONMENT_PATH)
endif ()

# set QNX_TARGET
if (DEFINED ENV{QNX_TARGET})
    set (QNX_TARGET $ENV{QNX_TARGET})
else ()
    find_path (QNX_TARGET
               NAME usr/include/qnx_errno.h
               PATHS "/opt/qnx${QNX_VERSION}/target/qnx6"
                     C:/QNX${QNX_VERSION}/target/qnx6
               NO_CMAKE_PATH
               NO_CMAKE_ENVIRONMENT_PATH)
endif ()

# set QNX_CONFIGURATIOn (only on Windows)
if (DEFINED ENV{QNX_CONFIGURATION})
    set (QNX_CONFIGURATION $ENV{QNX_CONFIGURATION})
else ()
    if (CMAKE_HOST_WIN32)
        find_path (QNX_CONFIGURATION
                   NAME "bin/qnxactivate.exe"
                   PATHS "C:/Program Files/QNX Software Systems/qconfig"
                   NO_CMAKE_PATH
                   NO_CMAKE_ENVIRONMENT_PATH)
    endif ()
endif ()

set (CMAKE_MAKE_PROGRAM "${QNX_HOST}/usr/bin/make${HOST_EXECUTABLE_SUFFIX}" CACHE PATH "QNX Make Program")
set (CMAKE_SH           "${QNX_HOST}/usr/bin/sh${HOST_EXECUTABLE_SUFFIX}"   CACHE PATH "QNX shell Program")
set (CMAKE_AR           "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ar${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX ar Program")
set (CMAKE_RANLIB       "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ranlib${HOST_EXECUTABLE_SUFFIX}"  CACHE PATH "QNX ranlib Program")
set (CMAKE_NM           "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-nm${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX nm Program")
set (CMAKE_OBJCOPY      "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-objcopy${HOST_EXECUTABLE_SUFFIX}" CACHE PATH "QNX objcopy Program")
set (CMAKE_OBJDUMP      "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-objdump${HOST_EXECUTABLE_SUFFIX}" CACHE PATH "QNX objdump Program")
set (CMAKE_LINKER       "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ld" CACHE PATH "QNX Linker Program")
set (CMAKE_STRIP        "${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-strip${HOST_EXECUTABLE_SUFFIX}"   CACHE PATH "QNX Strip Program")

set (CMAKE_C_COMPILER ${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-gcc${HOST_EXECUTABLE_SUFFIX})
set (CMAKE_C_FLAGS_DEBUG "-g")
set (CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set (CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_CXX_COMPILER ${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-c++${HOST_EXECUTABLE_SUFFIX})
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_FIND_ROOT_PATH ${QNX_TARGET}) 
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
