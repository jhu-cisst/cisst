#
# $Id$
#
# CMakeLists for cisstOSAbstraction
#
# (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

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
     osaTripleBuffer.cpp
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


# for windows, needs WinSock
if (WIN32)
    set (CISST_WSOCK_LIBRARY WSOCK32)
    target_link_libraries (cisstOSAbstraction ${CISST_WSOCK_LIBRARY})
    cisst_add_to_internal (CISST_ADDITIONAL_LIBRARIES CISST_WSOCK_LIBRARY)
endif (WIN32)


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
  target_link_libraries (cisstOSAbstraction ${CISST_SOCKET_LIBRARY})
  cisst_add_to_internal (CISST_ADDITIONAL_LIBRARIES CISST_SOCKET_LIBRARY)
else ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  if (UNIX AND NOT APPLE)
    # clock_gettime requires linking with librt
    set (CISST_RT_LIBRARY rt)
    target_link_libraries (cisstOSAbstraction ${CISST_RT_LIBRARY})
    cisst_add_to_internal (CISST_ADDITIONAL_LIBRARIES CISST_RT_LIBRARY)
  endif (UNIX AND NOT APPLE)
endif ("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")


# For atomic operations with gcc
include (CheckCSourceCompiles)
check_c_source_compiles (
"int main(int argc, char * argv[]) {
     int * p1; int * p2; int * p3; int * p4;
     p1 = __sync_val_compare_and_swap(&p2, p3, p4);
     return 1;
 }"
 CISST_OSA_HAS_sync_val_compare_and_swap)


# Create the config file
set (CISST_OSA_CONFIG_FILE ${cisst_BINARY_DIR}/libs/include/cisstOSAbstraction/osaConfig.h)
configure_file (${cisst_SOURCE_DIR}/libs/include/cisstOSAbstraction/osaConfig.h.in
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


# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstOSAbstraction
                        INTERFACE_DIRECTORY ../../include/cisstOSAbstraction
			CISST_LIBRARIES cisstCommon cisstOSAbstraction)
endif (CISST_HAS_SWIG_PYTHON)
