#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# All cisstOSAbstraction libraries
project (cisstOSAbstractionLibs)


# RTAI
if (CISST_HAS_LINUX_RTAI)
  # Check for existence of query_module, which was removed in kernel
  # versions 2.6+ (and possibly some earlier ones).
  include (CheckIncludeFiles)
  check_include_files ("linux/module.h" CMAKE_HAVE_MODULE_H)
  set (CISST_OSA_HAS_MODULE_H ${CMAKE_HAVE_MODULE_H})
  if (CMAKE_HAVE_MODULE_H)
    add_definitions (-DCMAKE_HAVE_MODULE_H)
  endif (CMAKE_HAVE_MODULE_H)
else (CISST_HAS_LINUX_RTAI)
  set (CISST_OSA_HAS_MODULE_H 0)
endif (CISST_HAS_LINUX_RTAI)


# QNX does not require rt library for clock_gettime (contained in libc)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  # QNX requires socket library
  set (CISST_SOCKET_LIBRARY socket)
  cisst_set_package_settings (cisstOSAbstraction Socket LIBRARIES CISST_SOCKET_LIBRARY)
else ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  if (UNIX AND NOT APPLE)
    # clock_gettime requires linking with librt
    set (CISST_RT_LIBRARY rt)
    cisst_set_package_settings (cisstOSAbstraction rt LIBRARIES CISST_RT_LIBRARY)
  endif (UNIX AND NOT APPLE)
endif ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")


# for windows, needs WinSock
if (WIN32)
    set (CISST_WSOCK_LIBRARY WSOCK32)
    cisst_set_package_settings (cisstOSAbstraction WinSock LIBRARIES CISST_WSOCK_LIBRARY)
endif (WIN32)


add_subdirectory (code)

if (CISST_HAS_SWIG_PYTHON)
  add_subdirectory (codePython)
endif (CISST_HAS_SWIG_PYTHON)

if (CISST_BUILD_EXAMPLES)
  option (CISST_BUILD_cisstOSAbstraction_EXAMPLES "Build examples for cisstOSAbstraction." ON)
  mark_as_advanced (CISST_BUILD_cisstOSAbstraction_EXAMPLES)
  if (CISST_BUILD_cisstOSAbstraction_EXAMPLES)
    add_subdirectory (examples)
  endif (CISST_BUILD_cisstOSAbstraction_EXAMPLES)
endif (CISST_BUILD_EXAMPLES)
