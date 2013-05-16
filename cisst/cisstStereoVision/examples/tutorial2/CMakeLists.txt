# See license at http://www.cisst.org/cisst/license.txt


PROJECT(svlTutorial2)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(cisst REQUIRED)

IF(cisst_FOUND)

    INCLUDE(${CISST_USE_FILE})

    SET(LIBRARIES_NEEDED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)

    CISST_SET_DIRECTORIES(${LIBRARIES_NEEDED})

    ADD_EXECUTABLE(${PROJECT_NAME}
                   CMyEventHandler.h
                   CMyEventHandler.cpp
                   main.cpp
                   )

    CISST_TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES_NEEDED})

ENDIF(cisst_FOUND)

