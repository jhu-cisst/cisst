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

qt4_wrap_cpp(QT_WRAPPED_CPP
             clientTask.h
             serverTask.h
             )
qt4_wrap_ui(QT_WRAPPED_UI
            client.ui
            server.ui
            )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(cisstMultiTaskQtCommandsAndEvents
               ${QT_WRAPPED_CPP}
               ${QT_WRAPPED_UI}
               clientTask.cpp
               clientTask.h
               main.cpp
               serverTask.cpp
               serverTask.h
               )

target_link_libraries(cisstMultiTaskQtCommandsAndEvents
                      ${QT_LIBRARIES}
                      )

cisst_requires(cisstMultiTaskQtCommandsAndEvents
               "cisstCommon;cisstOSAbstraction;cisstMultiTask"
               )
