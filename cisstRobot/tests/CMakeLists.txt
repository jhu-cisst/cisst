#
#
# (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# paths for headers/libraries
cisst_set_directories (cisstCommon cisstOSAbstraction cisstVector cisstNumerical cisstParameterTypes cisstRobot cisstTestsDriver)

if( CISST_cisstNumerical AND CISST_HAS_CISSTNETLIB )

  # all source files
  set (SOURCE_FILES
       robDHTest.cpp
       robManipulatorTest.cpp
  #     robMassTest.cpp
       robRobotsKinematics.cpp
       )

  # all header files
  set (HEADER_FILES
       robDHTest.h
       robManipulatorTest.h
  #     robMassTest.h
       robRobotsKinematics.h
       )

  # Add executable for C++ tests
  add_executable (cisstRobotTests ${SOURCE_FILES} ${HEADER_FILES})
  set_property (TARGET cisstRobotTests PROPERTY FOLDER "cisstRobot/tests")
  target_link_libraries (cisstRobotTests cisstTestsDriver)
  cisst_target_link_libraries (cisstRobotTests cisstCommon cisstVector cisstOSAbstraction cisstRobot cisstTestsDriver)

  # To generate a CTest list of tests
  cisst_add_test (cisstRobotTests ITERATIONS 10 INSTANCES 2)

endif( CISST_cisstNumerical AND CISST_HAS_CISSTNETLIB )
