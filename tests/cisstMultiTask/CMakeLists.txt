#
# $Id$
#
# CMakeLists for cisstMultiTask tests
#
# (C) Copyright 2008-2010 Johns Hopkins University (JHU), All Rights
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
add_library (cisstMultiTasksTestsLib
             mtsMacrosTestClasses.h
             mtsMacrosTestClasses.cpp)

cisst_target_link_libraries (cisstMultiTasksTestsLib cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

# all source files
set (SOURCE_FILES
     mtsCollectorBaseTest.cpp
     mtsCommandAndEventTest.cpp
     mtsComponentStateTest.cpp
     mtsQueueTest.cpp
     mtsStateTableTest.cpp
     mtsTaskTest.cpp
     mtsVectorTest.cpp
     mtsFixedSizeVectorTest.cpp
     mtsMacrosTest.cpp
     mtsMatrixTest.cpp
     mtsSerializationTest.cpp
     mtsManagerGlobalTest.cpp
     mtsManagerLocalTest.cpp
     )

# all header files
set (HEADER_FILES
     mtsCollectorBaseTest.h
     mtsCommandAndEventTest.h
     mtsComponentStateTest.h
     mtsQueueTest.h
     mtsStateTableTest.h
     mtsTaskTest.h
     mtsVectorTest.h
     mtsFixedSizeVectorTest.h
     mtsMacrosTest.h
     mtsMatrixTest.h
     mtsSerializationTest.h
     mtsTestComponents.h
     mtsManagerGlobalTest.h
     mtsManagerLocalTest.h
    )

# Add executable for C++ tests
add_executable (cisstMultiTaskTests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries (cisstMultiTaskTests cisstTestsMain cisstMultiTasksTestsLib)
cisst_target_link_libraries (cisstMultiTaskTests cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

# To generate a CTest list of tests
cisst_add_test (cisstMultiTaskTests ITERATIONS 1 INSTANCES 1)
