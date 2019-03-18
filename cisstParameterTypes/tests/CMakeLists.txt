#
# CMakeLists for cisstParameterTypes Tests
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
cisst_set_directories (cisstCommon cisstOSAbstraction cisstVector cisstMultiTask cisstParameterTypes cisstTestsDriver)

# all source files
set (SOURCE_FILES
     prmEventButtonTest.cpp
     prmPositionCartesianGetTest.cpp
     prmPositionJointGetTest.cpp
     prmVelocityCartesianGetTest.cpp
     prmVelocityJointGetTest.cpp
     prmForceCartesianGetTest.cpp
     prmTransformationManagerTests.cpp
     prmOperatingStateTest.cpp
     )

# all header files
set (HEADER_FILES
     prmSetAndTestGenericObjectSerialization.h
     prmTestGenericObjectConstructor.h
     prmEventButtonTest.h
     prmPositionCartesianGetTest.h
     prmPositionJointGetTest.h
     prmVelocityCartesianGetTest.h
     prmVelocityJointGetTest.h
     prmForceCartesianGetTest.h
     prmTransformationManagerTests.h
     prmOperatingStateTest.h
     )

# CppUnit include directories
include_directories (${CPPUNIT_INCLUDE_DIR})

# Add executable for C++ tests
add_executable (cisstParameterTypesTests ${SOURCE_FILES} ${HEADER_FILES})
set_property (TARGET cisstParameterTypesTests PROPERTY FOLDER "cisstParameterTypes/tests")
target_link_libraries (cisstParameterTypesTests cisstTestsDriver)
cisst_target_link_libraries (cisstParameterTypesTests cisstParameterTypes cisstVector cisstCommon cisstMultiTask cisstTestsDriver)

# To generate a CTest list of tests
cisst_add_test (cisstParameterTypesTests ITERATIONS 1 INSTANCES 1)
