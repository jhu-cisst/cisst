# See license at http://www.cisst.org/cisst/license.txt


PROJECT(svlTutorial1)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(cisst REQUIRED)

IF(cisst_FOUND)

    INCLUDE(${CISST_USE_FILE})

    ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)

    CISST_TARGET_LINK_LIBRARIES(${PROJECT_NAME} cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)

ENDIF(cisst_FOUND)

