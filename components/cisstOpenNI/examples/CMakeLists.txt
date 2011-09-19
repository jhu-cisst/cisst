## OPENNI - KINECT EXAMPLES
project( OpenNI )

# KINECT_TEST #################################################################
add_executable(cisstOpenNIExKinectTest                          cisstOpenNIExKinectTest.cpp )

target_link_libraries(cisstOpenNIExKinectTest                   cisstOpenNI )
  
cisst_target_link_libraries(cisstOpenNIExKinectTest             cisstCommon
                                                        		cisstVector 
                                                        		cisstOSAbstraction 
                                                        		cisstMultiTask )