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

project (cisstRobotUtilities)

set (REQUIRED_CISST_LIBRARIES
    cisstCommon
    cisstVector
    cisstOSAbstraction
    cisstRobot
    cisstNumerical)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  add_executable (cisstRandomJoints RandomJoints.cpp)
  cisst_target_link_libraries (cisstRandomJoints ${REQUIRED_CISST_LIBRARIES})

  add_executable (cisstForwardKinematics ForwardKinematics.cpp)
  cisst_target_link_libraries (cisstForwardKinematics ${REQUIRED_CISST_LIBRARIES})

  add_executable (cisstKinematicsIdentification KinematicsIdentification.cpp)
  cisst_target_link_libraries (cisstKinematicsIdentification ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)





