## OPENNI - KINECT EXAMPLES
project( OpenNI )

# KINECT_TEST #################################################################
add_executable(cniExKinectTest                          cniExKinectTest.cpp )

target_link_libraries(cniExKinectTest                   cisstOpenNI )
  
cisst_target_link_libraries(cniExKinectTest             cisstCommon
                                                        cisstVector 
                                                        cisstOSAbstraction 
                                                        cisstMultiTask )