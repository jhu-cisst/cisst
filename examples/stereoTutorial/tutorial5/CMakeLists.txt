# See license at http://www.cisst.org/cisst/license.txt


PROJECT(svlTutorial5)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(cisst REQUIRED)

IF(cisst_FOUND)

    INCLUDE(${CISST_USE_FILE})

    ADD_EXECUTABLE(${PROJECT_NAME}
                   CMyEventHandler2.h
                   CMyEventHandler2.cpp
                   main.cpp
                   )

    CISST_TARGET_LINK_LIBRARIES(${PROJECT_NAME} cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)

    # Copy media files to the build directory
    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/wall.bmp ${CMAKE_BINARY_DIR}/wall.bmp COPYONLY)
    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/xray.avi ${CMAKE_BINARY_DIR}/xray.avi COPYONLY)

ENDIF(cisst_FOUND)

