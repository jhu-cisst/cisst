#
# $Id$
#
# CMakeLists for cisstOSAbstraction
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (cisstOSAbstraction)

# all source files
set (SOURCE_FILES
     osaClassServices.cpp
     osaCPUAffinity.cpp
     osaCriticalSection.cpp
     osaDynamicLoader.cpp
     osaDynamicLoaderAndFactory.cpp
     osaGetTime.cpp
     osaMutex.cpp
     osaPipeExec.cpp
     osaSerialPort.cpp
     osaSleep.cpp
     osaSocket.cpp
     osaSocketServer.cpp
     osaStopwatch.cpp
     osaThread.cpp
     osaThreadBuddy.cpp
     osaThreadSignal.cpp
     osaTimeServer.cpp
     )

# all header files
set (HEADER_FILES
     osaForwardDeclarations.h
     osaCPUAffinity.h
     osaCriticalSection.h
     osaDynamicLoader.h
     osaDynamicLoaderAndFactory.h
     osaExport.h
     osaGetTime.h
     osaMutex.h
     osaPipeExec.h
     osaSerialPort.h
     osaSleep.h
     osaSocket.h
     osaSocketServer.h
     osaStopwatch.h
     osaThread.h
     osaThreadAdapter.h
     osaThreadBuddy.h
     osaThreadedLogFile.h
     osaThreadSignal.h
     osaTimeServer.h
     osaTripleBuffer.h
     )


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


# Create the config file
set (CISST_OSA_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstOSAbstraction/osaConfig.h)
configure_file (${cisstOSAbstractionLibs_SOURCE_DIR}/osaConfig.h.in
                ${CISST_OSA_CONFIG_FILE}
                @ONLY)
install_files (/include/cisstOSAbstraction
               ".h"
               ${CISST_OSA_CONFIG_FILE})

# Add the config file to the project
set_source_files_properties ("${CISST_OSA_CONFIG_FILE}"
                             PROPERTIES GENERATED TRUE)
set (ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES} ${CISST_OSA_CONFIG_FILE})


# Finally, create main library
cisst_add_library (
  LIBRARY cisstOSAbstraction
  DEPENDENCIES cisstCommon
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES}
  ADDITIONAL_HEADER_FILES ${ADDITIONAL_HEADER_FILES})

# for windows, needs WinSock
if (WIN32)
    set (CISST_WSOCK_LIBRARY WSOCK32)
    target_link_libraries (cisstOSAbstraction ${CISST_WSOCK_LIBRARY})
    cisst_set_package_settings (cisstOSAbstraction WinSock LIBRARIES CISST_WSOCK_LIBRARY)
endif (WIN32)


# QNX does not require rt library for clock_gettime (contained in libc)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  # QNX requires socket library
  target_link_libraries (cisstOSAbstraction ${CISST_SOCKET_LIBRARY})
else ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  if (UNIX AND NOT APPLE)
    # clock_gettime requires linking with librt
    target_link_libraries (cisstOSAbstraction ${CISST_RT_LIBRARY})
  endif (UNIX AND NOT APPLE)
endif ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
