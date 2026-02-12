#
# CMakeLists for cisstCommon tests
#
# (C) Copyright 2003-2019 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# paths for headers/libraries
cisst_set_directories (cisstCommon cisstTestsDriver)

if (CISST_HAS_XML AND NOT ("${CISST_XML_LIB}" STREQUAL "QtXML"))
  set (INCLUDE_XML_TEST TRUE)
endif (CISST_HAS_XML AND NOT ("${CISST_XML_LIB}" STREQUAL "QtXML"))

# set path for XML if needed
if (INCLUDE_XML_TEST)
  cisst_set_directories (cisstCommonXML)
endif (INCLUDE_XML_TEST)

# Configure the OS test program
get_filename_component (CMAKE_C_COMPILER_NAME ${CMAKE_C_COMPILER} NAME)
get_filename_component (CMAKE_CXX_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmnPortabilityTestCMake.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/cmnPortabilityTestCMake.h
                @ONLY)
# Add the path for the includes
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# create data type using the data generator
cisst_data_generator (cisstCommonTests
                      ${CMAKE_CURRENT_BINARY_DIR}
                      ""
                      cmnDataGeneratorTestTypes.cdg)

# all source files
set (SOURCE_FILES
     cmnClassRegisterTest.cpp
     cmnCommandLineOptionsTest.cpp
     cmnDataFunctionsTest.cpp
     cmnDataFunctionsVectorTest.cpp
     cmnDataGeneratorTest.cpp
     cmnLoggerTest.cpp
     cmnLogLoDTest.cpp
     cmnObjectRegisterTest.cpp
     cmnPathTest.cpp
     cmnPortabilityTest.cpp
     cmnPrintfTest.cpp
     cmnRequiresDeepCopyTest.cpp
     cmnTypeTraitsTest.cpp
     )

# all header files
set (HEADER_FILES
     cmnClassRegisterTest.h
     cmnCommandLineOptionsTest.h
     cmnDataFunctionsTest.h
     cmnDataFunctionsVectorTest.h
     cmnDataGeneratorTest.h
     cmnLoggerTest.h
     cmnLogLoDTest.h
     cmnObjectRegisterTest.h
     cmnPathTest.h
     cmnPortabilityTest.h
     cmnPortabilityTestCMake.h.in
     cmnPrintfTest.h
     cmnRequiresDeepCopyTest.h
     cmnTypeTraitsTest.h
    )

# tests that depends on external packages
if (INCLUDE_XML_TEST)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       cmnXMLPathTest.cpp)
  set (HEADER_FILES
       ${HEADER_FILES}
       cmnXMLPathTest.h)
endif (INCLUDE_XML_TEST)

# JSON tests
if (CISST_HAS_JSON)
  set (SOURCE_FILES
       ${SOURCE_FILES}
       cmnDataFunctionsJSONTest.cpp)
  set (HEADER_FILES
       ${HEADER_FILES}
       cmnDataFunctionsJSONTest.h)
endif (CISST_HAS_JSON)

# Add executable for C++ tests
add_executable (cisstCommonTests ${SOURCE_FILES} ${HEADER_FILES} ${cisstCommonTests_CISST_DG_SRCS})
set_property (TARGET cisstCommonTests PROPERTY FOLDER "cisstCommon/tests")
cisst_target_link_libraries (cisstCommonTests cisstCommon cisstTestsDriver)

# Add a static library with some classes registered
add_library (cmnClassRegisterTestStatic
             STATIC
             cmnClassRegisterTestStatic.cpp cmnClassRegisterTestStatic.h)
if (CISST_HAS_JSON AND NOT JSON_USE_SYSTEM_VERSION)
  add_dependencies (cmnClassRegisterTestStatic cisstJSONExternal)
endif ()
set_property (TARGET cmnClassRegisterTestStatic PROPERTY FOLDER "cisstCommon/tests")
target_link_libraries (cisstCommonTests cmnClassRegisterTestStatic)

# Add a dynamic library with some classes registered
if (NOT CISST_BUILD_SHARED_LIBS)
  message ("Information: Since the cisst libraries are compiled statically, the tests for class registration based on dynamically exported classes will be skipped.")
else (NOT CISST_BUILD_SHARED_LIBS)
  add_library (cmnClassRegisterTestDynamic
               SHARED
               cmnClassRegisterTestDynamic.cpp cmnClassRegisterTestDynamic.h)
  set_property (TARGET cmnClassRegisterTestDynamic PROPERTY FOLDER "cisstCommon/tests")
  target_link_libraries (cisstCommonTests cmnClassRegisterTestDynamic)
  cisst_target_link_libraries (cmnClassRegisterTestDynamic cisstCommon)
endif (NOT CISST_BUILD_SHARED_LIBS)

# To generate a CTest list of tests
cisst_add_test (cisstCommonTests ITERATIONS 2 INSTANCES 2)

if (INCLUDE_XML_TEST)
  target_link_libraries (cisstCommonTests cisstCommonXML)
endif (INCLUDE_XML_TEST)

set_target_properties (cisstCommonTests
                       PROPERTIES COMPILE_DEFINITIONS "CISST_SOURCE_DIR=\"${cisst_SOURCE_DIR}/\"")
