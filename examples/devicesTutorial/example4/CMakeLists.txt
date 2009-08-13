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

# name of project and executable
project(devicesTutorialExample4)

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

find_package(FLTK REQUIRED)
include_directories(${${PROJECT_NAME}_SOURCE_DIR}
  ${FLTK_INCLUDE_DIR}
  )

fltk_wrap_ui(${PROJECT_NAME}
  trackerUI.fl
  )

# name the main executable and specifies with source files to use
add_executable(${PROJECT_NAME}
  main.cpp
  trackerSimulator.cpp
  trackerSimulator.h
  ${${PROJECT_NAME}_FLTK_UI_SRCS}
  )

target_link_libraries(${PROJECT_NAME}
  ${FLTK_LIBRARIES}
  )

# link with the cisst libraries
cisst_requires(${PROJECT_NAME}
  "cisstCommon;cisstDevices;cisstOSAbstraction;cisstMultiTask;cisstParameterTypes;cisstVector"
  )
