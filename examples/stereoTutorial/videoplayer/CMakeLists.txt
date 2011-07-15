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

    ADD_EXECUTABLE(svlExVideoPlayer videoplayer.cpp)

    IF(CISST_HAS_QT)
        CISST_REQUIRES("svlExVideoPlayer" "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstStereoVisionQt")
    ELSE(CISST_HAS_QT)
        CISST_REQUIRES("svlExVideoPlayer" "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision")
    ENDIF(CISST_HAS_QT)

ENDIF(CISST_SVL_HAS_X11 OR WIN32)
