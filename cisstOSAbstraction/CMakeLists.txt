#
# (C) Copyright 2005-2026 Johns Hopkins University (JHU), All Rights Reserved.
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
# (platform link deps are applied after the target is created; see below)

add_subdirectory (code)

if (TARGET cisstOSAbstraction)
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
    target_link_libraries (cisstOSAbstraction PUBLIC socket)
  elseif (UNIX AND NOT APPLE)
    target_link_libraries (cisstOSAbstraction PUBLIC rt)
  elseif (WIN32)
    target_link_libraries (cisstOSAbstraction PUBLIC WSOCK32)
  endif ()
endif ()

cisst_offer_python (cisstOSAbstraction)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstOSAbstraction)
cisst_offer_tests (cisstOSAbstraction)
