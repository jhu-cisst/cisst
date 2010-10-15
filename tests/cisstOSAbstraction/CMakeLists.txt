#
# $Id$
#
# CMakeLists for cisstOSAbstraction tests
#
# (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
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
     osaMutexTest.cpp
     osaPipeTest.cpp
     osaSocketTest.cpp
     osaThreadTest.cpp
     )

# all header files
set (HEADER_FILES
     osaMutexTest.h
     osaPipeTest.h
     osaSocketTest.h
     osaThreadTest.h
     )

# add executable for C++ tests
add_executable (cisstOSAbstractionTestsPipeUtility osaPipeUtility.cpp)

add_executable (cisstOSAbstractionTests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries (cisstOSAbstractionTests cisstTestsMain )
cisst_target_link_libraries (cisstOSAbstractionTests cisstCommon cisstOSAbstraction)
add_dependencies(cisstOSAbstractionTests cisstOSAbstractionTestsPipeUtility)

# threads
find_package (Threads REQUIRED)
target_link_libraries (cisstOSAbstractionTests ${CMAKE_THREAD_LIBS_INIT})

# to generate a CTest list of tests
cisst_add_test (cisstOSAbstractionTests INSTANCES 2 ITERATIONS 2)
