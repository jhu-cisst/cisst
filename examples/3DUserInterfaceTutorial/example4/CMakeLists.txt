#
# $Id$
#

project (3DUserInterfaceTutorialExample4)

add_executable (3DUserInterfaceTutorialExample4
                main.cpp
               )
			   
target_link_libraries (3DUserInterfaceTutorialExample4
                       3DUserInterfaceTutorialBehaviors
                       ${daVinci_LIBRARIES}
                      )

cisst_requires (3DUserInterfaceTutorialExample4 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
