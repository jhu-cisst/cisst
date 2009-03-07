#
# $Id$
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

IF(CISST_SVL_HAS_X11 OR WIN32)

    ADD_EXECUTABLE(stereoTutorialStereoCameraViewer stereocameraviewer.cpp)

    IF(UNIX)
        # Link each executable against the X11 library
        FIND_PACKAGE(X11 REQUIRED)
        TARGET_LINK_LIBRARIES(stereoTutorialStereoCameraViewer ${X11_LIBRARIES})

        IF(CISST_SVL_HAS_LIBDC1394)
            # Also link the examples using live camera source against the DC1394 library if enabled
            TARGET_LINK_LIBRARIES(stereoTutorialStereoCameraViewer dc1394)
        ENDIF(CISST_SVL_HAS_LIBDC1394)
    ENDIF(UNIX)

    IF(CISST_SVL_HAS_OPENCV)
        # Link each example against the OpenCV library if enabled
        FIND_PACKAGE(OpenCV)
        INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
        LINK_DIRECTORIES(${OpenCV_LINK_DIRECTORIES})
        TARGET_LINK_LIBRARIES(stereoTutorialStereoCameraViewer ${OpenCV_LIBRARIES})
    ENDIF(CISST_SVL_HAS_OPENCV)

    IF(WIN32)
        ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
        set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib")
    ENDIF(WIN32)

    CISST_REQUIRES("stereoTutorialStereoCameraViewer" "cisstCommon;cisstVector;cisstOSAbstraction;cisstImage;cisstStereoVision")

ENDIF(CISST_SVL_HAS_X11 OR WIN32)

