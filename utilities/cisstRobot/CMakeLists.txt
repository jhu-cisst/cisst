#
# $Id: CMakeLists.txt 2722 2011-07-08 11:45:49Z sleonar7 $
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

project( cisstRobotUtilities )

set( PROJECT_DEPENDENCIES
  cisstCommon
  cisstVector
  cisstOSAbstraction
  cisstRobot
  cisstNumerical )

cisst_set_directories (${PROJECT_DEPENDENCIES})

add_executable( RandomJoints RandomJoints.cpp )
cisst_target_link_libraries( RandomJoints ${PROJECT_DEPENDENCIES} )

add_executable( ForwardKinematics ForwardKinematics.cpp )
cisst_target_link_libraries( ForwardKinematics ${PROJECT_DEPENDENCIES} )

add_executable( KinematicsIdentification KinematicsIdentification.cpp )
cisst_target_link_libraries( KinematicsIdentification ${PROJECT_DEPENDENCIES} )





