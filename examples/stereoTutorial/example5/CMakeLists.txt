#
# $Id: $
#
# (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

IF(WIN32)

    ADD_EXECUTABLE(stereoTutorialVideoPlayer videoplayer.cpp)

    IF(CISST_SVL_HAS_OPENCV)
        # Link each example against the OpenCV library if enabled
        FIND_PACKAGE(OpenCV)
        INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
        LINK_DIRECTORIES(${OpenCV_LINK_DIRECTORIES})
        TARGET_LINK_LIBRARIES(stereoTutorialVideoPlayer ${OpenCV_LIBRARIES})
    ENDIF(CISST_SVL_HAS_OPENCV)

    ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib")

    CISST_REQUIRES("stereoTutorialVideoPlayer" "cisstCommon;cisstVector;cisstOSAbstraction;cisstStereoVision")

ENDIF(WIN32)

IF(CISST_SVL_HAS_X11 AND CISST_SVL_HAS_ZLIB)

    ADD_EXECUTABLE(stereoTutorialVideoPlayer videoplayer.cpp)

    IF(CISST_SVL_HAS_OPENCV)
        # Link each example against the OpenCV library if enabled
        FIND_PACKAGE(OpenCV)
        INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
        LINK_DIRECTORIES(${OpenCV_LINK_DIRECTORIES})
        TARGET_LINK_LIBRARIES(stereoTutorialVideoPlayer ${OpenCV_LIBRARIES})
    ENDIF(CISST_SVL_HAS_OPENCV)

    # Link each executable against the X11 library
    FIND_PACKAGE(X11 REQUIRED)
    TARGET_LINK_LIBRARIES(stereoTutorialVideoPlayer ${X11_LIBRARIES})

    IF(CISST_SVL_HAS_LIBDC1394)
        # Also link the examples using live camera source against the DC1394 library if enabled
        TARGET_LINK_LIBRARIES(stereoTutorialVideoPlayer dc1394)
    ENDIF(CISST_SVL_HAS_LIBDC1394)

    CISST_REQUIRES("stereoTutorialVideoPlayer" "cisstCommon;cisstVector;cisstOSAbstraction;cisstStereoVision")

ENDIF(CISST_SVL_HAS_X11 AND CISST_SVL_HAS_ZLIB)
