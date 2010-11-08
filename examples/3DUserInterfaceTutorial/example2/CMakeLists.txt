#
# $Id$
#

project(3DUserInterfaceTutorialExample2)


add_executable(3DUserInterfaceTutorialExample2
               main.cpp
               )

target_link_libraries(3DUserInterfaceTutorialExample2
                      3DUserInterfaceTutorialBehaviors
                      )

CISST_REQUIRES(3DUserInterfaceTutorialExample2 "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstStereoVision;cisstDevices;cisstParameterTypes;cisst3DUserInterface")
