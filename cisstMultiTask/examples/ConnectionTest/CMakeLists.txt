#
# (C) Copyright 2005-2014 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (REQUIRED_CISST_LIBRARIES cisstCommon cisstOSAbstraction cisstMultiTask cisstFLTK)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # add include paths
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  # CMake command, generate .cxx and .h from .fl files
  fltk_wrap_ui (mtsExConnectionTestServer C3UI.fl serverUI.fl)

  # name the main executable and specifies with source files to use
  add_executable (mtsExConnectionTestServer
                  fltkMutex.cpp
                  C2ServerTask.cpp
                  C3Task.cpp
                  P2main.cpp
                  fltkMutex.h
                  C2ServerTask.h
                  C3Task.h
                  ${mtsExConnectionTestServer_FLTK_UI_SRCS}
                  )
  set_property (TARGET mtsExConnectionTestServer PROPERTY FOLDER "cisstMultiTask/examples")

  # link with the cisst libraries
  cisst_target_link_libraries (mtsExConnectionTestServer ${REQUIRED_CISST_LIBRARIES})

  # CMake command, generate .cxx and .h from .fl files
  fltk_wrap_ui (mtsExConnectionTestClient C1UI.fl C2ClientUI.fl)

  # name the main executable and specifies with source files to use
  add_executable (mtsExConnectionTestClient
                  fltkMutex.cpp
                  C1Task.cpp
                  C2ClientTask.cpp
                  P1main.cpp
                  fltkMutex.h
                  C1Task.h
                  C2ClientTask.h
                  ${mtsExConnectionTestClient_FLTK_UI_SRCS}
                  )
  set_property (TARGET mtsExConnectionTestClient PROPERTY FOLDER "cisstMultiTask/examples")

  # link with the cisst libraries
  cisst_target_link_libraries (mtsExConnectionTestClient ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
