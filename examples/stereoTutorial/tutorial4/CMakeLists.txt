# See license at http://www.cisst.org/cisst/license.txt


PROJECT(svlTutorial4)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(cisst REQUIRED)

IF(cisst_FOUND)

    INCLUDE(${CISST_USE_FILE})

    ADD_EXECUTABLE(${PROJECT_NAME}
                   CMyFilter2.h
                   CMyFilter2.cpp
                   CMyFilter3.h
                   CMyFilter3.cpp
                   main.cpp
                   )

    CISST_TARGET_LINK_LIBRARIES(${PROJECT_NAME} cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)

ENDIF(cisst_FOUND)

