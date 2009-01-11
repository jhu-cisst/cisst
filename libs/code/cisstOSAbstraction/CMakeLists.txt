#
# $Id: CMakeLists.txt,v 1.21 2009/01/05 08:24:44 pkaz Exp $
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
    osaDynamicLoader.cpp
    osaDynamicLoaderAndFactory.cpp
    osaGetTime.cpp
    osaMutex.cpp
    osaSerialPort.cpp
    osaSleep.cpp
    osaStopwatch.cpp
    osaThread.cpp
    osaThreadBuddy.cpp
    osaThreadSignal.cpp
    osaTimeServer.cpp
    )

# all header files
SET(HEADER_FILES
    osaDynamicLoader.h
    osaDynamicLoaderAndFactory.h
    osaExport.h
    osaGetTime.h
    osaMutex.h
    osaSerialPort.h
    osaSleep.h
    osaStopwatch.h
    osaThread.h
    osaThreadAdapter.h
    osaThreadBuddy.h
    osaThreadedLogFile.h
    osaThreadSignal.h
    osaTimeServer.h
    )

IF(UNIX AND NOT APPLE)
   # clock_gettime requires linking with librt
   LINK_LIBRARIES(rt)
ENDIF(UNIX AND NOT APPLE)

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

