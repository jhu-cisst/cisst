#
# CMakeLists for cisstMultiTask tests
#
# (C) Copyright 2008-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# paths for headers/libraries
cisst_set_directories (cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstTestsDriver)

# test library
add_library (cisstMultiTaskTestsLib
             ${IS_SHARED}
             mtsTestExport.h
             mtsMacrosTestClasses.h
             mtsMacrosTestClasses.cpp
             mtsTestComponents.h
             mtsTestComponents.cpp)
set_property (TARGET cisstMultiTaskTestsLib PROPERTY FOLDER "cisstMultiTask/tests")
cisst_target_link_libraries (cisstMultiTaskTestsLib cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

# all source files
set (SOURCE_FILES
     mtsCollectorStateTest.cpp
     mtsCommandAndEventLocalTest.cpp
     mtsComponentStateTest.cpp
     mtsQueueTest.cpp
     mtsStateTableTest.cpp
     mtsTaskTest.cpp
     mtsTypesTest.cpp
     mtsVectorTest.cpp
     mtsFixedSizeVectorTest.cpp
     mtsMacrosTest.cpp
     mtsMatrixTest.cpp
     mtsSerializationTest.cpp
     mtsSocketProxyTest.cpp
     mtsManagerLocalTest.cpp
     )

# all header files
set (HEADER_FILES
     mtsComponentStateTest.h
     mtsCommandAndEventLocalTest.h
     mtsComponentStateTest.h
     mtsQueueTest.h
     mtsStateTableTest.h
     mtsTaskTest.h
     mtsTypesTest.h
     mtsVectorTest.h
     mtsFixedSizeVectorTest.h
     mtsMacrosTest.h
     mtsMatrixTest.h
     mtsSerializationTest.h
     mtsSocketProxyTest.h
     mtsManagerLocalTest.h
    )

# to find executables generated, provide internal configuration
add_definitions(-DCMAKE_CFG_INTDIR_WITH_QUOTES="${CMAKE_CFG_INTDIR}")

# add executable for C++ tests
add_executable (cisstMultiTaskTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstMultiTaskTests PROPERTY FOLDER "cisstMultiTask/tests")
target_link_libraries (cisstMultiTaskTests cisstTestsDriver cisstMultiTaskTestsLib)
cisst_target_link_libraries (cisstMultiTaskTests cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstTestsDriver)


# to generate a CTest list of tests
cisst_add_test (cisstMultiTaskTests ITERATIONS 1 INSTANCES 1)

# To find generated executables
set_target_properties (cisstMultiTaskTests
                       PROPERTIES COMPILE_DEFINITIONS "CISST_BINARY_DIR=\"${cisst_BINARY_DIR}/\"")
