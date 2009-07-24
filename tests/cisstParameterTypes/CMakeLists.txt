#
# $Id$
#
# CMakeLists for cisstParameterTypes Tests
#
# (C) Copyright 2003-2008 Johns Hopkins University (JHU), All Rights
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
    prmEventButtonTest.cpp
    prmPositionCartesianGetTest.cpp
    prmPositionJointGetTest.cpp
    prmTransformationManagerTests.cpp
    )

# all header files
SET(HEADER_FILES
    prmSetAndTestGenericObjectSerialization.h
    prmTestGenericObjectConstructor.h
    prmEventButtonTest.h
    prmPositionCartesianGetTest.h
    prmPositionJointGetTest.h
    prmTransformationManagerTests.h
    )

# Add executable for C++ tests
ADD_EXECUTABLE(cisstParameterTypesTests ${SOURCE_FILES} ${HEADER_FILES})

TARGET_LINK_LIBRARIES(cisstParameterTypesTests cisstTestsMain)

CISST_REQUIRES("cisstParameterTypesTests" "cisstParameterTypes;cisstVector;cisstCommon;cisstMultiTask")

# To generate a CTest list of tests
CISST_ADD_TESTS(cisstParameterTypesTests)
