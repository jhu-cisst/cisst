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

    CISST_REQUIRES("stereoTutorialVideoPlayer" "cisstCommon;cisstVector;cisstOSAbstraction;cisstStereoVision")

ENDIF(WIN32)

IF(CISST_SVL_HAS_X11 AND CISST_SVL_HAS_ZLIB)

    ADD_EXECUTABLE(stereoTutorialVideoPlayer videoplayer.cpp)

    CISST_REQUIRES("stereoTutorialVideoPlayer" "cisstCommon;cisstVector;cisstOSAbstraction;cisstStereoVision")

ENDIF(CISST_SVL_HAS_X11 AND CISST_SVL_HAS_ZLIB)
