#
# $Id: CMakeLists.txt 646 2009-08-10 14:14:35Z bvagvol1 $
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

    ADD_EXECUTABLE(stereoTutorialSimpleStream simplestream.cpp)

    CISST_REQUIRES("stereoTutorialSimpleStream" "cisstCommon;cisstVector;cisstOSAbstraction;cisstStereoVision")

ENDIF(CISST_SVL_HAS_X11 OR WIN32)

