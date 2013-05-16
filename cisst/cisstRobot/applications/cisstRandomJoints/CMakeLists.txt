#
# $Id$
#
# (C) Copyright 2003-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (REQUIRED_CISST_LIBRARIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    cisstRobot
    cisstNumerical)

cisst_set_directories (${REQUIRED_CISST_LIBRARIES})

add_executable (cisstRandomJoints RandomJoints.cpp)
set_property (TARGET cisstRandomJoints PROPERTY FOLDER "cisstRobot/applications")
cisst_target_link_libraries (cisstRandomJoints ${REQUIRED_CISST_LIBRARIES})
