#
# $Id$
#
# (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
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

    set (CISST_LIBRARIES_NEEDED cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision)
    cisst_set_directories (${CISST_LIBRARIES_NEEDED})

    ADD_EXECUTABLE(svlExComputeStereo computestereo.cpp)

    # Copy example images to the build tree
    IF(WIN32)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tsukuba3.bmp
                    ${CMAKE_BINARY_DIR}/examples/stereoTutorial/example3/tsukuba3.bmp COPYONLY)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tsukuba4.bmp
                    ${CMAKE_BINARY_DIR}/examples/stereoTutorial/example3/tsukuba4.bmp COPYONLY)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/venus_l.bmp
                    ${CMAKE_BINARY_DIR}/examples/stereoTutorial/example3/venus_l.bmp COPYONLY)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/venus_r.bmp
                    ${CMAKE_BINARY_DIR}/examples/stereoTutorial/example3/venus_r.bmp COPYONLY)
    ELSE(WIN32)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tsukuba3.bmp
                    ${CMAKE_BINARY_DIR}/examples/bin/tsukuba3.bmp COPYONLY)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tsukuba4.bmp
                    ${CMAKE_BINARY_DIR}/examples/bin/tsukuba4.bmp COPYONLY)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/venus_l.bmp
                    ${CMAKE_BINARY_DIR}/examples/bin/venus_l.bmp COPYONLY)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/venus_r.bmp
                    ${CMAKE_BINARY_DIR}/examples/bin/venus_r.bmp COPYONLY)
    ENDIF(WIN32)

    cisst_target_link_libraries (svlExComputeStereo ${CISST_LIBRARIES_NEEDED})

ENDIF(CISST_SVL_HAS_X11 OR WIN32)

