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

# List cisst libraries needed
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstFLTK)

# find cisst and make sure the required libraries have been compiled
find_package (cisst COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # add include paths
  include_directories (${CMAKE_CURRENT_SOURCE_DIR})

  # CMake command, generate .cxx and .h from .fl files
  fltk_wrap_ui (mtsExCommandsAndEventsFLTKLocal clientUI.fl serverUI.fl)

  # name the main executable and specifies with source files to use
  add_executable (mtsExCommandsAndEventsFLTKLocal
                  clientTask.cpp
                  serverTask.cpp
                  localMain.cpp
                  clientTask.h
                  serverTask.h
                  ${mtsExCommandsAndEventsFLTKLocal_FLTK_UI_SRCS}
                  )
  set_property (TARGET mtsExCommandsAndEventsFLTKLocal PROPERTY FOLDER "cisstMultiTask/examples")

  # link with the cisst libraries
  cisst_target_link_libraries (mtsExCommandsAndEventsFLTKLocal ${REQUIRED_CISST_LIBRARIES})

  # compile network version (will use ICE or mtsSocketProxyClient/mtsSocketProxyServer)

  # CMake command, generate .cxx and .h from .fl files
  fltk_wrap_ui (mtsExCommandsAndEventsFLTKServer serverUI.fl)

  # name the main executable and specifies with source files to use
  add_executable (mtsExCommandsAndEventsFLTKServer
                  serverTask.cpp
                  serverMain.cpp
                  serverTask.h
                  ${mtsExCommandsAndEventsFLTKServer_FLTK_UI_SRCS}
                  )
  set_property (TARGET mtsExCommandsAndEventsFLTKServer PROPERTY FOLDER "cisstMultiTask/examples")

  # link with the cisst libraries
  cisst_target_link_libraries (mtsExCommandsAndEventsFLTKServer ${REQUIRED_CISST_LIBRARIES})

  # CMake command, generate .cxx and .h from .fl files
  fltk_wrap_ui (mtsExCommandsAndEventsFLTKClient clientUI.fl)

  # name the main executable and specifies with source files to use
  add_executable (mtsExCommandsAndEventsFLTKClient
                  clientTask.cpp
                  clientMain.cpp
                  clientTask.h
                  ${mtsExCommandsAndEventsFLTKClient_FLTK_UI_SRCS}
                  )
  set_property (TARGET mtsExCommandsAndEventsFLTKClient PROPERTY FOLDER "cisstMultiTask/examples")

  # link with the cisst libraries
  cisst_target_link_libraries (mtsExCommandsAndEventsFLTKClient ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
