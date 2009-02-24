#
# $Id$
#
# CMakeLists for cisstImage tests
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
    imgPixelTypesTest.cpp
    )

# all header files
SET(HEADER_FILES
    imgPixelTypesTest.h
    )

# Add executable for C++ tests
ADD_EXECUTABLE(cisstImageTests ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(cisstImageTests cisstTestsMain)
CISST_REQUIRES("cisstImageTests" "cisstCommon;cisstVector;cisstImage")

# To generate a CTest list of tests
CISST_ADD_TESTS(cisstImageTests)

