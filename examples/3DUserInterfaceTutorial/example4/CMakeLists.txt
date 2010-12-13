#
# $Id$
#

# (C) Copyright 2008-2010 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (3DUserInterfaceTutorialExample4)

add_executable (3DUserInterfaceTutorialExample4 main.cpp)

target_link_libraries (3DUserInterfaceTutorialExample4 3DUserInterfaceTutorialBehaviors ${daVinci_ALL_LIBRARIES})

cisst_target_link_libraries (3DUserInterfaceTutorialExample4 cisstCommon cisstVector cisstOSAbstraction cisstMultiTask cisstStereoVision cisstDevices cisstParameterTypes cisst3DUserInterface)
