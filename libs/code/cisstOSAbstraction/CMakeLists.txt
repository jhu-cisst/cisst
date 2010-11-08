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
SET(SOURCE_FILES
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
SET(HEADER_FILES
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
    )

cisst_add_library (
  LIBRARY cisstOSAbstraction
  DEPENDENCIES cisstCommon
  SOURCE_FILES ${SOURCE_FILES}
  HEADER_FILES ${HEADER_FILES})

IF(WIN32)
    SET(CISST_WSOCK_LIBRARY WSOCK32)
    TARGET_LINK_LIBRARIES(cisstOSAbstraction ${CISST_WSOCK_LIBRARY})
    CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES CISST_WSOCK_LIBRARY)
ENDIF(WIN32)

IF(CISST_HAS_LINUX_RTAI)
  # Check for existence of query_module, which was removed in kernel
  # versions 2.6+ (and possibly some earlier ones).
  INCLUDE(CheckFunctionExists)
  CHECK_FUNCTION_EXISTS("query_module" CMAKE_HAVE_QUERY_MODULE)
  IF (CMAKE_HAVE_QUERY_MODULE)
    ADD_DEFINITIONS(-DCMAKE_HAVE_QUERY_MODULE)
  ENDIF (CMAKE_HAVE_QUERY_MODULE)
ENDIF(CISST_HAS_LINUX_RTAI)

# QNX does not require rt library for clock_gettime (contained in libc)
IF("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  # QNX requires socket library
  SET(CISST_SOCKET_LIBRARY socket)
  TARGET_LINK_LIBRARIES(cisstOSAbstraction ${CISST_SOCKET_LIBRARY})
  CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES CISST_SOCKET_LIBRARY)
ELSE("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  IF(UNIX AND NOT APPLE)
    # clock_gettime requires linking with librt
    SET(CISST_RT_LIBRARY rt)
    TARGET_LINK_LIBRARIES(cisstOSAbstraction ${CISST_RT_LIBRARY})
    CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES CISST_RT_LIBRARY)
  ENDIF(UNIX AND NOT APPLE)
ENDIF("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module(MODULE cisstOSAbstraction
                        INTERFACE_DIRECTORY ../../include/cisstOSAbstraction
			CISST_LIBRARIES cisstCommon cisstOSAbstraction)
endif (CISST_HAS_SWIG_PYTHON)
