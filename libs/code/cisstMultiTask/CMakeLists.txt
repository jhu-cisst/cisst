#
# $Id$
#
# CMakeLists for cisstMultiTask
#
# (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
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
SET(LIBRARY cisstMultiTask)

# set dependencies
SET(DEPENDENCIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    )

# all source files
SET(SOURCE_FILES
    mtsVector.cpp

    mtsMulticastCommandVoid.cpp
    mtsMulticastCommandWriteBase.cpp

    mtsFunctionVoid.cpp
    mtsFunctionReadOrWrite.cpp
    mtsFunctionQualifiedReadOrWrite.cpp

    mtsCommandQueuedVoidBase.cpp
    mtsCommandQueuedWriteBase.cpp

    mtsDeviceInterface.cpp
    mtsDevice.cpp

    mtsMailBox.cpp
    mtsStateIndex.cpp
    mtsStateTable.cpp

    mtsTaskInterface.cpp
    mtsRequiredInterface.cpp
    mtsTask.cpp
    mtsTaskContinuous.cpp
    mtsTaskPeriodic.cpp
    mtsTaskFromCallback.cpp

    mtsTaskManager.cpp
    )

# all header files
SET(HEADER_FILES
    mtsVector.h
    mtsMap.h

    mtsCommandBase.h
    mtsFunctionBase.h

    mtsCommandVoid.h
    mtsCommandVoidBase.h
    mtsMulticastCommandVoid.h
    mtsCommandQueuedVoid.h
    mtsCommandQueuedVoidBase.h
    mtsFunctionVoid.h

    mtsCommandRead.h
    mtsCommandWrite.h
    mtsCommandReadOrWriteBase.h
    mtsMulticastCommandWrite.h
    mtsMulticastCommandWriteBase.h
    mtsCommandQueuedWrite.h
    mtsCommandQueuedWriteBase.h
    mtsFunctionReadOrWrite.h

    mtsCommandQualifiedRead.h
    mtsCommandQualifiedReadOrWriteBase.h
    mtsFunctionQualifiedReadOrWrite.h

    mtsExport.h
    mtsForwardDeclarations.h

    mtsDeviceInterface.h
    mtsDevice.h

    mtsQueue.h
    mtsMailBox.h
    mtsStateIndex.h

    mtsStateArray.h
    mtsStateArrayBase.h
    mtsStateTable.h
    mtsStateData.h

    mtsTaskInterface.h
    mtsRequiredInterface.h
    mtsTask.h
    mtsTaskContinuous.h
    mtsTaskPeriodic.h
    mtsTaskFromCallback.h
    
    mtsTaskManager.h
    )


CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

