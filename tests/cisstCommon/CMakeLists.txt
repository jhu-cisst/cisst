#
# $Id$
#
# CMakeLists for cisstCommon tests
#
# (C) Copyright 2003-2010 Johns Hopkins University (JHU), All Rights
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
     cmnClassRegisterTest.cpp
     cmnLogLoDTest.cpp
     cmnObjectRegisterTest.cpp
     cmnPathTest.cpp
     cmnPortabilityTest.cpp
     cmnRequiresDeepCopyTest.cpp
     cmnTypeTraitsTest.cpp
     )

# all header files
set (HEADER_FILES
     cmnClassRegisterTest.h
     cmnLogLoDTest.h
     cmnObjectRegisterTest.h
     cmnPathTest.h
     cmnPortabilityTest.h
     cmnPortabilityTestCMake.h.in
     cmnRequiresDeepCopyTest.h
     cmnTypeTraitsTest.h
    )

# tests that depends on external packages
if (CISST_HAS_XML)
  set (SOURCE_FILES ${SOURCE_FILES} cmnXMLPathTest.cpp)
  set (HEADER_FILES ${HEADER_FILES} cmnXMLPathTest.h)
endif (CISST_HAS_XML)

# Add executable for C++ tests
add_executable (cisstCommonTests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries (cisstCommonTests cisstTestsMain)
cisst_target_link_libraries (cisstCommonTests cisstCommon)

# Add a static library with some classes registered
add_library (cmnClassRegisterTestStatic
             STATIC
             cmnClassRegisterTestStatic.cpp cmnClassRegisterTestStatic.h)
target_link_libraries (cisstCommonTests cmnClassRegisterTestStatic)

# Add a dynamic library with some classes registered
if (NOT CISST_BUILD_SHARED_LIBS)
  message ("Info: Since the cisst libraries are compiled statically, the tests for class registration based on dynamically exported classes will be skipped.")
else (NOT CISST_BUILD_SHARED_LIBS)
  add_library (cmnClassRegisterTestDynamic
               SHARED
               cmnClassRegisterTestDynamic.cpp cmnClassRegisterTestDynamic.h)
  target_link_libraries (cisstCommonTests cmnClassRegisterTestDynamic)
  cisst_target_link_libraries (cmnClassRegisterTestDynamic cisstCommon)
endif (NOT CISST_BUILD_SHARED_LIBS)

# To generate a CTest list of tests
cisst_add_test (cisstCommonTests ITERATIONS 2 INSTANCES 2)

