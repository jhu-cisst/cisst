#
# $Id: CMakeLists.txt 3331 2012-01-10 22:34:09Z adeguet1 $
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

set(REQUIRED_CISST_LIBRARIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    cisstRobot
    cisstNumerical)

cisst_set_directories (${REQUIRED_CISST_LIBRARIES})

add_executable (cisstRobotSensorCalibration RobotSensorCalibration.cpp)
set_property (TARGET cisstRobotSensorCalibration PROPERTY FOLDER "cisstRobot/applications")
cisst_target_link_libraries (cisstRobotSensorCalibration ${REQUIRED_CISST_LIBRARIES})
