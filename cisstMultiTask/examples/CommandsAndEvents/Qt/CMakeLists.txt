#
# (C) Copyright 2009-2025 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# List cisst libraries needed
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstCommonQt
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstMultiTaskQt
                              cisstQt)


# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # wrap all Qt files

  set (CMAKE_AUTOMOC ON)
  qt5_wrap_ui (QT_CLIENT_WRAPPED_UI clientQtWidget.ui)
  qt5_wrap_ui (QT_SERVER_WRAPPED_UI serverQtWidget.ui)

  # needed to include Qt generated header files
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  # create example using a single process
  add_executable (mtsExCommandsAndEventsQtLocal
                  main.cpp
                  clientQtComponent.cpp
                  clientQtComponent.h
                  ${QT_CLIENT_WRAPPED_UI}
                  serverQtComponent.cpp
                  serverQtComponent.h
                  ${QT_SERVER_WRAPPED_UI}
                  serverWithDelay.h
                  serverWithDelay.cpp
                 )
  set_property (TARGET mtsExCommandsAndEventsQtLocal PROPERTY FOLDER "cisstMultiTask/examples")
  cisst_target_link_libraries (mtsExCommandsAndEventsQtLocal ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
