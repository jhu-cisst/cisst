#
# (C) Copyright 2013-2021 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---



# create a list of required cisst libraries
set (REQUIRED_CISST_LIBRARIES
     cisstCommon
     cisstVector
     cisstOSAbstraction
     cisstNumerical
     cisstRobot)

find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)
  include (${CISST_USE_FILE})

  add_executable (robExManipulator example.cpp)
  set_property (TARGET robExManipulator PROPERTY FOLDER "cisstRobot/examples")
  cisst_target_link_libraries (robExManipulator ${REQUIRED_CISST_LIBRARIES})

  add_executable (robExLinearRn mainLinearRn.cpp)
  set_property (TARGET robExLinearRn PROPERTY FOLDER "cisstRobot/examples")
  cisst_target_link_libraries (robExLinearRn ${REQUIRED_CISST_LIBRARIES})

  add_executable (robExQuinticRn mainQuinticRn.cpp)
  set_property (TARGET robExQuinticRn PROPERTY FOLDER "cisstRobot/examples")
  cisst_target_link_libraries (robExQuinticRn ${REQUIRED_CISST_LIBRARIES})

  add_executable (robExLSPB mainLSPB.cpp)
  set_property (TARGET robExLSPB PROPERTY FOLDER "cisstRobot/examples")
  cisst_target_link_libraries (robExLSPB ${REQUIRED_CISST_LIBRARIES})

  # add_executable (robExReflexxes mainReflexxes.cpp)
  # set_property (TARGET robExReflexxes PROPERTY FOLDER "cisstRobot/examples")
  # cisst_target_link_libraries (robExReflexxes ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires " ${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)
