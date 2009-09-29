#
# $Id: CMakeLists.txt 665 2009-08-12 17:21:59Z adeguet1 $
#

project(3DUserInterfaceTutorialExample5)

add_executable(3DUserInterfaceTutorialExample5
               main.cpp
               )

target_link_libraries(3DUserInterfaceTutorialExample5
                      3DUserInterfaceTutorialBehaviors
                      )

CISST_REQUIRES(3DUserInterfaceTutorialExample5 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
