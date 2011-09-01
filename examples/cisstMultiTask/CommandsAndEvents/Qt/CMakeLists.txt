#
# $Id$
#
# (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# List cisst libraries needed
set (CISST_LIBRARIES_NEEDED cisstCommon
                            cisstCommonQt
                            cisstVector
                            cisstVectorQt
                            cisstOSAbstraction
                            cisstMultiTask
                            cisstMultiTaskQt)

# Set include/link directories
cisst_set_directories (${CISST_LIBRARIES_NEEDED})

# wrap all Qt files
qt4_wrap_ui (QT_CLIENT_WRAPPED_UI
             clientQtWidget.ui
            )
qt4_wrap_cpp (QT_CLIENT_WRAPPED_CPP
              clientQtComponent.h
             )
qt4_wrap_ui (QT_SERVER_WRAPPED_UI
             serverQtWidget.ui
            )
qt4_wrap_cpp (QT_SERVER_WRAPPED_CPP
              serverQtComponent.h
             )

# needed to include Qt generated header files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# create example using a single process
add_executable (cisstMultiTaskCommandsAndEventsQtLocal
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

target_link_libraries (cisstMultiTaskCommandsAndEventsQtLocal       ${QT_LIBRARIES})
cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsQtLocal ${CISST_LIBRARIES_NEEDED})

# compile network version if ICE is available
if (CISST_MTS_HAS_ICE)

  add_executable (cisstMultiTaskCommandsAndEventsQtServer
                  serverMain.cpp
                  serverQtComponent.cpp
                  serverQtComponent.h
                  ${QT_SERVER_WRAPPED_UI}
                  ${QT_SERVER_WRAPPED_CPP}
                 )
  target_link_libraries (cisstMultiTaskCommandsAndEventsQtServer       ${QT_LIBRARIES})
  cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsQtServer ${CISST_LIBRARIES_NEEDED})

  add_executable (cisstMultiTaskCommandsAndEventsQtClient
                  clientMain.cpp
                  clientQtComponent.cpp
                  clientQtComponent.h
                  ${QT_CLIENT_WRAPPED_UI}
                  ${QT_CLIENT_WRAPPED_CPP}
                 )
  target_link_libraries (cisstMultiTaskCommandsAndEventsQtClient       ${QT_LIBRARIES})
  cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsQtClient ${CISST_LIBRARIES_NEEDED})

else (CISST_MTS_HAS_ICE)
  message(WARNING " cisstMultiTask example CommandsAndEventsQt will be compiled as single process only, client/server version requires CISST_MTS_HAS_ICE")
endif (CISST_MTS_HAS_ICE)
