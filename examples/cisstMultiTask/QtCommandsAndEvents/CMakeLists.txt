#
# $Id$
#
# (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

qt4_wrap_cpp(QT_CLIENT_WRAPPED_CPP
             clientTask.h
             )
qt4_wrap_cpp(QT_SERVER_WRAPPED_CPP
             serverTask.h
	     )
qt4_wrap_ui(QT_CLIENT_WRAPPED_UI
            client.ui
	    )
qt4_wrap_ui(QT_SERVER_WRAPPED_UI
            server.ui
            )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(cisstMultiTaskQtCommandsAndEvents
               main.cpp
               clientTask.cpp
               clientTask.h
               ${QT_CLIENT_WRAPPED_CPP}
               ${QT_CLIENT_WRAPPED_UI}
               serverTask.cpp
               serverTask.h
               ${QT_SERVER_WRAPPED_CPP}
               ${QT_SERVER_WRAPPED_UI}
               )

target_link_libraries(cisstMultiTaskQtCommandsAndEvents
                      ${QT_LIBRARIES}
                      )

cisst_requires(cisstMultiTaskQtCommandsAndEvents
               "cisstCommon;cisstOSAbstraction;cisstMultiTask"
               )


# compile network version if ICE is available
if (CISST_MTS_HAS_ICE)

  add_executable(cisstMultiTaskQtCommandsAndEventsServer
                 serverMain.cpp
                 serverTask.cpp
                 serverTask.h
                 ${QT_SERVER_WRAPPED_CPP}
                 ${QT_SERVER_WRAPPED_UI}
	         )
  target_link_libraries(cisstMultiTaskQtCommandsAndEventsServer
                        ${QT_LIBRARIES}
                        )
  cisst_requires(cisstMultiTaskQtCommandsAndEventsServer
                 "cisstCommon;cisstOSAbstraction;cisstMultiTask")

  add_executable(cisstMultiTaskQtCommandsAndEventsClient
                 clientMain.cpp
                 clientTask.cpp
                 clientTask.h
                 ${QT_CLIENT_WRAPPED_CPP}
                 ${QT_CLIENT_WRAPPED_UI}
                 )
  target_link_libraries(cisstMultiTaskQtCommandsAndEventsClient
                        ${QT_LIBRARIES}
                        )
  CISST_REQUIRES(cisstMultiTaskQtCommandsAndEventsClient
                 "cisstCommon;cisstOSAbstraction;cisstMultiTask")

endif (CISST_MTS_HAS_ICE)
