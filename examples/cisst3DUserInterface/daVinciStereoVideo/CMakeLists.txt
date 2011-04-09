#
# $Id$
#
# (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

add_executable (cisst3DUserInterfaceDaVinciStereoVideo
                main.cpp
                BehaviorLUS.cpp
                BehaviorLUS.h
                CSOpenGLStippleActor.cxx
                CSOpenGLStippleActor.h
                )

target_link_libraries (cisst3DUserInterfaceDaVinciStereoVideo
                       cisst3DUserInterfaceBehaviors
                       ${daVinci_ALL_LIBRARIES}
                       )

# Copy VTK models to the build tree
if (WIN32)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/probeEnd.STL
                    ${CMAKE_BINARY_DIR}/examples/cisst3DUserInterface/daVinciStereoVideo/probeEnd.STL COPYONLY)
else (WIN32)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/probeEnd.STL
                    ${CMAKE_BINARY_DIR}/examples/bin/probeEnd.STL COPYONLY)
endif (WIN32)

cisst_target_link_libraries (cisst3DUserInterfaceDaVinciStereoVideo
                             cisstCommon cisstVector cisstOSAbstraction cisstMultiTask
                             cisstStereoVision cisstRobot cisstParameterTypes cisst3DUserInterface)
