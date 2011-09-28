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
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstCommonQt
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstMultiTaskQt
                              cisstQt)


# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # wrap all Qt files
  qt4_wrap_ui (QT_CLIENT_WRAPPED_UI clientQtWidget.ui)
  qt4_wrap_cpp (QT_CLIENT_WRAPPED_CPP clientQtComponent.h)
  qt4_wrap_ui (QT_SERVER_WRAPPED_UI serverQtWidget.ui)
  qt4_wrap_cpp (QT_SERVER_WRAPPED_CPP serverQtComponent.h)

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

  cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsQtLocal ${REQUIRED_CISST_LIBRARIES})

  # compile network version if ICE is available
  if (CISST_MTS_HAS_ICE)

    add_executable (cisstMultiTaskCommandsAndEventsQtServer
                    serverMain.cpp
                    serverQtComponent.cpp
                    serverQtComponent.h
                    ${QT_SERVER_WRAPPED_UI}
                    ${QT_SERVER_WRAPPED_CPP}
                   )
    cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsQtServer ${REQUIRED_CISST_LIBRARIES})

    add_executable (cisstMultiTaskCommandsAndEventsQtClient
                    clientMain.cpp
                    clientQtComponent.cpp
                    clientQtComponent.h
                    ${QT_CLIENT_WRAPPED_UI}
                    ${QT_CLIENT_WRAPPED_CPP}
                   )
    cisst_target_link_libraries (cisstMultiTaskCommandsAndEventsQtClient ${REQUIRED_CISST_LIBRARIES})

  else (CISST_MTS_HAS_ICE)
    message(WARNING "Code in ${CMAKE_CURRENT_SOURCE_DIR}: example will be compiled as single process only, client/server version requires CISST_MTS_HAS_ICE")
  endif (CISST_MTS_HAS_ICE)

else (cisst_FOUND_AS_REQUIRED)
  message ("Code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
