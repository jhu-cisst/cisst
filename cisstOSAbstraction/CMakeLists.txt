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

# QNX does not require rt library for clock_gettime (contained in libc)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  # QNX requires socket library
  set (CISST_SOCKET_LIBRARY socket)
else ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  if (UNIX AND NOT APPLE)
    # clock_gettime requires linking with librt
    set (CISST_RT_LIBRARY rt)
  endif (UNIX AND NOT APPLE)
endif ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")


# for windows, needs WinSock
if (WIN32)
    set (CISST_WSOCK_LIBRARY WSOCK32)
endif (WIN32)


add_subdirectory (code)

cisst_offer_python (cisstOSAbstraction)

# build examples and tests, add an option to skip them
cisst_offer_examples (cisstOSAbstraction)
cisst_offer_tests (cisstOSAbstraction)
