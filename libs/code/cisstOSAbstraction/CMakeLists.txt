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

# set the library name
SET(LIBRARY cisstOSAbstraction)

# set dependencies
SET(DEPENDENCIES
    cisstCommon)

# all source files
SET(SOURCE_FILES
    osaClassServices.cpp
    osaCriticalSection.cpp
    osaDynamicLoader.cpp
    osaDynamicLoaderAndFactory.cpp
    osaGetTime.cpp
    osaMutex.cpp
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
    osaCriticalSection.h
    osaDynamicLoader.h
    osaDynamicLoaderAndFactory.h
    osaExport.h
    osaGetTime.h
    osaMutex.h
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

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

IF(WIN32)
    SET(CISST_WSOCK_LIBRARY WSOCK32)
    TARGET_LINK_LIBRARIES(cisstOSAbstraction ${CISST_WSOCK_LIBRARY})
    CISST_ADD_TO_INTERNAL(CISST_ADDITIONAL_LIBRARIES CISST_WSOCK_LIBRARY)
ENDIF(WIN32)

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
