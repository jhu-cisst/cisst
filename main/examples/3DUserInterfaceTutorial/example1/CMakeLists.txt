#
# $Id: CMakeLists.txt 788 2009-09-01 17:40:00Z gsevinc1 $
#

project(3DUserInterfaceTutorialExample1)

add_executable(3DUserInterfaceTutorialExample1
               main.cpp
               )
			   
target_link_libraries(3DUserInterfaceTutorialExample1
                      3DUserInterfaceTutorialBehaviors
                      )

CISST_REQUIRES(3DUserInterfaceTutorialExample1 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
