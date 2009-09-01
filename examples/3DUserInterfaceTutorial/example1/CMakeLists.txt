#
# $Id$
#

project(3DUserInterfaceTutorialExample1)

add_executable(3DUserInterfaceTutorialExample1
               main.cpp
               )
			   
target_link_libraries(3DUserInterfaceTutorialExample1
                      3DUserInterfaceTutorialBehaviors
                      )

CISST_REQUIRES(3DUserInterfaceTutorialExample1 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
