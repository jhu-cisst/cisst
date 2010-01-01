#
# $Id$
#
# CMakeLists for cisstCommon tests
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

# all source files
SET(SOURCE_FILES
    cmnClassRegisterTest.cpp
    cmnObjectRegisterTest.cpp
    cmnPathTest.cpp
    cmnPortabilityTest.cpp
    cmnRequiresDeepCopyTest.cpp
    cmnTypeTraitsTest.cpp
    )

# all header files
SET(HEADER_FILES
    cmnClassRegisterTest.h
    cmnObjectRegisterTest.h
    cmnPathTest.h
    cmnPortabilityTest.h
    cmnPortabilityTestCMake.h.in
    cmnRequiresDeepCopyTest.h
    cmnTypeTraitsTest.h
    )

# Add executable for C++ tests
ADD_EXECUTABLE(cisstCommonTests ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(cisstCommonTests cisstTestsMain)
CISST_REQUIRES("cisstCommonTests" "cisstCommon")

# Add a static library with some classes registered
ADD_LIBRARY(cmnClassRegisterTestStatic
            STATIC
            cmnClassRegisterTestStatic.cpp cmnClassRegisterTestStatic.h)
TARGET_LINK_LIBRARIES(cisstCommonTests cmnClassRegisterTestStatic)

# Add a dynamic library with some classes registered
IF(NOT CISST_BUILD_SHARED_LIBS)
  MESSAGE("Info: Since the cisst libraries are compiled statically, the tests for class registration based on dynamically exported classes will be skipped.")
ELSE(NOT CISST_BUILD_SHARED_LIBS)
  ADD_LIBRARY(cmnClassRegisterTestDynamic
              SHARED
              cmnClassRegisterTestDynamic.cpp cmnClassRegisterTestDynamic.h)
  CISST_REQUIRES("cmnClassRegisterTestDynamic" "cisstCommon")
  TARGET_LINK_LIBRARIES(cisstCommonTests cmnClassRegisterTestDynamic)
ENDIF(NOT CISST_BUILD_SHARED_LIBS)

# To generate a CTest list of tests
CISST_ADD_TESTS(cisstCommonTests)

