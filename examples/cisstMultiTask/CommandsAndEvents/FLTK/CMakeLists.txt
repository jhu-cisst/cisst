#
# $Id$
#
# (C) Copyright 2005-2011 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# add include paths
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

# CMake command, generate .cxx and .h from .fl files
fltk_wrap_ui (cisstMultiTaskCommandsAndEventsFLTKLocal clientUI.fl serverUI.fl)

# name the main executable and specifies with source files to use
add_executable (cisstMultiTaskCommandsAndEventsFLTKLocal
                clientTask.cpp
                serverTask.cpp
                localMain.cpp
                clientTask.h
                serverTask.h
                ${cisstMultiTaskCommandsAndEventsFLTKLocal_FLTK_UI_SRCS}
                )

# link with the cisst libraries
cisst_target_link_libraries(cisstMultiTaskCommandsAndEventsFLTKLocal
                            cisstCommon cisstOSAbstraction cisstMultiTask)


# compile network version if ICE is available
if (CISST_MTS_HAS_ICE)

  # CMake command, generate .cxx and .h from .fl files
  fltk_wrap_ui (cisstMultiTaskCommandsAndEventsFLTKServer serverUI.fl)

  # name the main executable and specifies with source files to use
  add_executable (cisstMultiTaskCommandsAndEventsFLTKServer
                  serverTask.cpp
                  serverMain.cpp
                  serverTask.h
                  ${cisstMultiTaskCommandsAndEventsFLTKServer_FLTK_UI_SRCS}
                  )
  # link with the cisst libraries
  cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsFLTKServer
                               cisstCommon cisstOSAbstraction cisstMultiTask)


  # CMake command, generate .cxx and .h from .fl files
  fltk_wrap_ui (cisstMultiTaskCommandsAndEventsFLTKClient clientUI.fl)

  # name the main executable and specifies with source files to use
  add_executable (cisstMultiTaskCommandsAndEventsFLTKClient
                  clientTask.cpp
                  clientMain.cpp
                  clientTask.h
                  ${cisstMultiTaskCommandsAndEventsFLTKClient_FLTK_UI_SRCS}
                  )
  # link with the cisst libraries
  cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsFLTKClient
                               cisstCommon cisstOSAbstraction cisstMultiTask)

else (CISST_MTS_HAS_ICE)
  message (WARNING " cisstMultiTask example CommandsAndEventsFLTK will be compiled as single process only, client/server version requires CISST_MTS_HAS_ICE")
endif (CISST_MTS_HAS_ICE)
