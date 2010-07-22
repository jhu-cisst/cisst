#
# $Id$
#
# (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

qt4_wrap_ui(QT_CLIENT_WRAPPED_UI
            clientQtWidget.ui
            )
qt4_wrap_cpp(QT_CLIENT_WRAPPED_CPP
             clientQtComponent.h
             )
qt4_wrap_ui(QT_SERVER_WRAPPED_UI
            serverQtWidget.ui
            )
qt4_wrap_cpp(QT_SERVER_WRAPPED_CPP
             serverQtComponent.h
             )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(cisstMultiTaskCommandsAndEventsQt
               main.cpp
               clientQtComponent.cpp
               clientQtComponent.h
               ${QT_CLIENT_WRAPPED_UI}
               ${QT_CLIENT_WRAPPED_CPP}
               serverQtComponent.cpp
               serverQtComponent.h
               ${QT_SERVER_WRAPPED_UI}
               ${QT_SERVER_WRAPPED_CPP}
               )

target_link_libraries(cisstMultiTaskCommandsAndEventsQt
                      ${QT_LIBRARIES}
                      )

cisst_requires(cisstMultiTaskCommandsAndEventsQt
               "cisstCommon;cisstOSAbstraction;cisstMultiTask"
               )

# compile network version if ICE is available
if (CISST_MTS_HAS_ICE)

  add_executable(cisstMultiTaskCommandsAndEventsQtServer
                 serverMain.cpp
                 serverQtComponent.cpp
                 serverQtComponent.h
                 ${QT_SERVER_WRAPPED_UI}
                 ${QT_SERVER_WRAPPED_CPP}
                 )
  target_link_libraries(cisstMultiTaskCommandsAndEventsQtServer
                        ${QT_LIBRARIES}
                        )
  cisst_requires(cisstMultiTaskCommandsAndEventsQtServer
                 "cisstCommon;cisstOSAbstraction;cisstMultiTask"
                 )

  add_executable(cisstMultiTaskCommandsAndEventsQtClient
                 clientMain.cpp
                 clientQtComponent.cpp
                 clientQtComponent.h
                 ${QT_CLIENT_WRAPPED_UI}
                 ${QT_CLIENT_WRAPPED_CPP}
                 )
  target_link_libraries(cisstMultiTaskCommandsAndEventsQtClient
                        ${QT_LIBRARIES}
                        )
  CISST_REQUIRES(cisstMultiTaskCommandsAndEventsQtClient
                 "cisstCommon;cisstOSAbstraction;cisstMultiTask"
                 )
else (CISST_MTS_HAS_ICE)
  message(WARNING " cisstMultiTask example CommandsAndEventsQt will be compiled as single process only, client/server version requires CISST_MTS_HAS_ICE")
endif (CISST_MTS_HAS_ICE)
