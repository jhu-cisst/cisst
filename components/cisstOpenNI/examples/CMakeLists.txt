## OPENNI - KINECT EXAMPLES
project( OpenNI )

# KINECT_TEST
add_executable(cniExKinectTest                          cniExKinectTest.cpp )

target_link_libraries(cniExKinectTest                   cisstOpenNI )
  
cisst_target_link_libraries(cniExKinectTest             cisstCommon
                                                        cisstVector 
                                                        cisstOSAbstraction 
                                                        cisstMultiTask
                                                        cisstDevices )

# KINECT_HAND_TRACKER
add_executable(cniExKinectHandTracker                   cniExKinectHandTracker.cpp )

target_link_libraries(cniExKinectHandTracker            cisstOpenNI )
  
cisst_target_link_libraries(cniExKinectHandTracker      cisstCommon
                                                        cisstVector 
                                                        cisstOSAbstraction 
                                                        cisstMultiTask
                                                        cisstDevices ) 
