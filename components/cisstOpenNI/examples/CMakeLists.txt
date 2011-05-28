

project( OpenNI )

add_executable( KinectTest KinectTest.cpp )

target_link_libraries( KinectTest cisstOpenNI )
  
cisst_target_link_libraries( 
  KinectTest 
  cisstCommon
  cisstVector 
  cisstOSAbstraction 
  cisstMultiTask
  cisstDevices )
  
