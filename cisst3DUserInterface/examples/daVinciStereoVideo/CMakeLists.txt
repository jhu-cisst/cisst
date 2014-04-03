#
#
# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project (ui3ExDaVinciStereoVideo)

# sawSensablePhantom configuration
include_directories (${${CISST_UI3_ISI_WRAPPER_USED}_INCLUDE_DIR})
link_directories (${${CISST_UI3_ISI_WRAPPER_USED}_LIBRARY_DIR})

add_definitions (-D${CISST_UI3_ISI_WRAPPER_DEFINITION})

add_executable (ui3ExDaVinciStereoVideo
                main.cpp
                BehaviorLUS.cpp
                BehaviorLUS.h)

set_property (TARGET ui3ExDaVinciStereoVideo PROPERTY FOLDER "cisst3DUserInterface/examples")

target_link_libraries (ui3ExDaVinciStereoVideo
                       ui3ExBehaviors
                       ${${CISST_UI3_ISI_WRAPPER_USED}_LIBRARIES})

cisst_target_link_libraries (ui3ExDaVinciStereoVideo
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask
                             cisstStereoVision cisstParameterTypes cisst3DUserInterface)

# Copy VTK models to the build tree
if (WIN32)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/probeEnd.STL
                    ${CMAKE_CURRENT_BINARY_DIR}/probeEnd.STL COPYONLY)
else (WIN32)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/probeEnd.STL
                    ${EXECUTABLE_OUTPUT_PATH}/probeEnd.STL COPYONLY)
endif (WIN32)
