#
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

project (ui3ExSensableNoVideo)

# sawSensablePhantom configuration
include_directories (${sawSensablePhantom_INCLUDE_DIR}
                     ${sawKeyboard_INCLUDE_DIR})
link_directories (${sawSensablePhantom_LIBRARY_DIR}
                  ${sawKeyboard_LIBRARY_DIR})


add_executable (ui3ExSensableNoVideo main.cpp)

target_link_libraries (ui3ExSensableNoVideo
                       ui3ExBehaviors
                       ${sawSensablePhantom_LIBRARIES}
                       ${sawKeyboard_LIBRARIES})

cisst_target_link_libraries (ui3ExSensableNoVideo
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask
                             cisstStereoVision cisstParameterTypes cisst3DUserInterface)
