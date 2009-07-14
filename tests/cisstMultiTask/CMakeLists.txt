#
# $Id: CMakeLists.txt mjung5 $
#
# CMakeLists for cisstMultiTask tests
#
# (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
# 
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

# test library
add_library(cisstMultiTasksTestsLib
            mtsMacrosTestClasses.h
	    mtsMacrosTestClasses.cpp)

CISST_REQUIRES(cisstMultiTasksTestsLib "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask")

# all source files
SET(SOURCE_FILES
    mtsCollectorBaseTest.cpp
    mtsStateTableTest.cpp
    mtsTaskManagerTest.cpp
    mtsTaskTest.cpp
    mtsVectorTest.cpp
    mtsMacrosTest.cpp
    mtsMatrixTest.cpp
    )

# all header files
SET(HEADER_FILES
    mtsCollectorBaseTest.h
    mtsStateTableTest.h
    mtsTaskManagerTest.h
    mtsTaskTest.h
    mtsVectorTest.h
    mtsMacrosTest.h
    mtsMatrixTest.h
    )

# Add executable for C++ tests
ADD_EXECUTABLE(cisstMultiTaskTests ${SOURCE_FILES} ${HEADER_FILES})

TARGET_LINK_LIBRARIES(cisstMultiTaskTests cisstTestsMain cisstMultiTasksTestsLib)

CISST_REQUIRES("cisstMultiTaskTests" "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask")

# To generate a CTest list of tests
CISST_ADD_TESTS(cisstMultiTaskTests)
