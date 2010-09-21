#
# $Id$
#
# CMakeLists for cisstOSAbstraction
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
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
set (HEADER_FILES
     osaForwardDeclarations.h
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

cisst_add_library (LIBRARY cisstOSAbstraction
                   DEPENDENCIES cisstCommon
                   SOURCE_FILES ${SOURCE_FILES}
                   HEADER_FILES ${HEADER_FILES}
                   )


if (WIN32)
    set (CISST_WSOCK_LIBRARY WSOCK32)
    target_link_libraries (cisstOSAbstraction ${CISST_WSOCK_LIBRARY})
    cisst_add_to_internal (CISST_ADDITIONAL_LIBRARIES CISST_WSOCK_LIBRARY)
endif (WIN32)

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

# Python wrappers
if (CISST_HAS_SWIG_PYTHON)
  cisst_add_swig_module (MODULE cisstOSAbstraction
                         INTERFACE_DIRECTORY ../../include/cisstOSAbstraction
                         CISST_LIBRARIES cisstCommon cisstOSAbstraction)
endif (CISST_HAS_SWIG_PYTHON)
