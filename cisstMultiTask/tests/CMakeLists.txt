#
# CMakeLists for cisstMultiTask tests
#
# (C) Copyright 2008-2020 Johns Hopkins University (JHU), All Rights Reserved.
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
     # mtsManagerGlobalTest.cpp
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
     # mtsManagerGlobalTest.h
     mtsManagerLocalTest.h
    )

# add ICE related tests
if (CISST_MTS_HAS_ICE)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       mtsCommandAndEventNetworkedTest.cpp
      )
  set (HEADER_FILES
       ${HEADER_FILES}
       mtsCommandAndEventNetworkedTest.h
      )
endif (CISST_MTS_HAS_ICE)

# to find executables generated, provide internal configuration
add_definitions(-DCMAKE_CFG_INTDIR_WITH_QUOTES="${CMAKE_CFG_INTDIR}")

# add executable for C++ tests
add_executable (cisstMultiTaskTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstMultiTaskTests PROPERTY FOLDER "cisstMultiTask/tests")
target_link_libraries (cisstMultiTaskTests cisstTestsDriver cisstMultiTaskTestsLib)
cisst_target_link_libraries (cisstMultiTaskTests cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstTestsDriver)


# ICE related code
if (CISST_MTS_HAS_ICE)
  # create our own component manager with special IOs for pipe communication
  add_executable (cisstMultiTaskTestsComponentManager
                  mtsTestComponentManager.cpp)
  set_property (TARGET cisstMultiTaskTestsComponentManager PROPERTY FOLDER "cisstMultiTask/tests")
  cisst_target_link_libraries (cisstMultiTaskTestsComponentManager cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
  # create an empty process usable to dynamically create components and connect
  add_executable (cisstMultiTaskTestsProcess
                  mtsTestProcess.cpp)
  set_property (TARGET cisstMultiTaskTestsProcess PROPERTY FOLDER "cisstMultiTask/tests")
  cisst_target_link_libraries (cisstMultiTaskTestsProcess cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)
  # create a program to configure multiple processes to be tested, also triggers creation and connection of components
  add_executable (cisstMultiTaskTestsConfigurationManager
                  mtsTestConfigurationManager.cpp)
  set_property (TARGET cisstMultiTaskTestsConfigurationManager PROPERTY FOLDER "cisstMultiTask/tests")
  cisst_target_link_libraries (cisstMultiTaskTestsConfigurationManager cisstCommon cisstVector cisstOSAbstraction cisstMultiTask)

  # make sure these programs are compiled before the test program
  add_dependencies (cisstMultiTaskTests
                    cisstMultiTaskTestsComponentManager
                    cisstMultiTaskTestsProcess
                    cisstMultiTaskTestsConfigurationManager)
endif (CISST_MTS_HAS_ICE)


# to generate a CTest list of tests
cisst_add_test (cisstMultiTaskTests ITERATIONS 1 INSTANCES 1)

# To find generated executables
set_target_properties (cisstMultiTaskTests
                       PROPERTIES COMPILE_DEFINITIONS "CISST_BINARY_DIR=\"${cisst_BINARY_DIR}/\"")
