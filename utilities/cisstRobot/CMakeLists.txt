project( cisstRobotUtilities )

set( PROJECT_DEPENDENCIES 
  cisstCommon 
  cisstVector 
  cisstOSAbstraction
  cisstRobot
  cisstNumerical )

add_executable( RandomJoints RandomJoints.cpp )
cisst_target_link_libraries( RandomJoints ${PROJECT_DEPENDENCIES} )

add_executable( ForwardKinematics ForwardKinematics.cpp )
cisst_target_link_libraries( ForwardKinematics ${PROJECT_DEPENDENCIES} )

add_executable( KinematicsIdentification KinematicsIdentification.cpp )
cisst_target_link_libraries( KinematicsIdentification ${PROJECT_DEPENDENCIES} )





