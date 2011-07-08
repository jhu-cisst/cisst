# See license at http://www.cisst.org/cisst/license.txt


PROJECT(svlTutorial3)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(cisst REQUIRED)

IF(cisst_FOUND)

    INCLUDE(${CISST_USE_FILE})

    ADD_EXECUTABLE(${PROJECT_NAME}
                   CMyFilter1.h
                   CMyFilter1.cpp
                   main.cpp
                   )

    CISST_TARGET_LINK_LIBRARIES(${PROJECT_NAME} cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)

ENDIF(cisst_FOUND)

