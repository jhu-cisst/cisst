#
#
# CMakeLists for cisstOSAbstraction tests
#
# (C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# paths for headers/libraries
cisst_set_directories (cisstCommon cisstOSAbstraction cisstTestsDriver)

# all source files
set (SOURCE_FILES
     osaMutexTest.cpp
     osaPipeExecTest.cpp
     osaSocketTest.cpp
     osaTimeServerTest.cpp
     osaThreadTest.cpp
     osaThreadSignalTest.cpp
     osaTripleBufferTest.cpp
     )

# all header files
set (HEADER_FILES
     osaMutexTest.h
     osaPipeExecTest.h
     osaSocketTest.h
     osaTimeServerTest.h
     osaThreadTest.h
     osaThreadSignalTest.h
     osaTripleBufferTest.h
     )

# add executable for C++ tests
add_executable (cisstOSAbstractionTestsPipeExecUtility osaPipeExecUtility.cpp)
set_property (TARGET cisstOSAbstractionTestsPipeExecUtility PROPERTY FOLDER "cisstOSAbstraction/tests")

# to find executables generated, provide internal configuration
add_definitions(-DCMAKE_CFG_INTDIR_WITH_QUOTES="${CMAKE_CFG_INTDIR}")
add_executable (cisstOSAbstractionTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstOSAbstractionTests PROPERTY FOLDER "cisstOSAbstraction/tests")
target_link_libraries (cisstOSAbstractionTests cisstTestsDriver )
cisst_target_link_libraries (cisstOSAbstractionTests cisstCommon cisstOSAbstraction cisstTestsDriver)
add_dependencies(cisstOSAbstractionTests cisstOSAbstractionTestsPipeExecUtility)

# threads
find_package (Threads REQUIRED)
target_link_libraries (cisstOSAbstractionTests ${CMAKE_THREAD_LIBS_INIT})

# to generate a CTest list of tests
cisst_add_test (cisstOSAbstractionTests INSTANCES 2 ITERATIONS 2)

# To find generated executables
set_target_properties (cisstOSAbstractionTests
                       PROPERTIES COMPILE_DEFINITIONS "CISST_BINARY_DIR=\"${cisst_BINARY_DIR}/\"")
