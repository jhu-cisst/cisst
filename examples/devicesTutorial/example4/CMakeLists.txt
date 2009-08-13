#
#   $Id$
#
#   Author(s): Ali Uneri
#
#   (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.
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

find_package(FLTK REQUIRED)
include_directories(${devicesTutorialExample4_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR}
                    )

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
  ${FLTK_LIBRARIES}
  OpenIGTLink
  )

cisst_requires(devicesTutorialExample4Simulator
  "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices"
  )


if(CISST_DEV_HAS_SENSABLEHD)
  find_package(Sensable REQUIRED)
  include_directories(${SENSABLE_INCLUDE_DIR})
  add_executable(devicesTutorialExample4Sensable
    mainSensable.cpp
    )
  target_link_libraries(devicesTutorialExample4Sensable
    ${SENSABLE_LIBRARIES}
    OpenIGTLink
  )
  cisst_requires(devicesTutorialExample4Sensable
    "cisstCommon;cisstVector;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstDevices"
    )
endif(CISST_DEV_HAS_SENSABLEHD)
