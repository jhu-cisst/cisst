#
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

IF(CISST_SVL_HAS_WEBGUI)
    ADD_EXECUTABLE(svlWebGUI webgui.cpp)

    CISST_REQUIRES("svlWebGUI" "cisstCommon;cisstVector;cisstMultiTask;cisstOSAbstraction;cisstStereoVision")
ENDIF(CISST_SVL_HAS_WEBGUI)

