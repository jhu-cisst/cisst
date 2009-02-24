#
# $Id$
#
# CMakeLists for cisstOSAbstraction tests
#
# (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
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
SET(SOURCE_FILES
    osaMutexTest.cpp
    osaThreadTest.cpp
    )

# all header files
SET(HEADER_FILES
    osaMutexTest.h
    osaThreadTest.h
    )

# Add executable for C++ tests
ADD_EXECUTABLE(cisstOSAbstractionTests ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(cisstOSAbstractionTests cisstTestsMain )
CISST_REQUIRES("cisstOSAbstractionTests" "cisstCommon;cisstOSAbstraction")

# Threads
FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(cisstOSAbstractionTests ${CMAKE_THREAD_LIBS_INIT})

# To generate a CTest list of tests
CISST_ADD_TESTS(cisstOSAbstractionTests)

