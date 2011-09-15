#
#   $Id$
#
#   Author(s):  Ali Uneri
#   Created on: 2009-08-13
#
#   (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

project(devicesTutorialExample4)

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

# List cisst libraries needed
set (CISST_LIBRARIES_NEEDED cisstCommon
                            cisstVector
                            cisstOSAbstraction
                            cisstMultiTask
                            cisstParameterTypes
                            cisstDevices
                            )

# Set include/link directories
cisst_set_directories (${CISST_LIBRARIES_NEEDED})

fltk_wrap_ui(devicesTutorialExample4Simulator
             trackerUI.fl
             )

add_executable(devicesTutorialExample4Simulator
               mainSimulator.cpp
               trackerSimulator.cpp
               trackerSimulator.h
               ${devicesTutorialExample4Simulator_FLTK_UI_SRCS}
               )

target_link_libraries(devicesTutorialExample4Simulator
                      OpenIGTLink
                      )

cisst_target_link_libraries(devicesTutorialExample4Simulator
               ${CISST_LIBRARIES_NEEDED}
               )

if(CISST_DEV_HAS_SENSABLEHD)
  find_package(Sensable REQUIRED)
  include_directories(${SENSABLE_INCLUDE_DIR})

  add_executable(devicesTutorialExample4Sensable
                 mainSensable.cpp
                 )

  target_link_libraries(devicesTutorialExample4Sensable
                        OpenIGTLink
                        ${SENSABLE_LIBRARIES}
                        )

  cisst_target_link_libraries(devicesTutorialExample4Sensable
                 ${CISST_LIBRARIES_NEEDED}
                 )
endif(CISST_DEV_HAS_SENSABLEHD)
