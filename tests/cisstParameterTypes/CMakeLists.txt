#
# $Id$
#
# CMakeLists for cisstParameterTypes Tests
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
     prmEventButtonTest.cpp
     prmPositionCartesianGetTest.cpp
     prmPositionJointGetTest.cpp
     prmVelocityCartesianGetTest.cpp
     prmVelocityJointGetTest.cpp
     prmForceCartesianGetTest.cpp
     prmTransformationManagerTests.cpp
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
     )

# Add executable for C++ tests
add_executable (cisstParameterTypesTests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries (cisstParameterTypesTests cisstTestsMain)
cisst_target_link_libraries (cisstParameterTypesTests cisstParameterTypes cisstVector cisstCommon cisstMultiTask)

# To generate a CTest list of tests
cisst_add_test (cisstParameterTypesTests ITERATIONS 1 INSTANCES 1)
